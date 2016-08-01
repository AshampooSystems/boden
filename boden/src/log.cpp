#include <bdn/init.h>
#include <bdn/log.h>

#include <iostream>

#if BDN_PLATFORM_ANDROID
#include <android/log.h>
#endif

namespace bdn
{


void logError(std::exception& e, const String& additionalInfo)
{
	logError( additionalInfo + " ("+String(e.what())+")" );
}


#if BDN_PLATFORM_ANDROID

void logError(const String& message)
{
    __android_log_write(ANDROID_LOG_INFO, "boden", message.asUtf8Ptr() );
}

void logInfo(const String& message)
{
	__android_log_write(ANDROID_LOG_INFO, "boden", message.asUtf8Ptr() );
}

#else

void logError(const String& message)
{
	std::cerr << ("ERROR: "+message) << std::endl;
}

void logInfo(const String& message)
{
	std::cerr << message << std::endl;
}


#endif

}

