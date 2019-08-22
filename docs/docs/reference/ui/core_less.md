path: tree/master/framework/ui/include/bdn/ui
source: CoreLess.h

# CoreLess

Helper class allowing you to easily create your own platform-independent composite views without having to create a specialized [`ViewCore`](view_core.md).

## Declaration

```C++
namespace bdn::ui {
	template<class BASE>
	class CoreLess : public BASE
}
```

## Example

```C++
#include <bdn/ui/CoreLess.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>

using namespace bdn::ui;

class MyCustomView : public CoreLess<ContainerView>
{
public:
	using CoreLess<ContainerView>::CoreLess;

	void init() override {
		_firstLabel = std::make_shared<Label>();
		_firstLabel->text = "Hello";

		_secondLabel = std::make_shared<Label>();
		_secondLabel->text = "World";

		addChildView(_firstLabel);
		addChildView(_secondLabel);
	}

private:
	std::shared_ptr<Label> _firstLabel;
	std::shared_ptr<Label> _secondLabel;
};

void test() {
	auto myCustomView = std::make_shared<MyCustomView>(needsInit);
	// ...
}
```

## Constuctor

* **explicit CoreLess(bdn::NeedsInit, std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr)**

	Expects a [`bdn::NeedsInit`](../foundation/needs_init.md) so that [`make_shared`](../foundation/needs_init.md) is used which will automatically call the `init()` function.  

## Virtual methods

* **virtual void init() = 0**

	Override the init function to create children. This is needed because
	[ContainerView::addChildView()](container_view.md#managing-child-views) uses shared_from_this() 
	which is not available during construction.
