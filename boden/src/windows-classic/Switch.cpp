#include <bdn/init.h>
#include <bdn/Switch.h>

namespace bdn
{


P<ISwitch> createSwitch(IWindow* pParent, const String& label)
{
	return newObj<Switch>(cast<Window>(pParent), label);
}
	


}


