#ifndef BDN_IOS_ViewCore_HH_
#define BDN_IOS_ViewCore_HH_

#import <UIKit/UIKit.h>

#include <bdn/IViewCore.h>
#include <bdn/View.h>

#import <bdn/ios/UIProvider.hh>
#import <bdn/ios/util.hh>


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
    
    
    void setBounds(const Rect& bounds) override
    {
        _view.frame = rectToIosRect(bounds);
    }
    
    
    int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixels(uiLength);
    }
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const override
    {
		CGSize iosSize = [_view systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
        
        Size size = iosSizeToSize(iosSize);
        
        Margin padding = getPaddingPixels();
        
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
    virtual Margin getDefaultPaddingPixels() const
    {
        return Margin();
    }

    Margin getPaddingPixels() const
    {
        // add the padding
        Margin padding;
        
        Nullable<UiMargin> pad;
        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pad = pView->padding();

        if(pad.isNull())
            padding = getDefaultPaddingPixels();
        else
            padding = uiMarginToPixelMargin(pad.get());
        
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
