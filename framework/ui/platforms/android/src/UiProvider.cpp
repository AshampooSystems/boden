
#include <bdn/android/UiProvider.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ButtonCore.h>
#include <bdn/android/SwitchCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/ScrollViewCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <iostream>

namespace bdn
{

    std::shared_ptr<UiProvider> getDefaultUiProvider() { return bdn::android::UiProvider::get(); }
}

namespace bdn
{
    namespace android
    {

        std::shared_ptr<UiProvider> UiProvider::get()
        {
            static std::shared_ptr<UiProvider> uiProvider = std::make_shared<UiProvider>();
            return uiProvider;
        }

        double UiProvider::getSemSizeDips(ViewCore &viewCore)
        {
            if (_semDips == -1) {
                // sem size is not yet initialized.

                JTextView textView(viewCore.getJView().getContext());
                JTextPaint paint(textView.getPaint());

                double textSize = paint.getTextSize();

                // getTextSize returns the size in real pixels.
                _semDips = textSize / viewCore.getUiScaleFactor();
            }

            return _semDips;
        }

        UiProvider::UiProvider()
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
        }

        String UiProvider::getName() const { return "android"; }
    }
}
