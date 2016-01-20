#ifndef BDN_Switch_H_
#define BDN_Switch_H_


#include <bdn/Base.h>
#include <bdn/Frame.h>

#include <windows.h>

#include <codecvt>
#include <list>

namespace bdn
{

	class Switch : public Base
	{
	public:
		Switch(Frame* pParent, const std::string& label)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring labelW = converter.from_bytes(label);

			_handle = ::CreateWindow(L"BUTTON", labelW.c_str(), BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD, 20, 60, 200, 30, pParent->getHandle(), NULL, NULL, NULL);

			::SetWindowLongPtr(_handle, GWLP_USERDATA, (LONG_PTR)((Base*)this));

			::SendMessage(_handle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), TRUE);
		}

		void setLabel(const std::string& label)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring labelW = converter.from_bytes(label);

			::SetWindowText(_handle, labelW.c_str());
		}


	protected:

		static LRESULT CALLBACK windowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return ::CallWindowProc(DefWindowProc, hWindow, message, wParam, lParam);
		}


		HWND _handle;
	};


}

#endif
