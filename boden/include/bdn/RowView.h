#ifndef BDN_RowView_H_
#define BDN_RowView_H_

#include <bdn/ContainerView.h>

namespace bdn
{
	

/** A container view that arranges its children in a horizontal row,
	one after the other. See #ColumnView for a similar vertical container.

	The children's View::verticalAlignment() property controls how 
	the child views are aligned vertically.

	If there is extra space available in the container after sizing all
	children to their preferred size then the remaining space is distributed
	according to the children's View::extraSpaceWeight() property.	
	
	*/
class RowView : public ContainerView
{
public:
	RowView()
	{		
	}

	
	void layout()
	{
		
	}

};


}

#endif