#ifndef BDN_TEST_MockWindowCore_H_
#define BDN_TEST_MockWindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/windowCoreUtil.h>

#include <bdn/test/MockViewCore.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" window core that does not actually show anything
    visible, but behaves otherwise like a normal window core.
    
    See MockUiProvider.
    */
class MockWindowCore : public MockViewCore, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
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

	
	Rect getScreenWorkArea() const
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		return Rect(100, 100, 800, 800);
	}
	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        MockViewCore::calcPreferredSize(availableSpace);

		BDN_REQUIRE_IN_MAIN_THREAD();

		// should not be called. The outer window should calculate the size
		REQUIRE(false);		
		return Size(0,0);
	}


    void requestAutoSize()
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            cast<MockUiProvider>(pWindow->getUiProvider())->getLayoutCoordinator()->windowNeedsAutoSizing(pWindow);
    }

	void requestCenter()
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            cast<MockUiProvider>(pWindow->getUiProvider())->getLayoutCoordinator()->windowNeedsCentering(pWindow);
    }

    
    void layout()
    {
        BDN_REQUIRE_IN_MAIN_THREAD();
		
		_layoutCount++;

        if( !_overrideLayoutFunc || !_overrideLayoutFunc() )
        {
		    P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
            if(pWindow!=nullptr)
                defaultWindowLayoutImpl( pWindow, getContentArea() );
        }
    }
    
	void autoSize() override
    {
        BDN_REQUIRE_IN_MAIN_THREAD();
		
		_autoSizeCount++;

        if( !_overrideAutoSizeFunc || !_overrideAutoSizeFunc() )
        {
            P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
            if(pWindow!=nullptr)
                defaultWindowAutoSizeImpl( pWindow, getScreenWorkArea().getSize() );
        }
	}	

    /** Sets a function that is executed instead of the normal autoSiuze() function implementation.
    
        If the return value of the override function is false then the normal
        autosize function implementation runs after the override function. If the
        return value is true then the normal implementation is not run.
        */
    void setOverrideAutoSizeFunc( const std::function<bool()> func )
    {
        _overrideAutoSizeFunc = func;
    }

    /** Returns the number of times that autoSize() was called.*/
    int getAutoSizeCount() const
    {
        return _autoSizeCount;
    }



    void center() override
    {
        BDN_REQUIRE_IN_MAIN_THREAD();
		
		_centerCount++;

        if( !_overrideCenterFunc  || !_overrideCenterFunc() )
        {
            P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
            if(pWindow!=nullptr)
                defaultWindowCenterImpl( pWindow, getScreenWorkArea() );
        }
	}	


    /** Sets a function that is executed instead of the normal center() function implementation.
    
        If the return value of the override function is false then the normal
        center function implementation runs after the override function. If the
        return value is true then the normal implementation is not run.
        */
    void setOverrideCenterFunc( const std::function<bool()> func )
    {
        _overrideCenterFunc = func;
    }

    /** Returns the number of times that center() was called.*/
    int getCenterCount() const
    {
        return _centerCount;
    }


protected:
	String  _title;
	int     _titleChangeCount = 0;	

    int     _autoSizeCount = 0;
    int     _centerCount = 0;

    std::function<bool()> _overrideCenterFunc;
    std::function<bool()> _overrideAutoSizeFunc;
};


}
}

#endif