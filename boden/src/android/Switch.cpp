#include <bdn/Switch.h>
#include <bdn/Switch_Impl.h>

#include <bdn/JavaConnector.h>

namespace bdn
{

Switch::Switch(IWindow* pParent, const std::string& label)
{
    _pImpl = new Impl(this, pParent, label);
}



}

