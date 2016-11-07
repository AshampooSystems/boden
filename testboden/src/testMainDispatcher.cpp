#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>

using namespace bdn;


TEST_CASE("mainDispatcher")
{
    bdn::test::testDispatcher( getMainDispatcher() );
}




