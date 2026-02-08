# ThorsMug API

The plugin interface for the Mug server. Implement `MugPlugin` to create dynamically-loaded HTTP route handlers.

**Namespace:** `ThorsAnvil::ThorsMug`

---

## Header

```cpp
#include "ThorsMug/MugPlugin.h"
```

---

## Writing a Plugin

### Step 1: Implement MugPlugin

The simplest approach is to subclass `MugPluginSimple` and return a list of routes:

```cpp
#include "ThorsMug/MugPlugin.h"

namespace NisHttp = ThorsAnvil::Nisse::HTTP;

class MyPlugin : public ThorsAnvil::ThorsMug::MugPluginSimple
{
public:
    std::vector<ThorsAnvil::ThorsMug::Action> getAction() override
    {
        return {
            {NisHttp::Method::GET, "/api/hello/{name}",
                [](NisHttp::Request const& req, NisHttp::Response& res) {
                    std::string name = req.variables()["name"];
                    std::string body = "Hello, " + name + "!";
                    res.body(body.size()) << body;
                    return true;
                }
            },
            {NisHttp::Method::POST, "/api/data",
                [](NisHttp::Request const& req, NisHttp::Response& res) {
                    std::string_view data = req.preloadStreamIntoBuffer();
                    std::string body = "Received: " + std::string(data);
                    res.body(body.size()) << body;
                    return true;
                }
            }
        };
    }
};
```

### Step 2: Export the Factory Function

Every plugin shared library must export a C function named after `MugFunc`:

```cpp
extern "C" ThorsAnvil::ThorsMug::MugPlugin* mugPlugin(char const* config)
{
    // 'config' is the JSON string from the Mug configuration file
    return new MyPlugin();
}
```

### Step 3: Build as a Shared Library

```bash
g++ -std=c++20 -shared -fPIC -o libMyPlugin.so MyPlugin.cpp \
    -lNisseHTTP -lNisseServer -lThorsSocket
```

### Step 4: Configure Mug

Add the plugin to your Mug configuration:

```json
{
    "servers": [
        {
            "port": 8080,
            "actions": [
                {
                    "pluginPath": "/path/to/libMyPlugin.so",
                    "config": {}
                }
            ]
        }
    ]
}
```

---

## MugPlugin Interface

For full control, implement `MugPlugin` directly:

```cpp
class MugPlugin {
public:
    virtual ~MugPlugin() {}
    virtual void start(NisHttp::HTTPHandler& handler) = 0;
    virtual void stop(NisHttp::HTTPHandler& handler) = 0;
};
```

| Method | Description |
|--------|-------------|
| `start(handler)` | Called when the plugin is loaded. Register routes on the `HTTPHandler`. |
| `stop(handler)` | Called when the plugin is unloaded. Remove your routes from the `HTTPHandler`. |

---

## MugPluginSimple

A convenience base class that handles `start()` and `stop()` automatically:

```cpp
class MugPluginSimple : public MugPlugin {
public:
    virtual std::vector<Action> getAction() = 0;
    // start() registers all actions, stop() removes them
};
```

### Action Struct

```cpp
struct Action {
    NisHttp::Method       method;    // HTTP method
    std::string           path;      // URL pattern with {captures}
    NisHttp::HTTPAction   action;    // Handler function
    NisHttp::HTTPValidate validate;  // Optional validation (default: always true)
};
```

---

## Plugin Lifecycle

1. Mug loads the shared library via `dlopen()`.
2. Mug calls the `MugFunc` factory function with the plugin's config JSON.
3. Mug calls `plugin->start(handler)` to register routes.
4. Plugin serves requests until shutdown or hot-reload.
5. Mug calls `plugin->stop(handler)` to unregister routes.
6. Mug unloads the shared library via `dlclose()`.
