#include <bdn/init.h>
#include <bdn/Frame.h>
#include <bdn/Button.h>
#include <bdn/Switch.h>

#include <bdn/appInit.h>
#include <bdn/AppControllerBase.h>

#include <list>
#include <functional>

using namespace bdn;

#if 0





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
    
    virtual ValType get() const=0;
    
    Notifier<const ReadableProperty&> onChange;
};


template<class ValType>
class Property : public ReadableProperty<ValType>
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
    
    virtual void set(const ValType& val) override
    {
        if(val!=_value)
        {
            _value = val;
        
            this->onChange.deliver(*this);
        }
    }
    
    virtual ValType get() const override
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
        _pModel->helloCounter().set( _pModel->helloCounter().get()+1 );
    }
    
    ReadableProperty<String>& helloMessage()
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

#endif

class MainViewController : public Base //ViewControllerBase
{
public:
    MainViewController()//ViewModel* pViewModel)
    {
        _pFrame = newObj<Frame>("hello");
        
        _pButton = newObj<Button>(_pFrame, "" );
        //_pButton->label().bind( pViewModel->helloMessage() );
        
        _pSwitch = newObj<Switch>(_pFrame, "This is a switch/checkbox");
        
        //_pButton->onClick().subscribeMember<MainViewController>(this, &MainViewController::onButtonClick);
    }
    
    void onButtonClick()
    {
        //_pViewModel->increaseHelloCounter();
    }
    
protected:
    //P<ViewModel> _pViewModel;
    
    P<Frame>    _pFrame;
    P<Button>   _pButton;
    P<Switch>   _pSwitch;
};


class AppController : public UiAppControllerBase
{
public:
   
    
    void beginLaunch() override
    {
        _pMainViewController = newObj<MainViewController>();
    }
    
protected:
    P<MainViewController> _pMainViewController;
};


BDN_INIT_UI_APP( AppController )



