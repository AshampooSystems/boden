#ifndef BDN_TEST_MockTextUi_H_
#define BDN_TEST_MockTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/NotImplementedError.h>

#include <bdn/test/MockTextSink.h>

namespace bdn
{
    namespace test
    {

        /** A mock implementation for bdn::ITextUi. This is intended for usage
            in unit tests.*/
        class MockTextUi : public Base, BDN_IMPLEMENTS ITextUi
        {
          public:
            MockTextUi()
            {
                _statusOrProblemSink = newObj<MockTextSink>();
                _outputSink = newObj<MockTextSink>();
            }

            /** Returns the list of text chunks that have been written to the
               output text sink. Each write call creates a new entry in this
               list. The writeLine routines add a newline at the end of the
               written text.
                */
            const Array<String> &getWrittenOutputChunks() const { return _outputSink->getWrittenChunks(); }

            /** Returns the list of text chunks that have been written to the
               statusOrProblem text sink. Each write call creates a new entry in
               this list. The writeLine routines add a newline at the end of the
               written text.
                */
            const Array<String> &getWrittenStatusOrProblemChunks() const
            {
                return _statusOrProblemSink->getWrittenChunks();
            }

            P<IAsyncOp<String>> readLine() override { throw NotImplementedError("MockTextUi::readLine"); }

            P<ITextSink> statusOrProblem() override { return _statusOrProblemSink; }

            P<ITextSink> output() override { return _outputSink; }

          private:
            P<MockTextSink> _outputSink;
            P<MockTextSink> _statusOrProblemSink;
        };
    }
}

#endif
