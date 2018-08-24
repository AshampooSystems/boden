#ifndef BDN_ISimpleCallable_H_
#define BDN_ISimpleCallable_H_


namespace bdn
{

/** Interface for a simple callable object that provides only
	a call() method without parameters or return values.

 	See SimpleFunctionCallable for a convenience implementation that calls a std::function object.
 */
class ISimpleCallable : BDN_IMPLEMENTS IBase
{
public:

	virtual void call()=0;
};


}


#endif

