#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#include <bdn/webems/UiProvider.h>
#include "TestWebemsViewCoreMixin.h"

using namespace bdn;



class TestWebemsContainerViewCore : public bdn::test::TestWebemsViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:

    void initCore() override
    {
        TestWebemsViewCoreMixin< TestContainerViewCore >::initCore();
    }        

};



TEST_CASE("webems.ContainerViewCore")
{
    P<TestWebemsContainerViewCore> pTest = newObj<TestWebemsContainerViewCore>();

    pTest->runTests();
}




