# ios::ViewCore

Base class for iOS view cores.

This effectively wraps `UIView` and serves as a foundation for all other iOS view cores.

## Declaration

```c++
namespace bdn::ios {
	class ViewCore : public bdn::View::Core, public std::enable_shared_from_this<ViewCore>
}
```

## Construction

* **ViewCore(const std::shared_ptr<bdn::ViewCoreFactory\> &viewCoreFactory, id<UIViewWithFrameNotification\> uiView)**
* **void init() override**


## Parent/Child relations

* **bool canMoveToParentView(std::shared_ptr<View> newParentView) const override**

	Always returns true on iOS as `UIView`s can move to any parent view.

* **virtual void addChildViewCore(const std::shared_ptr<ViewCore> &core)**
* **virtual void removeFromUISuperview()**

## Geometry

* **virtual void frameChanged()**
	
	Updates the geometry Property based on the UIView's frame.

* **virtual void onGeometryChanged(Rect newGeometry)**

	Updates the UIView's frame to the `Rect` given in `newGeometry`.

## Layout

* **Size sizeForSpace(Size availableSpace = Size::none()) const override**

	Uses UIView::systemLayoutSizeFittingSize to calculate the preferred Size of the [`View`](../view.md).

* **void scheduleLayout() override**

	Triggers the `setNeedsLayout` method of the managed `UIView`.

* **virtual bool canAdjustToAvailableWidth() const**

	Override to influence how `sizeForSpace` behaves.

* **virtual bool canAdjustToAvailableHeight() const**

	Override to influence how `sizeForSpace` behaves.

## Helper
	
* **template <class T\> std::shared_ptr<T\> shared_from_this()**

	Returns `shared_from_this` casted to `std::shared_ptr<T>` using `std::dynamic_pointer_cast<T>`. 

* **UIView \*uiView() const**

	Returns the `UIView` that was passed to the constructor.

## Reporting Size Changes to the Core

The Boden Framework supports native layout updates from the target platform. This is especially useful in cases where native widget layouts should be reused to eliminate the need for custom non-native layout implementations, e.g. in list views (`UITableView` on iOS).

When an iOS view changes its size, the framework therefore needs to be informed so it can update the view's `geometry` property.

To accommodate for native layout updates, `ios::ViewCore` and all subclasses wrapping native `UIView`s must implement the `UIViewWithFrameNotification` protocol:

```obj-c
@protocol UIViewWithFrameNotification
- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore;
@end
```

The `ios::ViewCore` constructor will call `setViewCore` on its native `UIView` instance. The `UIView` subclass then needs to call back the core's `frameChanged` method when its frame changes. For instance, this is the implementation used by `ios::ButtonCore`:

```obj-c
@interface BodenUIButton : UIButton <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ButtonCore> core;
@end

@implementation BodenUIButton

- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore
{
    self.core = std::dynamic_pointer_cast<bdn::ios::ButtonCore>(viewCore.lock());
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto core = self.core.lock()) {
        core->frameChanged();
    }
}
//...
@end
```

