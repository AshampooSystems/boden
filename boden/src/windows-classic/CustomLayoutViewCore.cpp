#include <bdn/init.h>
#include <bdn/CustomLayoutViewCore.h>

namespace bdn
{


CustomLayoutViewCore::CustomLayoutViewCore(Window* pWindow)
	: ViewCore(	pWindow,
				CustomLayoutViewCoreClass::get()->getName(),
				"",
				WS_CHILD | WS_VISIBLE,
				0 )
{
}


}



