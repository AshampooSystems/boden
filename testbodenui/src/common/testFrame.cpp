#include <bdn/init.h>
#include <bdn/IFrame.h>

#include <bdn/test.h>

#include <thread>
#include <chrono>

using namespace bdn;

TEST_CASE("Frame", "[ui]")
{   
    P<IFrame> pFrame = createFrame("Test");
    
    pFrame->visible() = true;
    
    std::this_thread::sleep_for( std::chrono::duration<int>(2) );    

	pFrame = nullptr;
}



