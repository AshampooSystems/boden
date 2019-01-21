
#include <bdn/test/MockUiProvider.h>
#include <bdn/test.h>

#include <bdn/test/MockContainerViewCore.h>
#include <bdn/test/MockWindowCore.h>
#include <bdn/test/MockButtonCore.h>
#include <bdn/test/MockCheckboxCore.h>
#include <bdn/test/MockToggleCore.h>
#include <bdn/test/MockSwitchCore.h>
#include <bdn/test/MockTextViewCore.h>
#include <bdn/test/MockTextFieldCore.h>
#include <bdn/test/MockScrollViewCore.h>

namespace bdn
{
    namespace test
    {

        std::shared_ptr<IViewCore> MockUiProvider::createViewCore(const String &coreTypeName,
                                                                  std::shared_ptr<View> view)
        {
            BDN_REQUIRE_IN_MAIN_THREAD();

            if (coreTypeName == ContainerView::getContainerViewCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockContainerViewCore>(std::dynamic_pointer_cast<ContainerView>(view));
            } else if (coreTypeName == Window::getWindowCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockWindowCore>(std::dynamic_pointer_cast<Window>(view));
            } else if (coreTypeName == Button::getButtonCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockButtonCore>(std::dynamic_pointer_cast<Button>(view));
            } else if (coreTypeName == Checkbox::getCheckboxCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockCheckboxCore>(std::dynamic_pointer_cast<Checkbox>(view));
            } else if (coreTypeName == Toggle::getToggleCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockToggleCore>(std::dynamic_pointer_cast<Toggle>(view));
            } else if (coreTypeName == Switch::getSwitchCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockSwitchCore>(std::dynamic_pointer_cast<Switch>(view));
            } else if (coreTypeName == TextView::getTextViewCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockTextViewCore>(std::dynamic_pointer_cast<TextView>(view));
            } else if (coreTypeName == TextField::getTextFieldCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockTextFieldCore>(std::dynamic_pointer_cast<TextField>(view));
            } else if (coreTypeName == ScrollView::getScrollViewCoreTypeName()) {
                _coresCreated++;

                return std::make_shared<MockScrollViewCore>(std::dynamic_pointer_cast<ScrollView>(view));
            } else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }
    }
}
