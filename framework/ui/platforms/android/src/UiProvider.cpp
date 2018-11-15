#include <bdn/init.h>
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
#include <bdn/StdioTextUi.h>
#include <bdn/TextUiCombiner.h>
#include <bdn/ViewTextUi.h>

namespace bdn
{

    P<IUiProvider> getDefaultUiProvider() { return &bdn::android::UiProvider::get(); }
}

namespace bdn
{
    namespace android
    {

        BDN_SAFE_STATIC_IMPL(UiProvider, UiProvider::get);

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

        P<IViewCore> UiProvider::createViewCore(const String &coreTypeName, View *pView)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                return newObj<ContainerViewCore>(cast<ContainerView>(pView));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                return newObj<ButtonCore>(cast<Button>(pView));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                return newObj<SwitchCore<Switch>>(cast<Switch>(pView));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                return newObj<SwitchCore<Toggle>>(cast<Toggle>(pView));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                return newObj<CheckboxCore<Checkbox>>(cast<Checkbox>(pView));

            else if (coreTypeName == TextView::getTextViewCoreTypeName())
                return newObj<TextViewCore>(cast<TextView>(pView));

            else if (coreTypeName == TextField::getTextFieldCoreTypeName())
                return newObj<TextFieldCore>(cast<TextField>(pView));

            else if (coreTypeName == ScrollView::getScrollViewCoreTypeName())
                return newObj<ScrollViewCore>(cast<ScrollView>(pView));

            else if (coreTypeName == Window::getWindowCoreTypeName())
                return newObj<WindowCore>(cast<Window>(pView));

            else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }

        P<ITextUi> UiProvider::getTextUi()
        {
            {
                Mutex::Lock lock(_textUiInitMutex);
                if (_pTextUi == nullptr) {
                    // we want the output of the text UI to go to both the
                    // View-based text UI, as well as the stdout/stderr streams.

                    _pTextUi = newObj<TextUiCombiner>(newObj<ViewTextUi>(),
                                                      newObj<StdioTextUi<char>>(&std::cin, &std::cout, &std::cerr));
                }
            }

            return _pTextUi;
        }
    }
}
