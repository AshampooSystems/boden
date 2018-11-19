#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/test/MockUiProvider.h>
#include <bdn/test/MockWindowCore.h>
#include "TestMockViewCoreMixin.h"

using namespace bdn;

class TestMockWindowCore : public bdn::test::TestMockViewCoreMixin<bdn::test::TestWindowCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMockViewCoreMixin<bdn::test::TestWindowCore>::initCore();

        _mockWindowCore = cast<bdn::test::MockWindowCore>(_mockCore);
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _window->title();

        REQUIRE(_mockWindowCore->getTitle() == expectedTitle);
    }

    void clearAllReferencesToCore() override
    {
        TestMockViewCoreMixin<TestWindowCore>::clearAllReferencesToCore();

        _mockWindowCore = nullptr;
    }

    P<IBase> createInfoToVerifyCoreUiElementDestruction() override { return nullptr; }

    void verifyCoreUiElementDestruction(IBase *verificationInfo) override {}

    P<bdn::test::MockWindowCore> _mockWindowCore;
};

TEST_CASE("mock.WindowCore")
{
    P<TestMockWindowCore> test = newObj<TestMockWindowCore>();

    test->runTests();
}
