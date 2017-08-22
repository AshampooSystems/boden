#ifndef BDN_TextUiStdStreamBuf_H_
#define BDN_TextUiStdStreamBuf_H_

#include <bdn/UtfCodec.h>
#include <bdn/ITextUi.h>
#include <bdn/localeUtil.h>

namespace bdn
{
	
/** A std::basic_streambuf implementation that outputs data to a bdn::ITextUi object.

    This class is rarely used directly - see bdn::TextUiStdIoStream instead.

    The streambuf uses the encoding of the locale that is selected into it with pubimbue()
    to decode the string data to Unicode.

    Special UTF-8 handling
    ----------------------

    There is special handling for UTF-8: the TextUiStdStreamBuf object will detect if
    the multibyte encoding of the locale is UTF-8. If it is then it will use its own
    UTF-8 decoding routines instead of using the locale codec. This is done to improve the
    consistency on different platforms because many C++ standard libraries have buggy
    UTF-8 implementations.

    If the locale uses any other encoding then the TextUiStdStreamBuf will use the
    codec provided by the locale.


*/
template<typename CharType > 
class TextUiStdStreamBuf : public std::basic_streambuf<CharType>
{
public:    
    TextUiStdStreamBuf(ITextUi* pUi )
        : _localeDecodingState( std::mbstate_t() )
    {
        _pUi = pUi;

        setp(_inBuffer, _inBuffer + (sizeof(_inBuffer)/sizeof(CharType)) );
    }

protected:

    int sync() override
    {
        return syncImpl<CharType>();
    }
    
    int_type overflow(int_type c) override
    {
        // write as much from the buffer data to the UI as we can
        sync();

        // if we have a real char then also write that. Note that
        // we have at most 5 bytes left in the buffer after sync,
        // so we know that the additional character will fit and
        // we will not enter an infinite loop.
        if(c!=EOF)
            sputc(c);

        return c;
    }

private:
    template<typename FuncCharType>
    int syncImpl()
    {
        // this branch is used if the char type is not recognized by us.
        // Use the locale provided decoding.
        return syncWithLocaleDecoding<FuncCharType>();
    }
    
    template<>
    int syncImpl<char>()
    {
        // for char we cannot be certain that we have utf-8. It depends on the selected locale.
        
        // If the locale uses UTF-8 then we want to use our own decoding implementation. It ensures that
        // the decoder works properly and consistently on all platforms. This special handling is necessary
        // because the UTF-8 implementation in many C++ standard libraries is buggy.
        if( isUtf8Locale( getloc() ) )
            return utfSync<char>();
        else
            return syncWithLocaleDecoding<char>();
    }

    template<>
    int syncImpl<wchar_t>()
    {
        return utfSync<wchar_t>();
    }

    template<>
    int syncImpl<char16_t>()
    {
        return utfSync<char16_t>();
    }

    template<>
    int syncImpl<char32_t>()
    {
        return utfSync<char32_t>();
    }



    template<class FuncCharType>
    int syncWithLocaleDecoding()
    {
        // This is the generic implementation that depends on the locale to provide a
        // conversion for the data.

        // For these we need the locale to provide the encoding.
        std::locale loc = getloc();
        if( ! std::has_facet< std::codecvt<wchar_t, FuncCharType, mbstate_t> >(loc ) )
        {
            // the locale does not provide a conversion to wchar_t.
            // This is an error.
            throw InvalidArgumentError("TextUiStdStreamBuf used with locale that does not provide a decoding facet to wchar_t.");
        }

        const std::codecvt<wchar_t, FuncCharType, mbstate_t>& codec = std::use_facet< std::codecvt<wchar_t, FuncCharType, mbstate_t> >( loc );
        
        const FuncCharType* pInStart = pbase();
        const FuncCharType* pInEnd = pptr();

        wchar_t* pOutStart = _localeDecodingOutBuffer;
        wchar_t* pOutEnd = _localeDecodingOutBuffer + (sizeof(_localeDecodingOutBuffer)/sizeof(wchar_t)); 

        while(pInStart!=pInEnd)
        {
            const FuncCharType*   pInNext = pInStart;
            wchar_t*              pOutNext = pOutStart;            
        
            // in converts from FuncCharType to wchar_t
            std::codecvt_base::result result = codec.in(
                _localeDecodingState,
                pInStart,
                pInEnd,
                pInNext,
                pOutStart,
                pOutEnd,
                pOutNext );

            if(result==std::codecvt_base::noconv)
            {
                // input data is already char32_t. For consistency, copy it into the out buffer
                pInNext = pInStart;
                pOutNext = pOutStart;
                while(pInNext!=pInEnd && pOutNext!=pOutEnd)                
                {
                    *pOutNext = *pInNext;
                    ++pOutNext;
                    ++pInNext;
                }

                if(pInNext==pInEnd)
                    result = std::codecvt_base::ok;
                else
                    result = std::codecvt_base::partial;
            }

            if(result==std::codecvt_base::error && pOutNext<pOutEnd)
            {
                // we have a broken character in the data.
                // pInNext SHOULD point to it.
                // We do not know how big the broken character is.
                // All we can do here is advance pInNext by one
                // and then try again.
                // We also write 0xfffd character to the output string.
                *pOutNext = 0xfffd;
                ++pOutNext;

                pInStart = pInNext+1;
            }
            else
            {
                // do another decoding iteration.
                pInStart = pInNext;
            }

            if(pOutNext > pOutStart)
            {
                String s(pOutStart, pOutNext-pOutStart);
                _pUi->write(s);
            }
            else
            {
                // if no output was produced then we abort the loop.
                // Note that we always produce at least one character of
                // output if we have a decoding error.
                break;
            }
        }

        // The codecvt SHOULD have consumed all input data. If a partial
        // character was at the end then it SHOULD have copied it into its
        // internal state.
        // However, many codecvt implementations are buggy, so we do not depend
        // on this. If input data remains then we copy it to the start of the buffer.

        // reset the "current" pointer of the buffer to the start
        setp( pbase(), epptr() );

        // copy the unconsumed data to the beginning of the buffer
        while(pInStart<pInEnd)
        {
            sputc(*pInStart);
            pInStart++;
        }    

        return 0;
    }

    
    template<class UtfCharType>
    int utfSync()
    {
        UtfCharType* pInStart = pbase();
        UtfCharType* pInEnd = pptr();


        // we need to decode the written data. So we can only flush full encoded character sequences
        // and need to ignore unfinished sequences at the end. So first we find the end of fully
        // encoded data.
        typename UtfCodec<UtfCharType>::DecodingIterator<UtfCharType*> startIt( pInStart, pInStart, pInEnd);
        typename UtfCodec<UtfCharType>::DecodingIterator<UtfCharType*> endIt( pInEnd, pInStart, pInEnd);

        // Note that the DecodingIterator class of the UTF codecs can iterate backwards.
        // So we use that to find the end of the valid data.

        while(endIt!=startIt)
        {
            // go backwards until we find a decodable character.
            typename UtfCodec<UtfCharType>::DecodingIterator<UtfCharType*> it( endIt );

            --it;
            if(*it!=0xfffd)
            {
                // ok, the last character is decodable => the current
                // endIt is the end of the valid data.
                break;
            }

            int unconsumedElements = pInEnd - it.getInner();
            if(unconsumedElements >= UtfCodec<UtfCharType>::getMaxEncodedElementsPerCharacter() )
            {
                // the "invalid" data at the end is definitely long enough 
                // for an encoded character. That means that the reason the
                // data is invalid is NOT that it is incomplete.
                // In these cases we use the invalid data as is.
                break;
            }

            // set the end to the current position, then try again
            endIt = it;
        }

        if(endIt!=startIt)
        {
            // we have some valid data to decode. Write that.
            String text(startIt, endIt);
            _pUi->write(text);
        }

        UtfCharType* pRemaining = endIt.getInner();
        
        // reset the "current" pointer of the buffer to the start
        setp( pbase(), epptr() );

        // copy the unconsumed data to the beginning of the buffer
        while(pRemaining < pInEnd)
        {
            sputc(*pRemaining);
            pRemaining++;
        }

        return 0;
    }

   
    P<ITextUi>                          _pUi;

    CharType                            _inBuffer[64];
    wchar_t                             _localeDecodingOutBuffer[128];
    std::mbstate_t                      _localeDecodingState;
};


}


#endif

