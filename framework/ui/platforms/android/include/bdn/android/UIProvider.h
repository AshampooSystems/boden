#pragma once

#include <bdn/UIProvider.h>
#include <bdn/android/ViewCore.h>

namespace bdn::android
{
    class UIProvider : virtual public bdn::UIProvider
    {
      public:
        UIProvider();
        UIProvider(UIProvider const &) = delete;
        virtual ~UIProvider() = default;

      public:
        void operator=(UIProvider const &) = delete;

      public:
        String getName() const override;

        static std::shared_ptr<UIProvider> get();

        double getSemSizeDips(bdn::android::ViewCore &core);

      private:
        double _semDips;
    };
}
