#ifndef BDN_ANDROID_IdleRunner_H_
#define BDN_ANDROID_IdleRunner_H_

#include <bdn/ISimpleCallable.h>

#include <list>

namespace bdn
{
namespace android
{


/** Executes handlers when the app enters idle state (i.e. when there are not pending
 * 	events.*/
class IdleRunner : public Base
{
public:

	static IdleRunner& get();

	void callOnceWhenIdle( ISimpleCallable* pCallable);

    /** Used internally. Notifies the idle runner that an idle phase has begun.*/
    void _beginningIdlePhase();


private:

    std::list< P<ISimpleCallable> >     _callWhenIdleList;
};

}
}

#endif
