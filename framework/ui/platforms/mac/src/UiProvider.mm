#include <bdn/init.h>

#import <bdn/mac/UiProvider.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/IAppRunner.h>
#include <bdn/StdioUiProvider.h>
#include <bdn/TextUiCombiner.h>
#include <bdn/StdioTextUi.h>
#include <bdn/ViewTextUi.h>

#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/CheckboxCore.hh>
#import <bdn/mac/SwitchCore.hh>
#import <bdn/mac/TextViewCore.hh>
#import <bdn/mac/ContainerViewCore.hh>
#import <bdn/mac/ScrollViewCore.hh>
#import <bdn/mac/TextFieldCore.hh>

namespace bdn
{

    P<IUiProvider> getDefaultUiProvider()
    {
        if (getAppRunner()->isCommandLineApp()) {
            static P<StdioUiProvider<char>> pProvider(newObj<StdioUiProvider<char>>(&std::cin, &std::cout, &std::cerr));

            return pProvider;
        } else {
            return &bdn::mac::UiProvider::get();
        }
    }
}

namespace bdn
{
    namespace mac
    {

        BDN_SAFE_STATIC_IMPL(UiProvider, UiProvider::get);

        UiProvider::UiProvider()
        {
            // mac uses DIPs natively. So no conversion necessary
            _semDips = NSFont.systemFontSize;

            _pLayoutCoordinator = newObj<LayoutCoordinator>();
        }

        String UiProvider::getName() const { return "mac"; }

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

        P<IViewCore> UiProvider::createViewCore(const String &coreTypeName, View *pView)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                return newObj<ContainerViewCore>(cast<ContainerView>(pView));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                return newObj<ButtonCore>(cast<Button>(pView));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                return newObj<CheckboxCore<Checkbox>>(cast<Checkbox>(pView));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                return newObj<SwitchCore<Switch>>(cast<Switch>(pView));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                return newObj<CheckboxCore<Toggle>>(cast<Toggle>(pView));

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
    }
}
