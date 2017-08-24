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

    HWND getParentHwndForChildren() const override;
        
	void setHorizontalScrollingEnabled( const bool& enabled) override;    
	void setVerticalScrollingEnabled( const bool& enabled) override;    

    
    void scrollAreaToVisible(const Rect& area) override;


    void layout() override;
	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;	

private:

    POINT   updateScrollInfo(bool horzScrollBar, bool vertScrollBar, const SIZE& viewPortSizeInPixels, const SIZE& fullContentSizeInPixels);
    void    updateContentContainerPos(const POINT& scrollPosInPixels);

    enum class ScrollPosType
    {
        /** Current position of the scrollbar.*/
        current,

        /** The current tracking position if the user is in the process of changing
            the scroll position
        */
        track
    };

    POINT getScrollPositionInPixels(ScrollPosType scrollPosType = ScrollPosType::current);

    virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

    class ScrollViewCoreClass : public Win32WindowClass
	{
	public:
		ScrollViewCoreClass()
			: Win32WindowClass("bdnScrollView", Win32Window::windowProc)
		{
			ensureRegistered();			
		}

		static ScrollViewCoreClass& get();
	};



    
    class ContentContainerClass : public Win32WindowClass
	{
	public:
		ContentContainerClass()
			: Win32WindowClass("bdnScrollViewContentContainer", Win32Window::windowProc)
		{
			ensureRegistered();			
		}

		static ContentContainerClass& get();
	};


    
    class ContentContainer : public Win32Window
    {
    public:
        ContentContainer(HWND parentHwnd);
    };


    
    bool                _showHorizontalScrollBar = false;
    bool                _showVerticalScrollBar = false;

    Margin              _nonClientMargins;

    P<ContentContainer> _pContentContainer;
};

}
}

#endif

