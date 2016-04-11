#ifndef _BDN_FRAME_H_
#define _BDN_FRAME_H_

#include <bdn/init.h>
#include <bdn/IWindow.h>

#include <string>

namespace bdn
{

    
/** A top level container / window.
 
    Frames are invisible when they are first created. You can then configure
    them and finally call show() when you are done and want to display it.
 
    On mobile platforms a Frame is the main app window and always
    covers the entire screen.
 
    On desktop platforms a Frame is a top level window that can usually
    be resized and repositioned (unless configured otherwise or running
    in fullscreen mode).
 
    */
class Frame : public Base, virtual public IWindow
{
public:
    /** @param title the title of the frame.
            On desktop platforms this is displayed at the top of the window.
            On mobile platforms this is not always visible. But it may show up under
            some circumstances (for example during app switching).
     */
    Frame(const String& title);
    
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


