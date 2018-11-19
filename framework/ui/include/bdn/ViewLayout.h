#ifndef BDN_ViewLayout_H_
#define BDN_ViewLayout_H_

namespace bdn
{

    /** Stores the layout for a set of views. This is usually created by
        ContainerView::calcContainerLayout().

        The "layout" may consist of values for several properties of the view,
       including size and position.

        Layouts can also contain values for arbitrary custom properties. For
       example, the layout could switch a control into a different mode. To add
       these custom values, one has to subclass ViewLayout::ViewLayoutData and
       extend it to store the additional information. The subclass will have to
       override ViewLayout::ViewLayoutData::applyToView() to update the view
       accordingly.

        */
    class ViewLayout : public Base
    {
      public:
        /** Applies the layout to the children of the specified parent view.

            If the layout does not contain data for a child view then the view
           is simply left unmodified.
        */
        virtual void applyTo(View *parentView) const
        {
            List<P<View>> childList;
            parentView->getChildViews(childList);

            for (auto &childView : childList) {
                P<const ViewLayoutData> data = getViewLayoutData(childView);
                if (data != nullptr)
                    data->applyTo(childView);
            }
        }

        /** Stores the layout information for one View object.*/
        class ViewLayoutData : public Base
        {
          public:
            /** Applies the layout data to the specified view.

                Subclasses that store modification information for custom
                properties should override this and apply the custom
               modifications.*/
            virtual void applyTo(View *view) const
            {
                if (_boundsInitialized)
                    view->adjustAndSetBounds(_bounds);
            }

            /** Sets the view's bounds.*/
            void setBounds(const Rect &bounds)
            {
                _bounds = bounds;
                _boundsInitialized = true;
            }

            /** If bounds were set then they are stored in the bounds parameter
               and true is returned. If bounds were not set then the bounds
               parameter is set to a zero rect and false is returned.*/
            bool getBounds(Rect &bounds) const
            {
                if (_boundsInitialized) {
                    bounds = _bounds;
                    return true;
                } else {
                    bounds = Rect();
                    return false;
                }
            }

          private:
            Rect _bounds;
            bool _boundsInitialized = false;
        };

        /** Sets the layout data for the view. If the layout already has data
           for the specified view then it is replaced.*/
        void setViewLayoutData(View *view, ViewLayoutData *data) { _dataMap[view] = data; }

        /** Returns the layout data for the specified view, or null if no data
           has been set for the view.*/
        P<ViewLayoutData> getViewLayoutData(View *view)
        {
            auto it = _dataMap.find(view);
            if (it != _dataMap.end())
                return it->second;
            else
                return nullptr;
        }

        P<const ViewLayoutData> getViewLayoutData(View *view) const
        {
            auto it = _dataMap.find(view);
            if (it != _dataMap.end())
                return it->second;
            else
                return nullptr;
        }

      private:
        Size _size;
        std::map<View *, P<ViewLayoutData>> _dataMap;
    };
}

#endif
