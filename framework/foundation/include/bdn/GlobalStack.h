#pragma once

#include <string>
#include <typeinfo>
#include <vector>

namespace bdn
{
    template <class T, T (*defaultFunction)() = nullptr> class GlobalStack
    {
      public:
        using Stack = std::vector<T>;

      public:
        static T &top()
        {
            if (stack().empty()) {
                if constexpr (defaultFunction == nullptr) {
                    throw std::runtime_error(std::string("Tried accessing empty GlobalStack, T = ") + typeid(T).name());
                } else {
                    push(defaultFunction());
                    return top();
                }
            }

            return stack().back();
        }

      public:
        static void push(T &&entry) { stack().push_back(std::move(entry)); }
        static void pop()
        {
            if (!stack().empty())
                stack().pop_back();
        }

      private:
        static Stack &stack()
        {
            thread_local static Stack g_stack;
            return g_stack;
        }
    };
}
