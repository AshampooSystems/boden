#include <bdn/init.h>
#include <bdn/android/UiProvider.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ButtonCore.h>
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


double UiProvider::getSemPixelsForViewCore(ViewCore &viewCore)
{
    if (_semPixelsAtScaleFactor1 == -1)
    {
        // sem size is not yet initialized.

        JTextView textView(viewCore.getJView().getContext());
        JTextPaint paint(textView.getPaint());

        double textSize = paint.getTextSize();

        double uiScaleFactor = viewCore.getUiScaleFactor();

        _semPixelsAtScaleFactor1 = textSize / uiScaleFactor;
    }

    return _semPixelsAtScaleFactor1 * viewCore.getUiScaleFactor();
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

    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );

    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}



}
}






