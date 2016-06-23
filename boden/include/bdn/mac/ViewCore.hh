#ifndef BDN_MAC_ViewCore_HH_
#define BDN_MAC_ViewCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IViewCore.h>
#include <bdn/PropertyWithMainThreadDelegate.h>


namespace bdn
{
namespace mac
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore()
    {
        _view = nullptr;
    }
    
    ~ViewCore()
    {
        _pVisible->detachDelegate();
    }
    
    Property<bool>& visible()
    {
        return *_pVisible;
    }
    
    ReadProperty<bool>& visible() const
    {
        return *_pVisible;
    }
    
protected:
    void initView(NSView* view)
    {
        _view = view;
        
        _pVisible = newObj<PropertyWithMainThreadDelegate<bool> >( newObj<VisibleDelegate>(_view), true);
    }

    class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
    {
    public:
        VisibleDelegate(NSView* view)
        {
            _view = view;
        }
        
        void	set(const bool& val)
        {
            _view.hidden = !val;
        }
        
        bool get() const
        {
            return !_view.hidden;
        }
        
        NSView* _view;
    };
    
    NSView* _view;
    
    P< PropertyWithMainThreadDelegate<bool> > _pVisible;
};


}
}


#endif
