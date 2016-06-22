#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/test.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/Button.h>


using namespace bdn;

void verifyMainThread()
{
	REQUIRE( Thread::isCurrentMain() );
}

class MockViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:

	bool		_visible = false;
	int			_visibleChangeCount = 0;

	UiMargin	_margin;
	int			_marginChangeCount = 0;

	UiMargin	_padding;
	int			_paddingChangeCount = 0;

	Rect		_bounds;
	int			_boundsChangeCount = 0;

	View*		_pParentView = nullptr;
	int			_parentViewChangeCount = 0;

	View*		_pOuterView = nullptr;

	explicit MockViewCore(View* pView)
	{
		verifyMainThread();

		_pOuterView = pView;

		_visible = pView->visible();
		_margin = pView->margin();
		_padding = pView->padding();
		_bounds = pView->bounds();
		_pParentView = pView->getParentView();
	}

	Size _getTextSize(const String& s) const
	{
		// our fake font has a size of 10x20 for each character.
		return Size( s.getLength()*10, 20);
	}

	void	setVisible(const bool& visible)
	{
		verifyMainThread();

		_visible = visible;
		_visibleChangeCount++;
	}
	
	
	void setMargin(const UiMargin& margin)
	{
		verifyMainThread();

		_margin = margin;	
		_marginChangeCount++;
	}

	
	void setPadding(const UiMargin& padding)
	{
		verifyMainThread();

		_padding = padding;
		_paddingChangeCount++;

		_pOuterView->needSizingInfoUpdate();
	}

	
	void setBounds(const Rect& bounds)
	{
		verifyMainThread();

		_bounds = bounds;
		_boundsChangeCount++;
	}

	
	int uiLengthToPixels(const UiLength& uiLength) const
	{
		verifyMainThread();

		if(uiLength.unit==UiLength::Unit::sem)
		{
			// one sem = 20 mock pixels
			return std::lround( uiLength.value*20 );
		}
		else if(uiLength.unit==UiLength::Unit::pixel96)
		{
			// one pixel 96 = 3 mock pixels
			return std::lround( uiLength.value*3 );
		}
		else if(uiLength.unit==UiLength::Unit::realPixel)
			return std::lround( uiLength.value );
		else
		{
			// invalid parameter passed to this function
			REQUIRE(false);
			return 0;
		}
	}
	

	Margin uiMarginToPixelMargin(const UiMargin& margin) const
	{
		verifyMainThread();

		return Margin( uiLengthToPixels(margin.top),
						uiLengthToPixels(margin.right),
						uiLengthToPixels(margin.bottom),
						uiLengthToPixels(margin.left) );
	}


	
	int calcPreferredHeightForWidth(int width) const
	{
		verifyMainThread();

		return calcPreferredSize().height;
	}


	int calcPreferredWidthForHeight(int height) const
	{
		verifyMainThread();

		return calcPreferredSize().width;
	}
	


	bool tryChangeParentView(View* pNewParent)
	{
		verifyMainThread();

		_pParentView = pNewParent;
		_parentViewChangeCount++;
		return true;
	}
};

class MockButtonCore : public MockViewCore, BDN_IMPLEMENTS IButtonCore
{
public:

	String _label;
	int    _labelChangeCount = 0;

	MockButtonCore(Button* pButton)
		: MockViewCore(pButton)
	{
		_label = pButton->label();		
	}
	
	void setLabel(const String& label)
	{
		_label = label;
		_labelChangeCount++;
	}


	
	Size calcPreferredSize() const
	{
		verifyMainThread();

		Size size = _getTextSize(_label);

		// add some space for the fake button border
		size += Margin( 4, 5);

		return size;
	}
	
	
	void generateClick()
	{
		ClickEvent evt(_pOuterView);

		cast<Button>(_pOuterView)->onClick().notify(evt);
	}

};

class MockWindowCore : public MockViewCore, BDN_IMPLEMENTS IWindowCore
{
public:

	String _title;
	int    _titleChangeCount = 0;

	MockWindowCore(Window* pWindow)
		: MockViewCore(pWindow)
	{
		verifyMainThread();

		_title = pWindow->title();
	}
	
	void setTitle(const String& title)
	{
		verifyMainThread();

		_title = title;
		_titleChangeCount++;
	}


	Rect getContentArea()
	{
		verifyMainThread();

		return Rect(0, 0, _bounds.width-10-10, _bounds.height-20-10);
	}


	Size calcWindowSizeFromContentAreaSize(const Size& contentSize)
	{
		verifyMainThread();

		return contentSize + Margin(20, 11, 12, 13);
	}


	Size calcContentAreaSizeFromWindowSize(const Size& windowSize)
	{
		verifyMainThread();

		return windowSize - Margin(20, 11, 12, 13);
	}


	Size calcMinimumSize() const
	{
		verifyMainThread();

		return Size(100, 32);
	}

	
	Rect getScreenWorkArea()
	{
		verifyMainThread();

		return Rect(100, 100, 800, 800);
	}
	
	Size calcPreferredSize() const
	{
		verifyMainThread();

		// should not be called. The outer window should calculate the size
		REQUIRE(false);		
		return Size(0,0);
	}
	
};


class MockUiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:

	String getName() const
	{
		return "mock";
	}


	int _windowsCreated = 0;
	int _buttonsCreated = 0;

	P<IViewCore> _pLastCreatedCore;


	P<IViewCore> createViewCore(const String& coreTypeName, View* pView)
	{
		verifyMainThread();

		if(coreTypeName==Window::getWindowCoreTypeName())
		{
			_windowsCreated++;

			_pLastCreatedCore = newObj<MockWindowCore>( cast<Window>(pView) );
			return _pLastCreatedCore;
		}
		else if(coreTypeName==Button::getButtonCoreTypeName())
		{
			_buttonsCreated++;

			_pLastCreatedCore = newObj<MockButtonCore>( cast<Button>(pView) );
			return _pLastCreatedCore;
		}
		else
			throw ViewCoreTypeNotSupportedError(coreTypeName);
	}

};



class TestWindow : public Window
{
public:
	TestWindow(IUiProvider* pUiProvider)
		: Window(pUiProvider)
	{		
	}

	int _sizingInfoUpdateCount = 0;

	void updateSizingInfo()
	{
		REQUIRE( Thread::isCurrentMain() );
		
		_sizingInfoUpdateCount++;
		Window::updateSizingInfo();		
	}	
};

void verifyOp(P<TestWindow> pWindow, std::function<void()> opFunc, std::function<void()> verifyFunc, int expectedSizingInfoUpdates )
{
	SECTION("mainThread")
	{
		// schedule the test asynchronously, so that the initial sizing
		// info update from the view construction is already done.

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow, opFunc, verifyFunc, expectedSizingInfoUpdates]()
			{
				// sizing info update count should be 1 now (from construction).
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );

				opFunc();
				verifyFunc();

				// sizing info updates should never happen immediately. We want them
				// to happen asynchronously, so that multiple changes can be handled
				// together with a single update.
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );	

				asyncCallFromMainThread(
					[pWindow, expectedSizingInfoUpdates]()
					{
						REQUIRE( pWindow->_sizingInfoUpdateCount==1 + expectedSizingInfoUpdates );

						END_ASYNC_TEST();			
					});
			} );
	}

	SECTION("otherThread")
	{
		// schedule the test asynchronously, so that the initial sizing
		// info update from the view construction is already done.

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow, opFunc, verifyFunc, expectedSizingInfoUpdates]()
			{
				// sizing info update count should be 1 now (from construction).
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );

				// note that we call get on the future object, so that we wait until the
				// other thread has finished (so that any changes have been scheduled)
				Thread::exec( opFunc ).get();		

				// any changes made to properties by the opFunc are asynchronously scheduled.
				// So they have not actually been made in the core yet,

				// we want to wait until the changes have actually been made in the core.
				// So do another async call. That one will be executed after the property
				// changes.
				asyncCallFromMainThread(
					[pWindow, verifyFunc, expectedSizingInfoUpdates]()
					{
						// the core should now have been updated.
						// However, sizing info updates should never happen immediately when
						// a core changes. We want them to happen asynchronously,
						// so that multiple changes can be handled together with a single update.

						// so the sizing info update count should still be unchanged
						REQUIRE( pWindow->_sizingInfoUpdateCount==1 );	

						// now we do another async step. At that point the scheduled
						// update should have happened and the sizing info should have been
						// updated (once!)
						asyncCallFromMainThread(
							[pWindow, verifyFunc, expectedSizingInfoUpdates]()
							{
								verifyFunc();
						
								REQUIRE( pWindow->_sizingInfoUpdateCount == 1+expectedSizingInfoUpdates );

								END_ASYNC_TEST();
							}	 );
					
					}	);				
			} );
	}
}


void testSizingWithContentView(P<TestWindow> pWindow, P<MockUiProvider> pUiProvider, std::function<Size()> getSizeFunc)
{
	
	// we add a button as a content view
	P<Button> pButton = newObj<Button>();
	pButton->label() = "HelloWorld";

	Margin buttonMargin;

	SECTION("noMargin")
	{
		// do nothing
	}

	SECTION("semMargin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::sem, 1, 2, 3, 4);

		// 1 sem = 20 pixels in our mock ui
		buttonMargin = Margin(20, 40, 60, 80 );
	}

	SECTION("pixel96Margin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::pixel96, 1, 2, 3, 4);

		// 1 pixel96 = 3 pixels in our mock ui
		buttonMargin = Margin(3, 6, 9, 12 );
	}

	SECTION("realPixelMargin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::realPixel, 1, 2, 3, 4);

		buttonMargin = Margin(1, 2, 3, 4 );
	}

	pWindow->setContentView( pButton );

	P<MockButtonCore> pButtonCore = cast<MockButtonCore>( pUiProvider->_pLastCreatedCore );

	// Sanity check. Verify the fake button size. 10x20 per character, plus 10x8 for border
	Size buttonSize( 10*10 + 10, 20 + 8);
	REQUIRE( pButtonCore->calcPreferredSize() == buttonSize );

	// window border size is 20, 11, 12, 13 in our fake UI
	Margin windowBorder( 20, 11, 12, 13);

	Size expectedSize = buttonSize + buttonMargin + windowBorder;

	// the sizing info will update asynchronously. So we need to do the
	// check async as well.
	MAKE_ASYNC_TEST(10);
			
	// the button's size change will propagate to the window asynchronously.
	// So we must check async as well
			
	asyncCallFromMainThread(
		[getSizeFunc, expectedSize]()
		{
			Size size = getSizeFunc();

			REQUIRE( size == expectedSize );

			END_ASYNC_TEST();				
		} );
}

TEST_CASE("Window", "[ui]")
{   
	P<MockUiProvider> pUiProvider = newObj<MockUiProvider>();

	SECTION("onlyNewAllocAllowed")
	{
		REQUIRE_THROWS_AS( Window window(pUiProvider), ProgrammingError);

		REQUIRE( pUiProvider->_windowsCreated==0 );
	}

	P<TestWindow> pWindow = newObj<TestWindow>( pUiProvider );				
	REQUIRE( pUiProvider->_windowsCreated==1 );

	P<MockWindowCore> pCore = cast<MockWindowCore>( pUiProvider->_pLastCreatedCore );
	REQUIRE( pCore!=nullptr );

	SECTION("construct")
	{
		// the core should initialize its properties from the outer window when it is created.
		// The outer window should not set them manually after construction.		
		REQUIRE( pCore->_visibleChangeCount==0 );
		REQUIRE( pCore->_marginChangeCount==0 );
		REQUIRE( pCore->_paddingChangeCount==0 );
		REQUIRE( pCore->_boundsChangeCount==0 );
		REQUIRE( pCore->_parentViewChangeCount==0 );
		REQUIRE( pCore->_titleChangeCount==0 );

		// windows should be invisible initially
		REQUIRE( !pWindow->visible() );

		REQUIRE( pWindow->title() == "");		
		REQUIRE( pWindow->getParentView()==nullptr );

		// sizing info should not yet have been updated. It should
		// update asynchronously, so that multiple property
		// changes can be handled with a single update.
		REQUIRE( pWindow->_sizingInfoUpdateCount==0);

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow]()
			{
				REQUIRE( pWindow->_sizingInfoUpdateCount==1);			

				END_ASYNC_TEST();
			});
	}

	SECTION("changeProperty")
	{
		SECTION("visible")
		{
			verifyOp(
				pWindow,
				[pWindow]()
				{
					pWindow->visible() = true;
				},
				[pCore, pWindow]()
				{
					REQUIRE( pCore->_visibleChangeCount==1 );
					REQUIRE( pCore->_visible );	
				},
				0	// should NOT have caused a sizing info update
				);
		}
	
		SECTION("margin")
		{
			UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, m]()
				{
					pWindow->margin() = m;
				},
				[pCore, m, pWindow]()
				{
					REQUIRE( pCore->_marginChangeCount==1 );
					REQUIRE( pCore->_margin == m);
				},
				0	// should NOT have caused a sizing info update
				);
		}

		SECTION("padding")
		{
			UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, m]()
				{
					pWindow->padding() = m;
				},
				[pCore, m, pWindow]()
				{
					REQUIRE( pCore->_paddingChangeCount==1 );
					REQUIRE( pCore->_padding == m);
				},
				1	// should have caused a sizing info update
				);
		}

		SECTION("bounds")
		{
			Rect b(1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, b]()
				{
					pWindow->bounds() = b;
				},
				[pCore, b, pWindow]()
				{
					REQUIRE( pCore->_boundsChangeCount==1 );
					REQUIRE( pCore->_bounds == b);
				},
				0	// should NOT have caused a sizing info update
				);
		}


		SECTION("title")
		{
			verifyOp( 
				pWindow,
				[pWindow]()
				{
					pWindow->title() = "hello";					
				},
				[pCore, pWindow]
				{
					REQUIRE( pCore->_titleChangeCount==1 );
					REQUIRE( pCore->_title=="hello" );					
				},
				0	// should NOT cause a sizing info update, since the
					// title is not part of the "preferred size" calculation
				);
		}
	}


	SECTION("multiplePropertyChangesThatInfluenceSizing")
	{
		verifyOp(
			pWindow,

			[pWindow]()
			{
				pWindow->padding() = UiMargin(UiLength::Unit::sem, 7, 8, 9, 10);
				pWindow->padding() = UiMargin(UiLength::Unit::sem, 6, 7, 8, 9);
			},

			[pCore, pWindow]()
			{
				// padding changed twice
				REQUIRE( pCore->_paddingChangeCount==2 );
				REQUIRE( pCore->_padding == UiMargin(UiLength::Unit::sem, 6, 7, 8, 9));
			},

			1	// should cause a single(!) sizing info update

			);		
	}

	SECTION("sizing")
	{
		SECTION("noContentView")
		{
			// the mock window core reports border margins 20,11,12,13
			// and a minimum size of 100x32.		
			// Since we do not have a content view we should get the min size here.
			Size expectedSize(100, 32);

			SECTION("calcPreferredSize")
				REQUIRE( pWindow->calcPreferredSize()==expectedSize );

			SECTION("sizingInfo")
			{

				// sizing info is updated asynchronously. So we need to check async as well.
				MAKE_ASYNC_TEST(10);

				asyncCallFromMainThread(
					[pWindow, expectedSize]()
					{
						View::SizingInfo sizingInfo = pWindow->sizingInfo();

						REQUIRE( sizingInfo.preferredSize == expectedSize );

						END_ASYNC_TEST();
					
					}	);				
			}
		}

		SECTION("withContentView")
		{
			SECTION("calcPreferredSize")
				testSizingWithContentView( pWindow, pUiProvider, [pWindow](){ return pWindow->calcPreferredSize(); } );

			SECTION("sizingInfo")
				testSizingWithContentView( pWindow, pUiProvider, [pWindow](){ return pWindow->sizingInfo().get().preferredSize; } );
		}
	}
		
}



