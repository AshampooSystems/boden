#ifndef BDN_MAC_ViewCore_HH_
#define BDN_MAC_ViewCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IViewCore.h>
#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/util.hh>


namespace bdn
{
namespace mac
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore, BDN_IMPLEMENTS IParentViewCore
{
public:
    ViewCore(View* pOuterView, NSView* nsView)
    {
        _pOuterViewWeak = pOuterView;
        
        _nsView = nsView;
        
        _addToParent( pOuterView->getParentView() );
        
        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );
    }
    
    
    void setVisible(const bool& visible) override
    {
        _nsView.hidden = !visible;
    }
    
    
    void setMargin(const UiMargin& margin) override
    {
        // we don't care about OUR margin. Our parent uses it during layout.
        // So, do nothing here.
    }
    

    void setPadding(const UiMargin& padding) override
    {
        // store the padding so that we can use it during size calulation
        _pOuterViewWeak->needSizingInfoUpdate();
    }
    

    void setBounds(const Rect& bounds) override
    {
        // our parent view's coordinate system is "normal" i.e. with
        // the top left being (0,0). So there is no need to flip the coordinates.
        _nsView.frame = rectToMacRect(bounds, -1);
        
        getOuterView()->needLayout();
    }
    
    
    int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixels(uiLength);
    }
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize() const override
    {
        Size size = macSizeToSize( _nsView.fittingSize );
        
        // add the padding
        Margin additionalPadding = uiMarginToPixelMargin( getOuterView()->padding() );

        // some controls auto-include a base padding in the fittingSize.
        // We need to subtract that.
        additionalPadding -= getPaddingIncludedInFittingSize();
        
        // if the padding we get from the outer window is less than the auto-included
        // padding then we have to use the auto-included padding. Otherwise parts of the content
        // might not be visible (the controls do not reduce the padding when they are
        // smaller than their fitting size - they just clip the content).
        additionalPadding.top = std::max(additionalPadding.top, 0);
        additionalPadding.right = std::max(additionalPadding.right, 0);
        additionalPadding.bottom = std::max(additionalPadding.bottom, 0);
        additionalPadding.left = std::max(additionalPadding.left, 0);
        
        size += additionalPadding;
        
        if(size.width<0)
            size.width = 0;
        if(size.height<0)
            size.height = 0;
        
        return size;
    }
    
    
    int calcPreferredHeightForWidth(int width) const override
    {
        // there is apparently no way to get the preferred size for a certain width.
        return calcPreferredSize().height;
    }
    
    
    int calcPreferredWidthForHeight(int height) const override
    {
        // there is apparently no way to get the preferred size for a certain width.
        return calcPreferredSize().width;
        
    }
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        _addToParent(pNewParent);
        
        return true;
    }
    
    
    
    View* getOuterView()
    {
        return _pOuterViewWeak;
    }

    const View* getOuterView() const
    {
      return _pOuterViewWeak;
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
             // classes derived from ViewCore MUST have a parent. Top level windows do not
             // derive from ViewCore.
             throw ProgrammingError("bdn::mac::ViewCore constructed for a view that does not have a parent.");
         }
         
         P<IViewCore> pParentCore = pParentView->getViewCore();
         if(pParentCore==nullptr)
         {
             // this should not happen. The parent MUST have a core - otherwise we cannot
             // initialize ourselves.
             throw ProgrammingError("bdn::mac::ViewCore constructed for a view whose parent does not have a core.");
         }
         
         cast<IParentViewCore>( pParentCore )->addChildNsView( _nsView );
    }
    
    View*       _pOuterViewWeak;
    NSView*     _nsView;
};


}
}


#endif
