# View::Core

Interface representing a platform specific "view core". Classes inheriting from `ViewCore` implement the actual user interface logic to display user interfaces on screen.

## Declaration

```C++
class View {
	class Core
}
```

## Properties

* **[Property](../foundation/property.md)<Rect\> geometry**

* **[Property](../foundation/property.md)<bool\> visible**

## Functions

* **virtual void init() = 0**

	Deriving classes must override this method to implement initialization of platform-specific objects.

* **virtual [Size](../foundation/size.md) sizeForSpace([Size](../foundation/size.md) availableSize = [Size](../foundation/size.md)::none()) const**

	Returns the fitting size for the view given the available size. Setting `availableSize` to `Size::none()` indicates that infinite space is available to the view.

* **virtual bool canMoveToParentView(std::shared_ptr<[View](view.md)\> newParentView) const = 0**

	Returns whether the view core can move to another parent view.

* **virtual void dispose() = 0**

	Called when the view core should be disposed.

* **virtual void scheduleLayout() = 0**

* **void startLayout()**

* **void markDirty()**

* **virtual void setLayout(std::shared_ptr<[Layout](../layout/layout.md)\> layout)**

