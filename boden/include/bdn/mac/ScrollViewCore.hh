#ifndef BDN_MAC_ScrollViewCore_HH_
#define BDN_MAC_ScrollViewCore_HH_


#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>


namespace bdn
{
namespace mac
{

class ScrollViewCore : public ChildViewCore, BDN_IMPLEMENTS IParentViewCore, BDN_IMPLEMENTS IScrollViewCore
{
private:
	static NSScrollView* _createScrollView(ScrollView* pOuter);

public:
	ScrollViewCore(	ScrollView* pOuter)
		: ChildViewCore(pOuter, _createScrollView(pOuter) )
	{
        _nsScrollView = (NSScrollView*)getNSView();
	}

    void setHorizontalScrollingEnabled(const bool& enabled) override;
    void setVerticalScrollingEnabled(const bool& enabled) override;
    
    Size calcPreferredSize( const Size& availableSpace ) const override;
    
    void layout() override;
    
    
    void addChildNsView( NSView* childView ) override;
    

    
private:
    NSScrollView*   _nsScrollView;
};

}
}

#endif
