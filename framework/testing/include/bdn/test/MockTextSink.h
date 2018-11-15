#ifndef BDN_TEST_MockTextSink_H_
#define BDN_TEST_MockTextSink_H_

#include <bdn/ITextSink.h>

#include <bdn/Array.h>

namespace bdn
{
    namespace test
    {

        /** A mock implementation for bdn::ITextSink. This is intended for usage
            in unit tests.*/
        class MockTextSink : public Base, BDN_IMPLEMENTS ITextSink
        {
          public:
            /** Returns the list of text chunks that have been written to the
               text sink. Each write call creates a new entry in this list. The
               writeLine routines add a newline at the end of the written text.
                */
            const Array<String> &getWrittenChunks() const { return _writtenChunks; }

            void write(const String &s) override { _writtenChunks.add(s); }

            void writeLine(const String &s) override { _writtenChunks.add(s + "\n"); }

          private:
            Array<String> _writtenChunks;
        };
    }
}

#endif
