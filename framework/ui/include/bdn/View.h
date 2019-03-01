#pragma once

namespace bdn
{
    class View;
    class ViewCore;
    class UIProvider;
}

#include <bdn/Layout.h>
#include <bdn/Rect.h>
#include <bdn/ViewCore.h>
#include <bdn/mainThread.h>
#include <bdn/property/Property.h>
#include <bdn/round.h>

#include <list>

namespace bdn
{
    /** Views are the building blocks of the visible user interface.
        A view presents data or provides some user interface functionality.
        For example, buttons, text fields etc are all view objects.

        View objects must be allocated with newObj or new.
       */
    class View : public Base
    {
        friend class Window;

      public:
        Property<Rect> geometry;
        Property<bool> visible;
        Property<std::shared_ptr<LayoutStylesheet>> layoutStylesheet;

      public:
        View();
        View(const View &o) = delete;

        virtual ~View();

        std::shared_ptr<View> shared_from_this() { return std::static_pointer_cast<View>(Base::shared_from_this()); }
        virtual String getViewCoreTypeName() const = 0;

      public:
        virtual Size sizeForSpace(Size availableSpace = Size::none()) const;

        std::shared_ptr<Layout> getLayout();
        void setLayout(std::shared_ptr<Layout> layout);
        void offerLayout(std::shared_ptr<Layout> layout);

        template <class T> void setLayoutStylesheet(T sheet) { layoutStylesheet = std::make_shared<T>(sheet); }

        void setViewCore(std::shared_ptr<UIProvider> uiProvider, std::shared_ptr<ViewCore> viewCore);
        std::shared_ptr<UIProvider> getUIProvider() { return _uiProvider; }

        virtual std::list<std::shared_ptr<View>> getChildViews() const { return std::list<std::shared_ptr<View>>(); }
        virtual void removeAllChildViews() {}
        virtual std::shared_ptr<View> findPreviousChildView(std::shared_ptr<View> childView) { return nullptr; }
        virtual void _childViewStolen(std::shared_ptr<View> childView) {}

        virtual std::shared_ptr<View> getParentView() { return _parentViewWeak.lock(); }
        void _setParentView(std::shared_ptr<View> parentView);

        void reinitCore(std::shared_ptr<UIProvider> uiProvider = nullptr);

        void scheduleLayout();

        std::shared_ptr<ViewCore> getViewCore() const { return _core; }

      protected:
        virtual void bindViewCore();

        template <class ValType, class CoreType> struct CorePropertyUpdater
        {
            using core_function_ptr_t = void (CoreType::*)(const ValType &);

            CorePropertyUpdater(View *view_, core_function_ptr_t setter_) : view(view_), setter(setter_) {}

            void operator()(typename Property<ValType>::value_accessor_t_ptr valueAccessor)
            {
                if (auto p = std::dynamic_pointer_cast<CoreType>(view->getViewCore())) {
                    ((*p).*setter)(valueAccessor->get());
                }

                if (view->getLayout()) {
                    view->getLayout()->markDirty(view);
                }
            }

            View *view;
            core_function_ptr_t setter;
        };

        virtual std::shared_ptr<UIProvider> determineUIProvider(std::shared_ptr<View> parentView = nullptr)
        {
            if (parentView == nullptr)
                parentView = getParentView();

            return (parentView != nullptr) ? parentView->getUIProvider() : nullptr;
        }

      private:
        bool _canMoveToParentView(std::shared_ptr<View> parentView);
        void updateLayout(std::shared_ptr<Layout> oldLayout, std::shared_ptr<Layout> newLayout);

      public:
        virtual void _deinitCore();
        virtual void _initCore(std::shared_ptr<UIProvider> uiProvider = nullptr);

      protected:
        std::shared_ptr<UIProvider> _uiProvider;
        std::weak_ptr<View> _parentViewWeak;
        std::shared_ptr<ViewCore> _core;

        template <class T> class OfferedValue
        {
          public:
            T set(T v)
            {
                T old = get();
                _own = v;
                return old;
            }

            T setOffered(T v)
            {
                T old = get();
                _offered = v;
                return old;
            }

            T get()
            {
                if (_own) {
                    return *_own;
                }
                return _offered;
            }

          private:
            T _offered;
            std::optional<T> _own;
        };

        OfferedValue<std::shared_ptr<Layout>> _layout;

        bool _hasLayoutSchedulePending;
    };
}
