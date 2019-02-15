
#include <bdn/test.h>

#include "TestMockViewCoreMixin.h"
#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/MockButtonCore.h>
#include <bdn/test/TestButtonCore.h>

using namespace bdn;

class TestMockButtonCore : public bdn::test::TestMockViewCoreMixin<bdn::test::TestButtonCore>
{
  protected:
    void verifyCoreLabel() override
    {
        String expectedLabel = _button->label;

        String label = std::dynamic_pointer_cast<bdn::test::MockButtonCore>(_mockCore)->getLabel();

        REQUIRE(label == expectedLabel);
    }
};

TEST_CASE("mock.ButtonCore")
{
    std::shared_ptr<TestMockButtonCore> test = std::make_shared<TestMockButtonCore>();

    test->runTests();
}
