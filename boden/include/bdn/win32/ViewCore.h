#ifndef BDN_WIN32_ViewCore_H_
#define BDN_WIN32_ViewCore_H_

#include <bdn/View.h>
#include <bdn/win32/Win32Window.h>
#include <bdn/win32/Font.h>

namespace bdn
{
namespace win32
{

/** Base implementation for win32 view cores (see IViewCore).
*/
class ViewCore : public Win32Window, BDN_IMPLEMENTS IViewCore
{
public:
	ViewCore(	View* pOuterView,
				const String& className,
				const String& name,
				DWORD style,
				DWORD exStyle,
				int x=0,
				int y=0,
				int width=0,
				int height=0 );

    ~ViewCore();


    Rect adjustAndSetBounds(const Rect& requestedBounds) override;
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const override;
		
	void setVisible(const bool& visible) override;
			
	void setPadding(const Nullable<UiMargin>& padding) override;
            
    void setHorizontalAlignment(const View::HorizontalAlignment& align);
    void setVerticalAlignment(const View::VerticalAlignment& align);


	double uiLengthToDips(const UiLength& uiLength) const override;
	Margin uiMarginToDipMargin(const UiMargin& margin) const override;



	/** Called when the outer view's parent has changed.
		
		tryChangeParentView should try to move the core over to the new parent.

		Note that this is also called when the order of child views inside the
		same parent has changed. In that case the "new parent" will be the same as the old parent.
		The core should update its ordering position inside the parent accordingly.

		If successful then it should return true, otherwise false.

		If false is returned then this will cause the outer view object to
		automatically re-create the core for the new parent and release the current
		core object.		
		*/
	bool tryChangeParentView(View* pNewParent) override;


	/** Causes the core to check the ordering of its view among its sibling views
		and update it accordingly. This is called when the view order inside the parent
		has changed.
		
		The default implementation does nothing.
		*/
	virtual void updateOrderAmongSiblings();


	/** Returns the handle of the win32 window for the specified view.
		If the view is a light view without its own win32 window then the
		handle of the first ancestor with a win32 is returned.

		Returns NULL if pView is null or if pView does not have an associated
		core object.
		*/
	static HWND getViewHwnd(View* pView)
	{
		if(pView!=nullptr)
		{
			IViewCore* pCore = pView->getViewCore();
			if(pCore!=nullptr)
				return cast<ViewCore>(pCore)->getHwnd();
		}

		return NULL;
	}


	/** Sets the view core's UI scale factor.
		The scale factor depends on the screen DPI (or what Windows calls "DPI" - it is not really the physical DPI
		of the screen). It can change at any time, for example when system settings are changed or when the view's
		window is moved to another screen.		
		*/
	void setUiScaleFactor(double factor);


	double getUiScaleFactor() const
	{
		return _uiScaleFactor;
	}


    P<View> getOuterViewIfStillAttached()
    {
        return _outerViewWeak.toStrong();
    }

    P<View> getOuterViewIfStillAttached() const
    {
        return _outerViewWeak.toStrong();
    }

    
    
protected:

	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

	
	/** This is called by the parent window when it receives a message that actually applies
		to this window.
		For example, Windows sends WM_COMMAND messages for button clicks to the button's parent,
		rather than the button. The parent then forwards the message to the button
		by calling its handleParentMessage function.
		*/
	virtual void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	/** Updates the core's font.*/
	void updateFont();


	/** Returns the child view core that the specified message should be forwarded to.*/
	virtual P<ViewCore> findChildCoreForMessage(UINT message, WPARAM wParam, LPARAM lParam);


	WeakP<View> _outerViewWeak;	// weak by design

	/** See setUiScaleFactor() */
	double _uiScaleFactor;

	P<Font> _pFont;
};


}
}

#endif

