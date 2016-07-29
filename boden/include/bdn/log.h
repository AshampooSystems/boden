#ifndef BDN_log_H_
#define BDN_log_H_


namespace bdn
{
	

/** Logs an error to the global logging facility. See */
void logError(std::exception& e, const String& info);

/** Logs an error to the global logging facility.*/
void logError(const String& info);



}


#endif
