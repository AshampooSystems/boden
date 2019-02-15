
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/Window.h>
#include <bdn/mac/CheckboxCore.hh>
#include <bdn/mac/ui_util.hh>
#include <bdn/mac/util.hh>
#include <bdn/test/TestCheckboxCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacCheckboxCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestCheckboxCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestCheckboxCore>::initCore();

        _nsButton = (NSButton *)_nsView;
        REQUIRE(_nsButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _checkbox->label;

        String label = bdn::mac::nsStringToString(_nsButton.title);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreState() override
    {
        TriState expectedState = _checkbox->state;
        TriState state = bdn::mac::nsControlStateValueToTriState(_nsButton.state);
        REQUIRE(state == expectedState);
    }

  protected:
    NSButton *_nsButton;
};

TEST_CASE("mac.CheckboxCore")
{
    std::shared_ptr<TestMacCheckboxCore> test = std::make_shared<TestMacCheckboxCore>();

    test->runTests();
}
