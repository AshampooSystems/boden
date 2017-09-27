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

    
void ViewTextUi::write(const String& s)
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
}


void ViewTextUi::writeLine(const String& s)
{
    write(s+"\n");
}


void ViewTextUi::writeError(const String& s)
{
    write(s);
}
	
    
void ViewTextUi::writeErrorLine(const String& s)
{
    writeLine(s);
}


void ViewTextUi::scrolledSizeChanged()
{
    // we want to scroll to the end of the client area.
    // scrollClientRectToVisible supports the infinity value
    // to scroll to the end, so we just use that.
    Rect rect( 0, std::numeric_limits<double>::infinity(), 0, 0 );    

    _pScrollView->scrollClientRectToVisible(rect);
}

}


