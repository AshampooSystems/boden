#ifndef BDN_ColumnView_H_
#define BDN_ColumnView_H_

#include <bdn/ContainerView.h>

namespace bdn
{
	

/** A container view that arranges its children in a vertical column,
	one below the other. See #RowView for a similar horizontal container.

	The children's View::horizontalAlignment() property controls how 
	the child views are aligned horizontally.

	If there is extra space available in the container after sizing all
	children to their preferred size then the remaining space is distributed
	according to the children's View::extraSpaceWeight() property.	
	
	*/
class ColumnView : public ContainerView
{
public:
	ColumnView()
	{		
	}

	
	/** Static function that returns the type name for #ColumnView objects.*/
	static String getColumnViewTypeName()
	{
		return "bdn.ColumnView";
	}

	String getViewTypeName() const override
	{
		return getColumnViewTypeName();
	}

};


}

#endif