#ifndef BDN_GTK_UiProvider_H_
#define BDN_GTK_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <bdn/log.h>
#include <bdn/LayoutCoordinator.h>
#include <bdn/ITextUi.h>

#include <gtk/gtk.h>

namespace bdn
{
    namespace gtk
    {

        class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
        {
          public:
            UiProvider();

            String getName() const override;

            P<IViewCore> createViewCore(const String &coreTypeName,
                                        View *pView) override;

            /** Returns the size of 1 sem in DIPs. See UiLength::Unit::sem for
             * more information.*/
            double getSemSizeDips() const { return _semDips; }

            /** Returns the layout coordinator that is used by view cores
             * created by this UI provider.*/
            P<LayoutCoordinator> getLayoutCoordinator()
            {
                return _pLayoutCoordinator;
            }

            P<ITextUi> getTextUi() override;

            static UiProvider &get();

          private:
            double _semDips;

            P<LayoutCoordinator> _pLayoutCoordinator;

            Mutex _textUiInitMutex;
            P<ITextUi> _pTextUi;
        };
    }
}

#endif
