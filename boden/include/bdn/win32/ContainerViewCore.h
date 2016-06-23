#ifndef BDN_ContainerViewCore_H_
#define BDN_ContainerViewCore_H_

#include <bdn/ViewCore.h>
#include <bdn/ContainerView.h>
#include <bdn/Win32WindowClass.h>

namespace bdn
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(ContainerView* pOuter);
	
	Size calcPreferredSize() const;
	int calcPreferredHeightForWidth(int width) const;
	int calcPreferredWidthForHeight(int height) const;

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

		static P<ContainerViewCoreClass> get()
		{
			static SafeInit<ContainerViewCoreClass> init;

			return init.get();
		}		
	};

	
};


}

#endif
