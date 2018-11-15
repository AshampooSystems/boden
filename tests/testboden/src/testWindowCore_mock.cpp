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

        _pMockWindowCore = cast<bdn::test::MockWindowCore>(_pMockCore);
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        REQUIRE(_pMockWindowCore->getTitle() == expectedTitle);
    }

    void clearAllReferencesToCore() override
    {
        TestMockViewCoreMixin<TestWindowCore>::clearAllReferencesToCore();

        _pMockWindowCore = nullptr;
    }

    P<IBase> createInfoToVerifyCoreUiElementDestruction() override { return nullptr; }

    void verifyCoreUiElementDestruction(IBase *pVerificationInfo) override {}

    P<bdn::test::MockWindowCore> _pMockWindowCore;
};

TEST_CASE("mock.WindowCore")
{
    P<TestMockWindowCore> pTest = newObj<TestMockWindowCore>();

    pTest->runTests();
}
