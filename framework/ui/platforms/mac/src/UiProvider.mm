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
            static P<StdioUiProvider<char>> provider(newObj<StdioUiProvider<char>>(&std::cin, &std::cout, &std::cerr));

            return provider;
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

            _layoutCoordinator = newObj<LayoutCoordinator>();
        }

        String UiProvider::getName() const { return "mac"; }

        P<ITextUi> UiProvider::getTextUi()
        {
            {
                Mutex::Lock lock(_textUiInitMutex);
                if (_textUi == nullptr) {
                    // we want the output of the text UI to go to both the
                    // View-based text UI, as well as the stdout/stderr streams.

                    _textUi = newObj<TextUiCombiner>(newObj<ViewTextUi>(),
                                                     newObj<StdioTextUi<char>>(&std::cin, &std::cout, &std::cerr));
                }
            }

            return _textUi;
        }

        P<IViewCore> UiProvider::createViewCore(const String &coreTypeName, View *view)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                return newObj<ContainerViewCore>(cast<ContainerView>(view));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                return newObj<ButtonCore>(cast<Button>(view));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                return newObj<CheckboxCore<Checkbox>>(cast<Checkbox>(view));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                return newObj<SwitchCore<Switch>>(cast<Switch>(view));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                return newObj<CheckboxCore<Toggle>>(cast<Toggle>(view));

            else if (coreTypeName == TextView::getTextViewCoreTypeName())
                return newObj<TextViewCore>(cast<TextView>(view));

            else if (coreTypeName == TextField::getTextFieldCoreTypeName())
                return newObj<TextFieldCore>(cast<TextField>(view));

            else if (coreTypeName == ScrollView::getScrollViewCoreTypeName())
                return newObj<ScrollViewCore>(cast<ScrollView>(view));

            else if (coreTypeName == Window::getWindowCoreTypeName())
                return newObj<WindowCore>(cast<Window>(view));

            else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }
    }
}
