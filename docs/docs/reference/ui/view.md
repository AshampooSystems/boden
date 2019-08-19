path: tree/master/framework/ui/include/bdn/ui
source: View.h

# View

Abstract base class for all user interface elements.

## Declaration

```C++
namespace bdn::ui {
	class View : public std::enable_shared_from_this<View>
}
```

## Properties

* **Property<Rect\> geometry**

	Current geometry of the view on screen.

* **Property<bool\> visible = true**

	Whether the view is visible.

* **Property<std::shared_ptr<json\>\> stylesheet**

	A stylesheet defining the styling and layout of the view. 

* **Property<bool\> isLayoutRoot = false**

	Whether the view is treated as a layout root. If set to true, the view's geometry will not be changed by the layout system. Children are not affected by this setting and will still be processed by the layout system. Defaults to `false`.

* **const Property<std::weak_ptr<View\>\> &parentView**

	*(read-only)* The current parent view.

## Stylesheet

* **{ "background-color" : [Color](../foundation/color.md) }**

	Controls the background color of the view.

## Layout

* **virtual Size sizeForSpace(Size availableSpace = Size::none()) const**

	Calculates the view's [`Size`](../foundation/size.md) for the given space.

* **std::shared_ptr<Layout\> getLayout()**

	Returns the view's [`Layout`](layout.md).

* **void setLayout(std::shared_ptr<Layout\> layout)**

	Sets the view's [`Layout`](layout.md).

* **void setFallbackLayout(std::shared_ptr<Layout\> layout)**

	Sets the view's [`Layout`](layout.md) fallback.	A view that has no layout set (via `setLayout`) will use the given `layout`. 

## View Core

* **std::shared_ptr<ViewCore\> viewCore() const**

	Returns the view's [`ViewCore`](view_core.md).

* **std::shared_ptr<ViewCoreFactory\> viewCoreFactory()**

	Returns the view's [`ViewCoreFactory`](view_core_factory.md).

* **virtual void bindViewCore()**

	Called after a new [`ViewCore`](view_core.md) is created to connect it to the view's properties and notifiers

* **virtual const std::type_info &typeInfoForCoreCreation() const**

	Returns the **std::type_info** that should be used to determine the [`ViewCore`](view_core.md) type.
	The default implementation returns **typeid(\*this)**. Override if you want to use a different [`ViewCore`](view_core.md).

## View Hierarchy

* **virtual std::vector<std::shared_ptr<View\>\> childViews() const**

	Returns the view's child views.

## Misc

* **std::shared_ptr<View\> shared_from_this()**

	Returns an `std::shared_ptr` referencing the view.

## Relationships

Inherits from `std::enable_shared_from_this<View>`.
