#ifndef BDN_MAC_ChildViewCore_HH_
#define BDN_MAC_ChildViewCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IViewCore.h>
#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/util.hh>


namespace bdn
{
namespace mac
{

class ChildViewCore : public Base, BDN_IMPLEMENTS IViewCore, BDN_IMPLEMENTS IParentViewCore
{
public:
    ChildViewCore(View* pOuterView, NSView* nsView)
    {
        _outerViewWeak = pOuterView;
        
        _nsView = nsView;
        
        _addToParent( pOuterView->getParentView() );
        
        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );
    }
        
    void setVisible(const bool& visible) override
    {
        _nsView.hidden = !visible;
    }
    
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // NSView does not have any padding properties. subclasses will override this
        // if the corresponding Cocoa view class supports setting a padding.
    }
    
    
    
    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        // first adjust the bounds so that they are on pixel boundaries
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
        
        // our parent view's coordinate system is usually "normal" i.e. with
        // the top left being (0,0). So there is no need to flip the coordinates.
        
        _nsView.frame = rectToMacRect(adjustedBounds, -1);
        
        return adjustedBounds;
    }
    
    
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override
    {
        // our parent view's coordinate system is usually "normal" i.e. with
        // the top left being (0,0). So there is no need to flip the coordinates.
        NSRect macRect = rectToMacRect( requestedBounds, -1);
        
        NSAlignmentOptions alignOptions =
            NSAlignRectFlipped;    // coordinate system is "flipped" for mac (normal for us), so we need this flag
                                   // apparently this only affects how halfway values are rounded, not
                                   // what "minY" and "maxY" refers to.
                
        if(positionRoundType==RoundType::down)
            alignOptions |= NSAlignMinXOutward | NSAlignMinYOutward;
        
        else if(positionRoundType==RoundType::up)
            alignOptions |= NSAlignMinXInward | NSAlignMinYInward;
        
        else
            alignOptions |= NSAlignMinXNearest | NSAlignMinYNearest;
        
        
        if(sizeRoundType==RoundType::down)
            alignOptions |= NSAlignWidthInward | NSAlignHeightInward;
        
        else if(sizeRoundType==RoundType::up)
            alignOptions |= NSAlignWidthOutward | NSAlignHeightOutward;
        
        else
            alignOptions |= NSAlignWidthNearest | NSAlignHeightNearest;
        
        
        NSRect adjustedMacRect =
            [_nsView backingAlignedRect:macRect
                                options:alignOptions ];
        
        Rect adjustedBounds = macRectToRect( adjustedMacRect, -1);
        
        return adjustedBounds;
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
        Size size = macSizeToSize( _nsView.fittingSize );
        
        // add the padding
        Nullable<UiMargin> pad;
        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pad = pView->padding();
        
        Margin additionalPadding;
        if(pad.isNull())
        {
            // we should use the "default" padding. So additionalPadding should be zero.
        }
        else
        {
            additionalPadding = uiMarginToDipMargin( pad );

            // some controls auto-include a base padding in the fittingSize.
            // We need to subtract that.
            additionalPadding -= getPaddingIncludedInFittingSize();
        }
        
        // if the padding we get from the outer window is less than the auto-included
        // padding then we have to use the auto-included padding. Otherwise parts of the content
        // might not be visible (the controls do not reduce the padding when they are
        // smaller than their fitting size - they just clip the content).
        additionalPadding.top = std::max(additionalPadding.top, 0.0);
        additionalPadding.right = std::max(additionalPadding.right, 0.0);
        additionalPadding.bottom = std::max(additionalPadding.bottom, 0.0);
        additionalPadding.left = std::max(additionalPadding.left, 0.0);
        
        size += additionalPadding;
        
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
    
    
    
    P<View> getOuterViewIfStillAttached()
    {
        return _outerViewWeak.toStrong();
    }

    P<const View> getOuterViewIfStillAttached() const
    {
      return _outerViewWeak.toStrong();
    }

    
    
    NSView* getNSView() const
    {
        return _nsView;
    }
    
    
    void addChildNsView( NSView* childView ) override
    {
        [_nsView addSubview:childView];
    }
    
    
protected:
    /** Returns an estimate padding that the NSView automatically includes in the calculation
        of NSView.fittingSize.
        The default implementation returns an zero padding.
        */
    virtual Margin getPaddingIncludedInFittingSize() const
    {
        return Margin();
    }
    
  
    
private:
     void _addToParent(View* pParentView)
     {
         if(pParentView==nullptr)
         {
             // classes derived from ChildViewCore MUST have a parent. Top level windows do not
             // derive from ChildViewCore.
             throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view that does not have a parent.");
         }
         
         P<IViewCore> pParentCore = pParentView->getViewCore();
         if(pParentCore==nullptr)
         {
             // this should not happen. The parent MUST have a core - otherwise we cannot
             // initialize ourselves.
             throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view whose parent does not have a core.");
         }
         
         cast<IParentViewCore>( pParentCore )->addChildNsView( _nsView );
    }
    
    WeakP<View>       _outerViewWeak;
    NSView*           _nsView;
};


}
}


#endif
