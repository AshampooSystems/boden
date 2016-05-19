#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/MessageWindowBase.h>

#include <windows.h>

using namespace bdn;


class TestMessageWindow : public MessageWindowBase
{
public:
	TestMessageWindow()
		: MessageWindowBase("testMessageWindow")
	{
	}

	UINT	lastMessage=0;
	WPARAM	lastWParam=0;
	LPARAM	lastLParam=0;

	bool	setResult = true;
	bool	callDefault = false;

	bool	throwException = false;

protected:
	
	virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override
	{
		if(message==WM_SETTEXT)
		{
			if(setResult)
				context.setResult(1111, callDefault);
		}

		lastMessage = message;
		lastWParam = wParam;
		lastLParam = lParam;

		if(message==WM_SETTEXT)
		{
			if(throwException)
				throw std::exception("dummy");
		}
	}

};


void verifyMessageWindow(bool setResult, bool callDefault)
{
	TestMessageWindow window;

	window.setResult = setResult;
	window.callDefault = callDefault;

	const wchar_t* newText = L"xyz";

	LRESULT result = SendMessage( window.getHandle()->getHwnd(), WM_SETTEXT, 0, (LPARAM)newText );

	REQUIRE( window.lastMessage==WM_SETTEXT );
	REQUIRE( window.lastWParam==0 );
	REQUIRE( window.lastLParam==(LPARAM)newText );

	if(setResult)
		REQUIRE( result==1111 );
	else
		REQUIRE( result==TRUE );

	wchar_t buf[10];
	buf[0]=0;
	buf[9]=0;
	SendMessage( window.getHandle()->getHwnd(), WM_GETTEXT, 9, (LPARAM)buf);

	String text = buf;
	
	if(callDefault)
	{
		// verify that the default implmentation was not called
		REQUIRE( text=="xyz");
	}
	else
	{
		// default implementation should have been called and the text should have been changed
		REQUIRE( text!="xyz");
	}
}


void testExceptionInHandleMessage()
{
	TestMessageWindow window;
		
	window.setResult = false;
	window.throwException = true;

	const wchar_t* newText = L"xyz";

	LRESULT result = SendMessage( window.getHandle()->getHwnd(), WM_SETTEXT, 0, (LPARAM)newText );

	REQUIRE( window.lastMessage==WM_SETTEXT );
	REQUIRE( window.lastWParam==0 );
	REQUIRE( window.lastLParam==(LPARAM)newText );

	// default implementation should have been called
	REQUIRE( result==TRUE );

	wchar_t buf[10];
	buf[0]=0;
	buf[9]=0;
	SendMessage( window.getHandle()->getHwnd(), WM_GETTEXT, 9, (LPARAM)buf);

	String text = buf;	
	REQUIRE( text=="xyz");
}

TEST_CASE("MessageWindowBase")
{
	SECTION("setResult")
	{
		SECTION("callDefault")
			verifyMessageWindow(true, true);

		SECTION("preventDefault")
			verifyMessageWindow(true, false);
	}

	SECTION("dontSetResult")
		verifyMessageWindow(false, true);	

	SECTION("exceptionInHandleMessage")
		testExceptionInHandleMessage();
}



