#ifndef _BDN_FRAME_H_
#define _BDN_FRAME_H_

#include <bdn/init.h>
#include <bdn/IWindow.h>

#include <string>

namespace bdn
{

class Frame : public Base, virtual public IWindow
{
public:
    Frame();
    
    class Impl;
    Impl* getImpl()
    {
        return _impl;
    }
    
    void center();
    
    virtual void show(bool visible=true) override;
    
    virtual void hide() override
    {
        show(false);
    }
    
protected:
    Impl* _impl;
};

}


#endif


