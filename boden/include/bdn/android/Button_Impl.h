#ifndef _BDN_BUTTON_IMPL_H_
#define _BDN_BUTTON_IMPL_H_

#include <bdn/Button.h>
#include <bdn/JavaButtonClass.h>
#include <bdn/JavaViewGroupClass.h>
#include <bdn/JavaClickListenerProxyClass.h>

namespace bdn
{

class Button::Impl : public Base
{
public:
    Impl(Button* pButton, IWindow* pParent, const std::string& label)
    {
        _pButton = pButton;
        _pClass = JavaButtonClass::get();

        _pClickEventSource = new EventSource<ClickEvent>;

        _buttonObj = _pClass->newInstance( JavaConnector::get()->getMainActivityObject() );

        jobject viewGroupObj = JavaConnector::get()->getMainViewGroupObject();
        JavaViewGroupClass::get()->addView(viewGroupObj, _buttonObj);

        setLabel(label);
    }
    
    void setLabel(const std::string& label)
    {
        _pClass->setText( _buttonObj, label);
    }

    EventSource<ClickEvent>* getClickEventSource()
    {
        if(_clickListenerProxyObject==nullptr)
        {
            _clickListenerProxyObject = JavaClickListenerProxyClass::get()->newInstance(JavaConnector::get()->wrapPointer(_pButton) );
            _pClass->setOnClickListener(_buttonObj, _clickListenerProxyObject);
        }

        return _pClickEventSource;
    }


    void show(bool visible=true)
    {
        // 0 = visible
        // 1 = invisible
        // 2 = invisible and ignored in layout
        _pClass->setVisibility(_buttonObj, visible ? 0 : 1);
    }

    
protected:
    Button*          _pButton;
    JavaButtonClass* _pClass;
    jobject          _buttonObj;
    jobject          _clickListenerProxyObject = nullptr;

    EventSource<ClickEvent>* _pClickEventSource;
};

}


#endif


