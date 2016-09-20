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
        // (=DIPs) are assumed to be 96 dpi. So we have to multiple accordingly.
        _semDips = fontSize / 72.0 * 96.0;
    }
    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;
    
    
    double uiLengthToDips(const UiLength& uiLength) const
	{
		if(uiLength.unit==UiLength::sem)
			return uiLength.value * _semDips;

		else if(uiLength.unit==UiLength::dip)
			return uiLength.value;

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
	}
    
    Margin uiMarginToDipMargin(const UiMargin& margin) const
	{
		return Margin(
			uiLengthToDips(margin.top),
			uiLengthToDips(margin.right),
			uiLengthToDips(margin.bottom),
			uiLengthToDips(margin.left) );
	}


	
    static UiProvider& get();
    
private:
    double _semDips;
};

}
}


#endif
