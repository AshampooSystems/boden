#ifndef BDN_Button_H_
#define BDN_Button_H_


#include <bdn/Base.h>
#include <bdn/Frame.h>
#include <bdn/Notifier.h>
#include <bdn/ClickEvent.h>
#include <bdn/Property.h>
#include <bdn/mainThread.h>
#include <bdn/WindowTextProperty.h>

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

			_handle = ::CreateWindow(L"BUTTON", labelW.c_str(), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 20, 20, 200, 30, pParent->getHandle(), NULL, NULL, NULL);

			::SetWindowLongPtr(_handle, GWLP_USERDATA, (LONG_PTR)((Base*)this));

			::SendMessage(_handle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), TRUE);

			_label.setWindowHandle(_handle);
		}
		/*
			_pLabelChangeSub = _label.onChange().subscribe(
				wrapCallFromMainThread(
					[this]()
					{

					}
					VoidMember<Button>(this, &Button::onLabelChanged);
		}
		*/
		/** Returns a reference to the button's label object. The object is read/write and can be
			used to change the button label.
			

			*/
		Property<String>& label()
		{
			return _label;			
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

		void generateClick()
		{
			ClickEvent evt(this);

			_onClick.notify(evt);
		}

		Notifier<const ClickEvent&>& onClick()
		{
			return _onClick;
		}

	protected:

		void onLabelChanged()
		{
			
		}

		static LRESULT CALLBACK windowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return ::CallWindowProc(DefWindowProc, hWindow, message, wParam, lParam);
		}


		HWND _handle;

		Notifier<const ClickEvent&> _onClick;

		WindowTextProperty			_label;
	};


}

#endif
