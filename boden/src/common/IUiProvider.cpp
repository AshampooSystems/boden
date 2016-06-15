#include <bdn/init.h>
#include <bdn/IUiProvider.h>


namespace bdn
{

static P<IUiProvider>& _getDefaultUiProviderRef()
{
	static P<IUiProvider> pProvider;

	return pProvider;
}

static Mutex& _getDefaultUiProviderMutex()
{
	static Mutex mutex;
	
	return mutex;
}

P<IUiProvider> getDefaultUiProvider()
{
	MutexLock		lock( _getDefaultUiProviderMutex() );
	P<IUiProvider>& pProvider = _getDefaultUiProviderRef();

	if(pProvider==nullptr)
		pProvider = getPlatformUiProvider();

	return pProvider;
}

/** Sets the default UI provider to use.	

	The default UI provider is used for newly created top level windows.
	Child views inherit the UI provider of their parent by default.

	Changing the default UI provider will only affect newly created windows.
	The UI provider of any existing windows is not modified by this.

	This function is thread-safe.
*/
void setDefaultUiProvider(IUiProvider* pProvider)
{
	MutexLock		lock( _getDefaultUiProviderMutex() );
	
	_getDefaultUiProviderRef() = pProvider;
}


}





