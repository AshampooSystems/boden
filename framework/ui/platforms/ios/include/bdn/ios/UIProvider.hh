#pragma once

#include <bdn/UIProvider.h>

namespace bdn::ios
{
    class UIProvider : public Base, virtual public bdn::UIProvider
    {
      public:
        UIProvider();
        UIProvider(UIProvider const &) = delete;
        void operator=(UIProvider const &) = delete;

        String getName() const override;

        static std::shared_ptr<UIProvider> get();

        /** Returns the size of 1 sem in DIPs.*/
        double getSemSizeDips() const { return _semDips; }

      private:
        double _semDips;
    };
}
