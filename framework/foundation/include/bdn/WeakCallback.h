#pragma once

#include <functional>
#include <memory>

namespace bdn
{
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
}
