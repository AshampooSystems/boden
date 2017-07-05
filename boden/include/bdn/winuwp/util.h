#ifndef BDN_WINUWP_util_H_
#define BDN_WINUWP_util_H_

#include <limits>

#include <bdn/Rect.h>

namespace bdn
{
namespace winuwp
{


/** Converts a UWP float width or height value to double.

    If the value is infinity or nan then the result is also infinity.
*/
inline double uwpDimensionToDouble(float val)
{
    if(std::isfinite(val))
        return static_cast<double>(val);
	else if( std::isnan(val) )
        return std::numeric_limits<double>::quiet_nan();
    else
        return std::numeric_limits<double>::infinity();
}

inline float doubleToUwpDimension(double val)
{
    if(std::isfinite(val))
    {
        if(val<0)	// UWP cannot represent negative dimension. It will yield an assertion
		    return 0;
        else
            return static_cast<float>(val);
    }
	else if( std::isnan(val) )
        return std::numeric_limits<float>::quiet_nan();
    else
        return std::numeric_limits<float>::infinity();
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


/** Converts a UWP size to a Boden size. Infinity and nan values will pass
    pass through unchanged (they will remain infinity/nan in the output size)
    */
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


