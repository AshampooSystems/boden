path: tree/master/framework/ui/include/bdn/ui
source: WebView.h

# WebView

A view for displaying web content within your app.

## Declaration

```C++
namespace bdn::ui {
	class WebView : public View
}
```

## Example

```C++
#include <bdn/ui/WebView.h>
// ...
auto webView = std::make_shared<WebView>();
webView->url = "https://www.example.com"
```

## Properties

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> url**
	
	The URL of the web page to be displayed in the web view. Set this property to load a given URL.

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> userAgent**

	(Optional.) The user agent string that will be sent as part of the HTTP request made to load the web view's `url`. If not set, the default system user agent will be used.

* **[Property](../foundation/property.md)<std::function<bool([RedirectRequest](#redirectrequest))\>\> redirectHandler**

	(Optional.) A function for handling redirect requests. If not set, the default redirect handling logic of the native web view implementation will be used.

## Loading a Web Page

* **void loadURL(const [String](../foundation/string.md) &url)**

	Loads the web page with the given URL and sets the `url` property accordingly.

	Calling `loadURL()` is essentially identical to setting the `url` property directly. Once the URL is set, the framework will load the resource asynchronously and display it to the user when it becomes ready.

## Types

### RedirectRequest

Represents a redirect request returned by a HTTP server.

#### Public Members

* [String](../foundation/string.md) url

	The redirect URL requested by the server.


## Relationships

Inherits from [`View`](view.md).

