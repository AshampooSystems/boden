path: tree/master/framework/net/include/bdn/net
source: HTTPRequest.h

# HTTPRequest

Represents an HTTP(S) request.

Use the [`http::request()`](http.md) method to perform an HTTP request.

## Declaration

```C++
namespace bdn::net {
	class HTTPRequest
}
```

## Handler

* **using DoneHandler = std::function<void(std::shared_ptr<HTTPResponse\>)\>**

	Called once a response has been received or an error occurred.

## Constructor

* **HTTPRequest(std::string requestUrl, DoneHandler requestDoneHandler)**

	Creates a HTTP `GET` request with the given request URL and done handler.

* **HTTPRequest([http::Method](http.md) requestMethod, std::string requestUrl, DoneHandler requestDoneHandler)**

	Creates a HTTP request with the specified request method.

