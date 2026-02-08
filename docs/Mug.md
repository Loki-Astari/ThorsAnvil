# Mug API

A configurable HTTP server application that dynamically loads shared-library plugins at runtime. Define your REST endpoints as plugins and Mug handles the server lifecycle, JSON configuration, TLS setup, and hot-reloading of plugins.

**Namespace:** `ThorsAnvil::ThorsMug`

---

## Overview

Mug is a standalone executable built on NisseServer and NisseHTTP. Instead of writing a custom `main()` function, you:

1. Write plugins (shared libraries) that implement `MugPlugin`.
2. Configure Mug with a JSON file that specifies which ports to listen on and which plugins to load.
3. Run the `mug` binary -- it loads your plugins and starts serving.

This architecture enables hot-reloading: Mug periodically checks if plugin libraries have been updated and reloads them without restarting the server.

---

## Running Mug

```bash
mug --config /path/to/config.json
```

### Command-Line Arguments

| Flag | Description |
|------|-------------|
| `--config <file>` | Path to the JSON configuration file |
| `--silent` | Run in headless mode (no interactive output) |
| `--help` | Display usage information |
| `--log-file <path>` | Add a log file |
| `--log-sys <name>` | Enable system logging |
| `--log-level <level>` | Set log verbosity |

If `--config` is not specified, Mug searches default paths for a configuration file.

---

## Configuration File

The configuration is a JSON file:

```json
{
    "controlPort": 8079,
    "libraryCheckTime": 5000,
    "servers": [
        {
            "port": 8080,
            "actions": [
                {
                    "pluginPath": "/path/to/libMyPlugin.so",
                    "config": { "key": "value" }
                }
            ]
        },
        {
            "port": 8443,
            "certPath": "/etc/letsencrypt/live/example.com",
            "actions": [
                {
                    "pluginPath": "/path/to/libSecurePlugin.so",
                    "config": {}
                }
            ]
        }
    ]
}
```

### Configuration Fields

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `controlPort` | int | 8079 | Port for the HTTP control endpoint (stop/ping) |
| `libraryCheckTime` | int | 0 | How often to check for plugin updates (ms). 0 = disabled. |
| `servers` | array | required | List of port configurations |

### Port Configuration

| Field | Type | Description |
|-------|------|-------------|
| `port` | int | Port to listen on |
| `certPath` | string (optional) | Path to TLS certificate directory (containing `fullchain.pem` and `privkey.pem`) |
| `actions` | array | Plugins to load for this port |

### Plugin Configuration

| Field | Type | Description |
|-------|------|-------------|
| `pluginPath` | string | Path to the shared library (`.so` / `.dylib`) |
| `config` | object | Arbitrary JSON passed to the plugin's factory function |

---

## Control Endpoint

Mug exposes an HTTP control endpoint on `controlPort`. Send requests to manage the server:

```bash
# Graceful shutdown
curl "http://localhost:8079/control?command=stopsoft"

# Immediate shutdown
curl "http://localhost:8079/control?command=stophard"

# Health check
curl "http://localhost:8079/control?command=ping"
```

---

## Hot-Reloading

When `libraryCheckTime` is non-zero, Mug periodically checks if any plugin shared libraries have been modified on disk. If a library has been updated:

1. The old plugin is stopped (`MugPlugin::stop()` is called).
2. The old library is unloaded.
3. The new library is loaded.
4. The new plugin is started (`MugPlugin::start()` is called).

This enables zero-downtime updates to your HTTP handlers.
