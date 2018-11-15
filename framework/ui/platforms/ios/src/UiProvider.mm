#include <bdn/init.h>

#import <UIKit/UIKit.h>
#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/CheckboxCore.hh>
#import <bdn/ios/SwitchCore.hh>
#import <bdn/ios/TextViewCore.hh>
#import <bdn/ios/TextFieldCore.hh>
#import <bdn/ios/ScrollViewCore.hh>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/TextUiCombiner.h>
#include <bdn/StdioTextUi.h>
#include <bdn/ViewTextUi.h>

namespace bdn
{

    P<IUiProvider> getDefaultUiProvider() { return &bdn::ios::UiProvider::get(); }
}

namespace bdn
{
    namespace ios
    {

        BDN_SAFE_STATIC_IMPL(UiProvider, UiProvider::get);

        UiProvider::UiProvider()
        {
            // iOS uses DIPs for font sizes (although they call it "points").
            // So no conversion necessary

            _semDips = UIFont.systemFontSize;

            _pLayoutCoordinator = newObj<LayoutCoordinator>();
        }

        String UiProvider::getName() const { return "ios"; }

        P<IViewCore> UiProvider::createViewCore(const String &coreTypeName, View *pView)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                return newObj<ContainerViewCore>(cast<ContainerView>(pView));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                return newObj<ButtonCore>(cast<Button>(pView));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                return newObj<CheckboxCore>(cast<Checkbox>(pView));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                return newObj<SwitchCore<Toggle>>(cast<Toggle>(pView));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                return newObj<SwitchCore<Switch>>(cast<Switch>(pView));

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
