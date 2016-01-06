#ifndef _BDN_SWITCH_IMPL_H_
#define _BDN_SWITCH_IMPL_H_

#include <bdn/Switch.h>
#include <bdn/JavaSwitchClass.h>
#include <bdn/JavaViewGroupClass.h>

namespace bdn
{

class Switch::Impl : public Base
{
public:
    Impl(Switch* pSwitch, IWindow* pParent, const std::string& label)
    {
        _pSwitch = pSwitch;
        _pClass = JavaSwitchClass::get();

        _switchObj = _pClass->newInstance( JavaConnector::get()->getMainActivityObject() );

        jobject viewGroupObj = JavaConnector::get()->getMainViewGroupObject();
        JavaViewGroupClass::get()->addView(viewGroupObj, _switchObj);

        setLabel(label);
    }

    void setLabel(const std::string& label)
    {
        _pClass->setText( _switchObj, label);
    }


protected:
    Switch*          _pSwitch;
    JavaSwitchClass* _pClass;
    jobject          _switchObj;
};

}


#endif


