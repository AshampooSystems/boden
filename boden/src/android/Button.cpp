#include <bdn/Button.h>
#include <bdn/Button_Impl.h>

#include <bdn/JavaConnector.h>

namespace bdn
{

Button::Button(IWindow* pParent, const std::string& label)
{
    _pImpl = new Impl(this, pParent, label);
}

void Button::setLabel(const std::string& label)
{
    _pImpl->setLabel(label);
}

EventSource<ClickEvent>* Button::getClickEventSource()
{
    return _pImpl->getClickEventSource();
}

void Button::show(bool visible)
{
    _pImpl->show(visible);
}


}

