#ifndef BDN_WINUWP_IFrameworkElementOwner_H_
#define BDN_WINUWP_IFrameworkElementOwner_H_

namespace bdn
{
namespace winuwp
{

/** Interface for objects that own a Windows.UI.Xaml.FrameworkElement object.
	Note that top level windows do not derive from this - they provider their own
	implementation of IViewCore.	
*/
class IFrameworkElementOwner : BDN_IMPLEMENTS IBase
{
public:	

    /** Returns the Windows.UI.Xaml.FrameworkElement associated with this object.
    */
    virtual ::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement()=0;
};


}
}

#endif

