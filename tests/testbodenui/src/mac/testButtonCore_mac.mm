#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacButtonCore
    : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestButtonCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<
            bdn::test::TestButtonCore>::initCore();

        _pNSButton = (NSButton *)_pNSView;
        REQUIRE(_pNSButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        String label = bdn::mac::macStringToString(_pNSButton.title);

        REQUIRE(label == expectedLabel);
    }

  protected:
    NSButton *_pNSButton;
};

TEST_CASE("mac.ButtonCore")
{
    P<TestMacButtonCore> pTest = newObj<TestMacButtonCore>();

    pTest->runTests();
}
