

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
#import <bdn/mac/ListViewCore.hh>
#import <bdn/mac/TextFieldCore.hh>

namespace bdn
{

    std::shared_ptr<UiProvider> getDefaultUiProvider()
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

            registerCoreType<ButtonCore, Button>();
            registerCoreType<ContainerViewCore, ContainerView>();
            registerCoreType<CheckboxCore, Checkbox>();
            registerCoreType<SwitchCore, Switch>();
            registerCoreType<TextViewCore, TextView>();
            registerCoreType<ScrollViewCore, ScrollView>();
            registerCoreType<WindowCore, Window>();
            registerCoreType<TextFieldCore, TextField>();
            registerCoreType<ListViewCore, ListView>();
        }

        String UiProvider::getName() const { return "mac"; }

        std::shared_ptr<UiProvider> UiProvider::get()
        {
            static std::shared_ptr<UiProvider> globalUiProvider = std::make_shared<UiProvider>();
            return globalUiProvider;
        }
    }
}
