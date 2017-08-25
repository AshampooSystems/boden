#include <bdn/init.h>
#include <bdn/ViewTextUi.h>

#include <bdn/ColumnView.h>
#include <bdn/NotImplementedError.h>


namespace bdn
{

ViewTextUi::ViewTextUi(IUiProvider* pUiProvider)
{
    _pWindow = newObj<Window>(pUiProvider);

    _pWindow->padding() = UiMargin(10);

    _pScrollView = newObj<ScrollView>();

    _pScrolledColumnView = newObj<ColumnView>();

    _pScrolledColumnView->size().onChange().subscribeParamless( weakMethod(this, &ViewTextUi::scrolledSizeChanged ) );

    _pScrollView->setContentView( _pScrolledColumnView );
    
    _pWindow->setContentView(_pScrollView);

    _pWindow->preferredSizeMinimum() = Size(600, 400);

    _pWindow->visible() = true;

    _pWindow->requestAutoSize();
    _pWindow->requestCenter();
}

P< IAsyncOp<String> > ViewTextUi::readLine()
{
    throw NotImplementedError("ViewTextUi::readLine");
}

    
P< IAsyncOp<void> > ViewTextUi::write(const String& s)
{
    MutexLock lock(_mutex);

    String remaining = s;
    // normalize linebreaks
    remaining.findReplace("\r\n", "\n");

    while(!remaining.isEmpty())
    {
        char32_t separator=0;
        String para = remaining.splitOffToken("\n", true, &separator);

        if(_pCurrParagraphView==nullptr)
        {
            _pCurrParagraphView = newObj<TextView>();
            _pScrolledColumnView->addChildView(_pCurrParagraphView);
        }

        _pCurrParagraphView->text() = _pCurrParagraphView->text().get() + para;

        if(separator!=0)
        {
            // linebreak was found => finish current paragraph.
            _pCurrParagraphView = nullptr;
        }
    }
    
    P<WriteOp> pOp = newObj<WriteOp>();
    // immediately done
    pOp->onDone().postNotification( pOp );

    return pOp;
}


P< IAsyncOp<void> > ViewTextUi::writeLine(const String& s)
{
    return write(s+"\n");
}


P< IAsyncOp<void> > ViewTextUi::writeError(const String& s)
{
    return write(s);
}
	
    
P< IAsyncOp<void> > ViewTextUi::writeErrorLine(const String& s)
{
    return writeLine(s);
}


void ViewTextUi::scrolledSizeChanged()
{
    // this is called when the size of the scrolled text view changed (i.e. when the layout of the scrollview
    // has been updated after text was written).
    // When this happens then we want to scroll down to the bottom.
    Size size = _pScrolledColumnView->size();

    // make sure the last pixel line of the scroll view is visible
    Rect rect( 0, size.height-1, 0, 1 );

    // the scrollview has no padding, so the scrolled columnview size is
    // exactly the size of the scrolled area

    _pScrollView->scrollAreaToVisible(rect);
}

}


