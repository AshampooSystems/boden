#ifndef BDN_GTK_ContainerViewCore_H_
#define BDN_GTK_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/gtk/ViewCore.h>

namespace bdn
{
    namespace gtk
    {

        class ContainerViewCore : public ViewCore
        {
          public:
            ContainerViewCore(View *pOuter)
                : ViewCore(pOuter, gtk_layout_new(nullptr, nullptr))
            {}

            void setPadding(const Nullable<UiMargin> &uiPadding) override
            {
                // padding is handled manually, so no need to set anything in
                // the GTK widget
            }

            Size calcPreferredSize(
                const Size &availableSpace = Size::none()) const override
            {
                // call the outer container's preferred size calculation

                P<ContainerView> pOuterView =
                    cast<ContainerView>(getOuterViewIfStillAttached());
                if (pOuterView != nullptr)
                    return pOuterView->calcContainerPreferredSize(
                        availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                P<ContainerView> pOuterView =
                    cast<ContainerView>(getOuterViewIfStillAttached());
                if (pOuterView != nullptr) {
                    P<ViewLayout> pLayout =
                        pOuterView->calcContainerLayout(pOuterView->size());
                    pLayout->applyTo(pOuterView);
                }
            }

            void _addChildViewCore(ViewCore *pChildCore) override
            {
                P<View> pChildView = pChildCore->getOuterViewIfStillAttached();

                Point position;
                if (pChildView != nullptr)
                    position = pChildView->position();

                GdkRectangle rect = rectToGtkRect(Rect(position, Size(1, 1)));

                gtk_layout_put(GTK_LAYOUT(getGtkWidget()),
                               pChildCore->getGtkWidget(), rect.x, rect.y);
            }

            void _moveChildViewCore(ViewCore *pChildCore, int gtkX,
                                    int gtkY) override
            {
                gtk_layout_move(GTK_LAYOUT(getGtkWidget()),
                                pChildCore->getGtkWidget(), gtkX, gtkY);
            }
        };
    }
}

#endif
