#ifndef BDN_DefaultProperty_H_
#define BDN_DefaultProperty_H_


#include <bdn/Property.h>
#include <bdn/func.h>

namespace bdn
{

/** The default implementation of a Property.

	The initial value of the property is the default-constructed ValType.
	For integers that is 0.

	*/
template<class ValType>
class DefaultProperty : public Property<ValType>
{
public:
	DefaultProperty(ValType value = ValType() )
	{
		_value = value;
	}

	~DefaultProperty()
    {
        if(_pBindSourceSubscriptionControl!=nullptr)
        {
            _pBindSourceSubscriptionControl->unsubscribe();
            _pBindSourceSubscriptionControl = nullptr;
        }
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
			_onChange.notify(*this);
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

	Notifier<const ReadProperty<ValType>& >& onChange() const override
	{
		return _onChange;
	}
    
    void bind(const ReadProperty<ValType>& sourceProperty) override
	{
        if(_pBindSourceSubscriptionControl!=nullptr)
        {
            _pBindSourceSubscriptionControl->unsubscribe();
            _pBindSourceSubscriptionControl = nullptr;
        }

        _pBindSourceSubscriptionControl = sourceProperty.onChange().subscribe(weakMethod(this, &DefaultProperty::bindSourceChanged) );
        
        bindSourceChanged(sourceProperty);
    }
    
    
protected:
    virtual void bindSourceChanged(const ReadProperty<ValType>& prop)
    {
        set( prop.get() );
    }

	virtual void onValueChanged()
    {
	    // do nothing by default.
    }

	mutable Mutex					_mutex;
	ValType							_value;
    
	mutable Notifier<const ReadProperty<ValType>&>		_onChange;
	P<INotifierSubControl>						        _pBindSourceSubscriptionControl;
};


}

#endif
