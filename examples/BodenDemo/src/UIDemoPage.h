#pragma once

#include <bdn/ui.h>

#include "Page.h"

namespace bdn
{
    class UIDemoPage : public CoreLess<ContainerView>
    {
      public:
        UIDemoPage(bdn::NeedsInit tg, std::shared_ptr<Window> window);
        void init() override;

      public:
        class WindowMatcher
        {
          public:
            WindowMatcher(std::shared_ptr<Window> window, std::shared_ptr<Styler> styler)
                : _window(window), _styler(styler)
            {
                windowGeometrySub = _window->geometry.onChange().subscribe([=](auto) { update(); });
                update();
            }

            ~WindowMatcher() { _window->geometry.onChange().unsubscribe(windowGeometrySub); }

            void update()
            {
                // TODO: Allow matcher to change, not rebuild ?
                _styler->setCondition("width", std::make_shared<Styler::equals_condition>(_window->geometry->width));
                _styler->setCondition("height", std::make_shared<Styler::equals_condition>(_window->geometry->height));
                _styler->setCondition("max-width", std::make_shared<Styler::less_condition>(_window->geometry->width));
                _styler->setCondition("max-height",
                                      std::make_shared<Styler::less_condition>(_window->geometry->height));
                _styler->setCondition("min-width",
                                      std::make_shared<Styler::greater_equal_condition>(_window->geometry->width));
                _styler->setCondition("min-height",
                                      std::make_shared<Styler::greater_equal_condition>(_window->geometry->height));
            }

            Property<Rect>::backing_t::notifier_t::Subscription windowGeometrySub;
            std::shared_ptr<Window> _window;
            std::shared_ptr<Styler> _styler;
        };

      private:
        std::shared_ptr<Styler> _styler;
        std::shared_ptr<WindowMatcher> _windowMatcher;
    };
}
