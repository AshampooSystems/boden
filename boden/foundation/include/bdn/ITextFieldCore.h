#ifndef BDN_ITextFieldCore_H_
#define BDN_ITextFieldCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{

    class ITextFieldCore : BDN_IMPLEMENTS IViewCore
    {
      public:
        // Implement setter functions for property observers here
        virtual void setText(const String &text) = 0;
    };
}

#endif
