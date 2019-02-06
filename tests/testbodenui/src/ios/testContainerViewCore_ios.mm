
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#import <bdn/ios/UIProvider.hh>
#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosContainerViewCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestContainerViewCore>
{
  protected:
    void initCore() override { TestIosViewCoreMixin<TestContainerViewCore>::initCore(); }
};

TEST_CASE("ios.ContainerViewCore")
{
    std::shared_ptr<TestIosContainerViewCore> test = std::make_shared<TestIosContainerViewCore>();

    test->runTests();
}
