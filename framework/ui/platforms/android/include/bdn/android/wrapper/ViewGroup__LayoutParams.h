#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kViewGroupLayoutParamsClassName[] = "android/view/ViewGroup$LayoutParams";

    class ViewGroup__LayoutParams : public java::wrapper::JTObject<kViewGroupLayoutParamsClassName, int, int>
    {
      public:
        using java::wrapper::JTObject<kViewGroupLayoutParamsClassName, int, int>::JTObject;

      public:
        enum
        {
            /** Special value for the height or width requested by a View.
             * MATCH_PARENT means that the view wants to be as big as its
             * parent, minus the parent's padding, if any. Introduced in API
             * Level 8.*/
            MATCH_PARENT = -1,

            /** Special value for the height or width requested by a View.
             WRAP_CONTENT means that the view wants to be just large enough
             to fit its own internal content, taking its own padding into
             account.*/
            WRAP_CONTENT = -2
        };

        /** Information about how tall the view wants to be. Can be one of
         * the constants FILL_PARENT (replaced by MATCH_PARENT in API Level
         * 8) or WRAP_CONTENT, or an exact size.
         * */
        bdn::java::ObjectField<int> width()
        {
            static bdn::java::ObjectField<int>::Id fieldId(getStaticClass_(), "width");

            return bdn::java::ObjectField<int>(getRef_(), fieldId);
        }

        /** Information about how wide the view wants to be. Can be one of
         * the constants FILL_PARENT (replaced by MATCH_PARENT in API Level
         * 8) or WRAP_CONTENT, or an exact size.
         * */
        bdn::java::ObjectField<int> height()
        {
            static bdn::java::ObjectField<int>::Id fieldId(getStaticClass_(), "height");

            return bdn::java::ObjectField<int>(getRef_(), fieldId);
        }
    };
}
