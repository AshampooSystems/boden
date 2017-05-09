#ifndef BDN_WIN32_ContainerViewCore_H_
#define BDN_WIN32_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/Win32WindowClass.h>

namespace bdn
{
namespace win32
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(ContainerView* pOuter);
	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;

protected:
	class ContainerViewCoreClass : public Win32WindowClass
	{
	public:
		ContainerViewCoreClass()
			: Win32WindowClass("bdnContainerView", ViewCore::windowProc)
		{
			_info.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

			ensureRegistered();			
		}

		static ContainerViewCoreClass& get();
	};

	
};

}
}

#endif
