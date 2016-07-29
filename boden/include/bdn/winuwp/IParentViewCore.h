#ifndef BDN_WINUWP_IParentViewCore_H_
#define BDN_WINUWP_IParentViewCore_H_


namespace bdn
{
namespace winuwp
{


/** Interface for view cores that can act as parents for other view cores.*/
class IParentViewCore : BDN_IMPLEMENTS IBase
{
public:

	/** Adds a child Ui element to the parent.*/
	virtual void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement )=0;
};
	
}
}

#endif