#ifndef BDN_StdioTextUi_H_
#define BDN_StdioTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/AsyncStdioReader.h>
#include <bdn/AsyncStdioWriter.h>

namespace bdn
{

    /** ITextUi implementation that uses stdio streams (stdin, stdout, stderr).
     */
    template <typename CharType> class StdioTextUi : public Base, BDN_IMPLEMENTS ITextUi
    {
      public:
        /** Constructor. The implementation does NOT take ownership of the
           specified streams, i.e. it will not delete them. So it is ok to use
           std::cin, std::cout and/or std::cerr.*/
        StdioTextUi(std::basic_istream<CharType> *pInStream, std::basic_ostream<CharType> *pOutStream,
                    std::basic_ostream<CharType> *pErrStream)
        {
            _pInReader = newObj<AsyncStdioReader<CharType>>(pInStream);

            _pOutputSink = newObj<Sink>(pOutStream);
            _pStatusOrProblemSink = newObj<Sink>(pErrStream);
        }

        /** Lets the user enter a line of text. This is done as an asynchronous
            operation - i.e. the function does not wait until the user is done.

            You can use AsyncOp.onDone() to register a handler that is notified
           when the user has entered the text.
            */
        P<IAsyncOp<String>> readLine() override { return _pInReader->readLine(); }

        P<ITextSink> statusOrProblem() override { return _pStatusOrProblemSink; }

        P<ITextSink> output() override { return _pOutputSink; }

      private:
        P<AsyncStdioReader<CharType>> _pInReader;

        class Sink : public Base, BDN_IMPLEMENTS ITextSink
        {
          public:
            Sink(std::basic_ostream<CharType> *pStream) : _pStream(pStream) {}

            void write(const String &s) override
            {
                Mutex::Lock lock(_mutex);

                (*_pStream) << s.toLocaleEncoding<CharType>(_pStream->getloc());
            }

            void writeLine(const String &s) override
            {
                Mutex::Lock lock(_mutex);

                (*_pStream) << s.toLocaleEncoding<CharType>(_pStream->getloc()) << std::endl;
            }

          private:
            Mutex _mutex;
            std::basic_ostream<CharType> *_pStream;
        };

        P<Sink> _pStatusOrProblemSink;
        P<Sink> _pOutputSink;
    };
}

#endif
