path: tree/master/framework/foundation/include/bdn/
source: Application.h

# Application

Represents the Application.

To define the setup and behavior of your own app, inherit from [ApplicationController](application_controller.md).

The application's entry point is defined by the `BDN_APP_INIT` macro. See [ApplicationController](application_controller.md) for an example.

## Declaration

```C++
namespace bdn {
	class Application 
		: public std::enable_shared_from_this<Application>
}
```

## Obtaining the Global Application Instance

* **static std::shared_ptr<Application\> globalApplication()**

	Returns the global `Application` instance for the current process.

## Registering an Application Controller

* **BDN_APP_INIT(ApplicationControllerClassName)**

	Use this macro to register your main application controller.

## Properties

* **Property<std::vector<String\>\> commandLineArguments**

	A list of command-line arguments as specified when the application was launched.

* **std::shared_ptr<[DispatchQueue](dispatch_queue.md)\> dispatchQueue()**

	The main thread's [`DispatchQueue`](dispatch_queue.md).

* **std::shared_ptr<[ApplicationController](application_controller.md)\> applicationController()**

	The user-defined application controller associated with the application.

## Commandline arguments

* **int argc()**

	Returns the number of command-line arguments.

* **char \*\*argv()**

	Returns the raw command-line arguments.

## Main thread

* **static std::thread::id mainThreadId()**

	Returns the thread-id of the main thread ( The thread that created the Application )

* **static bool isMainThread()**

	Returns true if the current thread is the main thread

* **static void assertInMainThread()**

	Asserts that the current thread is the main thread

## OS Services

* **virtual void openURL(const String &url)**

	Opens the given URL in a suitable external application. Web URLs will be opened in the system's standard web browser. Application-specific URLs will open in the respective application.

* **virtual void copyToClipboard(const String &str)**

	Copies the given string to the clipboard.

## Resources

* **virtual String uriToBundledFileUri(const String &uri)**

	Returns a platform specific representation of a resource path (see: [Resources](../../guides/fundamentals/resources.md)). If the path cannot be converted, the `uri` parameter is returned.

## Shutdown

* **void initiateExitIfPossible(int exitCode)** 

	Tries to shutdown the application with the given exit code.

	Does nothing on Android.