#ifndef BDN_StdioTextUi_H_
#define BDN_StdioTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/Window.h>
#include <bdn/ScrollView.h>
#include <bdn/TextView.h>
#include <bdn/ColumnView.h>
#include <bdn/OneShotStateNotifier.h>


namespace bdn
{	

/** ITextUi implementation that uses graphical bdn::View components.
    
    The UI implementation automatically creates a new top level Window
    when it is initialized.
*/
class ViewTextUi : public Base, BDN_IMPLEMENTS ITextUi
{
public:
    
    /** @param pUiProvider the UI provider that the text UI should use.
		See the IUiProvider documentation for more information.
		If this is nullptr then the default UI provider for the platform is used.*/
	ViewTextUi(IUiProvider* pUiProvider = nullptr);
    
    P< IAsyncOp<String> > readLine() override;
    
	P< IAsyncOp<void> > write(const String& s) override;
	P< IAsyncOp<void> > writeLine(const String& s) override;

	P< IAsyncOp<void> > writeError(const String& s) override;
	P< IAsyncOp<void> > writeErrorLine(const String& s) override;


private:
    class WriteOp : public Base, BDN_IMPLEMENTS IAsyncOp<void>
    {
    public:          
        WriteOp()
        {            
            _pDoneNotifier = newObj< OneShotStateNotifier< P<IAsyncOp<void>> > >();
        }

        void getResult() const
        {
            // cannot fail
        }

        void signalStop()
        {
            // do nothing - cannot be aborted
        }

        bool isDone() const
        {
            // done immediately
            return true;
        }

        
        INotifier< P<IAsyncOp> >& onDone() const
        {
            return *_pDoneNotifier;
        }

    protected:
        P< OneShotStateNotifier< P<IAsyncOp<void>> > > _pDoneNotifier;
    };

    Mutex           _mutex;
    P< Window >     _pWindow;
    P< ScrollView > _pScrollView;
    P< ColumnView > _pScrolledColumnView;
    P< TextView >   _pCurrParagraphView;
};


}


#endif

