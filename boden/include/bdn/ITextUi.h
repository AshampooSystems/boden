#ifndef BDN_ITextUi_H_
#define BDN_ITextUi_H_

#include <bdn/IAsyncOp.h>

namespace bdn
{
	

/** Provides a way to interact with the user via a text interface.

    The default implementation for the platform can be obtained via textUi().
*/
class ITextUi : BDN_IMPLEMENTS IBase
{
public:

    /** Lets the user enter a line of text. This is done as an asynchronous
        operation - i.e. the function does not wait until the user is done.
        
        You can use AsyncOp.onDone() to register a handler that is notified when
        the user has entered the text.
        */
    virtual P< IAsyncOp<String> > readLine()=0;

    
	/** Writes the specified text (without adding a linebreak).*/
	virtual P< IAsyncOp<void> >  write(const String& s)=0;

	/** Writes the specified line of text. A linebreak is automatically added.*/
	virtual P< IAsyncOp<void> > writeLine(const String& s)=0;


	/** Writes the specified text in a way that suggests an error.

        If the implementation does not distinguish between error output and normal 
        output then this will have the same effect as write().
    
        If the UI implementation works on stdio streams then writeError typically causes the
        text to be written to stderr. */
	virtual P< IAsyncOp<void> > writeError(const String& s)=0;
	
    
	/** Like writeError(), but also writes a line break after the text.*/
	virtual P< IAsyncOp<void> > writeErrorLine(const String& s)=0;	

};


/** Provides the default implementation for text based interaction with the user.

    For commandline(!) applications this is guarenteed to use stdio
    (stdin, stdout, stderr).

    For graphical applications the UI will typically be implemented with graphical UI components / views.

    In any case: the implementation takes care of presenting the text in a way that works
    well for the specific platform.
*/
P<ITextUi> textUi();


}


#endif

