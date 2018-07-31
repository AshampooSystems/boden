#include <bdn/init.h>
#include <bdn/ViewTextUi.h>

#include <bdn/ColumnView.h>
#include <bdn/NotImplementedError.h>
#include <bdn/Thread.h>


namespace bdn
{

ViewTextUi::ViewTextUi(IUiProvider* pUiProvider)
{
    _initialized = false;
    _flushPendingScheduled = false;
    _scrollDownPending = false;

    _pUiProvider = pUiProvider;

    if(Thread::isCurrentMain())
    {
        Mutex::Lock lock(_mutex);

        _ensureInitializedWhileMutexLocked();
    }
    else
    {
        P<ViewTextUi> pThis = this;

        asyncCallFromMainThread(
            [this, pThis]()
            {
                Mutex::Lock lock(_mutex);

                _ensureInitializedWhileMutexLocked();
            } );
    }
}

void ViewTextUi::_ensureInitializedWhileMutexLocked()
{
    if(!_initialized)
    {
        _initialized = true;
                
        _pWindow = newObj<Window>(_pUiProvider);

        _pWindow->setPadding( UiMargin(10) );

        _pScrollView = newObj<ScrollView>();

        _pScrolledColumnView = newObj<ColumnView>();

        _pScrolledColumnView->sizeChanged().subscribeParamless( weakMethod(this, &ViewTextUi::scrolledSizeChanged ) );

        _pScrollView->setContentView( _pScrolledColumnView );
    
        _pWindow->setContentView(_pScrollView);

        _pWindow->setPreferredSizeMinimum( Size(600, 400) );

        _pWindow->setVisible( true );

        _pWindow->requestAutoSize();
        _pWindow->requestCenter();

        // it may be that we get called with many small writes in a short period of time.
        // In these cases we do NOT want to update the UI immediately on each write -- that would
        // trigger a layout update that can introduce considerable overhead if it happens too often.
        // Instead we use a timer and update the UI at most 10 times per second.
        getMainDispatcher()->createTimer( 0.1, weakMethod(this, &ViewTextUi::timerCallback) );        
    }
}



P< IAsyncOp<String> > ViewTextUi::readLine()
{
    throw NotImplementedError("ViewTextUi::readLine");
}


void ViewTextUi::write(const String& s)
{
    Mutex::Lock lock(_mutex);

    // we used to update the UI immediately here (if we were on the main thread).
    // However, for performance reasons we now batch writes together within 100ms
    // time window. So here we just add the string to our pending list. Our timer
    // will take care of updating the UI when the next tick happens.
    _pendingList.add( s );

    /*if(Thread::isCurrentMain())
    {
        // we want the ordering of multithreaded writes to be honored.
        // So we have to make sure that any pending writes from other threads
        // are made before we do any writes in the main thread directly.

        _flushPendingWhileMutexLocked();

        _doWriteWhileMutexLocked(s);
    }
    else
    {
        if(!_flushPendingScheduled)
        {
            P<ViewTextUi> pThis = this;

            _flushPendingScheduled = true;
            asyncCallFromMainThread(
                [this, pThis]()
                {                    
                    Mutex::Lock lock( _mutex );
                    _flushPendingWhileMutexLocked();
                } );            
        }
    }*/
}
    
void ViewTextUi::writeLine(const String& s)
{
    write(s+"\n");
}

bool ViewTextUi::timerCallback()
{
    Mutex::Lock lock(_mutex);

    _flushPendingWhileMutexLocked();

    // we never stop ourselves. The timer will automatically destroy itself when the ViewTextUi
    // object is destroyed (since the timer uses a weak method, which will throw a DanglingFunctionError
    // in that case).
    return true;
}

void ViewTextUi::_doWriteWhileMutexLocked(const String& s)
{
    _ensureInitializedWhileMutexLocked();

    String remaining = s;
    // normalize linebreaks
    remaining.findAndReplace("\r\n", "\n");

    while(!remaining.isEmpty())
    {
        char32_t separator=0;
        String para = remaining.splitOffToken("\n", true, &separator);

        if(_pCurrParagraphView==nullptr)
        {
            _pCurrParagraphView = newObj<TextView>();
            _pScrolledColumnView->addChildView(_pCurrParagraphView);
        }

        _pCurrParagraphView->setText( _pCurrParagraphView->text() + para );

        if(separator!=0)
        {
            // linebreak was found => finish current paragraph.
            _pCurrParagraphView = nullptr;
        }
    }    
}

void ViewTextUi::_flushPendingWhileMutexLocked()
{    
    _flushPendingScheduled = false;

    for( auto& s: _pendingList )
        _doWriteWhileMutexLocked(s);
    _pendingList.clear();
}


void ViewTextUi::scrolledSizeChanged()
{
    // when the scrolled size has just changed then it may be that the
    // view itself has not yet updates its internal scrolling parameters.
    // So if we scroll down immediately then "all the way down" may not yet
    // reflect the new size.
    // So instead we post this asynchronously to the main thread event queue.
    // That way the scrolling down should happen after the view was updated.

    // if we already have a scroll request pending then we do not need to do schedule
    // another one.    
    if(!_scrollDownPending)
    {
        _scrollDownPending = true;

        // keep ourselves alive.
        P<ViewTextUi> pThis = this;

        asyncCallFromMainThread(
            [pThis, this]()
            {
                 // we want to scroll to the end of the client area.
                // scrollClientRectToVisible supports the infinity value
                // to scroll to the end, so we just use that.
                Rect rect( 0, std::numeric_limits<double>::infinity(), 0, 0 );    

                _scrollDownPending = false;

                _pScrollView->scrollClientRectToVisible(rect);                
            } );   
    }
}

}


