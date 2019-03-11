#pragma once

#include <bdn/Layout.h>
#include <bdn/Rect.h>
#include <bdn/Size.h>
#include <bdn/property/Property.h>
#include <bdn/round.h>
#include <optional>

namespace bdn
{
    class View;

    template <class _Fp> class WeakCallback;

    template <typename ReturnType, typename... Arguments> class WeakCallback<ReturnType(Arguments...)>
    {
      public:
        using Receiver = std::shared_ptr<std::function<ReturnType(Arguments...)>>;

        ReturnType fire(Arguments...)
        {
            if (auto callback = _callback.lock()) {
                (*callback)();
            }
        }

        auto set(std::function<ReturnType(Arguments...)> &&callback)
        {
            auto result = std::make_shared<std::function<ReturnType(Arguments...)>>(std::move(callback));
            _callback = result;
            return result;
        }

      private:
        std::weak_ptr<std::function<ReturnType(Arguments...)>> _callback;
    };

    class ViewCore : public Base
    {
        friend class View;

      public:
        Property<Rect> geometry;
        Property<bool> visible;

      public:
        virtual ~ViewCore() = default;

      public:
        virtual void init() = 0;

        virtual Size sizeForSpace(Size availableSize = Size::none()) const { return Size{0, 0}; }

        virtual bool canMoveToParentView(std::shared_ptr<View> newParentView) const = 0;

        virtual void scheduleLayout() = 0;

        void startLayout() { _layoutCallback.fire(); }
        void markDirty() { _dirtyCallback.fire(); }

        virtual void setLayout(std::shared_ptr<Layout> layout) { _layout = std::move(layout); }

        virtual void visitInternalChildren(std::function<void(std::shared_ptr<ViewCore>)>) {}

      public:
        std::shared_ptr<Layout> _layout;

      protected:
        WeakCallback<void()> _layoutCallback;
        WeakCallback<void()> _dirtyCallback;
    };
}
