#pragma once

namespace bdn
{
    namespace fk
    {

        /** Examines the active exception and rethrows it as a NSException
           (Objective C exception). If the exception is already an NSException
           then it it rethrown as is. Other exceptions are wrapped into an
           NSException object and then that is thrown.*/
        void rethrowAsNSException();
    }
}
