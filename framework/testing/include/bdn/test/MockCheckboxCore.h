#pragma once

#include <bdn/test/MockViewCore.h>

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" checkbox core that does not actually show
           anything visible, but behaves otherwise like a normal checkbox core.

            See MockUIProvider.
            */
        class MockCheckboxCore : public MockViewCore, virtual public CheckboxCore
        {
          public:
            MockCheckboxCore(std::shared_ptr<Checkbox> checkbox) : MockViewCore(checkbox)
            {
                _label = checkbox->label;
                _state = checkbox->state;
            }

            /** Returns the current label of the Checkbox.*/
            String getLabel() const { return _label; }

            /** Returns the number of times that a new label has been set.*/
            int getLabelChangeCount() const { return _labelChangeCount; }

            void setLabel(const String &label) override
            {
                _label = label;
                _labelChangeCount++;
            }

            TriState getState() const { return _state; }

            int getStateChangeCount() const { return _stateChangeCount; }

            void setState(const TriState &state) override
            {
                _state = state;
                _stateChangeCount++;
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                BDN_REQUIRE_IN_MAIN_THREAD();

                Size size = _getTextSize(_label);

                // add our padding
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    if (view->padding.get())
                        size += uiMarginToDipMargin(*view->padding.get());
                }

                // add some space for the fake Checkbox border
                size += Margin(4, 5);

                // ignore available space. We have a fixed size.

                if (view != nullptr) {
                    // clip to min and max size
                    size.applyMinimum(view->preferredSizeMinimum);
                    size.applyMaximum(view->preferredSizeMaximum);
                }

                return size;
            }

            void generateClick()
            {
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    ClickEvent evt(view);

                    std::dynamic_pointer_cast<Checkbox>(view)->onClick().notify(evt);
                }
            }

          protected:
            String _label;
            int _labelChangeCount = 0;
            TriState _state;
            int _stateChangeCount = 0;
        };
    }
}
