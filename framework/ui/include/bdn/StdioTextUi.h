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
        StdioTextUi(std::basic_istream<CharType> *inStream, std::basic_ostream<CharType> *outStream,
                    std::basic_ostream<CharType> *errStream)
        {
            _inReader = newObj<AsyncStdioReader<CharType>>(inStream);

            _outputSink = newObj<Sink>(outStream);
            _statusOrProblemSink = newObj<Sink>(errStream);
        }

        /** Lets the user enter a line of text. This is done as an asynchronous
            operation - i.e. the function does not wait until the user is done.

            You can use AsyncOp.onDone() to register a handler that is notified
           when the user has entered the text.
            */
        P<IAsyncOp<String>> readLine() override { return _inReader->readLine(); }

        P<ITextSink> statusOrProblem() override { return _statusOrProblemSink; }

        P<ITextSink> output() override { return _outputSink; }

      private:
        P<AsyncStdioReader<CharType>> _inReader;

        class Sink : public Base, BDN_IMPLEMENTS ITextSink
        {
          public:
            Sink(std::basic_ostream<CharType> *stream) : _stream(stream) {}

            void write(const String &s) override
            {
                Mutex::Lock lock(_mutex);

                (*_stream) << s.toLocaleEncoding<CharType>(_stream->getloc());
            }

            void writeLine(const String &s) override
            {
                Mutex::Lock lock(_mutex);

                (*_stream) << s.toLocaleEncoding<CharType>(_stream->getloc()) << std::endl;
            }

          private:
            Mutex _mutex;
            std::basic_ostream<CharType> *_stream;
        };

        P<Sink> _statusOrProblemSink;
        P<Sink> _outputSink;
    };
}

#endif
