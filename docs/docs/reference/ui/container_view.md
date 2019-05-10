path: tree/master/framework/ui/include/bdn/ui
source: ContainerView.h

# ContainerView

A view that can hold child views. 

## Declaration

```C++
namespace bdn::ui {
	class ContainerView : public View
}
```

## Example

```C++
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/TextView.h>
#include <bdn/ui/Button.h>
// ...
auto container = std::make_shared<Container>();

auto label = std::make_shared<TextView>();
auto button = std::make_shared<Button>();

container->addChildView(label);
container->addChildView(button);
```

## Managing Child Views

* **void void addChildView(const std::shared_ptr<[View](view.md)\> &childView)**

	Adds a new child view to the container view.

* **void void removeChildView(const std::shared_ptr<[View](view.md)\> &childView)**

	Remove a the given child view from the container view.

* **void removeAllChildViews()**

	Removes all child views from the container view.

## Accessing children

* **void std::list<std::shared_ptr<[View](view.md)\>\> childViews() override**

	Returns an `std::list` containg all child views of the container view.

## Relationships

Inherits from: [View](view.md)
 