#ifndef BDN_GTK_util_H_
#define BDN_GTK_util_H_

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{


inline Rect gtkRectToRect(const GdkRectangle& rect )
{
    // GTK also uses DIPs. So no conversion necessary.
	return Rect(
			rect.x,
			rect.y,
			rect.width,
			rect.height );
}

inline GdkRectangle rectToGtkRect(const Rect& rect)
{
    GdkRectangle result;
    
    result.x = std::lround(rect.x);
    result.y = std::lround(rect.y);
    result.width = std::lround(rect.width);
    result.height = std::lround(rect.height);
    
    return result;
}


inline Size gtkSizeToSize(const GtkRequisition& size)
{
    return Size( size.width,
                 size.height );
}


		
}
}


#endif


