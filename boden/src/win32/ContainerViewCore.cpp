#include <bdn/init.h>
#include <bdn/win32/ContainerViewCore.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( ContainerViewCore::ContainerViewCoreClass, ContainerViewCore::ContainerViewCoreClass::get );

ContainerViewCore::ContainerViewCore(ContainerView* pOuter)
	: ViewCore(	pOuter, 
				ContainerViewCoreClass::get().getName(),
				"",
				WS_CHILD,
				0 )
{
}

Size ContainerViewCore::calcPreferredSize( const Size& availableSpace ) const
{
	// this core function should never have been called.
	// The outer window is responsible for everything layout-related.
	programmingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
    return Size();
}



}
}


