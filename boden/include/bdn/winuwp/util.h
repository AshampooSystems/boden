#ifndef BDN_WINUWP_util_H_
#define BDN_WINUWP_util_H_

#include <limits>

#include <bdn/Rect.h>

namespace bdn
{
namespace winuwp
{

inline double uwpDimensionToDouble(float val)
{
	if(std::isnan(val) || val==std::numeric_limits<float>::infinity() )
		return std::numeric_limits<double>::max();
	else
        return val;
}

inline float doubleToUwpDimension(double val)
{
	if(val==std::numeric_limits<double>::max() )
		return std::numeric_limits<float>::infinity();
	
	else if(val<0)	// UWP cannot represent negative dimension. It will yield an assertion
		return 0;

	else
		return static_cast<float>(val);
}

inline Rect uwpRectToRect(const ::Windows::Foundation::Rect& rect)
{
    // UWP also uses DIPs as their unit. So no conversion necessary
	return Rect( rect.X, rect.Y, uwpDimensionToDouble(rect.Width), uwpDimensionToDouble(rect.Height) );
}

inline ::Windows::Foundation::Rect rectToUwpRect(const Rect& rect)
{
    // UWP also uses DIPs as their unit. So no conversion necessary

	return ::Windows::Foundation::Rect(
		static_cast<float>(rect.x),
		static_cast<float>(rect.y),
		doubleToUwpDimension(rect.width),
		doubleToUwpDimension(rect.height) );
}


inline Size uwpSizeToSize(const ::Windows::Foundation::Size& size)
{
	return Size( uwpDimensionToDouble(size.Width),
				 uwpDimensionToDouble(size.Height) );
}


inline ::Windows::Foundation::Size sizeToUwpSize(const Size& size)
{
    // UWP also uses DIPs as their unit. So no conversion necessary

	return ::Windows::Foundation::Size(
		doubleToUwpDimension(size.width),
		doubleToUwpDimension(size.height) );
}

		
}
}


#endif


