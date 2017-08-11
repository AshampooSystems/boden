#include <bdn/init.h>
#include <bdn/ViewTextUi.h>

#include <bdn/ColumnView.h>
#include <bdn/NotImplementedError.h>


namespace bdn
{

ViewTextUi::ViewTextUi(IUiProvider* pUiProvider)
{
    _pWindow = newObj<Window>(pUiProvider);

    P<ColumnView> pContainer = newObj<ColumnView>();

    _pScrollView = newObj<ScrollView>();
    pContainer->addChildView( _pScrollView );

    _pScrolledColumnView = newObj<ColumnView>();
    pContainer->addChildView( _pScrolledColumnView );
    
    _pWindow->setContentView(pContainer);

    _pWindow->visible() = true;
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
        String para = remaining.splitOffToken("\n", &separator);

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



}


