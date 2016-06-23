#ifndef _BDN_FRAME_H_
#define _BDN_FRAME_H_

#include <bdn/Base.h>
#include <bdn/IWindow.h>

#include <string>

namespace bdn
{

class Frame : public Base, virtual public IWindow
{
public:
    Frame();

    virtual void show(bool visible=true) override;
    
    virtual void hide() override
    {
        show(false);
    }
    
protected:
};

}


#endif


