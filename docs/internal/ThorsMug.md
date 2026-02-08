[Home](../index.html) | [API Documentation](../ThorsMug.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsMug Internal Documentation

Implementation details for the ThorsMug plugin interface.

**Source:** `third/Mug/src/ThorsMug/`

---

## Architecture

ThorsMug is a minimal library consisting of a single header (`MugPlugin.h`) that defines the plugin interface. Plugins are compiled as shared libraries and loaded by the Mug server at runtime.

---

## MugPlugin Interface

```cpp
class MugPlugin {
public:
    virtual ~MugPlugin() {}
    virtual void start(NisHttp::HTTPHandler& handler) = 0;
    virtual void stop(NisHttp::HTTPHandler& handler) = 0;
};
```

- `start()` is called when the plugin is loaded. The plugin should register its HTTP routes on the provided `HTTPHandler`.
- `stop()` is called when the plugin is being unloaded. The plugin should remove its routes.

---

## MugPluginSimple

A convenience base class that automates route registration:

```cpp
class MugPluginSimple : public MugPlugin {
public:
    virtual void start(NisHttp::HTTPHandler& handler) override {
        auto data = getAction();
        for (auto& item : data) {
            handler.addPath(item.method, item.path,
                           std::move(item.action), std::move(item.validate));
        }
    }

    virtual void stop(NisHttp::HTTPHandler& handler) override {
        auto data = getAction();
        for (auto& item : data) {
            handler.remPath(item.method, item.path);
        }
    }

    virtual std::vector<Action> getAction() = 0;
};
```

Note: `getAction()` is called in both `start()` and `stop()`, so it must be idempotent and return the same routes both times.

---

## Action Struct

```cpp
struct Action {
    NisHttp::Method       method;
    std::string           path;
    NisHttp::HTTPAction   action;
    NisHttp::HTTPValidate validate = [](NisHttp::Request const&){return true;};
};
```

---

## Factory Function

Every plugin shared library must export a C function matching `MugFunc`:

```cpp
extern "C" {
    typedef ThorsAnvil::ThorsMug::MugPlugin*(*MugFunc)(char const* config);
}
```

The `config` parameter is the JSON string from the Mug configuration file's `config` field for this plugin instance. The function must return a heap-allocated `MugPlugin*` (ownership transfers to Mug).

---

## Plugin Loading Sequence

1. Mug calls `dlopen()` on the shared library path.
2. Mug calls `dlsym()` to find the `mugPlugin` symbol.
3. Mug casts the symbol to `MugFunc` and calls it with the config string.
4. The returned `MugPlugin*` is stored alongside the `HTTPHandler` reference.
5. Mug calls `plugin->start(handler)`.

## Plugin Unloading Sequence

1. Mug calls `plugin->stop(handler)`.
2. Mug deletes the `MugPlugin*`.
3. Mug calls `dlclose()` on the shared library.

---

## Design Considerations

### dlclose() Safety

The `PathMatcher` in NisseHTTP stores actions as structured types rather than `std::function` directly. This is important because after `dlclose()`, any function pointers from the unloaded library become invalid. The `stop()` method must remove all routes before the library is unloaded.

### Hot-Reload

During hot-reload, `stop()` is called on the old plugin instance, then the library is unloaded and reloaded. The new `MugFunc` is called to create a fresh plugin instance, and `start()` is called. This means:
- Plugins should not maintain persistent state between hot-reloads.
- All state should be reconstructable from the config string.
