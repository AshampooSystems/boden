#include <bdn/init.h>
#include <bdn/test.h>


#include <bdn/test/TestContainerViewCore.h>

#include <bdn/gtk/UiProvider.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;



class TestGtkContainerViewCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:

    void initCore() override
    {
        TestGtkViewCoreMixin< TestContainerViewCore >::initCore();
    }    

    bool coreCanCalculatePreferredSize() override
    {
        return false;
    }

};



TEST_CASE("gtk.ContainerViewCore")
{
    P<TestGtkContainerViewCore> pTest = newObj<TestGtkContainerViewCore>();

    pTest->runTests();
}



