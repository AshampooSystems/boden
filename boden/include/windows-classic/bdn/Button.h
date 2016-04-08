#ifndef BDN_Button_H_
#define BDN_Button_H_


#include <bdn/Base.h>
#include <bdn/Frame.h>
#include <bdn/EventSource.h>
#include <bdn/ClickEvent.h>

#include <windows.h>

#include <codecvt>
#include <list>

namespace bdn
{

	class Button : public Base, virtual public IWindow
	{
	public:
		Button(Frame* pParent, const std::string& label)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring labelW = converter.from_bytes(label);

			_pClickEventSource = new EventSource<ClickEvent>;

			_handle = ::CreateWindow(L"BUTTON", labelW.c_str(), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 20, 20, 200, 30, pParent->getHandle(), NULL, NULL, NULL);

			::SetWindowLongPtr(_handle, GWLP_USERDATA, (LONG_PTR)((Base*)this));

			::SendMessage(_handle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), TRUE);
		}

		void setLabel(const std::string& label)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring labelW = converter.from_bytes(label);

			::SetWindowText(_handle, labelW.c_str());
		}
		
		virtual void show(bool visible = true) override
		{
			::ShowWindow(_handle, visible ? SW_SHOW : SW_HIDE);
		}

		virtual void hide() override
		{
			show(false);
		}

		void onClick()
		{
			ClickEvent evt(this);

			_pClickEventSource->deliver(evt);
		}

		EventSource<ClickEvent>* getClickEventSource()
		{
			return _pClickEventSource;
		}

	protected:

		static LRESULT CALLBACK windowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return ::CallWindowProc(DefWindowProc, hWindow, message, wParam, lParam);
		}


		HWND _handle;

		EventSource<ClickEvent>* _pClickEventSource;
	};


}

#endif
