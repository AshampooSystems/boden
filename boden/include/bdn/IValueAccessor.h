#ifndef BDN_IValueAccessor_H_
#define BDN_IValueAccessor_H_


namespace bdn
{

/** Interface for objects that provide a access to a value of the type indicated by the interface's
    template parameter.
	*/
template<typename ValType>
class IValueAccessor : BDN_IMPLEMENTS IBase
{
public:
        
	/** Returns the property value.*/
    virtual ValType get() const=0;


};


}

#endif
