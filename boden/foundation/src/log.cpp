#include <bdn/init.h>
#include <bdn/log.h>

#include <iostream>

#if BDN_PLATFORM_ANDROID
#include <android/log.h>
#endif

namespace bdn
{


void logError(const std::exception& e, const String& additionalInfo) noexcept
{
    try
    {
	    logError( additionalInfo + " ("+String(e.what())+")" );
    }
    catch(...)
    {
        // ignore
    }
}


#if BDN_PLATFORM_ANDROID

void logError(const String& message) noexcept
{
    try
    {
        __android_log_write(ANDROID_LOG_INFO, "boden", message.asUtf8Ptr() );
    }
    catch(...)
    {
        // ignore
    }
}

void logInfo(const String& message) noexcept
{
    try
    {
	    __android_log_write(ANDROID_LOG_INFO, "boden", message.asUtf8Ptr() );
    }
    catch(...)
    {
        // ignore
    }
}

#else

void logError(const String& message) noexcept
{
    try
    {
	    std::cerr << ("ERROR: "+message) << std::endl;
    }
    catch(...)
    {
        // ignore
    }
}

void logInfo(const String& message) noexcept
{
    try
    {
	    std::cerr << ("Info: "+message) << std::endl;
    }
    catch(...)
    {
        // ignore
    }
}


#endif


void logActiveException(const String& additionalInfo) noexcept
{
    std::exception_ptr p = std::current_exception();
}

}

