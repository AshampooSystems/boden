path: tree/master/framework/ui/include/bdn/
source: Layout.h

# Layout

## Declaration

```C++
class Layout
```

## Register

* **virtual void registerView(View \*view) = 0**

	Registers a [View](../ui/view.md) with the Layout. Called when the view is added into a Hierarchy

* **virtual void unregisterView(View \*view) = 0**

	Unregisters a [View](../ui/view.md) from the Layout. Called when the view is removed from its hierarchy.

## Update

* **virtual void markDirty(View \*view) = 0**
	
	Called when a [View](../ui/view.md) is dirty.

* **virtual void updateStylesheet(View \*view) = 0**

	Called when a [View](../ui/view.md)'s stylesheet changes.

## Apply

* **virtual void layout(View \*view) = 0**

	Called when it's time to layout a [View](../ui/view.md).
