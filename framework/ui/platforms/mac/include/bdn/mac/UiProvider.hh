#ifndef BDN_MAC_UiProvider_HH_
#define BDN_MAC_UiProvider_HH_

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>
#include <bdn/ITextUi.h>

namespace bdn
{
    namespace mac
    {

        class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
        {
          public:
            UiProvider();

            String getName() const override;

            P<IViewCore> createViewCore(const String &coreTypeName, View *view) override;

            P<ITextUi> getTextUi() override;

            static UiProvider &get();

            double getSemSizeDips() const { return _semDips; }

            /** Returns the layout coordinator that is used by view cores
             * created by this UI provider.*/
            P<LayoutCoordinator> getLayoutCoordinator() { return _layoutCoordinator; }

          private:
            double _semDips;

            P<LayoutCoordinator> _layoutCoordinator;

            Mutex _textUiInitMutex;
            P<ITextUi> _textUi;
        };
    }
}

#endif
