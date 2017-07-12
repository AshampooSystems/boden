#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>

namespace bdn
{

Window::Window(IUiProvider* pUiProvider)
{
	// windows are invisible by default
	_visible = false;

	_pUiProvider = (pUiProvider!=nullptr) ? pUiProvider : getDefaultUiProvider().getPtr();

	initProperty<String, IWindowCore, &IWindowCore::setTitle, (int)PropertyInfluence_::none>(_title);

	reinitCore();
}

Window::~Window()
{
    // if we have a content view, detach it from us.
    setContentView(nullptr);
}

void Window::requestAutoSize()
{
    P<IWindowCore> pCore = cast<IWindowCore>( getViewCore() );

    // forward the request to the core. Depending on the platform
    // it may be that the UI uses a layout coordinator provided by the system,
    // rather than our own.
    if(pCore!=nullptr)
        pCore->requestAutoSize();	
}

void Window::requestCenter()
{
	// Since autosizing is asynchronous, this must also be done via
	// the layout coordination system. Otherwise we might center with the old size
	// and would then autoSize afterwards.
    // So, also forward this to the core.

    P<IWindowCore> pCore = cast<IWindowCore>( getViewCore() );

    if(pCore!=nullptr)
        pCore->requestCenter();	
}


Size Window::calcPreferredSize( const Size& availableSpace ) const
{
    // XXX
    BDN_DEBUGGER_PRINT( String(typeid(*this).name())+".calcPreferredSize("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n"  );

	// lock the mutex so that our child hierarchy or core does not change during measuring
	MutexLock lock( getHierarchyAndCoreMutex() );
	
	P<IWindowCore> pCore = cast<IWindowCore>( getViewCore() );

	if(pCore==nullptr)
	{
		// cannot calculate anything without a core.
		return Size(0,0);
	}

    Margin contentMargin;
	P<const View>	pContentView = getContentView();
	if(pContentView!=nullptr)
		contentMargin = pContentView->uiMarginToDipMargin( _pContentView->margin() );

	Margin myPadding = getDipPadding();

    // combine maxSize with availableSpace
    Size maxSize = preferredSizeMaximum();
    maxSize.applyMaximum(availableSpace);

    bool widthConstrained = std::isfinite(maxSize.width);
    bool heightConstrained = std::isfinite(maxSize.height);

	Size availableContentAreaSize( Size::none() );
	if(widthConstrained || heightConstrained)
	{
		// subtract size of window border, titlebar, etc.
		availableContentAreaSize = pCore->calcContentAreaSizeFromWindowSize(
			Size(widthConstrained ? maxSize.width : 10000,
				 heightConstrained ? maxSize.height : 10000) );

		// subtract window padding
		availableContentAreaSize-=myPadding;
		// subtract margin of content view
		availableContentAreaSize-=contentMargin;

		if(availableContentAreaSize.width<0)
			availableContentAreaSize.width = 0;
		if(availableContentAreaSize.height<0)
			availableContentAreaSize.height = 0;

		if(!widthConstrained)
			availableContentAreaSize.width = Size::componentNone();
		if(!heightConstrained)
			availableContentAreaSize.height = Size::componentNone();
	}
		
	Size contentSize;
	if(pContentView!=nullptr)
		contentSize = pContentView->calcPreferredSize( availableContentAreaSize );

	Size contentAreaSize = contentSize + myPadding + contentMargin;
	
    // the core will round up here if any adjustments for the current display are needed.
	Size preferredSize = pCore->calcWindowSizeFromContentAreaSize( contentAreaSize );


    // apply minimum size constraint (the maximum constraint has already been applied above)
    preferredSize.applyMinimum( preferredSizeMinimum() );

    // also apply the core's minimm size (the implementation defined minimum size that all windows
    // must at least have)
    preferredSize.applyMinimum( pCore->getMinimumSize() );


    // also apply the preferredSizeMaximum. We already applied it at the start to
    // take the constraint into account from the beginning, but it may be that prefSize
    // is bigger than the max here because the content window does not fit.
    // So we clip the result against the max here, because we never want it to be exceeded.
    // Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
    // if the children do not fit.
    preferredSize.applyMaximum( preferredSizeMaximum() );


    // XXX
    BDN_DEBUGGER_PRINT( "/"+String(typeid(*this).name())+".calcPreferredSize()\n" );

	return preferredSize;
}



Margin Window::getDipPadding() const
{
    Margin myPadding;
    
    P<IViewCore> pCore = getViewCore();
    if(pCore!=nullptr)
    {
        // default padding is zero
        Nullable<UiMargin> pad = padding();
        if(!pad.isNull())
            myPadding = getViewCore()->uiMarginToDipMargin( pad );
    }
        
    return myPadding;
}



}


