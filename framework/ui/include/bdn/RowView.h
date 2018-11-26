#ifndef BDN_RowView_H_
#define BDN_RowView_H_

#include <bdn/LinearLayoutView.h>

namespace bdn
{

    /**
        @brief Represents a container view that arranges its child views
       horizontally in a row layout.

        You add child views by calling the ContainerView::addChildView() method.
       By default, child views are aligned with the top of the row view. To
       change that behavior, you may set the child view's
       View::verticalAlignment() property.

        @sa If you want to arrange child views in a vertical layout, see the
       ColumnView class.
     */
    class RowView : public LinearLayoutView
    {
      public:
        RowView();
    };
}

#endif
