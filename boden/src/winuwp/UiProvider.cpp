#include <bdn/init.h>
#include <bdn/winuwp/UiProvider.h>

#include <bdn/winuwp/WindowCore.h>
#include <bdn/winuwp/ScrollViewCore.h>
#include <bdn/winuwp/ContainerViewCore.h>
#include <bdn/winuwp/ButtonCore.h>
#include <bdn/winuwp/TextViewCore.h>
#include <bdn/winuwp/TextFieldCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/ViewTextUi.h>
#include <bdn/StdioTextUi.h>
#include <bdn/TextUiCombiner.h>

namespace bdn
{
    
P<IUiProvider> getDefaultUiProvider()
{
    return &bdn::winuwp::UiProvider::get();
}
    
}


namespace bdn
{
namespace winuwp
{


BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

String UiProvider::getName() const
{
    return "winuwp";
}
    
P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );
    
    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( this, cast<Window>(pView) );

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
                                              newObj< StdioTextUi<wchar_t> >(&std::wcin, &std::wcout, &std::wcerr)  );
        }
    }
    
    return _pTextUi;
}
    
}
}

