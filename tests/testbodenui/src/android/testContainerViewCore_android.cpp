
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/android/UIProvider.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidContainerViewCore : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestContainerViewCore>
{
  protected:
    void initCore() override { bdn::test::TestAndroidViewCoreMixin<bdn::test::TestContainerViewCore>::initCore(); }
};

TEST_CASE("android.ContainerViewCore")
{
    std::shared_ptr<TestAndroidContainerViewCore> test = std::make_shared<TestAndroidContainerViewCore>();

    test->runTests();
}
