#ifndef BDN_log_H_
#define BDN_log_H_


namespace bdn
{
	

/** Logs an exception as an error entry to the global logging facility.
 *  additionalInfo is a string message that can be used to add additional information about how and
 *  when the error occurred.
 * */
void logError(const std::exception& e, const String& additionalInfo) noexcept;

/** Logs an error message to the global logging facility.*/
void logError(const String& message) noexcept;


/** Logs an info message to the global logging facility.*/
void logInfo(const String& message) noexcept;



}


#endif
