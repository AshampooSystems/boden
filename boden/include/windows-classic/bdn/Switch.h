#ifndef BDN_Switch_H_
#define BDN_Switch_H_

#include <bdn/Window.h>
#include <bdn/ISwitch.h>


namespace bdn
{

class Switch : public Window, BDN_IMPLEMENTS ISwitch
{
public:
	Switch(Window* pParent, const String& label)
	{
		create(pParent ,"BUTTON", label, BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD, 0, 20, 60, 200, 30 );		
	}

	
	Property<String>& label() override
	{
		return *_pText;
	}

	ReadProperty<String>& label() const override
	{
		return *_pText;
	}
		

};


}

#endif
