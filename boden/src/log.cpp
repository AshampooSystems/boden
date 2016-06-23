#include <bdn/init.h>
#include <bdn/log.h>

#include <iostream>

namespace bdn
{

void logError(std::exception& e, const String& info)
{
	std::cerr << ("ERROR: "+info+" ("+String(e.what())+")") << std::endl;
}

}

