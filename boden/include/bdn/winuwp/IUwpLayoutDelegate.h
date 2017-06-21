#ifndef BDN_WINUWP_IUwpLayoutDelegate_H_
#define BDN_WINUWP_IUwpLayoutDelegate_H_


namespace bdn
{
namespace winuwp
{

/** Interface for a layout delegate that provides the layout implementation for a UWP
    UIElement.*/
class IUwpLayoutDelegate : BDN_IMPLEMENTS IBase
{
public:

    /** Implementation for the MeasureOverride UWP function.*/
    virtual ::Windows::Foundation::Size measureOverride( ::Windows::Foundation::Size availableSize )=0;


    /** Implementation for the ArrangeOverride UWP function.*/
    virtual ::Windows::Foundation::Size arrangeOverride( ::Windows::Foundation::Size finalSize )=0;
};

}
}

#endif


