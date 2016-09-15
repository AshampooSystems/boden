#ifndef BDN_WIN32_TextViewCore_H_
#define BDN_WIN32_TextViewCore_H_

#include <bdn/TextView.h>
#include <bdn/win32/ViewCore.h>

namespace bdn
{
namespace win32
{

class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
public:
	TextViewCore(TextView* pOuter);

	void setText(const String& text) override;
    
	Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const;
    
};


}
}

#endif
