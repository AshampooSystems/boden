#ifndef BDN_PropertyWithFilter_H_
#define BDN_PropertyWithFilter_H_

#include <bdn/DefaultProperty.h>

#include <functional>

namespace bdn
{

/** A property implementation that provides support for an additional filter function that
    analyzes and possibly modifies assigned values before they are stored in the property.

    The filter function can be set via the setFilter() function. If no filter function is set
    then the property works the same as DefaultProperty.
    
	*/
template<class ValType>
class PropertyWithFilter : public DefaultProperty<ValType>
{
public:
	PropertyWithFilter( ValType value = ValType() )
        : DefaultProperty(value)
	{
	}

    void setFilter( std::function<ValType(const ValType&)> filterFunction)
    {
        _filterFunction = filterFunction;
    }
    
	void set(const ValType& val) override
	{
        if(_filterFunction)
            DefaultProperty::set( _filterFunction(val) );
        else
            DefaultProperty::set( val );
	}

private:
    std::function<ValType(const ValType&)> _filterFunction;
};


}

#endif
