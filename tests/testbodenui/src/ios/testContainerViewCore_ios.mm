#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#import <bdn/mac/UiProvider.hh>
#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosContainerViewCore
    : public bdn::test::TestIosViewCoreMixin<bdn::test::TestContainerViewCore>
{
  protected:
    void initCore() override
    {
        TestIosViewCoreMixin<TestContainerViewCore>::initCore();
    }
};

TEST_CASE("ios.ContainerViewCore")
{
    P<TestIosContainerViewCore> pTest = newObj<TestIosContainerViewCore>();

    pTest->runTests();
}
