#ifndef BDN_ITextViewCore_H_
#define BDN_ITextViewCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{

class ITextViewCore : BDN_IMPLEMENTS IViewCore
{
public:

	/** Changes the text view's content text.*/
	virtual void setText(const String& text)=0;
	
};


}

#endif
