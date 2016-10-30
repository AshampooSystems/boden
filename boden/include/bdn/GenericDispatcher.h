#ifndef BDN_GenericDispatcher_H_
#define BDN_GenericDispatcher_H_

#include <bdn/IDispatcher.h>>

namespace bdn
{


class GenericDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
{
public:
	GenericDispatcher();


	void enqueue( std::function<void()> func, Priority priority = Priority::normal ) override;


	void enqueueInSeconds(double seconds, std::function<void()> func, Priority priority = Priority::normal ) override;


protected:
	std::list< std::function< void() > > _priorityLists[4];
};

}

#endif
