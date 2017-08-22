#ifndef BDN_TextUiStdOStream_H_
#define BDN_TextUiStdOStream_H_

#include <bdn/TextUiStdStreamBuf.h>


namespace bdn
{
	

/** A std::basic_ostream implementation that uses an ITextUi object to write
    data.

    The stream object (like all std::basic_ostream objects) has a locale associated
    with it that controls how the written data is formatted and encoded.

    If the CharType template parameter is "char" then the stream uses the
    locale's conversion routines to convert the text data from the locale's
    char multibyte string encoding to unicode strings.

    The initial locale (when imbue() is not called) is a copy of the global
    C++ locale (as constructed by std::locale() ).
*/
template<typename CharType >
class TextUiStdOStream : public std::basic_ostream<CharType>
{
public:
    TextUiStdOStream(ITextUi* pUi)
        : _streamBuf(pUi)
        , std::basic_ostream<CharType>(&_streamBuf)
    {
    }

private:
    TextUiStdStreamBuf<CharType> _streamBuf;
};



}


#endif

