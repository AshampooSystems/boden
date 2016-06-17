#ifndef BDN_CustomLayoutViewCore_H_
#define BDN_CustomLayoutViewCore_H_

#include <bdn/CustomLayoutView.h>
#include <bdn/ViewCore.h>
#include <bdn/Win32WindowClass.h>

#include <bdn/IWindowCore.h>


namespace bdn
{

class CustomLayoutViewCore : public ViewCore
{
public:
	CustomLayoutViewCore(CustomLayoutView* pWindow);			
	
protected:
	class CustomLayoutViewCoreClass : public Win32WindowClass
	{
	public:
		CustomLayoutViewCoreClass()
			: Win32WindowClass("bdnCustomLayoutView", Win32Window::windowProc)
		{
			_info.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

			ensureRegistered();			
		}

		static P<CustomLayoutViewCoreClass> get()
		{
			static SafeInit<CustomLayoutViewCoreClass> init;

			return init.get();
		}		
	};

};

}

#endif

