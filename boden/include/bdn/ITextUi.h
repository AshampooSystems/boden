#ifndef BDN_ITextUi_H_
#define BDN_ITextUi_H_

#include <bdn/IAsyncOp.h>

namespace bdn
{
	

/** Provides a way to interact with the user via a text interface.

    ITextUi objects are usually obtained from the app's Ui provider (see bdn::IUiProvider).
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
	virtual void write(const String& s)=0;

	/** Writes the specified line of text. A linebreak is automatically added.*/
	virtual void writeLine(const String& s)=0;


	/** Writes the specified text in a way that suggests an error.

        If the implementation does not distinguish between error output and normal 
        output then this will have the same effect as write().
    
        If the UI implementation works on stdio streams then writeError typically causes the
        text to be written to stderr. */
	virtual void writeError(const String& s)=0;
	
    
	/** Like writeError(), but also writes a line break after the text.*/
	virtual void writeErrorLine(const String& s)=0;	

};


}


#endif

