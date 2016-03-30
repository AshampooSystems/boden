#ifndef BDN_Console_H_
#define BDN_Console_H_

#include <iostream>

namespace bdn
{
	

/** Used to read / write data to and from the console / command prompt / stdio. */
class Console : public Base
{
public:
	/** Constructs a console object that uses the global stdio streams for input and output.*/
	Console();

	/** Construct a console object that uses the specified stdio streams for input and output.
		The console object does not take ownership of these stream objects and will not delete them.*/
	Console(std::istream* pIn, std::ostream* pOut, std::ostream* pErr);


	/** Prints the specified line of text. A linebreak is automatically added.*/
	void printLine(const String& s);

	/** Prints the specified text (without adding a linebreak).*/
	void print(const String& s);


protected:
	std::istream* _pIn;
	std::ostream* _pOut;
	std::ostream* _pErr;
};


}