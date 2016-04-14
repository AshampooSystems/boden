#include <bdn/init.h>
#include <bdn/Frame.h>
#include <bdn/Button.h>
#include <bdn/Switch.h>

#include <bdn/appInit.h>
#include <bdn/EventDispatcher.h>

#include <list>
#include <functional>

using namespace bdn;



template<class R, class... Args>
std::function<R(Args...)> asyncFromMainThread(std::function<R(Args...)> func)
{
    return  [func](Args&&... args)
            {
                return EventDispatcher::asyncCallFromMainThread(func, std::forward<Arguments>(args)...)
            };
}



template<class ValType>
class ReadableProperty : public Base
{
public:
    typedef ValType ValueType;
    
    virtual ValType   get() const=0;
    
    EventSource< ReadableProperty > onChange;
};


template<class ValType>
class Property : public ReadableProperty<Base>
{
public:
    
    ~Property()
    {
        // ensure that we unsubscribe before any of our members get destroyed
        _pBindSourceSubscription = nullptr;
    }
    
    virtual void      set(const ValType& val)=0;
    
    void bind(ReadableProperty<ValType>& sourceProperty)
    {
        _pBindSourceSubscription = sourceProperty.onChange.template subscribeMember<Property>(this, &Property::bindSourceChanged);
        
        bindSourceChanged(sourceProperty);
    }
    
    Property& operator=(const ValType& val)
    {
        set(val);
        
        return *this;
    }
    
protected:
    virtual void bindSourceChanged(const ReadableProperty& prop)
    {
        set( prop.get() );
    }
    
    P<IBase> _pBindSourceSubscription;
    
};


template<class ValType>
class SimpleProperty : public Property<ValType>
{
public:
    SimpleProperty(const ValType& val = ValType())
    {
        _value = val;
    }
    
    void      set(const ValType& val)
    {
        if(val!=_value)
        {
            _value = val;
        
            this->onChange.deliver(*this);
        }
    }
    
    ValType   get() const
    {
        return _value;
    }
    
protected:
    ValType _value;
};



template<class ValType, class ... InPropTypes >
class DerivedProperty : public ReadableProperty<ValType>
{
public:
    DerivedProperty(  std::function<ValType(const InPropTypes&...)> deriveFunc,
                      InPropTypes&... inProps )
    {
        _sourceSubDeleted = false;
        _freezeCounter = 0;
        _updatePostponed = false;
        
        // bind the input parameters to form a new combine function without parameters
        // that calls the original combineFund with the input properties as parameters.
        _derive = std::bind( deriveFunc, inProps... );
        
        // subscribe to the onChange event of the input properties
        for(auto prop: {inProps...} )
        {
            auto pSub = prop.onChange.template subscribeMember<DerivedProperty>(this, &DerivedProperty::update );
            
            _inPropSubscriptionList.push_back(pSub);
            
            // we must be notified when the subscription is deleted (i.e. when one of our source properties is deleted).
            // That is VERY important, because we do not hold a reference to the source properties - so if we access
            // them after they were deleted then we can cause a crash.
            
            pSub->onDelete.subscriberMember<DerivedProperty>(this, &DerivedProperty::onDeleteSourceSub );
        }
    
        _value = _derive();
    }
    
    ~DerivedProperty()
    {
        // ensure that we unsubscribe before we get destroyed. Note that if a notification
        // is in progress then it will have finished before clear returns (the destructor
        // of the subscription object's lock a mutex).
        _inPropSubscriptionList.clear();
    }
    
        
    ValType   get() const
    {
        MutexLock lock(_mutex);
        
        return _value;
    }
    
    
   
    class Freeze : public Base
    {
    public:
        ~Freeze()
        {
            _pParent->_unfreeze();
        }
        
    protected:
        Freeze(DerivedProperty* pParent)
        {
            _pParent = pParent;
        }
        
        DerivedProperty* _pParent;
    };
    
    
    /** Temporarily freezes the property. It will not be updated when its source properties
        change. Instead the updates will be postponed until the property is unfrozen.
     
        This is mainly useful when multiple changes to source properties are about to be done
        and you do not want intermediate changes to be propagated.
     
        A pointer to a freeze object is returned. The property will unfreeze when
        this freeze object is destroyed.
     
        Example:
     
        \code
     
        ... some code
        
        {
            auto freeze = myProp.freeze();
     
            ... update the source properties. The changes will not cause myProp to update.
     
        }   // freeze object is destroyed here and the property will now automatically update (once).
     
        \endcode
     */
    P<Freeze> freeze()
    {
        MutexLock lock(_mutex);
        
        _freezeCounter++;
        
        return newObj<Freeze>(this);
    }
    
    
    void update()
    {
        bool changed = false;
        
        {
            MutexLock lock(_mutex);
            
            if(_sourceSubDeleted)
            {
                // we cannot update because at least one of our source properties was deleted.
                // So we just keep the current value and do nothing.
            }
            else if(_freezeCounter>0)
            {
                // updating is postponed. This is often used when multiple source changes
                // are about to happen and the new value should only be derived once.
                
                _updatePostponed = true;
            }
            else
            {
                ValType newValue = _derive();
                
                if(newValue!=_value)
                {
                    _value = newValue;
                    changed = true;
                }
            }
        }
        
        if(changed)
            this->onChange.deliver(*this);
    }
    
protected:
    void onDeleteSourceSub()
    {
        // one of our source subscription is about to be deleted.
        // When that happens then we cannot derive any values anymore, since one of our
        // inputs is gone.
        MutexLock lock(_mutex);
        
        _sourceSubDeleted = true;
    }
    
    void _unfreeze()
    {
        bool needUpdate = false;
        
        {
            MutexLock lock(_mutex);
        
            _freezeCounter--;
            if(_freezeCounter==0)
            {
                needUpdate = _updatePostponed;
                _updatePostponed = false;
            }
        }
        
        if(needUpdate)
            update();
    }
    
    MutexLock                   _mutex;
    
    std::function<ValType()>    _derive;
    
    volatile ValType            _value;
    
    std::list< P<IBase> >       _inPropSubscriptionList;
    
    volatile bool               _sourceSubDeleted;
    volatile int                _freezeCounter;
    volatile bool               _updatePostponed;
};




class StringProperty : public String
{
public:
    
    EventSource<ObservableString> onChange;
    
    using String::operator=;

protected:
    virtual void onModified() override
    {
        onChange.deliver(*this);
    }
};


template<class NumType>
class ObservableNum : public Base
{
public:
    ObservableNum(NumType value=0)
    {
        _value_ = value;
    }
    
    ObservableNum(const ObservableNum& o)
    {
        _value_ = o.getValue();
    }
    
    template<class InputNumType>
    ObservableNum(const ObservableNum<InputNumType>& o)
    {
        _value_ = o.getValue();
    }
    
    
    EventSource< ObservableNum > onChange;
    
    
    NumType getValue() const
    {
        return _value_;
    }
    
    void setValue(NumType val)
    {
        _value_ = val;
        
        onChange.deliver(*this);
    }
    
    
    
    operator NumType() const
    {
        return getValue();
    }
    
    
    ObservableNum& operator=(NumType val)
    {
        setValue(val);
        return *this;
    }
    
    ObservableNum& operator=(const ObservableNum& o)
    {
        setValue(o.getValue());
        return *this;
    }
    
    template<class InputNumType>
    ObservableNum& operator=(const ObservableNum<InputNumType>& o)
    {
        setValue(o.getValue());
        return *this;
    }
    
    
    ObservableNum& operator++()
    {
        setValue( getValue()+1 );
        return *this;
    }
    
    ObservableNum operator++(int)
    {
        ObservableNum oldVal(*this);
        
        operator++();
        
        return oldVal;
    }

    
    ObservableNum& operator--()
    {
        setValue( getValue()-1 );
        return *this;
    }
    
    ObservableNum operator--(int)
    {
        ObservableNum oldVal(*this);
        
        operator--();
        
        return oldVal;
    }


    
    
    ObservableNum& operator+=(NumType val)
    {
        setValue( getValue()+val );
        return *this;
    }
    
    ObservableNum& operator-=(NumType val)
    {
        setValue( getValue()-val );
        return *this;
    }
    
    ObservableNum operator+(NumType val) const
    {
        return ObservableInt( getValue()+val );
    }
    
    ObservableNum operator-(NumType val) const
    {
        return ObservableInt( getValue()-val );
    }
    
    
    bool operator==(NumType val) const
    {
        return getValue()==val;
    }
    
    bool operator!=(NumType val) const
    {
        return getValue()!=val;
    }
    
    bool operator<(NumType val) const
    {
        return getValue()<val;
    }
    
    bool operator<=(NumType val) const
    {
        return getValue()<=val;
    }
    
    bool operator>(NumType val) const
    {
        return getValue()>val;
    }
    
    bool operator>=(NumType val) const
    {
        return getValue()>=val;
    }
    
    
    
private:
    // should not be accessed directly. Only use setValue and getValue!
    NumType _value_;
};



class Model : public Base
{
public:
    Model()
    {
    }
    
    Property<int>& helloCounter()
    {
        return _helloCounter;
    }
    
protected:
    SimpleProperty<int> _helloCounter;
};


class ViewModel : public Base
{
public:
    ViewModel(Model* pModel)
    : _helloMessage(&ViewModel::deriveHelloMessage, pModel->helloCounter() )
    {
        _pModel = pModel;
    }
    
    void increaseHelloCounter()
    {
        _pModel->helloCounter().set( pModel->helloCounter().get()+1 );
    }
    
    ReadableProperty& helloMessage()
    {
        return _helloMessage;
    }
    
protected:
    String deriveHelloMessage(const ReadableProperty<int>& counterProp)
    {
        String  message = "Hello World";
        int     counter = counterProp.get();
        
        if(counter>0)
            message += " "+std::to_string(counter);
        
        return message;
    }
    
    P<Model>                                            _pModel;
    DerivedProperty<String, ReadableProperty<int> >     _helloMessage;
};


class MainViewController : public ViewControllerBase
{
public:
    MainViewController(ViewModel* pViewModel)
    {
        _pFrame = newObj<Frame>("hello");
        
        _pButton = newObj<Button>(pFrame, "" );
        _pButton->label().bind( pViewModel->helloMessage() );
        
        _pSwitch = newObj<Switch>(pFrame, "This is a switch/checkbox");
        
        _pButton->onClick += MainViewController::onButtonClick;
    }
    
    void onButtonClick()
    {
        _pViewModel->increaseHelloCounter();
    }
    
protected:
    P<ViewModel> _pViewModel;
    
    P<Frame>    _pFrame;
    P<Button>   _pButton;
    P<Switch>   _pSwitch;
};


class AppController : public AppControllerBase
{
public:
    
    void onClick(ClickEvent& evt)
    {
        _counter++;
        
        dynamic_cast<Button*>(evt.getWindow())->setLabel("Hello World: "+std::to_string(_counter));
    }
    
    void beginLaunch(const std::map<String,String>& launchInfo) override
    {
        _pMainViewController = newObj<MainViewController>();
        
        
        pButton->getClickEventSource()->subscribeMember<UiDemoAppController>(this, &UiDemoAppController::onClick );
        
        Switch* pSwitch =
        
        pFrame->show();
    }
    
protected:
    P<MainViewController> _pMainViewController;
};


BDN_INIT_UI_APP( AppController )




