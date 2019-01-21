
#include <bdn/android/UiProvider.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ButtonCore.h>
#include <bdn/android/SwitchCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/TextViewCore.h>
#include <bdn/android/TextFieldCore.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/ScrollViewCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <iostream>

namespace bdn
{

    std::shared_ptr<IUiProvider> getDefaultUiProvider() { return bdn::android::UiProvider::get(); }
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

        String UiProvider::getName() const { return "android"; }

        std::shared_ptr<IViewCore> UiProvider::createViewCore(const String &coreTypeName, std::shared_ptr<View> pView)
        {
            std::shared_ptr<ViewCore> viewCore;

            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                viewCore = std::make_shared<ContainerViewCore>(std::dynamic_pointer_cast<ContainerView>(pView));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                viewCore = std::make_shared<ButtonCore>(std::dynamic_pointer_cast<Button>(pView));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                viewCore = std::make_shared<SwitchCore<Switch>>(std::dynamic_pointer_cast<Switch>(pView));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                viewCore = std::make_shared<SwitchCore<Toggle>>(std::dynamic_pointer_cast<Toggle>(pView));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                viewCore = std::make_shared<CheckboxCore<Checkbox>>(std::dynamic_pointer_cast<Checkbox>(pView));

            else if (coreTypeName == TextView::getTextViewCoreTypeName())
                viewCore = std::make_shared<TextViewCore>(std::dynamic_pointer_cast<TextView>(pView));

            else if (coreTypeName == TextField::getTextFieldCoreTypeName())
                viewCore = std::make_shared<TextFieldCore>(std::dynamic_pointer_cast<TextField>(pView));

            else if (coreTypeName == ScrollView::getScrollViewCoreTypeName())
                viewCore = std::make_shared<ScrollViewCore>(std::dynamic_pointer_cast<ScrollView>(pView));

            else if (coreTypeName == Window::getWindowCoreTypeName())
                viewCore = std::make_shared<WindowCore>(std::dynamic_pointer_cast<Window>(pView));

            else
                throw ViewCoreTypeNotSupportedError(coreTypeName);

            viewCore->getJViewPtr()->setTag(bdn::java::NativeWeakPointer(viewCore));

            return std::static_pointer_cast<IViewCore>(viewCore);
        }
    }
}
