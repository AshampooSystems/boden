path: tree/master/framework/foundation/include/bdn/
source: ApplicationController.h

# ApplicationController

A base class for managing the lifecycle of your Boden-based application.

Inherit from this class to create an application controller for your application.

## Declaration

```C++
class ApplicationController 
	: public std::enable_shared_from_this<ApplicationController>
```

## Example

```c++
#include <bdn/Window.h>
#include <bdn/ApplicationController.h>

class MyApplicationController : public bdn::ApplicationController
{
public:
	void beginLaunch() override
	{
	    _window = std::make_shared<bdn::Window>();
	    _window->title = "AwesomeApp";
	    _window->geometry = bdn::Rect{0, 0, 400, 300};

	    std::shared_ptr<bdn::Button> button = std::make_shared<bdn::Button>();
	    button->label = "Hello World";
	    button->geometry = bdn::Rect{150, 129, 100, 22};

	    _window->content = button;
	    _window->visible = true;
	}

private:
	std::shared_ptr<bdn::Window> _window;
};

BDN_APP_INIT(MyApplicationController)
```

## Startup

* **virtual void beginLaunch()**

	This is the first entry point for your application after it is started. You should start creating your UI and other objects here.

	If the operating system supports automatically restoring the state
	of the application UI (for example, on iOS) then the restoration has NOT
	yet happened when this function is called.

* **virtual void finishLaunch()**

	This is called after the Application has finished its launch.

	If the operating system supports automatically restoring the state
	of the app's UI (for example, on iOS) then the restoration has
	finished when this function is called.

## Activity

* **virtual void onActivate()**

	Called once is ready for the User to interact with it. 

	On mobile operating systems this can be called when the User switches back to the application.

* **virtual void onDeactivate()**

	Called when the application is deactivated.

	On mobile operating systems this can be called when the User switches focus to another application

## Sleep

* **virtual void onSuspend()**

	Called when the application is about to be suspended. An application can store the state it needs
	to be reactivated here. 

	This is typically called after onDeactivate 

	This function should return as quickly as possible since many Mobile Operating Systems may terminate 
	it otherwise ( usually 1-5 seconds )

* **virtual void onResume()**

	Called when the application is reactivated after a suspension.

	onActivate() will usually be called shortly after.

## Exit

* **virtual void onTerminate()**

	The app is about to be terminated by the OS. This function will not be called on all Operating Systems.
	Use onSuspend to store application state or User data.
