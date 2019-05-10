path: tree/master/framework/net/include/bdn/
source: HTTP.h

# HTTP

Provides methods and types for working with HTTP(S) requests.

!!! note
	Mobile platforms by default often disallow internet access, and especially non-HTTPS requests. 
	Make sure to specify `configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)` in your app's CMakeLists.txt

## Declaration

```C++
namespace bdn::net::http
{
    enum class Method
    {
        GET,
        POST,
        PUT,
        DELETE,
        HEAD,
        TRACE,
        OPTIONS,
        CONNECT
    };

    std::shared_ptr<HTTPResponse> request(HTTPRequest request);
}
```

## Example

```C++
#include <bdn/HTTP.h>
// ...
void handleResponse(net::HTTPResponse response) {
	json j = json::parse(r->data);
	std::cout << j.dump(1) << std::endl;
}
// ...
net::http::request({
	net::http::Method::GET, 
	"https://www.reddit.com/hot.json", 
	&handleResponse });
```

## Request

* **void request([HTTPRequest](http_request.md) request)**
	
	Performs the given request asynchronously and immediately returns. The request's `DoneHandler` is called on the main thread once a response has been received or an error has occurred.

