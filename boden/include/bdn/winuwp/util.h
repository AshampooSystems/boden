#ifndef BDN_WINUWP_util_H_
#define BDN_WINUWP_util_H_

#include <limits>

namespace bdn
{
namespace winuwp
{

inline int uwpDimensionToInt(float val, double scaleFactor)
{
	if(std::isnan(val) || val==std::numeric_limits<float>::infinity() )
		return std::numeric_limits<int>::max();
	else
		return static_cast<int>( std::ceil(val*scaleFactor) );
}

inline float intToUwpDimension(int val, double scaleFactor)
{
	if(val==std::numeric_limits<int>::max() )
		return std::numeric_limits<float>::infinity();
	
	else if(val<0)	// UWP cannot represent negative dimension. It will yield an assertion
		return 0;

	else
		return static_cast<float>(val / scaleFactor);
}

inline Rect uwpRectToRect(const ::Windows::Foundation::Rect& rect, double scaleFactor)
{
	return Rect(
			std::lround(rect.X * scaleFactor),
			std::lround(rect.Y * scaleFactor),
			uwpDimensionToInt(rect.Width, scaleFactor),
			uwpDimensionToInt(rect.Height, scaleFactor) );
}

inline ::Windows::Foundation::Rect rectToUwpRect(const Rect& rect, double scaleFactor)
{
	return ::Windows::Foundation::Rect(
		static_cast<float>(rect.x / scaleFactor),
		static_cast<float>(rect.y / scaleFactor),
		intToUwpDimension(rect.width, scaleFactor),
		intToUwpDimension(rect.height, scaleFactor) );
}


inline Size uwpSizeToSize(const ::Windows::Foundation::Size& size, double scaleFactor)
{
	return Size( uwpDimensionToInt(size.Width, scaleFactor),
				 uwpDimensionToInt(size.Height, scaleFactor) );
}

		
}
}


#endif


