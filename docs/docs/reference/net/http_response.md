path: tree/master/framework/net/include/bdn/
source: HTTPResponse.h

# HTTPResponse

Result class of a [http::request](http.md)

## Declaration

```C++
class HTTPResponse
```

## Fields

* **[HTTPRequest](./http_request) originalRequest**
	
	The original request

* **[String](../foundation/string.md) url**

	The actual url ( might be different from the request in cases of redirects etc. )

* **[String](../foundation/string.md) header**

	The HTTP header returned by the server

* **[String](../foundation/string.md) data**

	The data returned by the server

* **int responseCode**
	
	The HTTP reponse code returned by the server