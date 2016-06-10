#ifndef BDN_Win32UiProvider_H_
#define BDN_Win32UiProvider_H_

#include <bdn/IUiProvider.h>

namespace bdn
{
	

/** UI provider that creates classic Windows desktop UI components using the Win32 API.
	Note that Win32 is also the name of the Windows API on 64 Bit Windows,
	so this works on 32 and 64 bit systems.
*/
class Win32UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
	Win32UiProvider()
	{		
	}

	
	String getName() const
	{
		return "win32";
	}


	P<IViewCore> createViewCore(const String& viewTypeName, View* pView);

};


}

#endif