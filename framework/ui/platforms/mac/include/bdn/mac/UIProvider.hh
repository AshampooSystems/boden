#pragma once

#include <bdn/UIProvider.h>

namespace bdn::mac
{
    class UIProvider : public Base, virtual public bdn::UIProvider
    {
      public:
        UIProvider();

        String getName() const override;

        static std::shared_ptr<UIProvider> get();

        double getSemSizeDips() const { return _semDips; }

      private:
        double _semDips;
    };
}
