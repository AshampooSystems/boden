
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Checkbox.h>
#include <bdn/test/TestCheckboxCore.h>
#include <bdn/mac/CheckboxCore.hh>
#include <bdn/mac/util.hh>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacCheckboxCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestCheckboxCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestCheckboxCore>::initCore();

        _nSButton = (NSButton *)_nSView;
        REQUIRE(_nSButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _checkbox->label;

        String label = bdn::mac::macStringToString(_nSButton.title);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreState() override
    {
        TriState expectedState = _checkbox->state;
        TriState state = bdn::mac::nsControlStateValueToTriState(_nSButton.state);
        REQUIRE(state == expectedState);
    }

  protected:
    NSButton *_nSButton;
};

TEST_CASE("mac.CheckboxCore")
{
    std::shared_ptr<TestMacCheckboxCore> test = std::make_shared<TestMacCheckboxCore>();

    test->runTests();
}
