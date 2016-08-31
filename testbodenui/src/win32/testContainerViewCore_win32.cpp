#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/win32/UiProvider.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;



class TestWin32ContainerViewCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:

    void initCore() override
    {
        TestWin32ViewCoreMixin< TestContainerViewCore >::initCore();
    }    

    bool coreCanCalculatePreferredSize() override
    {
        return false;
    }

};



TEST_CASE("win32.ContainerViewCore")
{
    P<TestWin32ContainerViewCore> pTest = newObj<TestWin32ContainerViewCore>();

    pTest->runTests();
}







