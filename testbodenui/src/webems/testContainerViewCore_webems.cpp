#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/webems/UiProvider.h>
#include "TestWebViewCoreMixin.h"

using namespace bdn;



class TestWebContainerViewCore : public bdn::test::TestWebViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:

    void initCore() override
    {
        TestWebViewCoreMixin< TestContainerViewCore >::initCore();
    }        

};



TEST_CASE("webems.ContainerViewCore")
{
    P<TestWebContainerViewCore> pTest = newObj<TestWebContainerViewCore>();

    pTest->runTests();
}




