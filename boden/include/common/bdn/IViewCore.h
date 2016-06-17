#ifndef BDN_IViewCore_H_
#define BDN_IViewCore_H_

#include <bdn/UiMargin.h>
#include <bdn/Rect.h>

namespace bdn
{

class View;

class IViewCore : BDN_IMPLEMENTS IBase
{
public:

	/** Shows/hides the view core.*/
	virtual void	setVisible(const bool& visible)=0;
	
	/** Sets the view core's margin. See View::margin() */
	virtual void setMargin(const UiMargin& margin)=0;

	/** Sets the view core's padding. See View::padding() */
	virtual void setPadding(const UiMargin& padding)=0;

	/** Sets the view core's bounding rectangle. See View::bounds() */
	virtual void setBounds(const Rect& bounds)=0;


	
	/** Converts the specified Ui length to pixels.*/
	virtual double uiLengthToPixels(const UiLength& uiLength)=0;
	

	/** Converts a UiMargin object to a pixel-based margin object.*/
	virtual Margin uiMarginToPixelMargin(const UiMargin& margin)=0;




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

#endif
