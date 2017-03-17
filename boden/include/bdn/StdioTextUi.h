#ifndef BDN_StdioTextUi_H_
#define BDN_StdioTextUi_H_

#include <bdn/ITextUi.h>

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
        basic_ostream<CharType>* pErrStream);


    /** Lets the user enter a line of text. This is done as an asynchronous
        operation - i.e. the function does not wait until the user is done.
        
        You can use AsyncOp.onDone() to register a handler that is notified when
        the user has entered the text.
        */
    AsyncOp<String> readLine()
    {
        {
            MutexLock lock(_mutex);

            if(_pReadThreadRunnable==nullptr)
            {
                _pReadThreadRunnable = newObj<ReadThreadRunnable>(_pInStream);
                _pReadThread = newObj<Thread>( _pReadThreadRunnable );
            }
        }
            
        return _pReadThreadRunnable->readLine();
    }

    
	/** Writes the specified text (without adding a linebreak).*/
	AsyncOp<void> write(const String& s);

	/** Writes the specified line of text. A linebreak is automatically added.*/
	AsyncOp<void> writeLine(const String& s);


	/** Writes the specified text in a way that suggests an error.

        If the implementation does not distinguish between error output and normal 
        output then this will have the same effect as write().
    
        If the UI implementation works on stdio streams then writeError typically causes the
        text to be written to stderr. */
	AsyncOp<void> writeError(const String& s);
	
    
	/** Like writeError(), but also writes a line break after the text.*/
	AsyncOp<void> writeErrorLine(const String& s);	


private:
    class ReadThreadRunnable : public Base, BDN_IMPLEMENTS IThreadRunnable
    {
    public:
        ReadThreadRunnable(basic_istream<CharType>* pInStream)
        {
            _pInStream = pInStream;
        }


        AsyncOp<String> readLine()
        {
            MutexLock lock(_mutex);

            std::promise<String>* pPromise = new std::promise<String>;
            _promiseList.push_back( pPromise );
            _wakeSignal.set();

            return AsyncOp<String>( pPromise->get_future() );
        }
      

        void run()
        {
            while(true)
            {
                _wakeSignal.wait();

                std::promise<String>* pPromise = nullptr;

                {
                    MutexLock lock(_mutex);

                    if(_shouldStop)
                        break;

                    if(_promiseList.empty())
                        _wakeSignal.clear();
                    else
                    {
                        pPromise = _promiseList.front();
                        _promiseList.pop_front();
                    }
                }

                if(pPromise!=nullptr)
                {
                    std::basic_string<CharType> l;

                    try
                    {                    
                        std::getline(*_pInStream, l);
                        pPromise->set_value( decodeString(l, _pInStream->getloc() ) );
                    }
                    catch(...)
                    {
                        pPromise->set_exception( std::current_exception() );
                    }                   

                    delete pPromise;
                }
            }


            // mark all remaining promises as aborted
            while(true)
            {
                std::promise<String>* pPromise = nullptr;

                {
                    MutexLock lock(_mutex);

                    if(_promiseList.empty())
                        break;

                    pPromise = _promiseList.front();
                    _promiseList.pop_front();
                }

                pPromise->set_exception( make_exception_ptr(AbortedError()) );
                delete pPromise;
            }
        }

           
        void signalStop()
        {
            MutexLock lock(_mutex);

            _shouldStop = true;
            _wakeSignal.set();
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


        Mutex  _mutex;
        Signal _wakeSignal;
        bool   _shouldStop = false;
    
        std::list< std::promise<String>* > _promiseList;
    };

};


}


#endif

