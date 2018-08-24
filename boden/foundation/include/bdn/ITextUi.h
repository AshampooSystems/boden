#ifndef BDN_ITextUi_H_
#define BDN_ITextUi_H_

#include <bdn/IAsyncOp.h>
#include <bdn/ITextSink.h>

namespace bdn
{
	

/** Provides a way to interact with the user via a text interface.

    ITextUi objects are usually obtained from the app's Ui provider (see bdn::IUiProvider).
 
    ITextUi has two kinds of output functions: those for status, diagnostic and problem messages like warnings, errors,
    etc. (writeStatusOrProblem()) and those for the app's "output" text (writeOutput()). These two kinds of of output functions correspond roughly
    to the two C stdio streams stderr and stdout. In fact, depending on the type of app, the text UI might forward
    the texts to these stdio streams.
 
    ITextUi implementations must be thread safe.
*/
class ITextUi : BDN_IMPLEMENTS IBase
{
public:

    /** Lets the user enter a line of text. This is done as an asynchronous
        operation - i.e. the function does not wait until the user is done.
        
        You can use AsyncOp.onDone() to register a handler that is notified when
        the user has entered the text.

        This function is thread safe.
        */
    virtual P< IAsyncOp<String> > readLine()=0;
    
    
    /** Returns the text sink for status messages, diagnostic information, error
        messages and warning messages.
     
        This text sink roughly corresponds to the stderr stdio stream.

        Some implementations might return the same text sink as output() here, if they
        do not distinguish between output and status information.

        This function and the returned text sink are thread safe. When multiple threads
        write text with any of the sink's write functions then
        it is guaranteed that the final ordering of the individual output text blocks
        is the same as the order of the corresponding write calls.
     */
    virtual P<ITextSink> statusOrProblem()=0;
    
    
    /** Returns the text sink for output text.

        Output text is text that is considered to be the result of the app.
        Status messages, diagnostic information or error/warning messages should
        be written to the text sinks returned by statusOrProblem().
     
        This text sink roughly corresponds to the stdout stdio stream.
     
        Some implementations might return the same text sink as statusOrProblem() here, if they
        do not distinguish between output and status information.
     
        This function and the returned text sink are thread safe. When multiple threads
        write text with any of the sink's write functions then
        it is guaranteed that the final ordering of the individual output text blocks
        is the same as the order of the corresponding write calls.
     */
    virtual P<ITextSink> output()=0;
    
    

};


}


#endif

