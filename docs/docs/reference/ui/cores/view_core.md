# View::Core

Interface representing a platform specific "view core". Classes inheriting from `ViewCore` implement the actual user interface view logic to display user interfaces on screen.

## Declaration

```C++
class View {
	class Core
}
```

## Properties

* **Property<Rect\> geometry**

* **Property<bool\> visible**

## Functions

* **virtual void init() = 0**

	Subclasses must override this method to implement initialization of platform-specific objects.

* **virtual Size sizeForSpace(Size availableSize = Size::none()) const**

	Returns the fitting size for the view <span style="color: red">in device independent pixels?</span> (DOCFIXME) given the available size. Setting `availableSize` to `Size::none()` indicates that infinite space is available to size the view.

* **virtual bool canMoveToParentView(std::shared_ptr<View\> newParentView) const = 0**

	Returns whether the view core can move to another parent view. The default implementation returns `true`.

* **virtual void dispose() = 0**

	Called when the view core should be disposed.

* **virtual void scheduleLayout() = 0**

* **void startLayout()**

* **void markDirty()**

* **virtual void setLayout(std::shared_ptr<Layout\> layout)**

