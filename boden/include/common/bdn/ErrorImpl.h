#ifndef BDN_ErrorImpl_H_
#define BDN_ErrorImpl_H_

#include <bdn/IErrorParams.h>

#include <system_error>

namespace bdn
{


/** Helper for ErrorImpl. Do not use this directly.*/
class _ErrorParamsMixin : public Base, BDN_IMPLEMENTS IErrorParams
{
public:
	_ErrorParamsMixin(	const std::map<String,String>& paramMap = std::map<String,String>() )
	{
		_paramMap = paramMap;
	}

	_ErrorParamsMixin(const _ErrorParamsMixin& o)
	{
		_paramMap = o._paramMap;
	}

	~_ErrorParamsMixin() throw()
	{
	}



	String getParam(const String& paramName) const override
	{
		auto it = _paramMap.find(paramName);
		if(it!=_paramMap.end())
			return it->second;
		else
			return String();
	}


	
	bool hasParam(const String& paramName) const override
	{
		return (_paramMap.find(paramName) != _paramMap.end());
	}


	const std::map<String, String>& getParamMap() const override
	{
		return _paramMap;
	}


	std::map<String, String>& getParamMap() override
	{
		return _paramMap;
	}
	
protected:
	std::map<String,String> _paramMap;
};


/** Implementation helper for error classes. Derives from
	a standard C++ exception class and adds an implementation
	of #IErrorParams to them.

	T should be the type of the error class that derives from
	ErrorImpl (i.e. the class you are currently implementing).

	ExceptionBase should be one of the standard exception base classes.
		
	Use std::logic_error for logic errors / programming errors
	(like invalid argument, invalid values, etc.)

	Use std::system_error for errors that have a numerical error code.

	Use std::runtime_error for errors that can legitimately
	occur at runtime (i.e. errors which are not logic errors), but
	that do not have a numerical error code associated wth them.

	Example:

	\code
	class MyErrorClass : public ErrorImpl< MyErrorClass, std::system_error >
	{
	public:
		...
	};
	\endcode
	*/
template<typename T, typename ExceptionBase >
class ErrorImpl : public ExceptionBase, public _ErrorParamsMixin
{
public:
	ErrorImpl(	const String& message="",
				const std::map<String,String>& paramMap = std::map<String,String>() )
		: ExceptionBase(message)
		, _ErrorParamsMixin(paramMap)
	{
	}
	
	ErrorImpl(const ErrorImpl& o)
		: ExceptionBase( o )
		, _ErrorParamsMixin(o)
	{
	}
};


template<typename T>
class ErrorImpl<typename T, std::system_error>  : public std::system_error, public _ErrorParamsMixin
{
public:
	ErrorImpl(	const String& message="",				
				const std::map<String,String>& paramMap = std::map<String,String>())
		: ErrorImpl(std::error_code(), message, paramMap)
	{
	}

	ErrorImpl(const ErrorImpl& o)
		: std::system_error( o )
		, _ErrorParamsMixin(o)
	{
	}

	ErrorImpl(	const std::error_code& errorCode,
				const std::map<String,String>& paramMap = std::map<String,String>())
		: std::system_error( errorCode )
		, _ErrorParamsMixin(paramMap)
	{
	}

	ErrorImpl(	const std::error_code& errorCode,
				const String& message,
				const std::map<String,String>& paramMap = std::map<String,String>())
		: std::system_error( errorCode, message.toStd_Utf8() )
		, _ErrorParamsMixin(paramMap)
	{
	}

	ErrorImpl(	int errorNumber,
				const std::error_category& errorCategory,				
				const std::map<String,String>& paramMap = std::map<String,String>())
		: std::system_error( errorNumber, errorCategory )
		, _ErrorParamsMixin(paramMap)
	{
	}

	ErrorImpl(	int errorNumber,
				const std::error_category& errorCategory,				
				const String& message,
				const std::map<String,String>& paramMap = std::map<String,String>())
		: std::system_error( errorNumber, errorCategory, message.toStd_Utf8() )
		, _ErrorParamsMixin(paramMap)
	{
	}

};

}


#endif
