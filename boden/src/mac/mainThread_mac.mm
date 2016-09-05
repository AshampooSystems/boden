#include <bdn/init.h>
#include <bdn/mainThread.h>

#import <Cocoa/Cocoa.h>


@interface SimpleCallableWrapper : NSObject
{
    bdn::P<bdn::ISimpleCallable> pCallable;
    double delaySeconds;
}

@property bdn::P<bdn::ISimpleCallable> pCallable;
@property double delaySeconds;

- (void)invoke;

@end

@implementation SimpleCallableWrapper

@synthesize pCallable;
@synthesize delaySeconds;

-(void)invoke
{
    if(delaySeconds<=0)
        pCallable->call();
    else
    {
        double delay = delaySeconds;
        delaySeconds = 0;   // set to 0 so that we will not wait again
    
        [self performSelector:@selector(invoke)
                      withObject:nil
                      afterDelay:delay];
    }
}

@end


namespace bdn
{
	

void CallFromMainThreadBase_::dispatch()
{
    SimpleCallableWrapper* wrapper = [[SimpleCallableWrapper alloc] init];
    wrapper.pCallable = this;
    wrapper.delaySeconds = 0;
    
    [wrapper performSelectorOnMainThread:@selector(invoke)
                                 withObject:nil
                              waitUntilDone:NO];
    
}


void CallFromMainThreadBase_::dispatchWithDelaySeconds(double seconds)
{
    SimpleCallableWrapper* wrapper = [[SimpleCallableWrapper alloc] init];
    wrapper.pCallable = this;
    wrapper.delaySeconds = seconds;
    
    [wrapper performSelectorOnMainThread:@selector(invoke)
                              withObject:nil
                           waitUntilDone:NO];
    
}


}

