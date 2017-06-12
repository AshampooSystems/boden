#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ScrollViewCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;

class TestWin32ScrollViewCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

};

TEST_CASE("win32.ScrollViewCore")
{
    P<TestWin32ScrollViewCore> pTest = newObj<TestWin32ScrollViewCore>();

    pTest->runTests();
}

