#pragma once

#include <bdn/Base.h>
#include <bdn/String.h>

namespace bdn
{
    class View;

    class LayoutStylesheet
    {
      public:
        // The first member MUST be a const char* !
        String type() { return *((const char **)this); }
        bool isType(String typeName) { return type() == typeName; }
    };

    class Layout : public Base
    {
      public:
        virtual void registerView(View *view) = 0;
        virtual void unregisterView(View *view) = 0;

        virtual void markDirty(View *view) = 0;

        virtual void updateStylesheet(View *view) = 0;

        virtual void layout(View *view) = 0;
    };
}
