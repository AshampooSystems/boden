#include <bdn/init.h>
#include <bdn/Frame.h>

#include <bdn/test.h>

#include <thread>
#include <chrono>

using namespace bdn;

TEST_CASE("Frame", "[ui]")
{   
    P<Frame> pFrame = newObj<Frame>("Test");
    
    pFrame->show();
    
    std::this_thread::sleep_for( std::chrono::duration<int>(2) );
    
}



