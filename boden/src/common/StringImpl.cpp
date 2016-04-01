#include <bdn/init.h>


namespace bdn
{
    
 
const size_t StringImplBase::npos;
const size_t StringImplBase::noMatch;
const size_t StringImplBase::toEnd;

  
template<class Codec, class InType, class OutType>
inline int callCodecOut(    Codec& codec,
                            mbstate_t& state,
                            const InType*  pInBegin,
                            const InType*  pInEnd,
                            const InType*& pInNext,
                            OutType*  pOutBegin,
                            OutType*  pOutEnd,
                            OutType*& pOutNext)
{
    int result = codec.out(state, pInBegin, pInEnd, pInNext, pOutBegin, pOutEnd, pOutNext);
    if(result!=std::codecvt_base::error)
    {
        // some buggy codec implementations will return ok for zero characters,
        // but they will not write any data and also will not advance the next pointers.
        if(pInBegin!=pInEnd && pOutBegin!=pOutEnd && pInNext==pInBegin && pOutNext==pOutBegin)
        {
            // just copy the input character over. It is most likely a zero character.
            *pOutNext = *pInNext;
            pOutNext++;
            pInNext++;
        }
    }
    
    return result;
}
    
    
std::string wideToLocaleEncoding(const std::wstring& wideString, const std::locale& loc)
{
	const std::codecvt<wchar_t,char,mbstate_t>& codec = std::use_facet<std::codecvt<wchar_t,char,mbstate_t> >(loc);
	
	int len = wideString.length();
	if(len==0)
		return std::string();
	else
	{
		const wchar_t*		pCurrIn = wideString.c_str();		
		const wchar_t*		pInEnd = pCurrIn+len;
		mbstate_t			state;

		std::string			resultString;
		const int			outBufferSize = MB_LEN_MAX*4;
		char				outBuffer[outBufferSize];

		while(pCurrIn!=pInEnd)
		{
			const wchar_t* pInNext = pCurrIn;
			char*		   pOutNext = outBuffer;
            
			int convResult = callCodecOut(codec, state, pCurrIn, pInEnd, pInNext, outBuffer, outBuffer+outBufferSize, pOutNext);
            
			if(convResult==std::codecvt_base::error)
			{
				// a character cannot be converted. The standard defines that
                // pInNext SHOULD point to that character. And all others up to that point should have been converted.
                
                // But unfortunately with some standard libraries (e.g. on Mac with libc++) pInNext and pOutNext always point to
                // the first character, even if it is not the problem. So to work around that we try to convert character by character
                // until we hit the problem.
                if(pOutNext==outBuffer && pInNext==pCurrIn)
                {
                    while(pInNext!=pInEnd)
                    {
                        convResult = callCodecOut(codec, state, pInNext, pInNext+1, pInNext, pOutNext, outBuffer+outBufferSize, pOutNext);
                        if(convResult!=std::codecvt_base::ok)
                        {
                            // found the actual error. We now know that pInNext and pOutNext really do point to the problem character.
                            break;
                        }
                    }
                }
                
                // add the successfully converted part to the result.
                // That is important, because the replacement char might not fit otherwise.
                if(pOutNext!=outBuffer)
                    resultString.append( outBuffer, pOutNext-outBuffer );
                
                if(pInNext==pInEnd)
                {
                    // character by character conversion was successful. The codec is misbehaving, but we have to assume
                    // that everything is ok now.

                    // So. do nothing.
                }
                else
                {
                    // Insert a replacement character.
                
                    const wchar_t* pReplacement = L"\xfffd";
                    const wchar_t* pReplacementNext = pReplacement;

                    pOutNext = outBuffer;
                    convResult = callCodecOut(codec, state, pReplacement, pReplacement+1, pReplacementNext, outBuffer, outBuffer+outBufferSize, pOutNext);
                    if(convResult!=std::codecvt_base::ok)
                    {
                        // character cannot be represented. Use question mark instead.
                        pReplacement = L"?";
                        pReplacementNext = pReplacement;
                        pOutNext = outBuffer;
                        convResult = callCodecOut( codec, state, pReplacement, pReplacement+1, pReplacementNext, outBuffer, outBuffer+outBufferSize, pOutNext);
                    }
                    
                    if(pOutNext!=outBuffer)
                        resultString.append( outBuffer, pOutNext-outBuffer );
                    
                    // ignore the final replacement conversion result. If the ? character can also not be represented then we just insert nothing.

                    // skip over the problematic input character.
                    pInNext++;
                }
            }
            else
            {
                if(pOutNext!=outBuffer)
                    resultString.append( outBuffer, pOutNext-outBuffer );
            }
           
            pCurrIn = pInNext;
        }
        
        // "unshift", i.e. flush remaining state cleanup data.
		
		char* pOutNext = outBuffer;

		int convResult = codec.unshift(state, outBuffer, outBuffer+outBufferSize, pOutNext);
		// if we cannot unshift then we simply ignore that fact. It should never happen.
		if(convResult!=std::codecvt_base::error && pOutNext!=outBuffer)
			resultString.append( outBuffer, pOutNext-outBuffer );
	
		return resultString;
	}
}


std::wstring localeEncodingToWide(const std::string& multiByte, const std::locale& loc)
{
	const std::codecvt<wchar_t,char,mbstate_t>& codec = std::use_facet<std::codecvt<wchar_t,char,mbstate_t> >(loc);

	int	len = multiByte.length();
	if(len==0)
		return std::wstring();
	else
	{
		const char*			pCurrIn = multiByte.c_str();		
		const char*			pInEnd = pCurrIn+len;
		const char*			pInNext = pCurrIn;
		mbstate_t			state;

		std::wstring		resultString;
		const int			outBufferSize = 16;
		wchar_t				outBuffer[outBufferSize];

		while(pCurrIn!=pInEnd)
		{
			wchar_t*		pOutNext = outBuffer;

			int convResult = codec.in(state, pCurrIn, pInEnd, pInNext, outBuffer, outBuffer+outBufferSize, pOutNext);

			if(pOutNext!=outBuffer)
			{
				// some data has been written.
				resultString.append( outBuffer, pOutNext-outBuffer );
			}

			if(convResult==std::codecvt_base::error)
			{
				// a character cannot be converted. This should be pretty rare. On most systems
				// wchar_t is 32 bit and can hold the whole unicode range. On Windows it is 16 bit,
				// but Windows has no locale encodings that cannot be represented
				// by the 16 bit wchar_t (since wchar_t is the internal kernel encoding).
                
                // Usually we insert a replacement character.
                
                // However, on Macs this error happens when the input is a zero character (zero byte).
                // In that case we simply want to copy the zero character through.
                if(pInNext!=pInEnd && *pInNext==0)
                    resultString += L'\0';
                else
                    resultString += L'\xfffd';

				// now we want to skip over the problematic character. Unfortunately we do not know how big it is.
				// So we skip 1 byte and hope that the codec is able to resynchronize.
				if(pInNext!=pInEnd)
					pInNext++;
			}

			pCurrIn = pInNext;
		}

		// no need to unshift. wchar does not use shifting.
		return resultString;
	}
}


}