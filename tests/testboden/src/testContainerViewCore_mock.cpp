#include <bdn/init.h>
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
    P<TestMockContainerViewCore> test = newObj<TestMockContainerViewCore>();

    test->runTests();
}
