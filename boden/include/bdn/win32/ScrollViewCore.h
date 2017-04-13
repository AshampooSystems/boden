#ifndef BDN_WIN32_ScrollViewCore_H_
#define BDN_WIN32_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/Win32WindowClass.h>

#include <bdn/IScrollViewCore.h>


namespace bdn
{
namespace win32
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore
{
public:
	ScrollViewCore(ScrollView* pOuter);			
        
	void setHorizontalScrollingEnabled(bool enabled) override;    
	void setVerticalScrollingEnabled(bool enabled) override;    

    void layout() override;
	
	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override;	

private:

    void updateStyle();

    class ScrollViewCoreClass : public Win32WindowClass
	{
	public:
		ScrollViewCoreClass()
			: Win32WindowClass("bdnScrollView", ViewCore::windowProc)
		{
			ensureRegistered();			
		}

		static ScrollViewCoreClass& get();
	};

    bool _showHorizontalScrollBar = false;
    bool _showVerticalScrollBar = false;
};

}
}

#endif

