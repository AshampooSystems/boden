#ifndef BDN_WIN32_MessageDispatcher_H_
#define BDN_WIN32_MessageDispatcher_H_

#include <bdn/IDispatcher.h>
#include <bdn/RequireNewAlloc.h>

#include <bdn/win32/MessageWindowBase.h>

namespace bdn
{
namespace win32
{
    
/** IDispatcher implementation that uses the windows message loop.*/
class MessageDispatcher : public RequireNewAlloc<MessageWindowBase, MessageDispatcher>, BDN_IMPLEMENTS IDispatcher
{
public:
	MessageDispatcher();
    ~MessageDispatcher();
    
	void enqueue(
		std::function< void() > func,
		Priority priority = Priority::normal ) override;
    	
	void enqueueInSeconds(
		double seconds,
		std::function< void() > func,
		Priority priority = Priority::normal ) override;


	void createTimer(
		double intervalSeconds,
		std::function< bool() > func ) override;
    	

};
  		

    
}    
}

#endif

