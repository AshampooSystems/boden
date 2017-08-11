#ifndef BDN_WINUWP_IUwpViewCore_H_
#define BDN_WINUWP_IUwpViewCore_H_

namespace bdn
{
namespace winuwp
{

/** Interface for UWP view core objects.
*/
class IUwpViewCore : BDN_IMPLEMENTS IBase
{
public:	

    /** Returns the Windows.UI.Xaml.FrameworkElement associated with this object.
    */
    virtual ::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement()=0;
    

    /** Lays out the view. This is called during the Arrange phase.*/
    virtual void layout()=0;


    
    /** Calls FrameworkElement.InvalidateMeasure.*/
    virtual void invalidateMeasure()=0;
    
    /** Calls FrameworkElement.InvalidateArrange.*/
    virtual void invalidateArrange()=0;


};


}
}

#endif

