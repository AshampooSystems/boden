#pragma once

#include <bdn/UIProvider.h>

namespace bdn::ios
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

        double getSemSizeDips() const { return _semDips; }

      private:
        double _semDips;
    };
}
