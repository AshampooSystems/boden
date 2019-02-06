#pragma once

#include <Cocoa/Cocoa.h>

namespace bdn
{
    namespace mac
    {

        /** Interface for view cores that can act as parents for other view
         * cores.*/
        class IParentViewCore
        {
          public:
            /** Adds a child UI element to the parent.*/
            virtual void addChildNsView(NSView *nsView) = 0;
        };
    }
}
