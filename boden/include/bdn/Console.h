#ifndef BDN_Console_H_
#define BDN_Console_H_

#include <iostream>

#if 0

namespace bdn
{
	

/** Used to read / write data to and from the console / command prompt / stdio.

	Note that there is a global Console object called console that is simply connected to stdin, stdout and stderr.

	Example:

	\code

	console.writeLine("hello");

	console << "world";

	console.setColor( Console::Color::red );
	console << "this is red";

	String line = console.readLine();

	\endcode
*/
class Console : public Base
{
public:
	/** Constructs a console object that uses the global stdio streams for input and output.*/
	Console();

	/** Construct a console object that uses the specified stdio streams for input and output.
		The console object does not take ownership of these stream objects and will not delete them.
		
		Any of the input stream pointers can be null, in which case the Console will use the global
		stdio stream in its place.
		*/
	Console(std::istream* pIn, std::ostream* pOut, std::ostream* pErr);

	/** Construct a console object that uses the specified stdio streams for input and output.
		The console object does not take ownership of these stream objects and will not delete them.
		
		Any of the input stream pointers can be null, in which case the Console will use the global
		stdio stream in its place.
		*/
	Console(std::wistream* pIn, std::wostream* pOut, std::wostream* pErr);


	void setIn( std::istream* pStream );
	void setIn( std::wistream* pStream );

	void setOut( std::ostream* pStream );
	void setOut( std::wostream* pStream );

	void setError( std::ostream* pStream );
	void setError( std::wostream* pStream );


	/** Writes the specified text (without adding a linebreak).*/
	void write(const String& s);


	/** Writes the specified line of text. A linebreak is automatically added.*/
	void writeLine(const String& s);


	/** Prints the specified text (without adding a linebreak) to the error output.*/
	void writeError(const String& s);
	
	/** Prints the specified line of text to the error output. A linebreak is automatically added.*/
	void writeErrorLine(const String& s);	


	enum Color
	{
		black = 0,

		darkRed = 1,
		darkGreen = 2,	
		darkBlue = 4,

		darkYellow = darkRed | darkGreen,
		darkMagenta = darkRed | darkBlue,
		darkCyan = darkGreen | darkBlue,

		gray = darkRed | darkGreen | darkBlue,

		brightBit = 8,

		darkGray = black | brightBit,

		red = darkRed | brightBit,
		green = darkGreen | brightBit,		
		blue = darkBlue | brightBit,

		yellow = darkYellow | brightBit,
		magenta = darkMagenta | brightBit,
		cyan = darkCyan | brightBit,

		white = gray | brightBit,

		brown = darkYellow,
		purple = darkMagenta
	};


	/** Sets the text color for text that is written after this call. Not all consoles support colored text - if it is not supported
		then this function does nothing.*/
	void setColor( Console::Color color );


	/** Sets the background color for text written after this call. Not all consoles support colored text - if it is not supported
		then this function does nothing.*/
	void setBackgroundColor( Console::Color color );


	
protected:
	enum class StdConnection
	{
		none,

		out,
		in,
		err
	};


	class IWriter : BDN_IMPLEMENTS IBase
	{
	public:
		virtual void write(const String& s)=0;
		virtual void writeLine(const String& s)=0;
		
		virtual StdConnection getStdConnection()=0;
	};

	class IReader : BDN_IMPLEMENTS IBase
	{
	public:
	};

	class ITerminalController : BDN_IMPLEMENTS IBase
	{
	public:

		virtual void setColor(Console::Color color)=0;
		virtual void setBackgroundColor(Console::Color color)=0;
	};


	class DummyTerminalController : public Base, BDN_IMPLEMENTS ITerminalController
	{
	public:

		void setColor(Console::Color color) override
		{
			// do nothing
		}

		void setBackgroundColor(Console::Color color) override
		{
			// do nothing
		}
	};

	class LocaleStreamWriter : public Base, BDN_IMPLEMENTS IWriter
	{
	public:
		LocaleStreamWriter(std::ostream* pStream)
		{
			_pStream = pStream;
			_connectedToTerminal = -1;
		}

		void write(const String& s) override
		{
			std::string l = s.toLocaleEncoding( _pStream->getloc() );
			_pStream->write( l.data(), l.length() );
		}

		void writeLine(const String& s) override
		{
			write(s);
			(*_pStream) << std::endl;
		}

		StdConnection getStdConnection() override
		{
			// the API of std::ostream does not offer a good way to find out
			// what the stream is actually connected to.
			// However, there is a slightly hackish way:
			
			if( _pStream->rdbuf()==std::cout.rdbuf() )
				return StdConnection::out;
			else if( _pStream->rdbuf()==std::cerr.rdbuf() )
				return StdConnection::err;
			else
				return StdConnection::none;			
		}

	protected:
		std::ostream*	_pStream;
		int				_connectedToTerminal;
	};

	class LocaleStreamReader : public Base, BDN_IMPLEMENTS IReader
	{
	public:
		LocaleStreamReader(std::istream* pStream)
		{
			_pStream = pStream;
		}


	protected:
		std::istream* _pStream;
	};


	class WideStreamWriter : public Base, BDN_IMPLEMENTS IWriter
	{
	public:
		WideStreamWriter(std::wostream* pStream)
		{
			_pStream = pStream;
		}

		void write(const String& s) override
		{
			const std::wstring& w = s.asWide();
			_pStream->write( w.data(), w.length() );
		}

		void writeLine(const String& s) override
		{
			write(s);
			(*_pStream) << std::endl;
		}

		StdConnection getStdConnection() override
		{
			// the API of std::ostream does not offer a good way to find out
			// what the stream is actually connected to.
			// However, there is a slightly hackish way:
			
			if( _pStream->rdbuf()==std::wcout.rdbuf() )
				return StdConnection::out;
			else if( _pStream->rdbuf()==std::wcerr.rdbuf() )
				return StdConnection::err;
			else
				return StdConnection::none;			
		}

	protected:
		std::wostream* _pStream;
	};


	class WideStreamReader : public Base, BDN_IMPLEMENTS IReader
	{
	public:
		WideStreamReader(std::wistream* pStream)
		{
			_pStream = pStream;
		}


	protected:
		std::wistream* _pStream;
	};


#if BDN_PLATFORM_WINDOWS_CLASSIC

	class WindowsConsoleWriter : public Base, BDN_IMPLEMENTS IWriter
	{
	public:
		WindowsConsoleWriter(HANDLE consoleHandle)
		{
			_consoleHandle = consoleHandle;
		}

		void write(const String& s) override
		{
			DWORD				charsWritten = 0;
			const std::wstring& wide = s.asWide();

			::WriteConsoleW( _consoleHandle, wide.c_str(), wide.length(), &charsWritten, NULL );
		}

		void writeLine(const String& s) override
		{
			write(s);

			DWORD charsWritten=0;
			::WriteConsoleW( _consoleHandle, L"\r\n", 2, &charsWritten, NULL );
		}		

		StdConnection getStdConnection() override
		{
			if( _consoleHandle == ::GetStdHandle(STD_OUTPUT_HANDLE) )
				return StdConnection::out;

			else if( _consoleHandle == ::GetStdHandle(STD_ERROR_HANDLE) )
				return StdConnection::err;

			else
				return StdConnection::none;			
		}

	protected:
		HANDLE _consoleHandle;
	};

	class WindowsConsoleReader : public Base, BDN_IMPLEMENTS IReader
	{
	public:
		WindowsConsoleReader(HANDLE consoleHandle)
		{
			_consoleHandle = consoleHandle;
		}

	protected:
		HANDLE _consoleHandle;
	};

	class WindowsTerminalController : public Base, BDN_IMPLEMENTS ITerminalController
	{
	public:
		WindowsTerminalController(HANDLE consoleHandle)
		{
			_consoleHandle = consoleHandle;
		}

		void setColor(Console::Color color) override
		{
			WORD attrib = 0;

			if( (color & darkRed)!=0 )
				attrib |= FOREGROUND_RED;
			
			if( (color & darkGreen)!=0 )
				attrib |= FOREGROUND_GREEN;

			if( (color & darkBlue)!=0 )
				attrib |= FOREGROUND_BLUE;

			if( (color & 8)!=0 )
				attrib |= FOREGROUND_INTENSITY;
			
			::SetConsoleTextAttribute(_consoleHandle, attrib );
		}

		void setBackgroundColor(Console::Color color) override
		{
			WORD attrib = 0;

			if( (color & darkRed)!=0 )
				attrib |= BACKGROUND_RED;
			
			if( (color & darkGreen)!=0 )
				attrib |= BACKGROUND_GREEN;

			if( (color & darkBlue)!=0 )
				attrib |= BACKGROUND_BLUE;

			if( (color & 8)!=0 )
				attrib |= BACKGROUND_INTENSITY;
			
			::SetConsoleTextAttribute(_consoleHandle, attrib );
		}
	
	protected:
		HANDLE _consoleHandle;
	};

#endif


	class AnsiTerminalController : public Base, BDN_IMPLEMENTS ITerminalController
	{
	public:
		AnsiTerminalController(IWriter* pWriter)
		{
			_pWriter = pWriter;

			_code.reserve(10);			
		}

		void setColor(Console::Color color) override
		{
			initColorCode(color, '3');

			_pWriter->write(_code );
		}

		void setBackgroundColor(Console::Color color) override
		{
			initColorCode(color, '4');
			
			_pWriter->write(_code);
		}

	protected:
		void initColorCode(Console::Color color, char32_t funcChar )
		{
			// our colors use the same basic numbering system
			// as ANSI. Bit 0 for red, 1 for green, 2 for blue.
			int colorNum = color & 7;

			_code.clear();
			_code += '\x1b';
			_code += '[';
			_code += funcChar;
			_code += (char32_t)('0'+colorNum);

			if( (color & 8)!=0 )	// bright version. Set "bold / increased intensity" flag.
			{
				_code += ';';
				_code += '1';
			}

			_code += 'm';			
		}

		String		_code;

		P<IWriter> _pWriter;
	};
	
	void createMissingObjects();

	P<IReader> _pIn;
	P<IWriter> _pOut;
	P<IWriter> _pErr;

	P<ITerminalController> _pTerminalController;
};


extern Console console;

}

#endif //0

#endif

