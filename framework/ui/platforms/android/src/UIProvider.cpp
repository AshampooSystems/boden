
#include <bdn/android/UIProvider.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ButtonCore.h>
#include <bdn/android/SwitchCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/ScrollViewCore.h>
#include <bdn/android/StackCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <iostream>

namespace bdn
{

    std::shared_ptr<UIProvider> getDefaultUIProvider() { return bdn::android::UIProvider::get(); }
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

        double UIProvider::getSemSizeDips(ViewCore &viewCore)
        {
            if (_semDips == -1) {
                // sem size is not yet initialized.

                JTextView textView(viewCore.getJView().getContext());
                JTextPaint paint(textView.getPaint());

                double textSize = paint.getTextSize();

                // getTextSize returns the size in real pixels.
                _semDips = textSize / viewCore.getUIScaleFactor();
            }

            return _semDips;
        }

        UIProvider::UIProvider()
        {
            _semDips = -1;
            _layoutCoordinator = std::make_shared<LayoutCoordinator>();

            registerAndroidCoreType<ButtonCore, Button>();
            registerAndroidCoreType<ContainerViewCore, ContainerView>();
            registerAndroidCoreType<CheckboxCore, Checkbox>();
            registerAndroidCoreType<SwitchCore, Switch>();
            registerAndroidCoreType<TextViewCore, TextView>();
            registerAndroidCoreType<ScrollViewCore, ScrollView>();
            registerAndroidCoreType<WindowCore, Window>();
            registerAndroidCoreType<TextFieldCore, TextField>();
            registerAndroidCoreType<ListViewCore, ListView>();
            registerAndroidCoreType<StackCore, Stack>();
        }

        String UIProvider::getName() const { return "android"; }
    }
}
