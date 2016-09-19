#include <bdn/init.h>
#include <bdn/android/UiProvider.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ButtonCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/WindowCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>


namespace bdn
{

P<IUiProvider> getPlatformUiProvider()
{
    return &bdn::android::UiProvider::get();
}

}



namespace bdn
{
namespace android
{

BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );


double UiProvider::getSemDips(ViewCore &viewCore)
{
    if (_semDips == -1)
    {
        // sem size is not yet initialized.

        JTextView textView(viewCore.getJView().getContext());
        JTextPaint paint(textView.getPaint());

        double textSize = paint.getTextSize();

        // getTextSize returns the size in real pixels.
        _semDips = textSize / viewCore.getUiScaleFactor();
    }

    return _semDips;
}


String UiProvider::getName() const
{
    return "android";
}

P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );

    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );

    else if(coreTypeName == TextView::getTextViewCoreTypeName() )
        return newObj<TextViewCore>( cast<TextView>(pView) );

    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );

    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}



}
}






