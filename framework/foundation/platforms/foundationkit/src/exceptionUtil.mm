#include <bdn/init.h>

#include <bdn/foundationkit/exceptionUtil.h>

#import <bdn/foundationkit/objectUtil.hh>
#import <bdn/foundationkit/stringUtil.hh>

#include <bdn/ExceptionReference.h>
#include <bdn/ErrorInfo.h>

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace fk
    {

        void rethrowAsNSException()
        {
            try {
                throw;
            }
            catch (NSException *exception) {
                // let NSExceptions through
                throw;
            }
            catch (...) {
                bdn::ErrorInfo info(std::current_exception());

                NSString *message = stringToNSString(info.getMessage());

                NSObject *wrappedException =
                    bdn::fk::wrapIntoNSObject(bdn::ExceptionReference::newFromActiveException());

                NSDictionary *userInfoDict =
                    [[NSDictionary alloc] initWithObjectsAndKeys:wrappedException, @"bdn::ExceptionReference", nil];

                @throw [NSException exceptionWithName:@"CppException" reason:message userInfo:userInfoDict];
            }
        }
    }
}
