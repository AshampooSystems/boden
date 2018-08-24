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
    // call the outer container's preferred size calculation

    P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
        return pOuterView->calcContainerPreferredSize( availableSpace );
    else
        return Size(0,0);
}

void ContainerViewCore::layout()
{
    // call the outer container's layout function

    P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        P<ViewLayout> pLayout = pOuterView->calcContainerLayout( pOuterView->size() );
        pLayout->applyTo(pOuterView);
    }    
}




}
}


