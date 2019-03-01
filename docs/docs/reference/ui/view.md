# View

Abstract base class for all user interface elements.

## Properties

* **Property<Rect\> geometry**

	Current geometry of the view on screen.

* **Property<bool\> visible**

	Whether the view is visible.

* **Property<std::shared_ptr<LayoutStylesheet\>\> layoutStylesheet**

	A [`LayoutStylesheet`](layout_stylesheet.md) defining the layout of the view.

## Layout

* **virtual Size sizeForSpace(Size availableSpace = Size::none()) const**

	Calculates the view's [`Size`](../foundation/size.md) for the given space.

* **std::shared_ptr<Layout\> getLayout()**

	Returns the view's [`Layout`](layout.md).

* **void setLayout(std::shared_ptr<Layout\> layout)**

	Sets the view's [`Layout`](layout.md).

* **template <class T\> void setLayoutStylesheet(T sheet)**

	Convenience method for setting the layout stylesheet using a type `T` derived from [`LayoutStylesheet`](layout_stylesheet.md) without having to use `std::make_shared`.

## View Core

* **virtual String getViewCoreTypeName() const = 0**

	Super classes should return a [`String`](../string.md) presenting the type name of the view's core.

* **std::shared_ptr<ViewCore> getViewCore() const**

	Returns the view's [`ViewCore`](view_core.md).

* **void setViewCore(std::shared_ptr<UIProvider\> uiProvider, std::shared_ptr<ViewCore\> viewCore)**

	Sets the view's [`ViewCore`](view_core.md) and the given [`UIProvider`](ui_provider.md).

* **void reinitCore(std::shared_ptr<UIProvider\> uiProvider = nullptr)**

	Reinitializes the view's [`ViewCore`](view_core.md) using the given [`UIProvider`](ui_provider.md). If the `uiProvider` argument is set to `nullptr`, attempts to obtain a suitable UI provider from its current parent view.

* **virtual void \_deinitCore()**

	Deinitializes the view's currrent [`ViewCore`](view_core.md).

* **virtual void \_initCore(std::shared_ptr<UIProvider\> uiProvider = nullptr)**

	Initializes the view's [`ViewCore`](view_core.md) using the given [`UIProvider`](ui_provider.md). If the `uiProvider` argument is set to `nullptr`, attempts to obtain a suitable UI provider from its current parent view.

* **std::shared_ptr<UIProvider\> getUIProvider()**

	Returns the view's [`UIProvider`](ui_provider.md).

## View Hierarchy

* **virtual std::list<std::shared_ptr<View\>\> getChildViews() const**

	Returns the view's child views.

* **virtual void removeAllChildViews()**

	Removes all child views.

* **virtual std::shared_ptr<View\> findPreviousChildView(std::shared_ptr<View\> childView)**

	<span style="color: red">Deprecated</span>

* **virtual void \_childViewStolen(std::shared_ptr<View\> childView)**
	
	<span style="color: red">Deprecated</span>


* **virtual std::shared_ptr<View\> getParentView()**

* **void \_setParentView(std::shared_ptr<View\> parentView)**

## Length Conversion

* **double uiLengthToDips(const UILength &length) const**

	<span style="color: red">Deprecated</span>

## Misc

* **std::shared_ptr<View\> shared_from_this()**

	Returns an `std::shared_ptr` referencing the view.

## Relationships

Inherits from [`Base`](../foundation/base.md).
