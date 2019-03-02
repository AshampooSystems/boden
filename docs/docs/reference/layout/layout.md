# Layout

## Declaration

```C++
class Layout
```

## Register

* **virtual void registerView(View \*view) = 0**

	Registers the View with the Layout. Called when the View is added into a Hierarchy

* **virtual void unregisterView(View \*view) = 0**

	Unregisters the View from the Layout. Called when the View is removed from its Hierarchy

## Update

* **virtual void markDirty(View \*view) = 0**
	
	Called when a View is dirty

* **virtual void updateStylesheet(View \*view) = 0**

	Called when a Views stylesheet changes

## Apply

* **virtual void layout(View \*view) = 0**

	Called when its time to layout a View
