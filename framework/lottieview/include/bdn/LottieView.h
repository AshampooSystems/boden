#pragma once

#include <bdn/UIUtil.h>
#include <bdn/View.h>

namespace bdn
{
    namespace lottieview::detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(LottieView)
    }

    class LottieView : public View
    {
      public:
      public:
        Property<String> url;

        Property<bool> running;
        Property<bool> loop;

      public:
        static constexpr char coreTypeName[] = "bdn.LottieViewCore";

      public:
        LottieView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~LottieView() override = default;

      public:
        void loadURL(const String &url);

        String viewCoreTypeName() const override;

      protected:
        void bindViewCore() override;
    };
}
