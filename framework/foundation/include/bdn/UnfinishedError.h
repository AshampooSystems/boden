#ifndef BDN_UnfinishedError_H_
#define BDN_UnfinishedError_H_

#include <stdexcept>

namespace bdn
{

    /** Thrown when a functionality is used that requires an operation to be
       finished, but that operation has not yet finished.*/
    class UnfinishedError : public std::logic_error
    {
      public:
        UnfinishedError(const String &message) : std::logic_error(message) {}

        UnfinishedError() : std::logic_error("Operation has not finished yet")
        {}
    };
}

#endif
