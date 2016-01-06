#ifndef _BDN_Error_H_
#define _BDN_Error_H_

#include <exception>


namespace bdn
{

	/** Base class for boden errors.*/
	class Error : public std::exception
	{
	public:
		Error(		const String& message="",
					const String& errorDomain="",
					const String& errorCode="",
					const std::map<String,String>& paramMap = std::map<String,String>(),
					const Error* pCause=nullptr)
		{
			_paramMap = paramMap;

			if(!message.isEmpty())
				_paramMap["-message-"] = message;

			if(!errorDomain.isEmpty())
				_paramMap["-errorDomain-"] = errorDomain;

			if(!errorCode.isEmpty())
				_paramMap["-errorCode-"] = errorCode;
		}

		Error(const std::map<String, String>& paramMap)
		{
			_paramMap = paramMap;
		}

		Error(const Error& o)
		{
			_paramMap = o._paramMap;
		}

		String getMessage() const
		{
			return getParam("-message-");
		}

		String getErrorDomain() const
		{
			return getParam("-errorDomain-");
		}

		String getErrorCode() const
		{
			return getParam("-errorCode-");
		}

		String getParam(const String& paramName) const
		{
			auto it = _paramMap.find(paramName);
			if (it != _paramMap.end())
				return it->second;
			else
				return "";
		}

		bool hasParam(const String& paramName) const
		{
			return _paramMap.find(paramName) != _paramMap.end();
		}
		
		const std::map<String, String>& getParamMap() const
		{
			return _paramMap;
		}

		std::map<String, String>& getParamMap()
		{
			return _paramMap;
		}

		virtual const char* what() const noexcept override
		{
			if (_pWhat == nullptr)
				_pWhat = new std::string(getMessage().toCP());

			return _pWhat->c_str();
		}

	protected:
		std::map<String,String> _paramMap;
		P<std::string>			_pWhat;
	};



}

#endif


