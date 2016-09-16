#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/Win32Window.h>
#include <bdn/win32/WindowDeviceContext.h>

using namespace bdn;
using namespace bdn::win32;


void testMultipleLines(DeviceContext& deviceContext, const String& linebreak)
{
    SECTION("two lines same size")
    {
        Size oneLineSize = deviceContext.getTextSize("hello");
        
        Size twoLinesSize = deviceContext.getTextSize("hello"+linebreak+"hello");

        REQUIRE( twoLinesSize.width == oneLineSize.width );
        REQUIRE( twoLinesSize.height == oneLineSize.height*2 );
    }

    SECTION("two lines second longer")
    {
        Size oneLineSize = deviceContext.getTextSize("hellobla");
        
        Size twoLinesSize = deviceContext.getTextSize("hello"+linebreak+"hellobla");

        REQUIRE( twoLinesSize.width == oneLineSize.width );
        REQUIRE( twoLinesSize.height == oneLineSize.height*2 );
    }

    SECTION("two lines first longer")
    {
        Size oneLineSize = deviceContext.getTextSize("hellobla");
        
        Size twoLinesSize = deviceContext.getTextSize("hellobla"+linebreak+"hello");

        REQUIRE( twoLinesSize.width == oneLineSize.width );
        REQUIRE( twoLinesSize.height == oneLineSize.height*2 );
    }
}

TEST_CASE("win32.DeviceContext")
{
    Win32Window         window("BUTTON", "testWindow", WS_POPUP, 0, NULL );

    WindowDeviceContext deviceContext( window.getHwnd() );

    SECTION("getTextSize")
    {
        SECTION("empty")
        {
            Size textSize = deviceContext.getTextSize("");

            REQUIRE( textSize == Size() );
        }

        SECTION("one line")
        {
            Size textSize = deviceContext.getTextSize("hello");

            REQUIRE( textSize > Size() );
        }

        SECTION("multiple lines")
        {
            SECTION("LF")
                testMultipleLines(deviceContext, "\n");
            SECTION("CRLF")
                testMultipleLines(deviceContext, "\r\n");
        }

		SECTION("with wrapWidth")
		{
			SECTION("oneWord")
			{
				Size unwrappedSize = deviceContext.getTextSize("hello");
				
				// wrap width = 1 should be the same as unwrapped for a single word
				REQUIRE( deviceContext.getTextSize("hello", 1) == unwrappedSize);
			}

			SECTION("twoWords")
			{
				Size manualWrappedSize = deviceContext.getTextSize("hello\nhello");

				REQUIRE( deviceContext.getTextSize("hello hello", manualWrappedSize.width) == manualWrappedSize );
			}
		}        
    }
}

