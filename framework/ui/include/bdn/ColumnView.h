#pragma once

#include <bdn/LinearLayoutView.h>

namespace bdn
{

    /**
        @brief Represents a container view that arranges its child views
       vertically in a column layout.

        You add child views by calling the ContainerView::addChildView() method.
       By default, child views are aligned with the left of the column view. To
       change that behavior, you may set the child view's
       View::horizontalAlignment() property.

        @sa If you want to arrange child views in a horizontal layout, see the
       RowView class.
     */
    class ColumnView : public LinearLayoutView
    {
      public:
        ColumnView();
    };
}
