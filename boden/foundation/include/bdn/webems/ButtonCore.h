#ifndef BDN_WEBEMS_ButtonCore_H_
#define BDN_WEBEMS_ButtonCore_H_

#include <emscripten/html5.h>

#include <bdn/webems/ViewCore.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

namespace bdn
{
    namespace webems
    {

        class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
        {
          public:
            ButtonCore(Button *pOuterButton) : ViewCore(pOuterButton, "button")
            {
                setLabel(pOuterButton->label());

                emscripten::val styleObj = _domObject["style"];

                // we do not want automatic text wrapping for a simply button.
                styleObj.set("white-space", "nowrap");

                emscripten_set_click_callback(_elementId.asUtf8Ptr(), this,
                                              false, _clickedCallback);
            }

            void setLabel(const String &label) override
            {
                _domObject.set("textContent", label.asUtf8());
            }

          protected:
            bool _clicked(int eventType,
                          const EmscriptenMouseEvent *pMouseEvent)
            {
                if (eventType == EMSCRIPTEN_EVENT_CLICK) {
                    P<View> pView = getOuterViewIfStillAttached();
                    if (pView != nullptr) {
                        ClickEvent evt(pView);
                        cast<Button>(pView)->onClick().notify(evt);
                    }
                }

                return false;
            }

            static EM_BOOL
            _clickedCallback(int eventType,
                             const EmscriptenMouseEvent *pMouseEvent,
                             void *pUserData)
            {
                return ((ButtonCore *)pUserData)
                    ->_clicked(eventType, pMouseEvent);
            }

            void setMozAppearanceNone(bool activateNone)
            {
                if (_mozAppearanceNoneSet != activateNone) {
                    emscripten::val styleObj = _domObject["style"];

                    std::string appearance = activateNone ? "none" : "initial";

                    styleObj.set("-moz-appearance", appearance);

                    _mozAppearanceNoneSet = activateNone;
                }
            }

            void setDefaultPadding() override
            {
                ViewCore::setDefaultPadding();

                // we do not need to override the appearance anymore
                setMozAppearanceNone(false);
            }

            void setNonDefaultPadding(const Margin &padding) override
            {
                ViewCore::setNonDefaultPadding(padding);

                // on Mozilla browsers the padding for buttons is ignored or not
                // applied to the actual button interior (instead it is applied
                // as an additional margin). We want the padding to have an
                // actual effect. To achieve that, we have to set the appearance
                // to "none", to get an "unstyled" button, that will follow the
                // padding we set.
                setMozAppearanceNone(true);
            }

            bool _mozAppearanceNoneSet = false;
        };
    }
}

#endif
