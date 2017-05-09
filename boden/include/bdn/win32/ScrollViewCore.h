#ifndef BDN_WIN32_ScrollViewCore_H_
#define BDN_WIN32_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/Win32WindowClass.h>

#include <bdn/IScrollViewCore.h>


namespace bdn
{
namespace win32
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore
{
public:
	ScrollViewCore(ScrollView* pOuter);			
        
	void setHorizontalScrollingEnabled( const bool& enabled) override;    
	void setVerticalScrollingEnabled( const bool& enabled) override;    

    void layout() override;
	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;	

private:

    void updateScrollInfo(bool horzScrollBar, bool vertScrollBar, const Size& viewPortSize, const Size& fullContentSize);

    class ScrollViewCoreClass : public Win32WindowClass
	{
	public:
		ScrollViewCoreClass()
			: Win32WindowClass("bdnScrollView", ViewCore::windowProc)
		{
			ensureRegistered();			
		}

		static ScrollViewCoreClass& get();
	};

    bool _showHorizontalScrollBar = false;
    bool _showVerticalScrollBar = false;
};

}
}

#endif

