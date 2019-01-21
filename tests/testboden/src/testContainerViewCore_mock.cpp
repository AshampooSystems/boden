
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/test/MockUiProvider.h>
#include "TestMockViewCoreMixin.h"

using namespace bdn;

class TestMockContainerViewCore : public bdn::test::TestMockViewCoreMixin<bdn::test::TestContainerViewCore>
{
  protected:
    void initCore() override { TestMockViewCoreMixin<TestContainerViewCore>::initCore(); }
};

TEST_CASE("mock.ContainerViewCore")
{
    std::shared_ptr<TestMockContainerViewCore> test = std::make_shared<TestMockContainerViewCore>();

    test->runTests();
}
