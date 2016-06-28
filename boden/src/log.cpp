#include <bdn/init.h>
#include <bdn/log.h>

#include <iostream>

namespace bdn
{

void logError(std::exception& e, const String& info)
{
	logError( info + " ("+String(e.what())+")" );
}

void logError(const String& info)
{
	std::cerr << ("ERROR: "+info) << std::endl;
}

}

