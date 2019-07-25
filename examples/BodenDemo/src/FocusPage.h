#pragma once

#include "Page.h"

namespace bdn
{
    class FocusPage : public ui::CoreLess<ContainerView>
    {
      public:
        class FocusGroup
        {
          public:
            void focusNext(std::shared_ptr<TextField> current)
            {
                auto it = std::find_if(views.begin(), views.end(), [current](auto lhs) {
                    if (lhs.lock() == current) {
                        return true;
                    }
                    return false;
                });
                if (it == views.end() || it + 1 == views.end()) {
                    it = views.begin();
                } else {
                    it++;
                }
                if (auto nextTextField = it->lock()) {
                    nextTextField->focus();
                } else {
                    views.erase(it);
                }
            }

            void add(const std::shared_ptr<TextField> &textField) { views.push_back(textField); }

          private:
            std::vector<std::weak_ptr<TextField>> views;
        };

      public:
        using CoreLess<ContainerView>::CoreLess;
        void init() override;

      private:
        std::shared_ptr<FocusGroup> _focusGroup;
    };
}
