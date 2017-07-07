#include <bdn/init.h>
#include <bdn/winuwp/util.h>

namespace bdn
{
namespace winuwp
{


void layoutViewTree(View* pView)
{        
    P<IViewCore> pCore = pView->getViewCore();
    if(pCore!=nullptr)
    {
        pCore->layout();

        std::list< P<View> > childViews;
        pView->getChildViews(childViews);

        for( auto& pChildView: childViews)
            layoutViewTree(pChildView);        
    }
}



}
}


