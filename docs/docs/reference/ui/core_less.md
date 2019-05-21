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
	void init() {
		_firstLabel = std::make_shared<Label>();
		_firstLabel->text = "Hello";
		_secondLabel->text = "World";

		addChildView(_firstLabel);
		addChildView(_secondLabel);
	}

private:
	Label _firstLabel;
	Label _secondLabel;
};
```