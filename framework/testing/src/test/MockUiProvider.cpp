#include <bdn/init.h>
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

        P<IViewCore> MockUiProvider::createViewCore(const String &coreTypeName, View *pView)
        {
            BDN_REQUIRE_IN_MAIN_THREAD();

            if (coreTypeName == ContainerView::getContainerViewCoreTypeName()) {
                _coresCreated++;

                return newObj<MockContainerViewCore>(cast<ContainerView>(pView));
            } else if (coreTypeName == Window::getWindowCoreTypeName()) {
                _coresCreated++;

                return newObj<MockWindowCore>(cast<Window>(pView));
            } else if (coreTypeName == Button::getButtonCoreTypeName()) {
                _coresCreated++;

                return newObj<MockButtonCore>(cast<Button>(pView));
            } else if (coreTypeName == Checkbox::getCheckboxCoreTypeName()) {
                _coresCreated++;

                return newObj<MockCheckboxCore>(cast<Checkbox>(pView));
            } else if (coreTypeName == Toggle::getToggleCoreTypeName()) {
                _coresCreated++;

                return newObj<MockToggleCore>(cast<Toggle>(pView));
            } else if (coreTypeName == Switch::getSwitchCoreTypeName()) {
                _coresCreated++;

                return newObj<MockSwitchCore>(cast<Switch>(pView));
            } else if (coreTypeName == TextView::getTextViewCoreTypeName()) {
                _coresCreated++;

                return newObj<MockTextViewCore>(cast<TextView>(pView));
            } else if (coreTypeName == TextField::getTextFieldCoreTypeName()) {
                _coresCreated++;

                return newObj<MockTextFieldCore>(cast<TextField>(pView));
            } else if (coreTypeName == ScrollView::getScrollViewCoreTypeName()) {
                _coresCreated++;

                return newObj<MockScrollViewCore>(cast<ScrollView>(pView));
            } else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }
    }
}
