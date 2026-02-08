# Mug Internal Documentation

Implementation details for the Mug configurable HTTP server application.

**Source:** `third/Mug/src/Mug/`

---

## Architecture

Mug is a standalone executable that extends `NisseServer`:

```
┌──────────────────────────────────────────────────┐
│                 mug (executable)                  │
│   MugCLA (argument parsing)                       │
│   MugConfig (JSON configuration)                  │
├──────────────────────────────────────────────────┤
│                 MugServer                         │
│   Extends NisseServer                             │
│   + DLLibMap (plugin management)                  │
│   + LibraryChecker (hot-reload timer)             │
│   + PyntHTTPControl (control endpoint)            │
├──────────────────────────────────────────────────┤
│                 NisseServer / NisseHTTP            │
└──────────────────────────────────────────────────┘
```

---

## File Map

| File | Purpose |
|------|---------|
| `mug.cpp` | Main entry point |
| `MugServer.h/.cpp` | Server class extending NisseServer |
| `MugConfig.h` | Configuration structs with ThorsSerializer traits |
| `MugArgs.h/.cpp` | Command-line argument storage |
| `MugCLA.h/.cpp` | Command-line argument parser |
| `DLLib.h/.cpp` | Dynamic library loading and management |

---

## MugServer

Extends `NisseServer` with:

- **`PyntHTTPControl control`**: HTTP control endpoint on the configured control port.
- **`Hanlders servers`**: A `std::vector<HTTPHandler>`, one per configured port.
- **`DLLibMap libraries`**: Manages all loaded shared libraries.
- **`LibraryChecker libraryChecker`**: A `TimerAction` that periodically calls `checkLibrary()`.

### Construction

1. Creates one `HTTPHandler` per configured port.
2. For each port, iterates its `actions` and calls `libraries.load(handler, pluginInfo)` to load the shared library and register its routes.
3. Configures TLS if `certPath` is present.
4. Starts listening on each port.
5. Sets up the control endpoint.
6. If `libraryCheckTime > 0`, registers the `LibraryChecker` timer.

### Hot-Reload (checkLibrary)

The `LibraryChecker` timer calls `checkLibrary()` which delegates to `DLLibMap::checkAll()`. Each `DLLib` compares the file's modification time against the stored `lastModified`. If changed:

1. Call `plugin->stop(handler)` to unregister routes.
2. Call `dlclose()` on the old library.
3. Call `dlopen()` on the new library.
4. Call the factory function to get a new `MugPlugin*`.
5. Call `plugin->start(handler)` to register new routes.

---

## DLLib (Dynamic Library Management)

### Loading

1. `dlopen(path, RTLD_NOW)` loads the shared library.
2. `dlsym(lib, "mugPlugin")` retrieves the `MugFunc` factory function.
3. The factory is called with the config JSON string to create a `MugPlugin*`.
4. `plugin->start(handler)` registers the plugin's routes.

### Unloading

1. `plugin->stop(handler)` unregisters routes.
2. The `MugPlugin*` is deleted.
3. `dlclose(lib)` unloads the shared library.

### Error Handling

`dlerror()` is checked after each `dlopen`/`dlsym`/`dlclose` call. Errors are logged and thrown as `std::runtime_error`.

---

## Configuration Serialization

The configuration structs use ThorsSerializer for JSON deserialization:

```cpp
ThorsAnvil_MakeTrait(Plugin, pluginPath, config);
ThorsAnvil_MakeTrait(PortConfig, port, certPath, actions);
ThorsAnvil_MakeTrait(MugConfig, servers, controlPort, libraryCheckTime);
```

The `config` field in `Plugin` uses `ThorsAnvil::Serialize::AnyBlock` to accept arbitrary JSON that is passed through to the plugin factory function as a string.

---

## Command-Line Argument Parsing

### MugCLA

Parses arguments from `argv`:
- `--config <file>` / `-c <file>`
- `--silent` / `-s`
- `--help` / `-h`
- `--log-file <path>` / `--log-sys <name>` / `--log-level <level>`

If `--config` is not specified, searches a list of default paths (`searchPath`) for a configuration file.

### MugArgs

Stores the parsed results:
- `help`: whether to display help
- `silent`: headless mode flag
- `configPath`: resolved path to the config file

Implements `MugCLAInterface` to receive parsed values from `MugCLA`.

---

## Server Initialization Flow

1. Parse command-line arguments into `MugArgs`.
2. If `help` flag set, display help and exit.
3. Validate config file exists.
4. Deserialize JSON config via `jsonImporter(config)`.
5. Construct `MugServer(config, mode)`.
6. Call `server.run()` (blocks until stopped).

---

## Thread Safety

MugServer uses 4 worker threads (hardcoded `workerCount = 4`). The hot-reload check runs on the main thread (via the timer callback), so no locking is needed for plugin loading/unloading since the `Store` command queue pattern ensures all mutations happen on the main thread.
