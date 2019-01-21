#pragma once

namespace bdn
{

    /** An interface for objects that receive text.
     */
    class ITextSink
    {
      public:
        /** Writes the text, without adding a linebreak.
         */
        virtual void write(const String &s) = 0;

        /** Like write(), but also writes a line break after the text.
            The text being written can also contain additional line breaks - it
           does not have to be a single line.
        */
        virtual void writeLine(const String &s) = 0;
    };
}
