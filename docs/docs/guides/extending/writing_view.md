# Writing a New View

## Introduction

This guide will use the [`Button`](../../reference/ui/button.md) class implementation as an example to demonstrate the steps needed to extend the Boden Framework with new view components.

In contrast to [Writing a View Module](writing_view_modules.md), this guide will explain how to extend the Boden Framework itself with a new integrated view component rather than adding an optional component which can be shipped independently. Please refer to [Writing a View Module](writing_view_modules.md) if you want to create a standalone view component.

## The View

Boden views consist of two parts:

1. The outward facing platform-independent [`View`](../../reference/ui/view.md) class. This is what users of your view component will usually see and interact with.
2. The internal platform-specific [`View::Core`](../../reference/ui/view_core.md) implementation. This part of the view implementation is usually hidden from users of your new view component.

The first step in writing a new view is to create a new class derived from [`View`](../../reference/ui/view.md).

### Header

Create a new header in `framework/ui/include/bdn` called `ExampleButtonView.h`:

```c++
#pragma once
#include <bdn/ui/View.h>

namespace bdn
{
	class ExampleButtonView : public ui::View
	{
	  public:
		ExampleButtonView(
			std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
	};
}
```

### Source

In `boden/framework/ui/src`, create a file called `ExampleButtonView.cpp`:

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

## The View Core

Create a new `Core` interface as an inner class of `ExampleButtonView`:

```c++
#pragma once
namespace bdn
{
	class ExampleButtonView : public ui::View
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
		class ExampleButtonView : public ui::View
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

## The iOS Implementation

### Header

The core's actual implementation is platform-specific. Create a new Objective-C++ header in `framework/ui/platforms/ios/include/bdn/ios/` called `ExampleButtonViewCore.hh`:

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

Create a new Objective-C++ source file in `framework/ui/platforms/ios/src` called `ExampleButtonViewCore.mm`:

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
	For more information about the intricacies of implementing view cores please refer to [ViewCore](../../reference/ui/view_core.md) and [iOS ViewCore](../../reference/ui/ios/view_core.md).

## Connecting the View Core to the View

Boden uses the [`ViewCoreFactory`](../../reference/ui/view_core_factory.md) to create instances of the view core. To create the connection between a view and its core, add the following boilerplate to the classes:

### View Header

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
	#include <bdn/ui/View.h>
	#include <bdn/ViewUtilities.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public ui::View
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

### View Implementation

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

### View Core Implementation

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

To represent properties of a view, use the [`Property`](../../reference/foundation/property.md) class.

As an example, add a `label` property so users of your view can set the button's label.

```c++
class ExampleButtonView : public ui::View
{
public:
	Property<std::string> label;
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="12 13"
	#pragma once
	#include <bdn/ui/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public ui::View
		{
		  public:
			Property<std::string> label;

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

To connect the property to the core, replicate the property in the `Core` interface:

```c++
class Core
{
public:
	Property<std::string> label; 
}
```

??? note "ExampleButtonView.h"
	```c++ hl_lines="22 23"
	#pragma once
	#include <bdn/ui/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public ui::View
		{
		  public:
			Property<std::string> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

		  public:
			class Core
			{
			  public:
				Property<std::string> label;
			};
		};
	}
	```

To bind the two label properties in `View` and `Core` properties, override the [`View::bindViewCore()`](../../reference/ui/view.md#view-core) function:

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


Now it is easy to react to changes to `label` in the iOS Implementation by adding an `onChange()` handler in the view core's init function. 

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

## Sending Notifications to the View

To allow users of your new class to react to button clicks, you can use a combination of a [Notifier](../../reference/foundation/notifier.md) and a `WeakCallback`.

First, add the `WeakCallback` to your Core:

```C++
class ExampleButtonView : public ui::View
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
	#include <bdn/ui/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public ui::View
		{
		  public:
			Property<std::string> label;

		  public:
			ExampleButtonView(
				std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

		  public:
			class Core
			{
			  public:
				Property<std::string> label;

		      public:
		        WeakCallback<void()> _clickCallback;
			};
		};
	}
	```

Now you can fire the callback from your `UIButton` subclass:

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

Add the notifier and the callback receiver to your `View` class:

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
	#include <bdn/ui/View.h>

	namespace bdn
	{
		namespace detail
		{
			VIEW_CORE_REGISTRY_DECLARATION(ExampleButtonView)
		}
		class ExampleButtonView : public ui::View
		{
		  public:
			Property<std::string> label;

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
				Property<std::string> label;

		      public:
		        WeakCallback<void()> _clickCallback;
			};
		};
	}
	```

Finally, connect it to the `WeakCallback`:

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

Congratulations, your first custom view should be working now!