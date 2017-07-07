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
    

};


}
}

#endif

