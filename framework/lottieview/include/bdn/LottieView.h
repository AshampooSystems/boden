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
        LottieView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~LottieView() override = default;

      public:
        void loadURL(const String &url);

      protected:
        void bindViewCore() override;

      public:
        class Core
        {
          public:
            Property<bool> running;
            Property<bool> loop;

          public:
            virtual void loadURL(const String &url) = 0;
        };
    };
}
