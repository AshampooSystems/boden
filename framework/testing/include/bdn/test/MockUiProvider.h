#pragma once

#include <bdn/UiProvider.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    namespace test
    {

        /** A Ui provider that implements a "fake" UI that does not actually
           show anything visible, but behaves otherwise like a "real" UI.

            All the UI elements created by the provider also record some
           statistics about the calls that are made on them and they allow
           access to their internal state. This is useful for UI testing.

            The MockUiProvider is intended for UI tests. You can specify the UI
           provider to the top level #Window instance you use. Pass a
           MockUiProvider instance there and your UI code will run in a "fake"
           UI world that you can examine and manipulate for your tests. That
           allows you to test your application UI easily without having to
           modify the UI code (except for passing the UI provider to the top
           level window).

            Example:

            \code

            std::shared_ptr<bdn::test::MockUiProvider> uiProvider =
           std::make_shared<bdn::test::MockUiProvider>();

            std::shared_ptr<Window> myWindow = std::make_shared<Window>( uiProvider );

            // set up your UI as normal here.
            ...

            // You can access the mock UI core elements by calling
           View::getViewCore() and casting
            // to the appropriate Mock class.
            // For example:

            std::shared_ptr<Button> button = std::make_shared<Button>();
            button->label == "MyLabel";
            myWindow->setContentView(button);     // this will cause the
           button core to be created

            std::shared_ptr<MockButtonCore> buttonCore = std::dynamic_pointer_cast<MockButtonCore>(
           button->getViewCore() );

            // you can then verify the properties of the core like this:
            REQUIRE( buttonCore->getLabel() == "MyLabel" );

            // various statistics are also available
            REQUIRE( buttonCore->getLabelChangeCount() == 1);

            \endcore
            */
        class MockUiProvider : public Base, virtual public bdn::UiProvider
        {
          public:
            MockUiProvider();

            String getName() const override { return "mock"; }

            /** Returns the number of cores that the UI provider has created.*/
            int getCoresCreated() const { return _coresCreated; }

            /** Returns the layout coordinator object that all UI objects
               created by this provider share.*/
            std::shared_ptr<LayoutCoordinator> getLayoutCoordinator() { return _layoutCoordinator; }

            template <class CoreType, class ViewType>
            std::shared_ptr<IViewCore> makeMockCore(std::shared_ptr<View> view)
            {
                _coresCreated++;
                return std::make_shared<CoreType>(std::dynamic_pointer_cast<ViewType>(view));
            }

            template <class CoreType, class ViewType> void registerMockCoreType()
            {
                registerConstruction(
                    ViewType::coreTypeName,
                    std::bind(&MockUiProvider::makeMockCore<CoreType, ViewType>, this, std::placeholders::_1));
            }

          protected:
            int _coresCreated = 0;

            std::shared_ptr<LayoutCoordinator> _layoutCoordinator;
        };
    }
}
