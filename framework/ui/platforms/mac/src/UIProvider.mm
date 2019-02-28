

#include <iostream>

#import <bdn/mac/UIProvider.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/AppRunnerBase.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>

#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/CheckboxCore.hh>
#import <bdn/mac/ContainerViewCore.hh>
#import <bdn/mac/ListViewCore.hh>
#import <bdn/mac/ScrollViewCore.hh>
#import <bdn/mac/StackCore.hh>
#import <bdn/mac/SwitchCore.hh>
#import <bdn/mac/TextFieldCore.hh>
#import <bdn/mac/TextViewCore.hh>
#import <bdn/mac/WindowCore.hh>

namespace bdn
{

    std::shared_ptr<UIProvider> getDefaultUIProvider()
    {
        /*if (getAppRunner()->isCommandLineApp()) {
            static std::shared_ptr<StdioUIProvider<char>> provider(
                std::make_shared<StdioUIProvider<char>>(&std::cin, &std::cout, &std::cerr));

            return provider;
        } else */
        {
            return bdn::mac::UIProvider::get();
        }
    }
}

namespace bdn
{
    namespace mac
    {
        UIProvider::UIProvider()
        {
            // mac uses DIPs natively. So no conversion necessary
            _semDips = NSFont.systemFontSize;

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
        }

        String UIProvider::getName() const { return "mac"; }

        std::shared_ptr<UIProvider> UIProvider::get()
        {
            static std::shared_ptr<UIProvider> globalUIProvider = std::make_shared<UIProvider>();
            return globalUIProvider;
        }
    }
}
