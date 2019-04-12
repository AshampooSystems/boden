path: tree/master/framework/foundation/include/bdn/
source: Application.h

# Application

Represents the Application. Inherit from [ApplicationController](application_controller.md) to set up your application.

The application's entry point is defined by the `BDN_APP_INIT` macro. See [ApplicationController](application_controller.md) for an example.

## Declaration

```C++
class Application 
	: public std::enable_shared_from_this<Application>
```

## Macros

* **BDN_APP_INIT(ClassName)**

	Use this macro to register your main application controller

## Properties

* **Property<std::vector<String\>\> commandLineArguments**

	A list of the command-line arguments

* **std::shared_ptr<[DispatchQueue](dispatch_queue.md)> dispatchQueue()**

	The main thread [DispatchQueue](dispatch_queue.md)

* **std::shared_ptr<ApplicationController> applicationController()**

	The user-supplied application controller

## Commandline arguments

* **int argc()**
* **char \*\*argv()**

	Returns the raw commandline arguments

## Main thread

* **static std::thread::id mainThreadId()**

	Returns the thread-id of the main thread ( The thread that created the Application )

* **static bool isMainThread()**

	Returns true if the current thread is the main thread

* **static void assertInMainThread()**

	Asserts that the current thread is the main thread

## OS Services

* **virtual void openURL(const String &url)**

	Calls the operating system to open the supplied URL.

## Shutdown

* **void initiateExitIfPossible(int exitCode)** 

	Tries to shutdown the application with the supplied exit code.

	This function does nothing on mobile operating systems that do not allow the app to exit on its own.