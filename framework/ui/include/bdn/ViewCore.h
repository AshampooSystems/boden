#pragma once

#include <bdn/Layout.h>
#include <bdn/Rect.h>
#include <bdn/Size.h>
#include <bdn/property/Property.h>
#include <bdn/round.h>
#include <optional>
#include <utility>

namespace bdn
{
    class View;
    class UIProvider;

    template <class _Fp> class WeakCallback;

    template <typename... Arguments> class WeakCallback<void(Arguments...)>
    {
      public:
        using FunctionPointer = std::function<void(Arguments...)>;
        using Receiver = std::shared_ptr<FunctionPointer>;

        void fire(Arguments... arguments)
        {
            if (auto callback = _callback.lock()) {
                (*callback)(arguments...);
            }
        }

        auto set(FunctionPointer &&callback)
        {
            auto result = std::make_shared<FunctionPointer>(std::move(callback));
            _callback = result;
            return result;
        }

      private:
        std::weak_ptr<FunctionPointer> _callback;
    };

    template <typename ReturnType, typename... Arguments> class WeakCallback<ReturnType(Arguments...)>
    {
      public:
        using FunctionPointer = std::function<ReturnType(Arguments...)>;
        using Receiver = std::shared_ptr<FunctionPointer>;

        WeakCallback(ReturnType defaultReturnValue = ReturnType()) : _defaultReturnValue(defaultReturnValue) {}

        ReturnType fire(Arguments... arguments)
        {
            if (auto callback = _callback.lock()) {
                return (*callback)(arguments...);
            }
            return _defaultReturnValue;
        }

        auto set(FunctionPointer &&callback)
        {
            auto result = std::make_shared<FunctionPointer>(std::move(callback));
            _callback = result;
            return result;
        }

      private:
        std::weak_ptr<FunctionPointer> _callback;
        const ReturnType _defaultReturnValue;
    };

    class ViewCore : public Base
    {
        friend class View;

      public:
        Property<Rect> geometry;
        Property<bool> visible;

      public:
        ViewCore() = delete;
        ViewCore(std::shared_ptr<bdn::UIProvider> uiProvider) : _uiProvider(std::move(uiProvider)) {}
        ~ViewCore() override = default;

      public:
        virtual void init() = 0;

        virtual Size sizeForSpace(Size availableSize = Size::none()) const { return Size{0, 0}; }

        virtual bool canMoveToParentView(std::shared_ptr<View> newParentView) const = 0;

        virtual void scheduleLayout() = 0;

        void startLayout() { _layoutCallback.fire(); }
        void markDirty() { _dirtyCallback.fire(); }

        virtual void setLayout(std::shared_ptr<Layout> layout) { _layout = std::move(layout); }
        std::shared_ptr<Layout> layout() { return _layout; }

        virtual void visitInternalChildren(const std::function<void(std::shared_ptr<ViewCore>)> & /*unused*/) {}

        std::shared_ptr<bdn::UIProvider> uiProvider() { return _uiProvider; }

      private:
        std::shared_ptr<bdn::UIProvider> _uiProvider;
        std::shared_ptr<Layout> _layout;

        WeakCallback<void()> _layoutCallback;
        WeakCallback<void()> _dirtyCallback;
    };
}
