# ios::ViewCore

Base class for iOS view cores.

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

	Always returns true.

* **virtual void addChildViewCore(const std::shared_ptr<ViewCore> &core)**
* **virtual void removeFromUISuperview()**

## Geometry

* **virtual void frameChanged()**
	
	Updates the geometry Property based on the UIView's frame

* **virtual void onGeometryChanged(Rect newGeometry)**

	Updates the UIView's frame to newGeometry

## Layout

* **Size sizeForSpace(Size availableSpace = Size::none()) const override**

	Uses UIView::systemLayoutSizeFittingSize to calculate the preferred Size of the View

* **void scheduleLayout() override**

	Triggers ```[_uiView setNeedsLayout]```

* **virtual bool canAdjustToAvailableWidth() const**

	Override to influence how sizeForSpace behaves

* **virtual bool canAdjustToAvailableHeight() const**

	Override to influence how sizeForSpace behaves

## Helper
	
* **template <class T\> std::shared_ptr<T\> shared_from_this()**

	Returns shared_from_this but std::dynamic_pointer_cast<T>'s 

* **UIView \*uiView() const**

	Returns the UIView that was passed into the constructor

## Reporting external size changes to the Core

When an IOS View changes size, it needs to report the change back to the ViewCore to update its geometry Property.

The Base bdn::ios::ViewCore therefor expects an `UIViewWithFrameNotification`:

```obj-c
@protocol UIViewWithFrameNotification
- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore;
@end
```

ios::ViewCore Constructor will call setViewCore. The actual UIView class then needs to call back to the core when its frame changes. This is the implementation used by bdn::ios::ButtonCore:

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

