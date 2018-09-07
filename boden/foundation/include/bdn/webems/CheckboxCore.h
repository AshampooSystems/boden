#ifndef BDN_WEBEMS_CheckboxCore_H_
#define BDN_WEBEMS_CheckboxCore_H_

#include <emscripten/html5.h>

#include <bdn/webems/ViewCore.h>
#include <bdn/ISwitchCore.h>
#include <bdn/ICheckboxCore.h>
#include <bdn/Checkbox.h>
#include <bdn/Toggle.h>

#include <emscripten/val.h>
#include <emscripten/html5.h>

namespace bdn
{
    namespace webems
    {

        template <class T>
        class CheckboxCore : public ViewCore,
                             BDN_IMPLEMENTS ICheckboxCore,
                             BDN_IMPLEMENTS ISwitchCore
        {
          public:
            CheckboxCore(T *pOuter)
                : ViewCore(pOuter, "div"),
                  _checkboxDomObject(emscripten::val::null()),
                  _labelDomObject(emscripten::val::null())
            {
                // Checkbox consists of an <input> and a <label> element. The
                // composite wraps both in the view core's <div>. Currently no
                // special styling is applied.
                createCheckboxCompositeInner(pOuter);

                setLabel(pOuter->label());
                setState(pOuter->state());

                // The click event can also be used to listen for state changes
                // triggered by user interaction. However, this will not include
                // any changes made to the core's checkbox state
                // programatically.
                //
                // The HTML standard only defines that <input type="checkbox">
                // will fire a global input and a local change event when the
                // checkbox state is changed by user interaction (for reference,
                // see
                // https://html.spec.whatwg.org/multipage/input.html#checkbox-state-(type=checkbox)).
                // In practice, the global input event is fired by certain
                // browsers only (e.g. Firefox). The change event appears to be
                // unreliable on IE (fires only after the checkbox has lost
                // focus in some cases). It seems as if the only consistent user
                // interface event to rely on is a click event.
                //
                // As a workaround one can consistently use setAttribute() and
                // removeAttribute() to set the 'checked' attribute of the
                // HTMLInputElement in the DOM. We could observe attribute
                // changes via MutationObserver, so it is safe to assume that a
                // programmatic change of the checked attribute bubbles up to
                // the outer checkbox as long as a DOM manipulation has been
                // made. This, however, does not capture the case where a user
                // builds a platform-specific part of the view that includes
                // scripts to directly set the checked property of
                // HTMLInputElement. The latter case is NOT a DOM change and
                // there are no mechanisms to observe value changes on built-in
                // JavaScript properties of HTMLInputElement.
                //
                // As the click event will always fire on user interaction even
                // if the keyboard is used for toggling, we stick to using it to
                // report back the state of the checkbox to the framework.

                emscripten_set_click_callback(_elementId.asUtf8Ptr(), this,
                                              false, _clickedCallback);
            }

            void setLabel(const String &label) override
            {
                _labelDomObject.set("textContent", label.asUtf8());
            }

            void setState(const TriState &state) override
            {
                if (state != _currentState) {
                    _currentState = state;

                    switch (state) {
                    case TriState::on:
                        _checkboxDomObject.call<void>(
                            "setAttribute", emscripten::val("checked"),
                            emscripten::val("checked"));
                        _checkboxDomObject.set("indeterminate", false);
                        break;
                    case TriState::mixed:
                        _checkboxDomObject.call<void>(
                            "removeAttribute", emscripten::val("checked"));

                        // https://developer.mozilla.org/en-US/docs/Web/CSS/:indeterminate
                        // Shows indeterminate state correctly on Chrome,
                        // Firefox, Safari, Edge. Does not show indeterminate
                        // state on: iOS Safari Mixed state logic still works
                        // even if the state is not shown correctly by the
                        // browser. In this case, the checkbox should be
                        // presented as off.
                        _checkboxDomObject.set("indeterminate", true);
                        break;
                    case TriState::off:
                        _checkboxDomObject.call<void>(
                            "removeAttribute", emscripten::val("checked"));
                        _checkboxDomObject.set("indeterminate", false);
                        break;
                    }
                }
            }

            void setOn(const bool &on) override
            {
                setState(on ? TriState::on : TriState::off);
            }

            emscripten::val &_getLabelDomObject() { return _labelDomObject; }

            emscripten::val &_getCheckboxDomObject()
            {
                return _checkboxDomObject;
            }

          protected:
            bool _clicked(int eventType,
                          const EmscriptenMouseEvent *pMouseEvent)
            {
                if (eventType == EMSCRIPTEN_EVENT_CLICK) {
                    P<View> pView = getOuterViewIfStillAttached();
                    if (pView != nullptr) {
                        ClickEvent evt(pView);

                        P<Checkbox> pCheckbox = tryCast<Checkbox>(pView);
                        P<Toggle> pToggle = tryCast<Toggle>(pView);

                        if (pCheckbox) {
                            // Set outer on state on click (see comment in
                            // constructor)
                            pCheckbox->setState(_checkboxDomObject["checked"]
                                                        .template as<bool>()
                                                    ? TriState::on
                                                    : TriState::off);
                        } else if (pToggle) {
                            pToggle->setOn(_checkboxDomObject["checked"]
                                               .template as<bool>());
                        }

                        cast<T>(pView)->onClick().notify(evt);
                    }
                }

                return false;
            }

            static EM_BOOL
            _clickedCallback(int eventType,
                             const EmscriptenMouseEvent *pMouseEvent,
                             void *pUserData)
            {
                return ((CheckboxCore *)pUserData)
                    ->_clicked(eventType, pMouseEvent);
            }

            // HDU: Could move into ViewCore?
            void createCheckboxCompositeInner(T *pOuter)
            {
                _checkboxDomObject = createElement("input");
                _labelDomObject = createElement("label");
                _addChildElement(_checkboxDomObject);
                _addChildElement(_labelDomObject);

                _checkboxDomObject.set("type", "checkbox");

                // Label shouldn't wrap (for now)
                emscripten::val styleObj = _labelDomObject["style"];
                styleObj.set("white-space", "nowrap");

                // Link label to checkbox, so clicks on the label also Checkbox
                // the checkbox state
                _labelDomObject.call<void>("setAttribute",
                                           emscripten::val("for"),
                                           _checkboxDomObject["id"]);
            }

            // Helper to create elements
            emscripten::val createElement(String elementName)
            {
                emscripten::val docVal = emscripten::val::global("document");
                emscripten::val elementVal = docVal.call<emscripten::val>(
                    "createElement", elementName.asUtf8());
                elementVal.set("id", _nextElementId().asUtf8());
                return elementVal;
            }

            emscripten::val _checkboxDomObject;
            emscripten::val _labelDomObject;
            TriState _currentState = TriState::off;
        };
    }
}

#endif
