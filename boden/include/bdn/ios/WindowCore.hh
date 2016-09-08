#ifndef BDN_IOS_WindowCore_HH_
#define BDN_IOS_WindowCore_HH_

#import <UIKit/UIKit.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>

#import <bdn/ios/util.hh>
#import <bdn/ios/ViewCore.hh>

namespace bdn
{
namespace ios
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
private:
    UIWindow* _createUIWindow(Window* pOuterWindow);
    
public:
    WindowCore(Window* pOuterWindow)
    : ViewCore(pOuterWindow, _createUIWindow(pOuterWindow) )
    {
        _window = (UIWindow*)getUIView();
        
        // set the outer object's bounds to the bounds of the ios window
        pOuterWindow->bounds() = iosRectToRect(_window.frame);
    }
    
    ~WindowCore()
    {
        // there are always references to a visible window. So we have to make
        // sure that the window is hidden before we release our own reference.
        if(_window!=nil)
        {
            _window.hidden = YES;
            
            _window.rootViewController = nil;
            
            _window = nil;
        }
    }
    
    UIWindow* getUIWindow() const
    {
        return _window;
    }
    
    
    void setBounds(const Rect& bounds) override
    {
        // we do not modify our frame. Just reset the bounds property back to the current bounds.
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pView->bounds() = iosRectToRect(_window.frame);
    }
    
    
    void setTitle(const String& title) override
    {
        _window.rootViewController.title = stringToIosString(title);
    }
    
    
    Rect getContentArea() override
    {
        // Same size as bounds. There is no border or title bar on ios.
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            return Rect( Point(0,0), pView->bounds().get().getSize() );
        else
            return Rect();
    }
    
    
    Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        // no border or title bar. So window size = content size
        return contentSize;
    }
    
    
    Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        // no border or title bar. So window size = content size
        return windowSize;
    }
    
    
    Size calcMinimumSize() const override
    {
        // no border or title bar => no minimum size.
        return Size(0,0);
    }
    
    
    
    
    Rect getScreenWorkArea() const override
    {
        UIScreen* screen = _getUIScreen();
        
        return iosRectToRect(screen.nativeBounds);
    }
    
    
    
    Size calcPreferredSize() const override
    {
        // the implementation for this must be provided by the outer Window object.
        throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");
    }
    
    
    int calcPreferredHeightForWidth(int width) const override
    {
        // the implementation for this must be provided by the outer Window object.
        throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");
    }
    
    
    int calcPreferredWidthForHeight(int height) const override
    {
        // the implementation for this must be provided by the outer Window object.
        throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");
    }
    
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        // we don't have a parent. Report that we cannot do this.
        return false;
    }
    
            
private:
    UIScreen* _getUIScreen() const
    {
        return _window.screen;
    }

    UIWindow* _window;
};

}
}


#endif
