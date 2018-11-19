#ifndef BDN_AsyncStdioWriter_H_
#define BDN_AsyncStdioWriter_H_

#include <bdn/AsyncOpRunnable.h>
#include <bdn/ThreadPool.h>

namespace bdn
{

    /** Implements asynchronous writing to a std::basic_ostream.
     */
    template <typename CharType> class AsyncStdioWriter : public Base
    {
      public:
        /** Constructor. The implementation does NOT take ownership of the
           specified streams, i.e. it will not delete it. So it is ok to use
           std::cin here.*/
        AsyncStdioWriter(std::basic_ostream<CharType> *stream) : _stream(stream) {}

        /** Asynchronously writes text to the stream. No newline is added.

            The function does not wait until the text is actually written - it
           returns immediately.

            Use IAsyncOp::onDone() to register a callback that is executed when
           the operation has finished.
            */
        P<IAsyncOp<void>> write(const String &s)
        {
            P<WriteOp> op = newObj<WriteOp>(_stream, s, false);

#if BDN_HAVE_THREADS
            {
                Mutex::Lock lock(_mutex);

                // we need a thread with a queue that we can have execute our
                // jobs one by one. We can use a thread pool with a single
                // thread for that.
                if (_opExecutor == nullptr)
                    _opExecutor = newObj<ThreadPool>(1, 1);

                _opExecutor->addJob(op);
            }
#else
            // we have to run the operation synchronously. We have to trust that
            // this will not take too long (since we are on the only thread).

            op->run();
#endif

            return op;
        }

        /** Asynchronously writes a line of text to the stream. Note that it is
           perfectly ok to pass multiline text that contains newlines here.
           However, note that an additional newline is always automatically
           added at the end.

            The stream is also automatically flushed after the text and newline
           are written. This makes writeLine equivalent to this stdio stream
           code:

            \code
            stream << textToWrite << std::endl;
            \endcode

            The function does not wait until the text is actually written - it
           returns immediately.

            Use IAsyncOp::onDone() to register a callback that is executed when
           the operation has finished.
            */
        P<IAsyncOp<void>> writeLine(const String &s)
        {
            // we add a linebreak at the end and set flush to true for this.
            // That makes our code work the same as pushing std::endl into the
            // stream.
            P<WriteOp> op = newObj<WriteOp>(_stream, s + "\n", true);

#if BDN_HAVE_THREADS
            {
                Mutex::Lock lock(_mutex);

                // we need a thread with a queue that we can have execute our
                // jobs one by one. We can use a thread pool with a single
                // thread for that.
                if (_opExecutor == nullptr)
                    _opExecutor = newObj<ThreadPool>(1, 1);

                _opExecutor->addJob(op);
            }
#else
            // see write() for information on why this is ok.
            op->run();
#endif

            return op;
        }

      private:
        class WriteOp : public AsyncOpRunnable<void>
        {
          public:
            WriteOp(std::basic_ostream<CharType> *stream, const String &textToWrite, bool flush)
                : _stream(stream), _textToWrite(textToWrite), _flush(flush)
            {}

          protected:
            void doOp() override
            {
                (*_stream) << _textToWrite.toLocaleEncoding<CharType>(_stream->getloc());
                if (_flush)
                    _stream->flush();
            }

          private:
            std::basic_ostream<CharType> *_stream;
            String _textToWrite;
            bool _flush;
        };

        Mutex _mutex;
        std::basic_ostream<CharType> *_stream;

#if BDN_HAVE_THREADS
        P<ThreadPool> _opExecutor;
#endif
    };
}

#endif
