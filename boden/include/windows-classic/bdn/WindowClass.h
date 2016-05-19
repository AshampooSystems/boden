#ifndef BDN_WindowClass_H_
#define BDN_WindowClass_H_


#include <windows.h>

namespace bdn
{
	

/** Base class for window class objects.
*/
class WindowClass : public Base
{
public:
	WindowClass(const String& name, WNDPROC windowProc);

	/** Registers the class if it is not yet registered.
	
		This is usually called by derived classes at the end of their constructor.
	*/
	void ensureRegistered();
	
	String getName() const
	{
		return _name;
	}

protected:
	String		_name;
	WNDCLASSEX	_info;
	bool		_registered;
};


}

#endif