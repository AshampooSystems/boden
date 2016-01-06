#ifndef _BDN_SWITCH_H_
#define _BDN_SWITCH_H_

#include <bdn/Base.h>
#include <bdn/IWindow.h>

#include <string>

namespace bdn
{

class Switch : public Base
{
public:
    Switch(IWindow* pParent, const std::string& label);

    class Impl;
    Impl* getImpl()
    {
        return _pImpl;
    }

protected:
    Impl* _pImpl;
};

}


#endif


