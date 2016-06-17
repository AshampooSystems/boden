#ifndef BDN_CustomLayoutView_H_
#define BDN_CustomLayoutView_H_

#include <bdn/ContainerView.h>

namespace bdn
{
	

/** A container view that uses a custom layout to arrange its children.

	To use this, derive a sub class from this and override the layout methods
	to arrange the child views.	
	*/
class CustomLayoutView : public ContainerView
{
public:
	CustomLayoutView()
	{		
	}


	virtual void layoutChildViews()=0;

	
	/** Static function that returns the type name for #CustomLayoutView objects.*/
	static String getCustomLayoutViewTypeName()
	{
		return "bdn.CustomLayoutView";
	}

	String getViewTypeName() const override
	{
		return getCustomLayoutViewTypeName();
	}


	
};


}

#endif