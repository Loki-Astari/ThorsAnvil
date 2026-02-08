# NisseHTTP API

HTTP/1.x layer built on NisseServer. Provides request parsing, response generation, URL routing with path captures, chunked transfer encoding, and client-side HTTP utilities.

**Namespace:** `ThorsAnvil::Nisse::HTTP`

---

## Quick Start

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseHTTP/HTTPHandler.h"
#include <ThorsSocket/Server.h>

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
namespace TASock    = ThorsAnvil::ThorsSocket;

class MyApp : public NisServer::NisseServer
{
    NisHttp::HTTPHandler http;

public:
    MyApp(int port)
    {
        http.addPath(NisHttp::Method::GET, "/hello/{name}",
            [](NisHttp::Request const& req, NisHttp::Response& res) {
                std::string name = req.variables()["name"];
                std::string body = "Hello, " + name + "!";
                res.body(body.size()) << body;
                return true;
            });

        listen(TASock::ServerInfo{port}, http);
    }
};

int main()
{
    MyApp server(8080);
    server.run();
}
```

---

## Headers

| Header | Purpose |
|--------|---------|
| `NisseHTTP/HTTPHandler.h` | URL-pattern-based route dispatcher |
| `NisseHTTP/PyntHTTP.h` | Base class for custom HTTP handlers |
| `NisseHTTP/Request.h` | HTTP request object |
| `NisseHTTP/Response.h` | HTTP response builder |
| `NisseHTTP/URL.h` | URL parser |
| `NisseHTTP/PyntHTTPControl.h` | HTTP control endpoint (stop/ping) |
| `NisseHTTP/ClientStream.h` | Outbound TLS connection |
| `NisseHTTP/ClientRequest.h` | Outbound HTTP request builder |
| `NisseHTTP/ClientResponse.h` | Inbound HTTP response parser |

---

## HTTPHandler (URL Routing)

The most common way to handle HTTP requests. Register URL patterns with handler functions.

### Registering Routes

```cpp
NisHttp::HTTPHandler http;

// Route with path capture
http.addPath(NisHttp::Method::GET, "/user/{id}/profile",
    [](NisHttp::Request const& req, NisHttp::Response& res) {
        std::string userId = req.variables()["id"];
        std::string body = "Profile for user " + userId;
        res.body(body.size()) << body;
        return true;
    });

// Route for any HTTP method
http.addPath(NisHttp::All::Method, "/api/{endpoint}",
    [](NisHttp::Request const& req, NisHttp::Response& res) {
        // handles GET, POST, PUT, DELETE, etc.
        return true;
    });

// Route with validation
http.addPath(NisHttp::Method::POST, "/data",
    [](NisHttp::Request const& req, NisHttp::Response& res) {
        // action
        return true;
    },
    [](NisHttp::Request const& req) {
        // validation -- return false to reject with 400
        return req.headers().hasHeader("Content-Type");
    });

// Remove a route
http.remPath(NisHttp::Method::GET, "/user/{id}/profile");
```

### Path Pattern Syntax

- Literal segments: `/api/v1/users`
- Capture groups: `/user/{id}` -- matches any characters except `/`
- Mixed: `/content/{file}.html`
- Multiple captures: `/person/{first}/{last}`

Captured values are URL-decoded and available via `req.variables()["name"]`.

### Handler Signature

```cpp
using HTTPAction = std::function<bool(Request const& request, Response& response)>;
```

Return `true` if handled, `false` to fall through.

---

## Request

Represents a parsed HTTP request.

### Key Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `getMethod()` | `Method` | HTTP method (GET, POST, PUT, etc.) |
| `getUrl()` | `URL const&` | Parsed URL |
| `getVersion()` | `Version` | HTTP version |
| `headers()` | `HeaderRequest const&` | Request headers |
| `variables()` | `RequestVariables&` | Captured path variables, query params, headers, form fields |
| `body()` | `std::istream&` | Request body stream |
| `isValidRequest()` | `bool` | Whether request parsed successfully |

### Reading the Request Body

```cpp
// Stream-style
std::string input;
std::getline(req.body(), input);

// Read entire body into memory
std::string_view bodyData = req.preloadStreamIntoBuffer();
```

### Accessing Variables

`HTTPHandler` populates `variables()` with:
- URL path captures (e.g., `{id}`)
- URL query parameters
- HTTP headers (lowercased)
- Form body fields (if `content-type: application/x-www-form-urlencoded`)

```cpp
std::string id    = req.variables()["id"];        // path capture
std::string name  = req.variables()["name"];      // query param: ?name=value
bool exists       = req.variables().exists("id"); // check existence
```

---

## Response

Builds an HTTP response.

### Setting Status

```cpp
response.setStatus(404);  // must be called before headers are sent
```

### Sending Headers

```cpp
NisHttp::HeaderResponse headers;
headers.add("X-Custom", "value");
response.addHeaders(headers);
```

### Sending a Body

```cpp
// Fixed content-length
std::string body = "Hello World";
response.body(body.size()) << body;

// Chunked transfer encoding
response.body(NisHttp::Encoding::Chunked) << "Hello " << "World";
```

### Error Responses

```cpp
response.error(404, "Not Found");
```

If neither `addHeaders()` nor `body()` is called before the response is destroyed, a minimal response with `content-length: 0` is sent automatically.

---

## URL

Parsed URL with component accessors:

```
http://localhost:53/status?name=ryan#234
```

| Accessor | Example Return |
|----------|----------------|
| `protocol()` | `http:` |
| `hostname()` | `localhost` |
| `port()` | `53` |
| `pathname()` | `/status` |
| `query()` | `?name=ryan` |
| `param("name")` | `ryan` |

---

## PyntHTTP (Custom HTTP Handler)

For more control than `HTTPHandler`, subclass `PyntHTTP` directly:

```cpp
class MyHTTP : public NisHttp::PyntHTTP {
    void processRequest(NisHttp::Request& request, NisHttp::Response& response) override {
        // Full control over request/response
        std::string body = "Custom handler";
        response.body(body.size()) << body;
    }
};
```

---

## Client-Side HTTP

Make outbound HTTP requests from within a handler:

```cpp
// Open a TLS connection
NisHttp::ClientStream upstream("https://api.example.com");

// Send a request
NisHttp::ClientRequest req(upstream, "/api/data", NisHttp::Method::GET);
req.flushRequest();

// Read the response
NisHttp::ClientResponse resp(upstream);
std::cout << "Status: " << resp.getContentSize() << "\n";
```

---

## PyntHTTPControl

HTTP-based server control endpoint. Responds to query parameter `command`:

| Command | Action |
|---------|--------|
| `stophard` | Immediate shutdown |
| `stopsoft` | Graceful shutdown |
| `ping` | Health check (no-op) |

```cpp
NisHttp::PyntHTTPControl control(server);
server.listen(TASock::ServerInfo{9090}, control);
// Usage: GET /control?command=stopsoft
```

---

## Enumerations

```cpp
enum class Method  { GET, HEAD, OPTIONS, TRACE, PUT, DELETER, POST, PATCH, CONNECT, Other };
enum class Version { HTTP1_0, HTTP1_1, HTTP2, HTTP3, Unknown };
enum class Encoding { Chunked };
```

---

## Complete Example: Static File Server

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseServer/PyntControl.h"
#include "NisseServer/Context.h"
#include "NisseHTTP/HTTPHandler.h"
#include <ThorsSocket/SocketStream.h>
#include <filesystem>

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
namespace TASock    = ThorsAnvil::ThorsSocket;
namespace FS        = std::filesystem;

class WebServer : public NisServer::NisseServer
{
    NisHttp::HTTPHandler    http;
    NisServer::PyntControl  control;
    FS::path                contentDir;

public:
    WebServer(int port, FS::path const& contentDir)
        : control(*this), contentDir(contentDir)
    {
        http.addPath(NisHttp::Method::GET, "/{path}",
            [&](NisHttp::Request const& req, NisHttp::Response& res) {
                FS::path filePath = FS::canonical(contentDir / req.variables()["path"]);

                // Serve file using async I/O
                TASock::SocketStream file{TASock::Socket{
                    TASock::FileInfo{filePath.string(), TASock::FileMode::Read},
                    TASock::Blocking::No}};
                NisServer::AsyncStream async(file, req.getContext(), NisServer::EventType::Read);

                res.body(NisHttp::Encoding::Chunked) << file.rdbuf();
                return true;
            });

        listen(TASock::ServerInfo{port}, http);
        listen(TASock::ServerInfo{port + 2}, control);
    }
};

int main()
{
    WebServer server(8080, "./content");
    server.run();
}
```

## Complete Example: Reverse Proxy

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseServer/Context.h"
#include "NisseHTTP/HTTPHandler.h"
#include "NisseHTTP/HeaderPassThrough.h"
#include <ThorsSocket/SocketStream.h>

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
namespace TASock    = ThorsAnvil::ThorsSocket;

class ReverseProxy : public NisServer::NisseServer
{
    NisHttp::HTTPHandler http;
    std::string          dest;
    int                  destPort;

public:
    ReverseProxy(int port, std::string dest, int destPort)
        : dest(std::move(dest)), destPort(destPort)
    {
        http.addPath(NisHttp::All::Method, "/{command}",
            [&](NisHttp::Request const& req, NisHttp::Response& res) {
                // Connect to upstream
                TASock::SocketStream upstream{TASock::Socket{
                    TASock::SocketInfo{dest, destPort}, TASock::Blocking::No}};
                NisServer::AsyncStream async(upstream, req.getContext(),
                                             NisServer::EventType::Write);

                // Forward request
                upstream << req << req.body().rdbuf() << std::flush;

                // Read and forward response
                upstream >> res;
                NisHttp::HeaderPassThrough headers(upstream);
                NisHttp::StreamInput body(upstream, headers.getEncoding());
                res.addHeaders(headers);
                res.body(headers.getEncoding()) << body.rdbuf();
                return true;
            });

        listen(TASock::ServerInfo{port}, http);
    }
};
```
