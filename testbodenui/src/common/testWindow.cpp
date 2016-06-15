#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/test.h>

#include <thread>
#include <chrono>

using namespace bdn;

TEST_CASE("Window", "[ui]")
{   
    P<Window> pWindow = newObj<Window>();
	pWindow->title() = "Test";

    pWindow->visible() = true;
    
    std::this_thread::sleep_for( std::chrono::duration<int>(2) );    

	pWindow = nullptr;
}



