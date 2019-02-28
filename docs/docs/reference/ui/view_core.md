# ViewCore

Interface representing a platform specific "view core". Classes inheriting from `ViewCore` implement the actual user interface view logic to display user interfaces on screen.

## Properties

* **Property<Rect\> geometry**

* **Property<bool\> visible**

## Functions

* **virtual Size sizeForSpace(Size availableSize = Size::none()) const**

* **virtual double uiLengthToDips(const UILength &uiLength) const = 0**

* **virtual bool canMoveToParentView(std::shared_ptr<View> newParentView) const = 0**

* **virtual void moveToParentView(std::shared_ptr<View> newParentView) = 0**

* **virtual void dispose() = 0**
