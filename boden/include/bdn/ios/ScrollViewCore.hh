#ifndef BDN_IOS_ScrollViewCore_HH_
#define BDN_IOS_ScrollViewCore_HH_


#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

#include <bdn/ScrollViewLayoutHelper.h>

#import <bdn/ios/ViewCore.hh>


namespace bdn
{
namespace ios
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore
{
private:
	static UIScrollView* _createScrollView(ScrollView* pOuter);

public:
	ScrollViewCore(	ScrollView* pOuter);
    
    void setPadding(const Nullable<UiMargin>& padding) override;

    void setHorizontalScrollingEnabled(const bool& enabled) override;
    void setVerticalScrollingEnabled(const bool& enabled) override;
    
    Size calcPreferredSize( const Size& availableSpace ) const override;
    
    void layout() override;
    
    void scrollClientRectToVisible(const Rect& clientRect) override;
    
    void addChildUIView( UIView* childView ) override;
    
protected:
    bool canAdjustToAvailableWidth() const override
    {
        return true;
    }
    
    bool canAdjustToAvailableHeight() const override
    {
        return true;
    }

    
private:
    UIScrollView*   _uiScrollView;
    UIView*         _uiContentViewParent;
    
    bool            _horzScrollEnabled = false;
    bool            _vertScrollEnabled = false;
};

}
}

#endif
