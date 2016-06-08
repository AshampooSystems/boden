#ifndef BDN_IFrame_H_
#define BDN_IFrame_H_

#include <bdn/IView.h>

namespace bdn
{
	

/** A frame is a top-level container window.

	Objects can be created with createFrame().
*/
class IFrame : BDN_IMPLEMENTS IView
{
public:


	/** Returns the Frame's title property.
		It is safe to use from any thread.
		*/
	virtual Property<String>& title()=0;
	virtual ReadProperty<String>& title() const=0;
	
};


/** Creates a frame window (top level window).*/
P<IFrame> createFrame(const String& title);

}


#endif


