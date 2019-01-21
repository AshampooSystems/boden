

#include <iostream>

#import <bdn/mac/UiProvider.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/AppRunnerBase.h>

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

    std::shared_ptr<IUiProvider> getDefaultUiProvider()
    {
        /*if (getAppRunner()->isCommandLineApp()) {
            static std::shared_ptr<StdioUiProvider<char>> provider(
                std::make_shared<StdioUiProvider<char>>(&std::cin, &std::cout, &std::cerr));

            return provider;
        } else */
        {
            return bdn::mac::UiProvider::get();
        }
    }
}

namespace bdn
{
    namespace mac
    {
        UiProvider::UiProvider()
        {
            // mac uses DIPs natively. So no conversion necessary
            _semDips = NSFont.systemFontSize;

            _layoutCoordinator = std::make_shared<LayoutCoordinator>();
        }

        String UiProvider::getName() const { return "mac"; }

        std::shared_ptr<UiProvider> UiProvider::get()
        {
            static std::shared_ptr<UiProvider> globalUiProvider = std::make_shared<UiProvider>();
            return globalUiProvider;
        }

        std::shared_ptr<IViewCore> UiProvider::createViewCore(const String &coreTypeName, std::shared_ptr<View> view)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName()) {
                return std::make_shared<ContainerViewCore>(std::dynamic_pointer_cast<ContainerView>(view));

            } else if (coreTypeName == Button::getButtonCoreTypeName()) {
                return std::make_shared<ButtonCore>(std::dynamic_pointer_cast<Button>(view));
            } else if (coreTypeName == Checkbox::getCheckboxCoreTypeName()) {
                return std::make_shared<CheckboxCore<Checkbox>>(std::dynamic_pointer_cast<Checkbox>(view));
            } else if (coreTypeName == Switch::getSwitchCoreTypeName()) {
                return std::make_shared<SwitchCore<Switch>>(std::dynamic_pointer_cast<Switch>(view));
            } else if (coreTypeName == Toggle::getToggleCoreTypeName()) {
                return std::make_shared<CheckboxCore<Toggle>>(std::dynamic_pointer_cast<Toggle>(view));
            } else if (coreTypeName == TextView::getTextViewCoreTypeName()) {
                return std::make_shared<TextViewCore>(std::dynamic_pointer_cast<TextView>(view));
            } else if (coreTypeName == TextField::getTextFieldCoreTypeName()) {
                auto core = std::make_shared<TextFieldCore>(std::dynamic_pointer_cast<TextField>(view));
                core->init(std::dynamic_pointer_cast<TextField>(view));
                return core;
            } else if (coreTypeName == ScrollView::getScrollViewCoreTypeName()) {
                return std::make_shared<ScrollViewCore>(std::dynamic_pointer_cast<ScrollView>(view));
            } else if (coreTypeName == Window::getWindowCoreTypeName()) {
                return std::make_shared<WindowCore>(std::dynamic_pointer_cast<Window>(view));
            } else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }
    }
}
