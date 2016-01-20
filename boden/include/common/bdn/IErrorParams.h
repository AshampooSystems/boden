#ifndef BDN_IErrorParams_H_
#define BDN_IErrorParams_H_

#include <map>

namespace bdn
{

/** An interface that can be implemented by exception classes to provide
	arbitrary additional information about the error.

	The additional information is stored in a name-value map.

	Example:

	\code
	try
	{
		int result = someFunc(filePath);
		if(result!=0)
			throw MyError(result, { {"path", filePath} } );
	}
	catch(MyError& e)
	{
		showMessage("Error accessing the file "+e.getParam("filePath") );
	}
	\endcode
	*/
class IErrorParams : BDN_IMPLEMENTS IBase
{
public:

	/** Returns the parameter with the specified name. Returns an empty string
		if the parameter was not found.*/
	virtual String getParam(const String& paramName) const=0;


	/** Returns true if the specified parameter exists.*/
	virtual bool hasParam(const String& paramName) const=0;
	

	/** Returns a reference to a map with all parameters.*/
	virtual const std::map<String, String>& getParamMap() const=0;
	

	/** Returns a reference to a map with all parameters. This map
		can be modified - for example, to add additional information.*/
	virtual std::map<String, String>& getParamMap()=0;


};

}

#endif
