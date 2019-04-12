# Writing a new View

## Introduction

Boden views consist of two parts:

1. The outward facing View class
2. The inner platform specific ViewCore implementation

This guide will use the Button class implementation to highlight the specific steps needed to implement new Views. 

## The View

The first step in writing new Views is to create a new class derived from [View](../../reference/ui/view.md).

### Header

Lets create a new header in `framework/ui/include/bdn` called `ExampleButtonView.h`:

```c++
#pragma once
#include <bdn/View.h>

namespace bdn
{
	class ExampleButtonView : public View
	{
	  public:
		ExampleButtonView(
			std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
	};
}
```

### Source

In `boden/framework/ui/src` we create a file called `ExampleButtonView.cpp`:

```c++
#include <bdn/ExampleButtonView.h>

namespace bdn 
{
	ExampleButtonView::ExampleButtonView(
		std::shared_ptr<ViewCoreFactory> viewCoreFactory)
	 : View(viewCoreFactory)
	{
	} 
}
```

## The Core

Create a new Core Interface:

```c++
#pragma once
namespace bdn
{
	class ExampleButtonView : public View
	{
		// ...
	public:
		class Core
		{
		};
	};
}
```

??? note "Full"
	```c++ hl_lines="10 11 12"
	#pragma once
	namespace bdn
	{
		class ExampleButtonView : public View
		{
		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
		  public:
			class Core
			{
			};
		};
	}
	```

## The IOS Implementation

### Header

The actual Core implementation is platform specific. Create a new *objective-c* header in `framework/ui/platforms/ios/include/bdn/ios/` called `ExampleButtonViewCore.hh`:

```c++
#pragma once

#include <bdn/ExampleButtonView.h>
#include <bdn/ExampleButtonViewCore.h>
#import <bdn/ios/ViewCore.hh>

namespace bdn::ios
{
	class ExampleButtonViewCore : public ViewCore, 
								  virtual public bdn::ExampleButtonView::Core
	{
	  public:
		ExampleButtonViewCore(
			const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
	};
}
```

### Source

Create a new *objective-c* source file in `framework/ui/platforms/ios/src` called `ExampleButtonViewCore.mm`:

```obj-c
#import <bdn/ios/ExampleButtonViewCore.hh>

@interface ExampleUIButton : UIButton <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ExampleButtonViewCore> core;
@end

@implementation ExampleUIButton
- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore
{
	self.core = std::dynamic_pointer_cast<bdn::ios::ExampleButtonViewCore>(viewCore.lock());
}
- (void)setFrame:(CGRect)frame
{
	[super setFrame:frame];
	if(auto core = self.core.lock()) {
		core->frameChanged();
	}
}

namespace bdn::ios
{
	ExampleButtonViewCore::ExampleButtonViewCore(const std::shared_ptr<ViewCoreFactory>& viewCoreFactory)
	: ViewCore(viewCoreFactory, [ExampleUIButton buttonWithType:UIButtonTypeSystem])
	{

	}
}
```

!!! note ViewCores
	For more information about the intricacies of implementing ViewCores please see [ViewCore](../../reference/ui/view_core.md) and [IOS ViewCore](../../reference/ui/ios/view_core.md)

## Connecting the ViewCore to the View

Boden uses the [ViewCoreFactory](../../reference/ui/view_core_factory.md) to create instances of the ViewCore. To create the connection between a View and its ViewCore we have to add some boilerplate to the classes.

### View header

Add the following code to `ExampleButtonView.h`:

```c++
#include <bdn/ViewUtilities.h>

namespace bdn::detail
{
	VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="3 7 8 9 10"
	#pragma once
	#include <bdn/View.h>
	#include <bdn/ViewUtilities.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public View
		{
		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
		  public:
			class Core
			{
			};
		};
	}
	```

### View implementation

Add the following code to `ExampleButtonView.cpp`:

```c++
namespace bdn::detail
{
	VIEW_CORE_REGISTRY_IMPLEMENTATION(ExampleButtonView)
}

ExampleButtonView::ExampleButtonView(
	std::shared_ptr<ViewCoreFactory> viewCoreFactory)
 : View(viewCoreFactory)
{
	bdn::detail::VIEW_CORE_REGISTER(ExampleButtonView, View::viewCoreFactory());
}
```

??? note "ExampleButtonView.cpp"
	```c++ hl_lines="5 6 7 8 14"
	#include <bdn/ExampleButtonView.h>

	namespace bdn 
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_IMPLEMENTATION(ExampleButtonView)
		}

		ExampleButtonView::ExampleButtonView(
			std::shared_ptr<ViewCoreFactory> viewCoreFactory)
		 : View(viewCoreFactory)
		{
			bdn::detail::VIEW_CORE_REGISTER(ExampleButtonView, View::viewCoreFactory());
		} 
	}
	```

### ViewCore implementation

Add the following code to `ExampleButtonViewCore.mm`:

```c++
namespace bdn::detail
{
	CORE_REGISTER(ExampleButtonView, 
				  bdn::ios::ExampleButtonViewCore, 
				  ExampleButtonView)
}
```

??? note "ExampleButtonViewCore.mm"
	```obj-c hl_lines="21 22 23 24"
	#import <bdn/ios/ExampleButtonViewCore.hh>
	
	@interface ExampleUIButton : UIButton <UIViewWithFrameNotification>
	@property(nonatomic, assign) std::weak_ptr<bdn::ios::ExampleButtonViewCore> core;
	@end
	
	@implementation ExampleUIButton
	- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore
	{
		self.core = std::dynamic_pointer_cast<bdn::ios::ExampleButtonViewCore>(viewCore.lock());
	}
	- (void)setFrame:(CGRect)frame
	{
		[super setFrame:frame];
		if (auto core = self.core.lock()) {
			core->frameChanged();
		}
	}
	@end

	namespace bdn::detail
	{
		CORE_REGISTER(ExampleButtonView, bdn::ios::ExampleButtonViewCore, ExampleButtonView)
	}

	namespace bdn::ios
	{
		ExampleButtonViewCore::ExampleButtonViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
			: ViewCore(
				viewCoreFactory, 
				[ExampleUIButton buttonWithType:UIButtonTypeSystem])
		{
		}
		ExampleButtonViewCore::~ExampleButtonViewCore()
		{
		}
	}
	```

## Adding Properties

For a simple property of a View boden uses the class [Property<T\>](../../reference/foundation/property.md).

In this example we add a "label" property to allow the user to change the text displayed on the Button.

```c++
class ExampleButtonView : public View
{
public:
	Property<String> label;
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="12 13"
	#pragma once
	#include <bdn/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public View
		{
		  public:
			Property<String> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

		  public:
			class Core
			{
			};
		};
	}
	```

To connect the property to the core we replicate the Property:

```c++
class Core
{
public:
	Property<String> label; 
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="22 23"
	#pragma once
	#include <bdn/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public View
		{
		  public:
			Property<String> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

		  public:
			class Core
			{
			  public:
				Property<String> label;
			};
		};
	}
	```

To bind View and Core properties we override the [View::bindViewCore()](../../reference/ui/view.md#view-core) function:

```c++
void ExampleButtonView::bindViewCore()
{
	View::bindViewCore();
	auto buttonCore = core<ExampleButtonView::Core>();
	buttonCore->label.bind(label);
}
```

??? note "ExampleButtonView.cpp"
	```c++ hl_lines="17 18 19 20 21 22"
	#include <bdn/ExampleButtonView.h>

	namespace bdn 
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_IMPLEMENTATION(ExampleButtonView)
		}

		ExampleButtonView::ExampleButtonView(
			std::shared_ptr<ViewCoreFactory> viewCoreFactory)
		 : View(viewCoreFactory)
		{
			bdn::detail::VIEW_CORE_REGISTER(ExampleButtonView, View::viewCoreFactory());			
		}

		void ExampleButtonView::bindViewCore()
		{
			View::bindViewCore();
			auto buttonCore = core<ExampleButtonView::Core>();
			buttonCore->label.bind(label);
		}
	}
	```


Now we can react to changes in our IOS Implementation by adding an onChange handler in its init function. 

```c++
namespace bdn::ios {
	void ExampleButtonViewCore::init()
	{
		ViewCore::init();
		label.onChange() += [=](auto newValue) {
			auto uiButton = (UIButton*)uiView();
			[uiButton setTitle:bdn::fk::stringToNSString(value) forState:UIControlStateNormal]];
		};
	}
}
```

??? note "ExampleButtonViewCore.mm"
	```obj-c hl_lines="2 38 39 40 41 42 43 44 45"
	#import <bdn/ios/ExampleButtonViewCore.hh>
	#import <bdn/foundationkit/stringUtil.hh>

	@interface ExampleUIButton : UIButton <UIViewWithFrameNotification>
	@property(nonatomic, assign) std::weak_ptr<bdn::ios::ExampleButtonViewCore> core;
	@end
	
	@implementation ExampleUIButton
	- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore
	{
		self.core = std::dynamic_pointer_cast<bdn::ios::ExampleButtonViewCore>(viewCore.lock());
	}
	- (void)setFrame:(CGRect)frame
	{
		[super setFrame:frame];
		if (auto core = self.core.lock()) {
			core->frameChanged();
		}
	}
	@end

	namespace bdn::detail
	{
		CORE_REGISTER(ExampleButtonView, bdn::ios::ExampleButtonViewCore, ExampleButtonView)
	}

	namespace bdn::ios
	{
		ExampleButtonViewCore::ExampleButtonViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
			: ViewCore(
				viewCoreFactory, 
				[ExampleUIButton buttonWithType:UIButtonTypeSystem])
		{
		}
		ExampleButtonViewCore::~ExampleButtonViewCore()
		{
		}
		void ExampleButtonViewCore::init()
		{
			ViewCore::init();
			label.onChange() += [=](auto newValue) {
				auto uiButton = (UIButton*)uiView();
				[uiButton setTitle:bdn::fk::stringToNSString(value) forState:UIControlStateNormal]];
			};
		}
	}
	```

## Sending notifications to the View

To allow the user to react the Button clicks we need a way to call functions of the View from the Core.
For this we can use a combination of a [Notifier](../../reference/foundation/notifier.md) and a [WeakCallback](../../reference/foundation/weak_callback.md).

First we add the [WeakCallback](../../reference/foundation/weak_callback.md) to our Core:

```C++
class ExampleButtonView : public View
{
public:
	class Core
	{
      public:
        WeakCallback<void()> _clickCallback;
	}
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="25 26"
	#pragma once
	#include <bdn/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public View
		{
		  public:
			Property<String> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

		  public:
			class Core
			{
			  public:
				Property<String> label;

		      public:
		        WeakCallback<void()> _clickCallback;
			};
		};
	}
	```

Now we can fire the callback from our UIButton derived class:

```obj-c
- (void)clicked
{
    if (auto core = self.core.lock()) {
        core->_clickCallback.fire();
    }
}
```

??? note "ExampleButtonViewCore.mm"
	```obj-c hl_lines="19 20 21 22 23 24 25"
	#import <bdn/ios/ExampleButtonViewCore.hh>
	#import <bdn/foundationkit/stringUtil.hh>
	@interface ExampleUIButton : UIButton <UIViewWithFrameNotification>
	@property(nonatomic, assign) std::weak_ptr<bdn::ios::ExampleButtonViewCore> core;
	@end
	
	@implementation ExampleUIButton
	- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore
	{
		self.core = std::dynamic_pointer_cast<bdn::ios::ExampleButtonViewCore>(viewCore.lock());
	}
	- (void)setFrame:(CGRect)frame
	{
		[super setFrame:frame];
		if (auto core = self.core.lock()) {
			core->frameChanged();
		}
	}
	- (void)clicked
	{
		if (auto core = self.core.lock()) {
			core->_clickCallback.fire();
		}
	}
	@end

	namespace bdn::detail
	{
		CORE_REGISTER(ExampleButtonView, bdn::ios::ExampleButtonViewCore, ExampleButtonView)
	}

	namespace bdn::ios
	{
		ExampleButtonViewCore::ExampleButtonViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
			: ViewCore(
				viewCoreFactory, 
				[ExampleUIButton buttonWithType:UIButtonTypeSystem])
		{
		}
		ExampleButtonViewCore::~ExampleButtonViewCore()
		{
		}
		void ExampleButtonViewCore::init()
		{
			ViewCore::init();
			label.onChange() += [=](auto newValue) {
				auto uiButton = (UIButton*)uiView();
				[uiButton setTitle:bdn::fk::stringToNSString(value) forState:UIControlStateNormal]];
			};
		}
	}
	```

We still have to add the notifier and the callback receiver to our View class:

```c++
class ExampleButtonView
{
  public:
	Notifier<const ClickEvent &> _onClick;
	WeakCallback<void()>::Receiver _clickCallbackReceiver;
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="19 20 21"
	#pragma once
	#include <bdn/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public View
		{
		  public:
			Property<String> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
		  
		  public:
			Notifier<const ClickEvent &> _onClick;
			WeakCallback<void()>::Receiver _clickCallbackReceiver;
		  
		  public:
			class Core
			{
			  public:
				Property<String> label;

		      public:
		        WeakCallback<void()> _clickCallback;
			};
		};
	}
	```

And at last we connect it to the [WeakCallback](../../reference/foundation/weak_callback.md):

```c++
void ExampleButtonView::bindViewCore()
{
	_clickCallbackReceiver = buttonCore->_clickCallback.set([=](){
		ClickEvent evt(shared_from_this());
		_onClick.notify(evt);
	});
}
```

??? note "ExampleButtonView.cpp"
	```c++ hl_lines="23 24 25 26"
	#include <bdn/ExampleButtonView.h>

	namespace bdn 
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_IMPLEMENTATION(ExampleButtonView)
		}

		ExampleButtonView::ExampleButtonView(
			std::shared_ptr<ViewCoreFactory> viewCoreFactory)
		 : View(viewCoreFactory)
		{
			bdn::detail::VIEW_CORE_REGISTER(ExampleButtonView, View::viewCoreFactory());			
		}

		void ExampleButtonView::bindViewCore()
		{
			View::bindViewCore();
			auto buttonCore = core<ExampleButtonView::Core>();
			buttonCore->label.bind(label);

			_clickCallbackReceiver = buttonCore->_clickCallback.set([=](){
				ClickEvent evt(shared_from_this());
				_onClick.notify(evt);
			});
		}
	}
	```
