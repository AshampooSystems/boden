#ifndef BDN_ANDROID_ContainerViewCore_H_
#define BDN_ANDROID_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/android/ViewCore.h>


namespace bdn
{
namespace android
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(ContainerView* pOuter)
     : ViewCore(pOuter, newObj<JNativeViewGroup>() )
	{
	}

	Size calcPreferredSize() const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

	int calcPreferredHeightForWidth(int width) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredHeightForWidth must be overloaded in derived class.");
	}

	int calcPreferredWidthForHeight(int height) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredWidthForHeight must be overloaded in derived class.");
	}

};

}
}

#endif
