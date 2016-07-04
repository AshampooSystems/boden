#ifndef BDN_MAC_WindowCore_HH_
#define BDN_MAC_WindowCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>

#import <bdn/mac/UiProvider.hh>

#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/util.hh>


namespace bdn
{
namespace mac
{


class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS IParentViewCore
{
public:
    WindowCore(View* pOuter)
    {
        _pOuterWindowWeak = cast<Window>(pOuter);
    
        NSRect rect = rectToMacRect( pOuter->bounds() );
        
        _nsWindow  = [[NSWindow alloc] initWithContentRect:rect
                                               styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        
        
        NSRect contentRect{};
        contentRect.size = rect.size;
        
        _nsContentParent = [[NSView alloc] initWithFrame:contentRect];
        
        _nsWindow.contentView = _nsContentParent;
        
        setTitle(_pOuterWindowWeak->title());
        setVisible(_pOuterWindowWeak->visible());
    }
    
   
    NSWindow* getNSWindow()
    {
        return _nsWindow;
    }
    
    
    void setTitle(const String& title) override
    {
        [_nsWindow setTitle: stringToMacString(title)];
    }
    
    
    Rect getContentArea() override
    {
        return macRectToRect( _nsContentParent.frame );
    }
    
    
    Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        NSRect macContentRect = rectToMacRect( Rect(Point(0,0), contentSize) );
    
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];
        
        return macRectToRect(macWindowRect).getSize();
    }
    
    
    Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        NSRect macWindowRect = rectToMacRect( Rect(Point(0,0), windowSize) );
        
        NSRect macContentRect = [_nsWindow contentRectForFrameRect:macWindowRect];
        
        return macRectToRect(macContentRect).getSize();
        
    }
    

    Size calcMinimumSize() const override
    {
        return macSizeToSize( _nsWindow.minSize );
    }
    
    
    Rect getScreenWorkArea() const override
    {
        NSRect area = _nsWindow.screen.visibleFrame;
        return macRectToRect(area);
    }



    
    void	setVisible(const bool& visible) override
    {
        if(visible)
            [_nsWindow makeKeyAndOrderFront:NSApp];
        else
            [_nsWindow orderOut:NSApp];
    }
    
    
    
    void setMargin(const UiMargin& margin) override
    {
        // margins have no effect on top level windows. So, ignore.
    }
    
    
    
    void setPadding(const UiMargin& padding) override
    {
        // the outer window handles padding during layout. So nothing to do here.
    }
    

    void setBounds(const Rect& bounds) override
    {
        NSRect macBounds = rectToMacRect(bounds);
        
        [_nsWindow setFrame:macBounds display: FALSE];
    }
    
    
    
    int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixels(uiLength);
    }
    
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMargin(margin);
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
    
    
    void addChildNsView( NSView* childView ) override
    {
        [_nsContentParent addSubview:childView];
    }
    
    
protected:
    Window*     _pOuterWindowWeak;
    NSWindow*   _nsWindow;
    NSView*     _nsContentParent;
};


}
}


#endif
