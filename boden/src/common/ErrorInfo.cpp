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
		
			bool removedSpace = false;
			if(startIt!=_message.begin())
			{
				// if a space precedes the fields string then we remove that as well
				--startIt;
				if(*startIt==' ')
					removedSpace = true;
				else
					++startIt;
			}

			if(!removedSpace && endIt!=_message.end())
			{
				// if a space follows the fields string and we have not removed one before the
				// fields string then we remove the space.
				if(*endIt==' ')
				{
					++endIt;
					removedSpace = true;
				}
			}
			
			_message.erase(startIt, endIt);			
		}
	}
}




}


