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
		auto endIt = _message.find("]]", startIt);
		if(endIt != _message.end() )
		{
			endIt+=2;
			
			String fieldString = _message.subString(startIt, endIt);
			_fields = ErrorFields(fieldString);
		
			bool removedSpaceAtStart = false;
			if(startIt!=_message.begin())
			{
				// if a space precedes the fields string then we remove that as well
				--startIt;
				if(*startIt==' ')
					removedSpaceAtStart = true;
				else
					++startIt;
			}

			if(!removedSpaceAtStart)
			{
				while(endIt!=_message.end() && *endIt==' ')
					++endIt;

				if(endIt!=_message.end() && *endIt==':')
					++endIt;

				while(endIt!=_message.end() && *endIt==' ')
					++endIt;
			}
			
			_message.erase(startIt, endIt);			
		}
	}
}




}


