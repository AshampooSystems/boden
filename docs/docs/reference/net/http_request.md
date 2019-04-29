path: tree/master/framework/net/include/bdn/
source: HTTPRequest.h

# HTTPRequest

Class used with [http::request](http.md)

## Declaration

```C++
class HTTPRequest
```

## Handler

* **using DoneHandler = std::function<void(std::shared_ptr<HTTPResponse\>)\>**

	Called once the request is done.

## Constructor

* **HTTPRequest([String](../foundation/string.md) requestUrl, DoneHandler requestDoneHandler)**

	Creates a HTTP GET Request

* **HTTPRequest([http::Method](http.md) requestMethod, [String](../foundation/string.md) requestUrl, DoneHandler requestDoneHandler)**

	Creates a HTTP Request with the specified `requestMethod`

