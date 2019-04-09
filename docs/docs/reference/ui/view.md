path: tree/master/framework/ui/include/bdn/
source: View.h

# View

Abstract base class for all user interface elements.

## Declaration

```C++
class View : public Base
```

## Properties

* **Property<Rect\> geometry**

	Current geometry of the view on screen.

* **Property<bool\> visible = true**

	Whether the view is visible.

* **Property<std::shared_ptr<LayoutStylesheet\>\> layoutStylesheet**

	A [`LayoutStylesheet`](../layout/layout_stylesheet.md) defining the layout of the view.

* **Property<bool\> isLayoutRoot = false**

	If true, this views geometry will not be changed by the layouting system. 
	Children will still be layouted, as if this view is the root in the layout hierarchy.

## Layout

* **virtual Size sizeForSpace(Size availableSpace = Size::none()) const**

	Calculates the view's [`Size`](../foundation/size.md) for the given space.

* **std::shared_ptr<Layout\> getLayout()**

	Returns the view's [`Layout`](../layout/layout.md).

* **void setLayout(std::shared_ptr<Layout\> layout)**

	Sets the view's [`Layout`](../layout/layout.md).

* **template <class T\> void setLayoutStylesheet(T sheet)**

	Convenience method for setting the layout stylesheet using a type `T` derived from [`LayoutStylesheet`](../layout/layout_stylesheet.md) without having to use `std::make_shared`.

## View Core

* **virtual String viewCoreTypeName() const = 0**

	Super classes should return a [`String`](../foundation/string.md) presenting the type name of the view's core.

* **std::shared_ptr<ViewCore\> viewCore() const**

	Returns the view's [`ViewCore`](cores/view_core.md).

* **sstd::shared_ptr<ViewCoreFactory\> viewCoreFactory()**

	Returns the view's [`ViewCoreFactory`](view_core_factory.md).

* **virtual void bindViewCore()**

	Called after a new View Core is created to connect it to the Views properties and notifiers

* **virtual const std::type_info &typeInfoForCoreCreation() const**

	Returns the **std::type_info** that should be used to determine the ViewCore type.
	The default implementation returns **typeid(\*this)**. Override if you want to 
	use a different ViewCore.

## View Hierarchy

* **virtual std::list<std::shared_ptr<View\>\> childViews() const**

	Returns the view's child views.

* **virtual void removeAllChildViews()**

	Removes all child views.

* **virtual void \_childViewStolen(std::shared_ptr&lt;View&gt; childView)**

* **virtual std::shared_ptr<View\> getParentView()**

* **void \_setParentView(std::shared_ptr<View\> parentView)**

## Misc

* **std::shared_ptr<View\> shared_from_this()**

	Returns an `std::shared_ptr` referencing the view.

## Relationships

Inherits from [`Base`](../foundation/base.md).
