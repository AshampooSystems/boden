#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include <bdn/test/MockButtonCore.h>
#include "TestMockViewCoreMixin.h"

using namespace bdn;

class TestMockButtonCore
    : public bdn::test::TestMockViewCoreMixin<bdn::test::TestButtonCore>
{
  protected:
    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        String label = cast<bdn::test::MockButtonCore>(_pMockCore)->getLabel();

        REQUIRE(label == expectedLabel);
    }
};

TEST_CASE("mock.ButtonCore")
{
    P<TestMockButtonCore> pTest = newObj<TestMockButtonCore>();

    pTest->runTests();
}
