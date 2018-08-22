#include <bdn/init.h>
#include <bdn/gtk/UiProvider.h>

#include <bdn/gtk/WindowCore.h>
#include <bdn/gtk/ContainerViewCore.h>
#include <bdn/gtk/ButtonCore.h>
#include <bdn/gtk/CheckboxCore.h>
#include <bdn/gtk/SwitchCore.h>
#include <bdn/gtk/TextViewCore.h>
#include <bdn/gtk/TextFieldCore.h>
#include <bdn/gtk/ScrollViewCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/IAppRunner.h>
#include <bdn/StdioUiProvider.h>
#include <bdn/TextUiCombiner.h>
#include <bdn/ViewTextUi.h>

namespace bdn
{

P<IUiProvider> getDefaultUiProvider()
{
    if( getAppRunner()->isCommandLineApp() )
    {
        static P< StdioUiProvider<char> > pProvider( newObj< StdioUiProvider<char> >(&std::cin, &std::cout, &std::cerr) );

        return pProvider;
    }
    else
        return &bdn::gtk::UiProvider::get();
}

}


namespace bdn
{
namespace gtk
{


BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );


UiProvider::UiProvider()
{
    _pLayoutCoordinator = newObj<LayoutCoordinator>();

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


String UiProvider::getName() const
{
    return "gtk";
}

P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );

    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );

    else if(coreTypeName == Checkbox::getCheckboxCoreTypeName() )
        return newObj<CheckboxCore<Checkbox>>( cast<Checkbox>(pView) );

    else if(coreTypeName == Toggle::getToggleCoreTypeName() )
        return newObj<CheckboxCore<Toggle>>( cast<Toggle>(pView) );

    else if(coreTypeName == Switch::getSwitchCoreTypeName() )
        return newObj<SwitchCore<Switch>>( cast<Switch>(pView) );

    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );

    else if(coreTypeName == TextView::getTextViewCoreTypeName() )
        return newObj<TextViewCore>( cast<TextView>(pView) );

    else if(coreTypeName == TextField::getTextFieldCoreTypeName() )
        return newObj<TextFieldCore>( cast<TextField>(pView) );
        
    else if(coreTypeName == ScrollView::getScrollViewCoreTypeName() )
        return newObj<ScrollViewCore>( cast<ScrollView>(pView) );

    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}

P<ITextUi> UiProvider::getTextUi()
{
    {
        Mutex::Lock lock( _textUiInitMutex );
        if(_pTextUi==nullptr)
        {
            // we want the output of the text UI to go to both the
            // View-based text UI, as well as the stdout/stderr streams.
            
            _pTextUi = newObj<TextUiCombiner>(
                                              newObj< ViewTextUi >(),
                                              newObj< StdioTextUi<char> >( &std::cin, &std::cout, &std::cerr ) );
        }
    }

    return _pTextUi;
}


}
}

