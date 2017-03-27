#ifndef BDN_AsyncStdioReader_H_
#define BDN_AsyncStdioReader_H_

#include <bdn/AsyncOpRunnable.h>
#include <bdn/ThreadPool.h>

namespace bdn
{
	

/** Implements asynchronous reading from a std::basic_istream.   
*/
template<typename CharType>
class AsyncStdioReader : public Base
{
public:
    
    /** Constructor. The implementation does NOT take ownership of the specified streams, 
        i.e. it will not delete it. So it is ok to use std::cin here.*/
    AsyncStdioReader( std::basic_istream<CharType>* pStream )
        : _pStream(pStream)
    {
    }


    /** Asynchronously reads a line of text from the stream.
        The function does not wait until the line is read - it returns
        immediately.
        
        Use IAsyncOp::onDone() to register a callback that is executed when the
        operation has finished.
        */
    P< IAsyncOp<String> > readLine()
    {
        P<ReadLineOp> pOp = newObj<ReadLineOp>(_pStream);

        {
            MutexLock lock(_mutex);

            // we need a thread with a queue that we can have execute our read jobs one by one.
            // We can use a thread pool with a single thread for that.
            if(_pOpExecutor==nullptr)
                _pOpExecutor = newObj<ThreadPool>(1,1);

            _pOpExecutor->addJob( pOp );
        }

        return pOp;
    }


private:
    
    class ReadLineOp : public AsyncOpRunnable<String>
    {
    public:
        ReadLineOp(std::basic_istream<CharType>* pStream)
            : _pStream(pStream)
        {
        }

    protected:        
        String doOp() override
        {           
            std::basic_string<CharType> l;
            std::getline(*_pStream, l);
            
            return decodeString(l, _pStream->getloc() );
        }

    private:       
        
        template<class EncodedStringType>
        static String decodeString( const EncodedStringType& s, std::locale loc )
        {
            // the locale is not needed for most string types. See below for char specialization.
            return String(s);
        }

        template<>
        static String decodeString< std::basic_string<char> >( const std::basic_string<char>& s, std::locale loc )
        {
            return String::fromLocaleEncoding( s.c_str(), loc );
        }

    private:
        std::basic_istream<CharType>* _pStream;
    };


    Mutex       _mutex;
    std::basic_istream<CharType>* _pStream;
    P<ThreadPool>                 _pOpExecutor;
};


}


#endif

