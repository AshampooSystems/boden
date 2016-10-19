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
    WindowCore(View* pOuter);
    
    ~WindowCore();
   
    NSWindow* getNSWindow()
    {
        return _nsWindow;
    }
    
    
    P<Window> getOuterWindowIfStillAttached()
    {
        return _pOuterWindowWeak.toStrong();
    }
    
    P<const Window> getOuterWindowIfStillAttached() const
    {
        return _pOuterWindowWeak.toStrong();
    }

    
    void setTitle(const String& title) override
    {
        [_nsWindow setTitle: stringToMacString(title)];
    }
    
    
    Rect getContentArea() override
    {
        // the content parent is inside the inverted coordinate space of the window
        // origin is bottom left. So we need to pass the content height, so that
        // the coordinates can be flipped.
        return macRectToRect( _nsContentParent.frame, _nsContentParent.frame.size.height );
    }
    
    
    Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        // we can ignore the coordinate space being inverted here. We do not
        // care about the position, just the size.
        NSRect macContentRect = rectToMacRect( Rect(Point(0,0), contentSize), -1 );
    
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];
        
        return macRectToRect(macWindowRect, -1).getSize();
    }
    
    
    Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        // we can ignore the coordinate space being inverted here. We do not
        // care about the position, just the size.
        NSRect macWindowRect = rectToMacRect( Rect(Point(0,0), windowSize), -1 );
        
        NSRect macContentRect = [_nsWindow contentRectForFrameRect:macWindowRect];
        
        Size resultSize = macRectToRect(macContentRect, -1).getSize();
        
        resultSize.width = std::max(resultSize.width, 0.0);
        resultSize.height = std::max(resultSize.height, 0.0);
        
        return resultSize;
    }
    

    Size getMinimumSize() const override
    {
        return macSizeToSize( _nsWindow.minSize );
    }
    
    
    
    
    Rect getScreenWorkArea() const override
    {
        NSScreen* screen = _getNsScreen();
    
        NSRect workArea = screen.visibleFrame;
        NSRect fullArea = screen.frame;
        
        return macRectToRect(workArea, fullArea.size.height);
    }



    
    void	setVisible(const bool& visible) override
    {
        if(visible)
            [_nsWindow makeKeyAndOrderFront:NSApp];
        else
            [_nsWindow orderOut:NSApp];
    }
    
        
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // the outer window handles padding during layout. So nothing to do here.
    }
    
    
    
    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        // first adjust the bounds so that they are on pixel boundaries
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
        
        if(adjustedBounds!=_currActualWindowBounds)
        {
            // the screen's coordinate system is inverted (from our point of view). So we need to
            // flip the coordinates.
            NSScreen* screen = _getNsScreen();
            
            // the screen's coordinate system is inverted. So we need to
            // flip the coordinates.
            NSRect macRect = rectToMacRect(adjustedBounds, screen.frame.size.height);
            
            [_nsWindow setFrame:macRect display: FALSE];
            _currActualWindowBounds = adjustedBounds;
        }
        
        return adjustedBounds;
    }
    
    
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override
    {
        NSScreen* screen = _getNsScreen();
        
        // the screen's coordinate system is inverted (from our point of view). So we need to
        // flip the coordinates.
        double screenHeight = screen.frame.size.height;
        NSRect macRect = rectToMacRect( requestedBounds, screenHeight);
        
        NSAlignmentOptions alignOptions = 0;
        
        // our "position" indicates the top/left position of the window. However, in the
        // flipped screen coordinate system the (0,0) is actually the bottom left cordner
        // of the window.
        // The top/left of the window is minX/maxY.
        if(positionRoundType==RoundType::down)
            alignOptions |= NSAlignMinXOutward | NSAlignMaxYOutward;
        
        else if(positionRoundType==RoundType::up)
            alignOptions |= NSAlignMinXInward | NSAlignMaxYInward;
        
        else
            alignOptions |= NSAlignMinXNearest | NSAlignMaxYNearest;
        
        
        if(sizeRoundType==RoundType::down)
            alignOptions |= NSAlignWidthInward | NSAlignHeightInward;
        
        else if(sizeRoundType==RoundType::up)
            alignOptions |= NSAlignWidthOutward | NSAlignHeightOutward;
        
        else
            alignOptions |= NSAlignWidthNearest | NSAlignHeightNearest;
        
        
        NSRect adjustedMacRect =
            [_nsWindow backingAlignedRect:macRect
                                options:alignOptions ];
        
        Rect adjustedBounds = macRectToRect( adjustedMacRect, screenHeight);
        
        return adjustedBounds;
    }
    
    

    

	double uiLengthToDips(const UiLength& uiLength) const override
    {        
         switch( uiLength.unit )
        {
        case UiLength::Unit::none:
            return 0;

        case UiLength::Unit::dip:
            return uiLength.value;

        case UiLength::Unit::em:
            return uiLength.value * getEmSizeDips();

        case UiLength::Unit::sem:
			return uiLength.value * getSemSizeDips();

        default:
			throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
        }
	}

    
	Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        return Margin(
            uiLengthToDips(margin.top),
            uiLengthToDips(margin.right),
            uiLengthToDips(margin.bottom),
            uiLengthToDips(margin.left) );
    }

   
    
    
    Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
    {
        // the implementation for this must be provided by the outer Window object.
        throw NotImplementedError("WindowCore::calcPreferredSize");
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
    
    
    void _movedOrResized()
    {
        _currActualWindowBounds = macRectToRect( _nsWindow.frame, _getNsScreen().frame.size.height );

        P<Window> pOuter = getOuterWindowIfStillAttached();
        if(pOuter!=nullptr)
            pOuter->adjustAndSetBounds( _currActualWindowBounds );
    }
    
private:

    NSScreen* _getNsScreen() const
    {
        NSScreen* screen = _nsWindow.screen;
        
        if(screen==nil) // happens when window is not visible
            screen = [NSScreen mainScreen];
        
        return screen;
    }
    
    
    WeakP<Window>   _pOuterWindowWeak;
    NSWindow*       _nsWindow;
    NSView*         _nsContentParent;
    
    NSObject*       _ourDelegate;
    
    Rect            _currActualWindowBounds;
};


}
}


#endif
