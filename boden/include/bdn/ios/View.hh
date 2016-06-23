#ifndef BDN_View_HH_
#define BDN_View_HH_

#import <UIKit/UIKit.h>

#include <bdn/IView.h>
#include <bdn/PropertyWithMainThreadDelegate.h>


namespace bdn
{

class View : public Base, BDN_IMPLEMENTS IView
{
public:
    View()
    {
        _view = nullptr;
    }
    
    ~View()
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
    void initView(UIView* view, UIView* secondaryView = nil)
    {
        _view = view;
        
        _pVisible = newObj<PropertyWithMainThreadDelegate<bool> >( newObj<VisibleDelegate>(_view, secondaryView), true);
    }

    class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
    {
    public:
        VisibleDelegate(UIView* view, UIView* secondaryView)
        {
            _view = view;
            _secondaryView = secondaryView;
        }
        
        void	set(const bool& val)
        {
            _view.hidden = !val;
            if(_secondaryView!=nil)
                _secondaryView.hidden = !val;
        }
        
        bool get() const
        {
            return !_view.hidden;
        }
        
        UIView* _view;
        UIView* _secondaryView;
    };
    
    UIView* _view;
    
    P< PropertyWithMainThreadDelegate<bool> > _pVisible;
};


}


#endif
