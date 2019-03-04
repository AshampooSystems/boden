
#include <bdn/android/UIProvider.h>

#include <bdn/android/ButtonCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ImageViewCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/ScrollViewCore.h>
#include <bdn/android/StackCore.h>
#include <bdn/android/SwitchCore.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/WebViewCore.h>
#include <bdn/android/WindowCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <iostream>

namespace bdn
{

    std::shared_ptr<UIProvider> defaultUIProvider() { return bdn::android::UIProvider::get(); }
}

namespace bdn
{
    namespace android
    {

        std::shared_ptr<UIProvider> UIProvider::get()
        {
            static std::shared_ptr<UIProvider> uiProvider = std::make_shared<UIProvider>();
            return uiProvider;
        }

        double UIProvider::getSemSizeDips(ViewCore &core)
        {
            if (_semDips == -1) {
                // sem size is not yet initialized.

                JTextView textView(core.getJView().getContext());
                JTextPaint paint(textView.getPaint());

                double textSize = paint.getTextSize();

                // getTextSize returns the size in real pixels.
                _semDips = textSize / core.getUIScaleFactor();
            }

            return _semDips;
        }

        UIProvider::UIProvider()
        {
            _semDips = -1;

            registerAndroidCoreType<ButtonCore, Button>();
            registerAndroidCoreType<ContainerViewCore, ContainerView>();
            registerAndroidCoreType<CheckboxCore, Checkbox>();
            registerAndroidCoreType<SwitchCore, Switch>();
            registerAndroidCoreType<TextViewCore, TextView>();
            registerAndroidCoreType<ScrollViewCore, ScrollView>();
            registerAndroidCoreType<WindowCore, Window>();
            registerAndroidCoreType<WebViewCore, WebView>();
            registerAndroidCoreType<TextFieldCore, TextField>();
            registerAndroidCoreType<ListViewCore, ListView>();
            registerAndroidCoreType<StackCore, Stack>();
            registerAndroidCoreType<ImageViewCore, ImageView>();
        }

        String UIProvider::getName() const { return "android"; }
    }
}
