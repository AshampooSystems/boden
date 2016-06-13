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

		initProperty<String, IWindowCore, &IWindowCore::setTitle>(_title);
	}
	

	/** Sets the specified view as the content view of the window.
		Note that windows can only have a single child content view. If one is already
		set then it will be replaced.
		See #Window class documentation for more information.*/
	void setContentView(View* pContentView)
	{
		MutexLock lock(_mutex);

		if(pContentView!=_pContentView)
		{
			if(_pContentView!=nullptr)
				_pContentView->setParentView(nullptr);

			_pContentView = pContentView;

			_pContentView->setParentView(this);
		}
	}


	/** Returns the window's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	P<View> getContentView()
	{
		MutexLock lock(_mutex);

		return _pContentView;
	}


	/** Returns the window's title property.

		Depending on the platform, the title may or may not be visible in a window title bar on
		the screen.
		On some platforms a window does not have a title bar, so the title may be invisible.
		However, it can also be used in other places (e.g. when switching between windows, etc.),
		so it should always be set to a reasonable string.

		It is safe to use the property from any thread.
		*/
	Property<String>& title()
	{
		return _title;
	}

	const ReadProperty<String>& title() const
	{
		return _title;
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


	void getChildViews(std::list< P<View> >& childViews)
	{
		MutexLock lock(_mutex);

		if(_pContentView!=nullptr)
			childViews.push_back(_pContentView);	
	}


protected:		
	DefaultProperty<String> _title;

	P<View>					_pContentView;
};

}

#endif

