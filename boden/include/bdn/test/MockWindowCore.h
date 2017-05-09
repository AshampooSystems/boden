#ifndef BDN_TEST_MockWindowCore_H_
#define BDN_TEST_MockWindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/test/MockViewCore.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" window core that does not actually show anything
    visible, but behaves otherwise like a normal window core.
    
    See MockUiProvider.
    */
class MockWindowCore : public MockViewCore, BDN_IMPLEMENTS IWindowCore
{
public:
	MockWindowCore(Window* pWindow)
		: MockViewCore(pWindow)
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_title = pWindow->title();
	}

    /** Returns the window title that is currently configured.*/
    String getTitle() const
	{
        return _title;
	}

    /** Returns the number of times a new window title has been set.*/
    int getTitleChangeCount() const
	{
	    return _titleChangeCount;
	}
	
	void setTitle(const String& title) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		_title = title;
		_titleChangeCount++;
	}


	Rect getContentArea() override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return Rect(0, 0, _bounds.width-10-10, _bounds.height-20-10);
	}


	Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return contentSize + Margin(20, 11, 12, 13);
	}


	Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();
        
        Size contentSize = windowSize - Margin(20, 11, 12, 13);
        contentSize.width = std::max(contentSize.width, 0.0);
        contentSize.height = std::max(contentSize.height, 0.0);
        
        return contentSize;
	}


	Size getMinimumSize() const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return Size(100, 32);
	}

	
	Rect getScreenWorkArea() const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return Rect(100, 100, 800, 800);
	}
	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		// should not be called. The outer window should calculate the size
		REQUIRE(false);		
		return Size(0,0);
	}


protected:
	String _title;
	int    _titleChangeCount = 0;	
};


}
}

#endif