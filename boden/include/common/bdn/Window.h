#ifndef BDN_Window_H_
#define BDN_Window_H_

#include <bdn/View.h>

#include <bdn/IWindowCore.h>
#include <bdn/IUiProvider.h>

namespace bdn
{

/** A top level window.	

	Windows have a single child view (the "content view") that displays the window
	contents. Usually this content view will be a view container object,
	which can then contain multiple child views.
*/
class Window : public View
{
public:
	/** @param pUiProvider the UI provider that the window should use.
			See the IUiProvider documentation for more information.
			If this is nullptr then the default UI provider for the platform is used.*/
	Window(IUiProvider* pUiProvider = nullptr)
	{
		_pUiProvider = (pUiProvider!=nullptr) ? pUiProvider : getDefaultUiProvider();
		_pTitle = newObj< PropertyWithMainThreadDelegate<String> >( nullptr, "" );
	}
	

	/** Sets the specified view as the content view of the window.
		Note that windows can only have a single child content view. If one is already
		set then it will be replaced.
		See #Window class documentation for more information.*/
	void setContentView(View* pContentView)
	{
		removeAllChildViews();

		addChildView(pContentView);
	}


	/** Returns the window's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	P<View> getContentView()
	{
		if(_childViews.empty())
			return nullptr;
		else
			return _childViews.front();
	}


	/** Returns the window's title property.

		Depending on the platform, the title may or may not be visible in a window title bar on
		the screen.
		On some platforms a window does not have a title bar, so the title may be invisible.
		However, it can also be used in other places (e.g. when switching between windows, etc.),
		so it should always be set to a reasonable string.

		It is safe to use the property from any thread.
		*/
	Property<String>& title() override
	{
		return *_pTitle;
	}

	ReadProperty<String>& title() const override
	{
		return *_pTitle;
	}

	

	/** Static function that returns the type name for #Window objects.*/
	static String getWindowViewTypeName()
	{
		return "bdn.Window";
	}

	String getViewTypeName() const override
	{
		return getWindowViewTypeName();
	}


protected:	
	void		setViewCore(IViewCore* pCore) override
	{
		_pCore = cast<IWindowCore>(pCore);

		if(pCore==nullptr)
			_pTitle->setDelegate(nullptr);
		else
		{
			// note that it might be tempting to add a parameter here that prevents the property
			// to update the delegate with its current value. After all, the core was just created,
			// so the delegate should already have the correct value.
			// BUT it is important to note that properties can be changed from ANY thread.
			// So the property value might have changed in the short time since the core was created.
			_pTitle->setDelegate( newObj<ITitleCore::TitleDelegate>(pCore) );
		}

		View::setViewCore(pCore);
	}

	IViewCore*	getViewCore() override
	{
		return _pCore;
	}	

	P<IWindowCore> _pCore;

	P< PropertyWithMainThreadDelegate<String> > _pTitle;
};

}

#endif

