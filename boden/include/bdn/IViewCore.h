#ifndef BDN_IViewCore_H_
#define BDN_IViewCore_H_

namespace bdn
{
    class IViewCore;
}

#include <bdn/UiMargin.h>
#include <bdn/Rect.h>
#include <bdn/Size.h>
#include <bdn/View.h>

namespace bdn
{

class IViewCore : BDN_IMPLEMENTS IBase
{
public:

	/** Shows/hides the view core.*/
	virtual void	setVisible(const bool& visible)=0;
	
	/** Sets the view core's padding. See View::padding() */
	virtual void setPadding(const UiMargin& padding)=0;

	/** Sets the view core's bounding rectangle. See View::bounds() */
	virtual void setBounds(const Rect& bounds)=0;
    
	
	/** Converts the specified Ui length to pixels.*/
	virtual int uiLengthToPixels(const UiLength& uiLength) const=0;
	

	/** Converts a UiMargin object to a pixel-based margin object.*/
	virtual Margin uiMarginToPixelMargin(const UiMargin& margin) const=0;



	

	/** Asks the view core to calculate its preferred size, based on it current content
		and properties.
		*/	
	virtual Size calcPreferredSize() const=0;

	
	/** Asks the view core to calculate its preferred height for the case that the view had
		the specified width.

		This function is called in cases when there is not enough space to size the view
		according to its unconstrained preferred size (see #calcPreferredSize()).
		The view should pretend that it has the specified width and return its preferred
		height for that case.

		Note that the \c width parameter can also be BIGGER than the unconstrained preferred width
		returned by calcPreferredSize().
		*/	
	virtual int calcPreferredHeightForWidth(int width) const=0;


	/** Asks the view core to calculate its preferred width for the case that the view had
		the specified height.

		This function is called in cases when there is not enough space to size the view
		according to its unconstrained preferred size (see #calcPreferredSize()).
		The view should pretend that it has the specified height and return its preferred
		width for that case.

		Note that the \c height parameter can also be BIGGER than the unconstrained preferred height
		returned by calcPreferredSize().
		*/	
	virtual int calcPreferredWidthForHeight(int height) const=0;
	


	/** Called when the outer view's parent has changed.
		
		tryChangeParentView should try to move the core over to the new parent.

		If successful then it should return true, otherwise false.

		If false is returned then this will cause the outer view object to
		automatically re-create the core for the new parent and release the current
		core object.		
		*/
	virtual bool tryChangeParentView(View* pNewParent)=0;
    
    
	
};


}

#include <bdn/View.h>

#endif
