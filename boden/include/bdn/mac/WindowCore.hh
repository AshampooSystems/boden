#ifndef BDN_MAC_WindowCore_HH_
#define BDN_MAC_WindowCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>
#include <bdn/windowCoreUtil.h>

#import <bdn/mac/UiProvider.hh>

#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/util.hh>


namespace bdn
{
namespace mac
{


class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS IParentViewCore, BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
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
    
    
    void setMargin(const UiMargin& margin) override
    {
        // Ignore - window margins have no effect.
    }

    
    
    void invalidateSizingInfo(View::InvalidateReason reason) override
    {
        // nothing to do since we do not cache sizing info in the core.
    }
    
    
    void needLayout(View::InvalidateReason reason) override
    {
        P<View> pOuterView = getOuterWindowIfStillAttached();
        if(pOuterView!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pOuterView->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->viewNeedsLayout( pOuterView );
        }
    }
    
    void childSizingInfoInvalidated(View* pChild) override
    {
        P<View> pOuterView = getOuterWindowIfStillAttached();
        if(pOuterView!=nullptr)
        {
            pOuterView->invalidateSizingInfo( View::InvalidateReason::childSizingInfoInvalidated );
            pOuterView->needLayout( View::InvalidateReason::childSizingInfoInvalidated );
        }
    }
    
    
    
    void setHorizontalAlignment(const View::HorizontalAlignment& align) override
    {
        // do nothing. The View handles this.
    }
    
    void setVerticalAlignment(const View::VerticalAlignment& align) override
    {
        // do nothing. The View handles this.
    }
    
    
    void setPreferredSizeHint(const Size& hint) override
    {
        // nothing to do by default. Most views do not use this.
    }
    
    
    void setPreferredSizeMinimum(const Size& limit) override
    {
        // do nothing. The View handles this.
    }
    
    void setPreferredSizeMaximum(const Size& limit) override
    {
        // do nothing. The View handles this.
    }
    


    
    Rect adjustAndSetBounds(const Rect& requestedBounds) override;
    
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override;

    

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

   
   
    
    void layout() override;
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;
    
    
    void requestAutoSize() override;
    void requestCenter() override;
    
    void autoSize() override;
    void center() override;
    
    
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

    Rect getContentArea()
    {
        // the content parent is inside the inverted coordinate space of the window
        // origin is bottom left. So we need to pass the content height, so that
        // the coordinates can be flipped.
        return macRectToRect( _nsContentParent.frame, _nsContentParent.frame.size.height );
    }
    
    
    Rect getScreenWorkArea() const
    {
        NSScreen* screen = _getNsScreen();
        
        NSRect workArea = screen.visibleFrame;
        NSRect fullArea = screen.frame;
        
        return macRectToRect(workArea, fullArea.size.height);
    }

    
    Size getMinimumSize() const
    {
        return macSizeToSize( _nsWindow.minSize );
    }
    
    Margin getNonClientMargin() const
    {
        Size dummyContentSize = getMinimumSize();
        
        NSRect macContentRect = rectToMacRect( Rect(Point(0,0), dummyContentSize), -1 );
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];
        
        Rect windowRect = macRectToRect(macWindowRect, -1);
        
        return Margin(
                      fabs( windowRect.y ),
                      fabs( windowRect.x+windowRect.width-dummyContentSize.width ),
                      fabs( windowRect.y+windowRect.height-dummyContentSize.height ),
                      fabs( windowRect.x ) );
    }
    
    Size calcWindowSizeFromContentAreaSize(const Size& contentSize)
    {
        // we can ignore the coordinate space being inverted here. We do not
        // care about the position, just the size.
        NSRect macContentRect = rectToMacRect( Rect(Point(0,0), contentSize), -1 );
        
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];
        
        return macRectToRect(macWindowRect, -1).getSize();
    }
    
    
    Size calcContentAreaSizeFromWindowSize(const Size& windowSize)
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
    
    


    
    
    double getEmSizeDips() const
    {
        if(_emDipsIfInitialized==-1)
        {
            // windows on mac cannot have their own font attached. So
            // use the system font size
            _emDipsIfInitialized = getSemSizeDips();
        }
        
        return _emDipsIfInitialized;
    }
    
    
    double getSemSizeDips() const
    {
        if(_semDipsIfInitialized==-1)
            _semDipsIfInitialized = UiProvider::get().getSemSizeDips();
        
        return _semDipsIfInitialized;
    }
    


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
    
    mutable double  _emDipsIfInitialized = -1;
    mutable double  _semDipsIfInitialized = -1;

};



}
}


#endif
