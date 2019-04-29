path: tree/master/framework/net/include/bdn/
source: HTTP.h

# HTTP

A simple way to fetch data via HTTP.

!!! note
	Mobile platforms by default often disallow internet access, and especially non-https requests. 
	Make sure to specify `configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)` in your Apps CMakeLists.txt

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

* **std::shared_ptr<HTTPResponse\> request(HTTPRequest request)**
	
	Starts the request. The response will arrive asynchronously.

