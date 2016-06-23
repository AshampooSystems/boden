#include <bdn/init.h>
#include <bdn/win32/ContainerViewCore.h>

namespace bdn
{
namespace win32
{

ContainerViewCore::ContainerViewCore(ContainerView* pOuter)
	: ViewCore(	pOuter, 
				ContainerViewCoreClass::get()->getName(),
				"",
				WS_VISIBLE | WS_CHILD,
				0 )
{
}

Size ContainerViewCore::calcPreferredSize() const
{
	// this core function should never have been called.
	// The outer window is responsible for everything layout-related.
	throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
}

int ContainerViewCore::calcPreferredHeightForWidth(int width) const
{
	// this core function should never have been called.
	// The outer window is responsible for everything layout-related.
	throw ProgrammingError("ContainerView::calcPreferredHeightForWidth must be overloaded in derived class.");
}

int ContainerViewCore::calcPreferredWidthForHeight(int height) const
{
	// this core function should never have been called.
	// The outer window is responsible for everything layout-related.
	throw ProgrammingError("ContainerView::calcPreferredWidthForHeight must be overloaded in derived class.");
}



}
}


