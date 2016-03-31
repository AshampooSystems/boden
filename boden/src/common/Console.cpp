#include <bdn/init.h>
#include <bdn/Console.h>

#if 0

namespace bdn
{

Console console;


Console::Console()
{
	createMissingObjects();
}


Console::Console(std::istream* pIn, std::ostream* pOut, std::ostream* pErr)
{
	if(pIn!=nullptr)
		_pIn = newObj<LocaleStreamReader>(pIn);

	if(pOut!=nullptr)
		_pOut = newObj<LocaleStreamWriter>(pOut);

	if(pErr!=nullptr)
		_pErr = newObj<LocaleStreamWriter>(pErr);

	createMissingObjects();
}

Console::Console(std::wistream* pIn, std::wostream* pOut, std::wostream* pErr)
{
	if(pIn!=nullptr)
		_pIn = newObj<WideStreamReader>(pIn);

	if(pOut!=nullptr)
		_pOut = newObj<WideStreamWriter>(pOut);

	if(pErr!=nullptr)
		_pErr = newObj<WideStreamWriter>(pErr);

	createMissingObjects();
}

void Console::createMissingObjects()
{
#if BDN_TARGET_WINDOWS

	// On Windows we use the windows-specific functions instead of the normal stdio
	// classes. That is because with direct access we can ensure that we always have
	// proper Unicode support - no matter which codepage the system uses.

	if(_pIn==nullptr)
		_pIn = newObj<WindowsConsoleReader>( GetStdHandle( STD_INPUT_HANDLE) );
	
	if(_pOut==nullptr)
		_pOut = newObj<WindowsConsoleWriter>( GetStdHandle( STD_OUTPUT_HANDLE) );

	if(_pErr==nullptr)
		_pErr = newObj<WindowsConsoleWriter>( GetStdHandle( STD_ERROR_HANDLE) );

#else
	if(_pIn==nullptr)
	{
		if( fwide(stdin, 0)>0 )
		{
			// stdin is wide oriented
			_pIn = newObj<WideStreamReader>( &std::wcin  );
		}
		else
			_pIn = newObj<LocaleStreamReader>( &std::cin  );
	}
	
	if(_pOut==nullptr)
	{
		if( fwide(stdout, 0)>0 )
		{
			// stdout is wide oriented
			_pIn = newObj<WideStreamWriter>( &std::wcout  );
		}
		else
			_pIn = newObj<LocaleStreamWriter>( &std::cout  );
	}

	if(_pErr==nullptr)
	{
		if( fwide(stderr, 0)>0 )
		{
			// stderr is wide oriented
			_pIn = newObj<WideStreamWriter>( &std::wcerr  );
		}
		else
			_pIn = newObj<LocaleStreamWriter>( &std::cerr  );
	}
	
#endif


#if BDN_TARGET_WINDOWS
	// On Windows we do not know if stdout is connected to a terminal.
	// BUT that does not matter. If it is not a terminal then Windows will simply
	// ignore the control calls.
	_pTerminalController = newObj<WindowsTerminalController>( ::GetStdHandle(STD_OUTPUT_HANDLE) );

#else

	// on other systems we control the terminal by writing ANSI control codes. It is important
	// that we only do that when the output is actually a terminal. Otherwise we will write
	// garbage to the stream.
	// Luckily we can use isatty to check for that.
	if( isatty(stdout) )
	{
		// stdout is a terminal. See if one of our writers writes to it.
		// If it does then we should use it to ensure that caching and buffering
		// is all done in one place.

		if( _pOut->getStdConnection()==StdConnection::out )
			_pTerminalController = newObj<AnsiTerminalController>( _pOut );
		
		else if( _pErr->getStdConnection()==StdConnection::out )
			_pTerminalController = newObj<AnsiTerminalController>( _pErr );
	}

	if(_pTerminalController==nullptr && isatty(stderr) )
	{
		// stderr is a terminal.
		// Same thing as above.

		if( _pOut->getStdConnection()==StdConnection::err )
			_pTerminalController = newObj<AnsiTerminalController>( _pOut );
		
		else if( _pErr->getStdConnection()==StdConnection::err )
			_pTerminalController = newObj<AnsiTerminalController>( _pErr );
	}

#endif

	if(_pTerminalController==nullptr)
		_pTerminalController = newObj<DummyTerminalController>();
}


void Console::write(const String& s)
{	
	_pOut->write(s);
}

void Console::writeLine(const String& s)
{
	_pOut->writeLine(s);
}


void Console::writeError(const String& s)
{	
	_pErr->write(s);
}

void Console::writeErrorLine(const String& s)
{
	_pErr->writeLine(s);
}


void Console::setColor( Console::Color color)
{
	_pTerminalController->setColor(color);
}


void Console::setBackgroundColor( Console::Color color)
{
	_pTerminalController->setBackgroundColor(color);
}



}

#endif //0


