#ifndef BDN_TextSinkStdOStream_H_
#define BDN_TextSinkStdOStream_H_

#include <bdn/TextSinkStdStreamBuf.h>

namespace bdn
{

    /** A std::basic_ostream implementation that forwards the written data
        to an ITextSink object.

        Note that bdn::ITextUi also exposes ITextSink objects, so this class can
       also be used to connect a stdio stream to an ITextUi instance.

        The stream object (like all std::basic_ostream objects) has a locale
       associated with it that controls how the written data is formatted and
       encoded.

        If the CHAR_TYPE template parameter is "char" then the stream uses the
        locale's conversion routines to convert the text data from the locale's
        char multibyte string encoding to unicode strings.

        The initial locale (when imbue() is not called) is a copy of the global
        C++ locale (as constructed by std::locale() ).
    */
    template <typename CHAR_TYPE> class TextSinkStdOStream : public std::basic_ostream<CHAR_TYPE>
    {
      public:
        TextSinkStdOStream(ITextSink *pSink) : std::basic_ostream<CHAR_TYPE>(new TextSinkStdStreamBuf<CHAR_TYPE>(pSink))
        {
            _pStreamBuf = dynamic_cast<TextSinkStdStreamBuf<CHAR_TYPE> *>(this->rdbuf());
        }

        ~TextSinkStdOStream() { delete _pStreamBuf; }

      private:
        TextSinkStdStreamBuf<CHAR_TYPE> *_pStreamBuf;
    };
}

#endif
