#ifndef BDN_ANDROID_ContainerViewCore_H_
#define BDN_ANDROID_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/JNativeViewGroup.h>


namespace bdn
{
namespace android
{

class ContainerViewCore : public ViewCore
{
private:
	static P<JNativeViewGroup> _createJNativeViewGroup(ContainerView* pOuter)
	{
		// we need to know the context to create the view.
		// If we have a parent then we can get that from the parent's core.
		P<View> 	pParent = pOuter->getParentView();
		if(pParent==nullptr)
			throw ProgrammingError("ContainerViewCore instance requested for a ContainerView that does not have a parent.");

		P<ViewCore> pParentCore = cast<ViewCore>( pParent->getViewCore() );
		if(pParentCore==nullptr)
			throw ProgrammingError("ContainerViewCore instance requested for a ContainerView with core-less parent.");

		JContext context = pParentCore->getJView().getContext();

		return newObj<JNativeViewGroup>(context);
	}

public:
	ContainerViewCore(ContainerView* pOuter)
     : ViewCore(pOuter, _createJNativeViewGroup(pOuter) )
	{
	}

	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}


    Rect adjustAndSetBounds(const Rect& requestedBounds) override
	{
		Rect adjustedBounds = ViewCore::adjustAndSetBounds(requestedBounds);

		JNativeViewGroup thisGroup( getJView().getRef_() );

        double scaleFactor = getUiScaleFactor();

		thisGroup.setSize(
				std::lround(adjustedBounds.width * scaleFactor),
				std::lround(adjustedBounds.height * scaleFactor) );

		return adjustedBounds;
	}

};

}
}

#endif
