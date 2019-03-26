#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/SwitchCore.h>
#include <bdn/TriState.h>
#include <bdn/View.h>

#include <bdn/SimpleNotifier.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Switch)
    }

    class Switch : public View
    {
      public:
        Property<String> label;
        Property<bool> on;

      public:
        Switch(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        TriState state() const;

      public:
        ISyncNotifier<const ClickEvent &> &onClick();

      public:
        static constexpr char coreTypeName[] = "bdn.SwitchCore";
        String viewCoreTypeName() const override { return coreTypeName; }

      protected:
        void bindViewCore() override;

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
