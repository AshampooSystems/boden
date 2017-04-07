#ifndef BDN_AsyncStdioWriter_H_
#define BDN_AsyncStdioWriter_H_

#include <bdn/AsyncOpRunnable.h>
#include <bdn/ThreadPool.h>

namespace bdn
{


template<class CharType>
class AsyncStdioWriterEncodingHelper_
{
public:
    static std::basic_string<CharType> encodeString( const String& s, std::locale loc );
};

template<>
class AsyncStdioWriterEncodingHelper_<char>
{
public:
    static std::basic_string<char> encodeString( const String& s, std::locale loc )
    {
        return s.toLocaleEncoding( loc );
    }
};

template<>
class AsyncStdioWriterEncodingHelper_<wchar_t>
{
public:
    static std::basic_string<wchar_t> encodeString( const String& s, std::locale loc )
    {
        return s.asWide();
    }
};

template<>
class AsyncStdioWriterEncodingHelper_<char16_t>
{
public:
    static std::basic_string<char16_t> encodeString( const String& s, std::locale loc )
    {
        return s.asUtf16();
    }
};

    
template<>
class AsyncStdioWriterEncodingHelper_<char32_t>
{
public:
    static std::basic_string<char32_t> encodeString( const String& s, std::locale loc )
    {
        return s.asUtf32();
    }
};
	

/** Implements asynchronous writing to a std::basic_ostream.
*/
template<typename CharType>
class AsyncStdioWriter : public Base
{
public:
    
    /** Constructor. The implementation does NOT take ownership of the specified streams, 
        i.e. it will not delete it. So it is ok to use std::cin here.*/
    AsyncStdioWriter( std::basic_ostream<CharType>* pStream )
        : _pStream(pStream)
    {
    }


    /** Asynchronously writes text to the stream. No newline is added.

        The function does not wait until the text is actually written - it returns
        immediately.
        
        Use IAsyncOp::onDone() to register a callback that is executed when the
        operation has finished.
        */
    P< IAsyncOp<void> > write(const String& s)
    {
        P<WriteOp> pOp = newObj<WriteOp>(_pStream, s, false);

        {
            MutexLock lock(_mutex);

            // we need a thread with a queue that we can have execute our jobs one by one.
            // We can use a thread pool with a single thread for that.
            if(_pOpExecutor==nullptr)
                _pOpExecutor = newObj<ThreadPool>(1,1);

            _pOpExecutor->addJob( pOp );
        }

        return pOp;
    }


    /** Asynchronously writes a line of text to the stream. Note that it is perfectly ok to
        pass multiline text that contains newlines here. However, note that an additional newline is
        always automatically added at the end.
        
        The stream is also automatically flushed after the text and newline are written. This makes
        writeLine equivalent to this stdio stream code:
        
        \code
        stream << textToWrite << std::endl;
        \endcode

        The function does not wait until the text is actually written - it returns
        immediately.
        
        Use IAsyncOp::onDone() to register a callback that is executed when the
        operation has finished.
        */
    P< IAsyncOp<void> > writeLine(const String& s)
    {
        // we add a linebreak at the end and set flush to true for this.
        // That makes our code work the same as pushing std::endl into the stream.
        P<WriteOp> pOp = newObj<WriteOp>(_pStream, s+"\n", true);

        {
            MutexLock lock(_mutex);

            // we need a thread with a queue that we can have execute our jobs one by one.
            // We can use a thread pool with a single thread for that.
            if(_pOpExecutor==nullptr)
                _pOpExecutor = newObj<ThreadPool>(1,1);

            _pOpExecutor->addJob( pOp );
        }

        return pOp;
    }


private:
    
    class WriteOp : public AsyncOpRunnable<void>
    {
    public:
        WriteOp(std::basic_ostream<CharType>* pStream, const String& textToWrite, bool flush)
            : _pStream(pStream)
            , _textToWrite(textToWrite)
            , _flush(flush)
        {
        }

    protected:        
        void doOp() override
        {
            (*_pStream) << AsyncStdioWriterEncodingHelper_<CharType>::encodeString( _textToWrite, _pStream->getloc());
            if(_flush)
                _pStream->flush();
        }

    private:
        std::basic_ostream<CharType>* _pStream;
        String                        _textToWrite;
        bool                          _flush;
    };


    Mutex                           _mutex;
    std::basic_ostream<CharType>*   _pStream;
    P<ThreadPool>                   _pOpExecutor;
};


}


#endif

