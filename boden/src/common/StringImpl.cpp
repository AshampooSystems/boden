#include <bdn/init.h>


namespace bdn
{

std::string wideToLocaleEncoding(const std::wstring& wideString, const std::locale& loc)
{
	const std::codecvt<wchar_t,char,mbstate_t>& codec = std::use_facet<std::codecvt<wchar_t,char,mbstate_t> >(loc);
	
	int					len = wideString.length();
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

		int					replacementIndex=-1;

		// out replacement list
		const wchar_t*		pReplacements = L"\ufffd?";

		while(true)
		{
			const wchar_t* pSource;
			const wchar_t* pSourceEnd;

			if(replacementIndex>=0)
			{
				pSource = pReplacements+replacementIndex;
				if(*pSource==0)
				{
					// none of our replacements can be encoded.
					// Just skip it entirely,.
					replacementIndex=-1;
					continue;
				}

				pSourceEnd = pSource+1;
			}
			else
			{
				if(pCurrIn==pInEnd)
					break;

				pSource = pCurrIn;
				pSourceEnd = pInEnd;
			}

			const wchar_t* pSourceNext = pSource;
			char*		   pOutNext = outBuffer;

			int convResult = codec.out(state, pSource, pSourceEnd, pSourceNext, outBuffer, outBuffer+outBufferSize, pOutNext);
			if(convResult==std::codecvt_base::error)
			{
				// a character cannot be converted. pWideNext points to that character.
				// All others up to that point have been converted. Insert a replacement character
				// and skip over the problematic character.
				if(replacementIndex==-1)
					pSourceNext++;

				replacementIndex++;
			}
			else
				replacementIndex = -1;

			if(pSource==pCurrIn)
				pCurrIn = pSourceNext;

			if(pOutNext!=outBuffer)
			{
				// some data has been written.
				resultString.append( outBuffer, pOutNext-outBuffer );
			}
		}
		
		char* pOutNext = outBuffer;

		int convResult = codec.unshift(state, outBuffer, outBuffer+outBufferSize, pOutNext);
		// if we cannot unshift then we simply ignore that fact. It should never happen.
		if(convResult!=std::codecvt_base::error)
			resultString.append( outBuffer, pOutNext-outBuffer );
	
		return resultString;
	}
}


std::wstring localeEncodingToWide(const std::string& multiByte, const std::locale& loc)
{
	const std::codecvt<wchar_t,char,mbstate_t>& codec = std::use_facet<std::codecvt<wchar_t,char,mbstate_t> >(loc);

	int					len = multiByte.length();
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
				// but to my knowledge it has no locale encodings that cannot be represented
				// by the 16 bit wchar_t.

				// Insert a replacement character
				resultString += L'\ufffd';

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