#include <bdn/init.h>
#include <bdn/ErrorInfo.h>

#include <bdn/Uri.h>

namespace bdn
{
	

ErrorInfo::ErrorInfo(const std::exception& error)
	: ErrorInfo( error.what() )
{
}

ErrorInfo::ErrorInfo(const String& errorMessage)
{
	_message = errorMessage;

	auto startIt = _message.find("[[", _message.begin());
	if(startIt != _message.end())
	{
		auto fieldsStartIt = startIt+2;

		auto fieldsEndIt = _message.find("]]", fieldsStartIt);
		if(fieldsEndIt != _message.end() )
		{
			auto endIt = fieldsEndIt+2;
			
			String fieldString = _message.subString(fieldsStartIt, fieldsEndIt);
			_fields = ErrorFields(fieldString);
		
			if(startIt!=_message.begin())
			{
				// if a space precedes the fields string then we remove that as well
				--startIt;
				if(*startIt!=' ')
					++startIt;
			}
			
			_message.erase(startIt, endIt);			
		}
	}
}




}


