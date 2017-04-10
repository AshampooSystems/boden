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

    _pTextView = newObj<TextView>();
    pContainer->addChildView(_pTextView);

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

    String newText = _pTextView->text() + s;

    _pTextView->text() = newText;
    _pWindow->requestAutoSize();

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


