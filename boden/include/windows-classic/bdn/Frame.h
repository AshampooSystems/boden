#ifndef BDN_Frame_H_
#define BDN_Frame_H_

#include <bdn/IFrame.h>
#include <bdn/Window.h>

#include <windows.h>
#include <bdn/WindowClass.h>

namespace bdn
{

class Frame : public Window, BDN_IMPLEMENTS IFrame
{
public:
	Frame(const String& title)
	{		
		String className = FrameClass::get()->getName();

		create(	nullptr,
				className,
				title,
				WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW,
				WS_EX_APPWINDOW,
				200,
				200,
				300,
				200 );
	}


	Property<String>& title() override
	{
		return *_pText;
	}

	ReadProperty<String>& title() const override
	{
		return *_pText;
	}


		
protected:

	class FrameClass : public WindowClass
	{
	public:
		FrameClass()
			: WindowClass("bdnFrame", WindowBase::windowProc)
		{
			_info.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

			ensureRegistered();			
		}

		static P<FrameClass> get()
		{
			static SafeInit<FrameClass> init;

			return init.get();
		}		
	};


	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;
	
		
};


}

#endif
