#ifndef BDN_DefaultProperty_H_
#define BDN_DefaultProperty_H_


#include <bdn/Property.h>
#include <bdn/func.h>
#include <bdn/DefaultNotifier.h>

namespace bdn
{

/** The default implementation of a Property.

	The initial value of the property is the default-constructed ValType.
	For integers that is 0.

	*/
template<class ValType>
class DefaultProperty : public RequireNewAlloc< Property<ValType>, DefaultProperty<ValType> >
{
public:
	DefaultProperty(ValType value = ValType() )
	{
        _pOnChange = newObj< DefaultNotifier< P<const ReadProperty<ValType> > > > ();
		_value = value;
	}
    
	ValType get() const override
	{
		MutexLock lock(_mutex);

		return _value;
	}

	void set(const ValType& val) override
	{
		bool changed = false;

		{
			MutexLock lock(_mutex);

			if(_value!=val)
			{
				_value = val;
				onValueChanged();
				changed = true;
			}
		}

		if(changed)
			_pOnChange->postNotification(this);
	}


	Property<ValType>& operator=(const ValType& val) override
    {
        set(val);
        
        return *this;
    }

	Property<ValType>& operator=(const ReadProperty<ValType>& prop) override
    {
        set(prop.get());
        
        return *this;
    }

	INotifier< P<const ReadProperty<ValType> > >& onChange() const override
	{
		return *_pOnChange;
	}
    
    void bind(const ReadProperty<ValType>& sourceProperty) override
	{
        sourceProperty.onChange() += weakMethod(this, &DefaultProperty::bindSourceChanged);
        
        bindSourceChanged(&sourceProperty);
    }
    
    
protected:
    virtual void bindSourceChanged( P<const ReadProperty<ValType> > pProp )
    {
        set( pProp->get() );
    }

	virtual void onValueChanged()
    {
	    // do nothing by default.
    }

	mutable Mutex					_mutex;
	ValType							_value;
    
    mutable P< DefaultNotifier< P<const ReadProperty<ValType> > >	> _pOnChange;
};


}

#endif
