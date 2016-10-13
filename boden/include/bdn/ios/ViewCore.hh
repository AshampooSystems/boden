#ifndef BDN_IOS_ViewCore_HH_
#define BDN_IOS_ViewCore_HH_

#import <UIKit/UIKit.h>

#include <bdn/IViewCore.h>
#include <bdn/View.h>

#import <bdn/ios/UIProvider.hh>
#import <bdn/ios/util.hh>

#include <bdn/PixelAligner.h>


namespace bdn
{
namespace ios
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore(View* pOuterView, UIView* view)
    {
        _outerViewWeak = pOuterView;
        _view = view;
        
        _addToParent( pOuterView->getParentView() );
        
        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );
    }
    
    
    ~ViewCore()
    {
        _view = nil;        
    }
    
    P<const View> getOuterViewIfStillAttached() const
    {
        return _outerViewWeak.toStrong();
    }
    
    P<View> getOuterViewIfStillAttached()
    {
        return _outerViewWeak.toStrong();
    }
    
    UIView* getUIView() const
    {
        return _view;
    }
    
    
    
    void setVisible(const bool& visible) override
    {
        _view.hidden = !visible;
    }
    
    
    void setPadding(const Nullable<UiMargin>& padding) override
    {
    }
    
    
    
    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        // first adjust the bounds so that they are on pixel boundaries
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
        
        _view.frame = rectToIosRect(adjustedBounds);
        
        return adjustedBounds;
    }
    
    
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override
    {
        // most example code for ios simply aligns on integer values when pixel alignment
        // is discussed.
        // While this DOES actually align on a grid that coincides with some pixel boundaries, the actual
        // screen resolution can be much higher (and indeed it is for all modern iPhones). iOS uses an
        // integral scale factor from "points" (=what we call DIPs) to pixels. Aligning to integers aligns
        // to full points. But there can be 1, 2, 3 or more actual pixels per point.
        
        // Aligning to full points has the disadvantage that we do not take full advantage of the display
        // resolution when it comes to positioning things. This can make animations less smooth than they could be.
        
        // On macOS there is the function backingAlignedRect which can align to a proper boundary
        // in a high level way. Apparently such a function does not exist on iOS. So we have to manually
        // align.
        
        UIScreen* screen = [UIScreen mainScreen];
        
        double scale = screen.scale;    // 1 for old displays, 2 for retina iphone, 3 for iphone plus, etc.
        
        return PixelAligner(scale).alignRect(requestedBounds, positionRoundType, sizeRoundType);
    }
    

    double uiLengthToDips(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToDips(uiLength);
    }
    

    Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToDipMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
    {
        CGSize constraintSize = UILayoutFittingCompressedSize;
        if(availableWidth!=-1)
            constraintSize.width = availableWidth;
        if(availableHeight!=-1)
            constraintSize.height = availableHeight;
        
		CGSize iosSize = [_view systemLayoutSizeFittingSize:constraintSize];
        
        Size size = iosSizeToSize(iosSize);
        
        if(size.width<0)
            size.width = 0;
        if(size.height<0)
            size.height = 0;
        
        Margin padding = getPaddingDips();
        
        size += padding;
        
        if(size.width<0)
            size.width = 0;
        if(size.height<0)
            size.height = 0;
        
        return size;
    }
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        _addToParent(pNewParent);
        
        return true;
    }
    
    
    virtual void addChildUIView( UIView* childView )
    {
        [_view addSubview:childView];
    }
    
    
protected:

    /** Returns the default padding for the control.
        The default implementation returns zero-padding.*/
    virtual Margin getDefaultPaddingDips() const
    {
        return Margin();
    }

    Margin getPaddingDips() const
    {
        // add the padding
        Margin padding;
        
        Nullable<UiMargin> pad;
        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pad = pView->padding();

        if(pad.isNull())
            padding = getDefaultPaddingDips();
        else
            padding = uiMarginToDipMargin(pad.get());
        
        return padding;
    }
    

    
    
private:
    void _addToParent(View* pParentView)
    {
        if(pParentView==nullptr)
        {
            // top level window. Nothing to do.
            return;
        }
        
        P<IViewCore> pParentCore = pParentView->getViewCore();
        if(pParentCore==nullptr)
        {
            // this should not happen. The parent MUST have a core - otherwise we cannot
            // initialize ourselves.
            throw ProgrammingError("bdn::ios::ViewCore constructed for a view whose parent does not have a core.");
        }
        
        cast<ViewCore>( pParentCore )->addChildUIView( _view );
    }


    WeakP<View>   _outerViewWeak;
    
    UIView* _view;
};


}
}


#endif
