#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/TestContainerViewCore.h>

#import <bdn/mac/UiProvider.hh>
#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;



class TestMacContainerViewCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:
    
    void initCore() override
    {
        TestMacChildViewCoreMixin< TestContainerViewCore >::initCore();
    }
    
    bool coreCanCalculatePreferredSize() override
    {
        return false;
    }
    
};



TEST_CASE("mac.ContainerViewCore")
{
    P<TestMacContainerViewCore> pTest = newObj<TestMacContainerViewCore>();
    
    pTest->runTests();
}







