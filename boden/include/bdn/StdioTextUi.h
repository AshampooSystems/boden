#ifndef BDN_StdioTextUi_H_
#define BDN_StdioTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/AsyncStdioReader.h>
#include <bdn/AsyncStdioWriter.h>

namespace bdn
{
	

/** ITextUi implementation that uses stdio streams (stdin, stdout, stderr).
*/
template<typename CharType>
class StdioTextUi : public Base, BDN_IMPLEMENTS ITextUi
{
public:
    
    /** Constructor. The implementation does NOT take ownership of the specified streams, 
        i.e. it will not delete them. So it is ok to use std::cin, std::cout and/or std::cerr.*/
    StdioTextUi(
        basic_istream<CharType>* pInStream,
        basic_ostream<CharType>* pOutStream,
        basic_ostream<CharType>* pErrStream)
    {
        _pInReader = newObj<AsyncStdioReader>(pInStream);
        _pOutWriter = newObj<AsyncStdioWriter>(pOutStream);
        _pErrWriter = newObj<AsyncStdioWriter>(pErrStream);
    }


    /** Lets the user enter a line of text. This is done as an asynchronous
        operation - i.e. the function does not wait until the user is done.
        
        You can use AsyncOp.onDone() to register a handler that is notified when
        the user has entered the text.
        */
    P< IAsyncOp<String> > readLine()
    {
        return _pInReader->readLine();
    }

    
	/** Writes the specified text (without adding a linebreak).*/
	P< IAsyncOp<void> > write(const String& s)
    {
        return _pOutWriter->write(s);
    }

	/** Writes the specified line of text. A linebreak is automatically added.*/
	P< IAsyncOp<void> > writeLine(const String& s)
    {
        return _pOutWriter->writeLine(s);
    }


	/** Writes the specified text in a way that suggests an error.

        If the implementation does not distinguish between error output and normal 
        output then this will have the same effect as write().
    
        If the UI implementation works on stdio streams then writeError typically causes the
        text to be written to stderr. */
	AsyncOp<void> writeError(const String& s)
    {
        return _pErrWriter->write(s);
    }
	
    
	/** Like writeError(), but also writes a line break after the text.*/
	AsyncOp<void> writeErrorLine(const String& s)
    {
        return _pErrWriter->writeLine(s);
    }


private:
    P<AsyncStdioReader> _pInReader;
    P<AsyncStdioWriter> _pOutWriter;
    P<AsyncStdioWriter> _pErrWriter;
};


}


#endif

