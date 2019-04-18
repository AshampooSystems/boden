path: tree/master/framework/layout/include/bdn
source: Layout.h

# Layout

Base class for layout implementations.

## Declaration

```C++
class Layout
```

## Register

* **virtual void registerView([View](../ui/view.md) \*view) = 0**

	Registers a [`View`](../ui/view.md) with the Layout. Called when the view is added to a hierarchy.

* **virtual void unregisterView([View](../ui/view.md) \*view) = 0**

	Unregisters a [View](../ui/view.md) from the Layout. Called when the view is removed from its hierarchy.

## Update

* **virtual void markDirty([View](../ui/view.md) \*view) = 0**
	
	Called when a [`View`](../ui/view.md) is dirty.

* **virtual void updateStylesheet([View](../ui/view.md) \*view) = 0**

	Called when a [`View`](../ui/view.md)'s stylesheet changes.

## Apply

* **virtual void layout([View](../ui/view.md) \*view) = 0**

	Called when it's time to layout a [`View`](../ui/view.md).
