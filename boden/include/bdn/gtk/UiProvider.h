#ifndef BDN_GTK_UiProvider_H_
#define BDN_GTK_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <bdn/log.h>

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider()
    {
        GtkSettings* pSettings = gtk_settings_get_default();
        
        int fontSize = 0;
        
        gchar* pFontName = nullptr;
        g_object_get( pSettings, "gtk-font-name", &pFontName, NULL );
        
        if(pFontName!=nullptr)
        {            
            String fontName(pFontName);
        
            // The font name has the format "NAME SIZE"
            // reverse iterate through the font and find the last number in the name,
            
            int digitFactor = 1;
            String::Iterator it = fontName.end();
            while(it!=fontName.begin())
            {
                it--;
                
                char32_t chr = *it;
                
                if(isdigit(chr))
                {   
                    fontSize += ((int)(chr-'0')) * digitFactor;
                    digitFactor*=10;
                }
                else
                {
                    if(fontSize>0)
                    {
                        // we found a valid number. stop here.
                        break;                    
                    }
                    
                    fontSize = 0;                
                    digitFactor = 1;
                }
            }
        }
        
        if(fontSize<=0)
        {
            // unable to extract a font size. Log an error and use 11 as a default.
            logError("Unable to determine default GTK font size. Using default.");
            
            fontSize = 11;
        }
        
        // the font size is in points, which is "72 DPI". The virtual device coordinates
        // (=scale factor 1) are assumed to be 96 dpi. So we have to multiple accordingly.
        _semSizeAtScaleFactor1 = fontSize / 72.0 * 96.0;
    }
    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;
    
    
    int uiLengthToPixels(const UiLength& uiLength, double scaleFactor) const
	{
		if(uiLength.unit==UiLength::sem)
			return std::lround( uiLength.value * _semSizeAtScaleFactor1 * scaleFactor );

		else if(uiLength.unit==UiLength::dip)
		{
			// See UiLength documentation for more information about the dip unit
			// and why this is correct.
			return std::lround( uiLength.value * scaleFactor );
		}

		else if(uiLength.unit==UiLength::realPixel)
			return std::lround( uiLength.value );

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
	}
    
    
    int uiLengthToPixelsForWidget(GtkWidget* pWidget, const UiLength& uiLength) const
	{
		if(uiLength.unit==UiLength::realPixel)
			return std::lround( uiLength.value );
            
        else
            return uiLengthToPixels(uiLength, gtk_widget_get_scale_factor(pWidget) );
	}
	

	Margin uiMarginToPixelMarginForWidget(GtkWidget* pWidget, const UiMargin& margin) const
	{
        double scaleFactor = gtk_widget_get_scale_factor(pWidget);
		return Margin(
			uiLengthToPixels(margin.top, scaleFactor),
			uiLengthToPixels(margin.right, scaleFactor),
			uiLengthToPixels(margin.bottom, scaleFactor),
			uiLengthToPixels(margin.left, scaleFactor) );
	}


	
    static UiProvider& get();
    
private:
    double _semSizeAtScaleFactor1;
};

}
}


#endif
