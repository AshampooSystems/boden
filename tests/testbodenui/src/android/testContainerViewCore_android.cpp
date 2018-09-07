#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/android/UiProvider.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidContainerViewCore : public bdn::test::TestAndroidViewCoreMixin<
                                         bdn::test::TestContainerViewCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<
            bdn::test::TestContainerViewCore>::initCore();
    }
};

TEST_CASE("android.ContainerViewCore")
{
    P<TestAndroidContainerViewCore> pTest =
        newObj<TestAndroidContainerViewCore>();

    pTest->runTests();
}
