
#include <bdn/android/UIProvider.h>

#include <bdn/android/ButtonCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ImageViewCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/RowContainerView.h>
#include <bdn/android/ScrollViewCore.h>
#include <bdn/android/StackCore.h>
#include <bdn/android/SwitchCore.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/WindowCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <iostream>

namespace bdn
{
    std::shared_ptr<UIProvider> defaultUIProvider() { return bdn::android::UIProvider::get(); }
}

namespace bdn::android
{
    std::shared_ptr<UIProvider> UIProvider::get()
    {
        static std::shared_ptr<UIProvider> uiProvider = std::make_shared<UIProvider>();
        return uiProvider;
    }

    double UIProvider::getSemSizeDips(bdn::android::ViewCore &core)
    {
        if (_semDips == -1) {
            // sem size is not yet initialized.

            wrapper::TextView textView(core.getJView().getContext());
            wrapper::TextPaint paint(textView.getPaint());

            double textSize = paint.getTextSize();

            // getTextSize returns the size in real pixels.
            _semDips = textSize / core.getUIScaleFactor();
        }

        return _semDips;
    }

    UIProvider::UIProvider()
    {
        _semDips = -1;

        registerCoreType<ButtonCore, Button>();
        registerCoreType<ContainerViewCore, ContainerView>();
        registerCoreType<CheckboxCore, Checkbox>();
        registerCoreType<SwitchCore, Switch>();
        registerCoreType<TextViewCore, TextView>();
        registerCoreType<ScrollViewCore, ScrollView>();
        registerCoreType<WindowCore, Window>();
        registerCoreType<TextFieldCore, TextField>();
        registerCoreType<ListViewCore, ListView>();
        registerCoreType<StackCore, Stack>();
        registerCoreType<ImageViewCore, ImageView>();
        registerCoreType<RowContainerCore, RowContainerView>();
    }

    String UIProvider::getName() const { return "android"; }
}
