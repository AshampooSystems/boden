# ContainerView

A content-less view that can hold child views. 

## Declaration

```C++
class ContainerView : public View
```

## Example

```C++
#include <bdn/ContainerView.h>
#include <bdn/TextView.h>
#include <bdn/Button.h>
// ...
auto container = std::make_shared<Container>();

auto label = std::make_shared<TextView>();
auto button = std::make_shared<Button>();

container->addChildView(label);
container->addChildView(button);
```

## Actions

* **void void addChildView(const std::shared_ptr<View\> &childView)**

	Add a new child view

* **void void removeChildView(const std::shared_ptr<View\> &childView)**

	Remove a specific child view

* **void removeAllChildViews()**

	Removes all child views

## Accessing children

* **void std::list<std::shared_ptr<View>> childViews() override**

	Returns all child views

## Relationships

Inherits from: [View](view.md)
 