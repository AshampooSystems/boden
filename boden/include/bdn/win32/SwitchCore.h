#ifndef BDN_SwitchCore_H_
#define BDN_SwitchCore_H_

#include <bdn/ViewCore.h>

#include <bdn/ISwitchCore.h>

namespace bdn
{

class SwitchCore : public ViewCore, BDN_IMPLEMENTS ISwitchCore
{
public:
	SwitchCore(Switch* pOuter)
		: ViewCore(	pOuter,
					"BUTTON",
					pOuter->label(),
					BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD,
					0,
					20,
					60,
					200,
					30 )
	{		
	}

	void setLabel(const String& label) override
	{
		setWindowText(getHwnd(), label);
	}
	
};


}

#endif
