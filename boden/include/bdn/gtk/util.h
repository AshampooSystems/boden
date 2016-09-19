#ifndef BDN_GTK_util_H_
#define BDN_GTK_util_H_

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{

inline int gtkDimensionToPixels(int dim, double scaleFactor)
{
    XXX
    return std::lround(dim*scaleFactor);
}

inline int pixelsToGtkDimension(int pixels, double scaleFactor)
{
    int dim = pixels / scaleFactor;
    if(dim==0 && pixels>0)
        dim = 1;
        
    return dim;
}

inline Rect gtkRectToRect(const GdkRectangle& rect, double scaleFactor )
{
    XXX
	return Rect(
			std::lround(rect.x * scaleFactor),
			std::lround(rect.x * scaleFactor),
			gtkDimensionToPixels(rect.width, scaleFactor),
			gtkDimensionToPixels(rect.height, scaleFactor) );
}

inline GdkRectangle rectToGtkRect(const Rect& rect, double scaleFactor)
{
    XXX
    GdkRectangle result;
    
    result.x = std::lround( rect.x / scaleFactor);
    result.y = std::lround( rect.y / scaleFactor);
    result.width = pixelsToGtkDimension(rect.width, scaleFactor);
    result.height = pixelsToGtkDimension(rect.height, scaleFactor);
    
    return result;
}


inline Size gtkSizeToSize(const GtkRequisition& size, double scaleFactor)
{
    return Size( gtkDimensionToPixels(size.width, scaleFactor),
                 gtkDimensionToPixels(size.height, scaleFactor) );
}


		
}
}


#endif


