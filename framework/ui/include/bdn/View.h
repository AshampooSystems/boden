#pragma once

namespace bdn
{
    class View;
    class ViewCore;
    class UIProvider;
}

#include <bdn/Layout.h>
#include <bdn/OfferedValue.h>
#include <bdn/Rect.h>
#include <bdn/ViewCore.h>
#include <bdn/mainThread.h>
#include <bdn/property/Property.h>
#include <bdn/round.h>

#include <list>

namespace bdn
{
    class View : public Base
    {
        friend class Window;

      public:
        Property<Rect> geometry;
        Property<bool> visible;
        Property<std::shared_ptr<LayoutStylesheet>> layoutStylesheet;

      public:
        View() = delete;
        View(std::shared_ptr<UIProvider> uiProvider = nullptr);
        View(const View &o) = delete;

        ~View() override;

        auto shared_from_this() { return std::static_pointer_cast<View>(Base::shared_from_this()); }
        auto shared_from_this() const { return std::static_pointer_cast<View const>(Base::shared_from_this()); }

        virtual String viewCoreTypeName() const = 0;

      public:
        virtual Size sizeForSpace(Size availableSpace = Size::none()) const;

        std::shared_ptr<Layout> getLayout();
        void setLayout(std::shared_ptr<Layout> layout);
        void offerLayout(std::shared_ptr<Layout> layout);

        template <class T> void setLayoutStylesheet(T sheet) { layoutStylesheet = std::make_shared<T>(sheet); }

        std::shared_ptr<UIProvider> uiProvider() { return _uiProvider; }

        virtual std::list<std::shared_ptr<View>> childViews() { return std::list<std::shared_ptr<View>>(); }
        virtual void removeAllChildViews() {}
        virtual void childViewStolen(const std::shared_ptr<View> &childView) {}

        virtual std::shared_ptr<View> getParentView() { return _parentView.lock(); }

        void scheduleLayout();

        template <class T> auto core() { return std::dynamic_pointer_cast<T>(viewCore()); }

        std::shared_ptr<ViewCore> viewCore();
        std::shared_ptr<ViewCore> viewCore() const;

        void setParentView(const std::shared_ptr<View> &parentView);

      protected:
        virtual void bindViewCore();

      protected:
        void onCoreLayout();
        void onCoreDirty();

      private:
        bool canMoveToParentView(const std::shared_ptr<View> &parentView);
        void updateLayout(const std::shared_ptr<Layout> &oldLayout, const std::shared_ptr<Layout> &newLayout);

      private:
        void lazyInitCore() const;

      protected:
        OfferedValue<std::shared_ptr<Layout>> _layout;

      private:
        mutable std::shared_ptr<ViewCore> _core;
        WeakCallback<void()>::Receiver _layoutCallbackReceiver;
        WeakCallback<void()>::Receiver _dirtyCallbackReceiver;

        std::shared_ptr<UIProvider> _uiProvider;
        std::weak_ptr<View> _parentView;
        bool _hasLayoutSchedulePending{false};
    };

    template <typename ViewType, typename P> void registerCoreCreatingProperties(ViewType *view, P p)
    {
        p->onChange() += [=](auto) { view->viewCore(); };
    }

    template <typename ViewType, typename P, typename... Prest>
    void registerCoreCreatingProperties(ViewType *view, P p, Prest... rest)
    {
        registerCoreCreatingProperties(view, p);
        registerCoreCreatingProperties(view, rest...);
    }

    class SingleChildHelper
    {
      public:
        void update(const std::shared_ptr<View> &self, const std::shared_ptr<View> &newChild)
        {
            if (_currentChild) {
                _currentChild->setParentView(nullptr);
                _currentChild->offerLayout(nullptr);
            }
            _currentChild = newChild;

            if (newChild) {
                newChild->setParentView(self);
                newChild->offerLayout(self->getLayout());
            }
        }

      private:
        std::shared_ptr<View> _currentChild;
    };
}
