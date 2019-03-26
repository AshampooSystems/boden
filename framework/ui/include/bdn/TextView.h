#pragma once

#include <bdn/TextViewCore.h>
#include <bdn/View.h>

namespace bdn
{

    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(TextView)
    }

    class TextView : public View
    {
      public:
        Property<String> text;
        Property<bool> wrap;

      public:
        TextView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        static constexpr char coreTypeName[] = "bdn.TextViewCore";
        String viewCoreTypeName() const override { return coreTypeName; }

      protected:
        void bindViewCore() override;
    };
}
