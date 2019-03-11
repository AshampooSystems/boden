#pragma once

#include <bdn/View.h>

#include <list>

namespace bdn
{

    /** Base class for views that contains multiple child views.

        Derived classes must override the calcContainerLayout() and
       calcContainerPreferredSize() methods.

        */
    class ContainerView : public View
    {
      public:
        ContainerView(std::shared_ptr<UIProvider> uiProvider = nullptr);

        /** Static function that returns the type name for #ContainerView core
         * objects.*/

        static constexpr char coreTypeName[] = "bdn.ContainerViewCore";

        String viewCoreTypeName() const override { return coreTypeName; }

        void addChildView(std::shared_ptr<View> childView);
        void removeChildView(std::shared_ptr<View> childView);

        void removeAllChildViews() override;

        std::list<std::shared_ptr<View>> childViews() override;

        void childViewStolen(std::shared_ptr<View> childView) override;
    };
}
