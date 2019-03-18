#pragma once

#include <bdn/ButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/View.h>

namespace bdn
{

    /** A simple button with a text label.*/
    class Button : public View
    {
      public:
        Property<String> label;

      public:
        Button(std::shared_ptr<UIProvider> uiProvider = nullptr) : View(std::move(uiProvider))
        {
            _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();
        }

        ISyncNotifier<const ClickEvent &> &onClick() { return *_onClick; }

        String viewCoreTypeName() const override { return coreTypeName; }

        void bindViewCore() override
        {
            View::bindViewCore();
            auto buttonCore = core<ButtonCore>();
            buttonCore->label.bind(label);
            _clickCallbackReceiver = buttonCore->_clickCallback.set([=]() {
                ClickEvent evt(shared_from_this());
                _onClick->notify(evt);
            });
        }

      public:
        static constexpr char coreTypeName[] = "bdn.ButtonCore";

      private:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
        WeakCallback<void()>::Receiver _clickCallbackReceiver;
    };
}
