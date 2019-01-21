#pragma once

#include <bdn/ITextSink.h>

#include <vector>

namespace bdn
{
    namespace test
    {

        /** A mock implementation for bdn::ITextSink. This is intended for usage
            in unit tests.*/
        class MockTextSink : public Base, virtual public ITextSink
        {
          public:
            /** Returns the list of text chunks that have been written to the
               text sink. Each write call creates a new entry in this list. The
               writeLine routines add a newline at the end of the written text.
                */
            const std::vector<String> &getWrittenChunks() const { return _writtenChunks; }

            void write(const String &s) override { _writtenChunks.push_back(s); }

            void writeLine(const String &s) override { _writtenChunks.push_back(s + "\n"); }

          private:
            std::vector<String> _writtenChunks;
        };
    }
}
