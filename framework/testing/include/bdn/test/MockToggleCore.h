#ifndef BDN_TEST_MockToggleCore_H_
#define BDN_TEST_MockToggleCore_H_

#include <bdn/test/MockViewCore.h>

#include <bdn/ISwitchCore.h>
#include <bdn/Toggle.h>
#include <bdn/Checkbox.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" toggle core that does not actually show
           anything visible, but behaves otherwise like a normal toggle core.

            See MockUiProvider.
            */
        class MockToggleCore : public MockViewCore, BDN_IMPLEMENTS ISwitchCore
        {
          public:
            MockToggleCore(Toggle *toggle) : MockViewCore(toggle)
            {
                _label = toggle->label();
                _on = toggle->on();
            }

            /** Returns the current label of the toggle.*/
            String getLabel() const { return _label; }

            /** Returns the number of times that a new label has been set.*/
            int getLabelChangeCount() const { return _labelChangeCount; }

            void setLabel(const String &label) override
            {
                _label = label;
                _labelChangeCount++;
            }

            bool getOn() const { return _on; }

            int getOnChangeCount() const { return _onChangeCount; }

            void setOn(const bool &on) override
            {
                _on = on;
                _onChangeCount++;
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                BDN_REQUIRE_IN_MAIN_THREAD();

                Size size = _getTextSize(_label);

                // add our padding
                P<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    if (!view->padding().isNull())
                        size += uiMarginToDipMargin(view->padding());
                }

                // add some space for the fake toggle border
                size += Margin(4, 5);

                // ignore available space. We have a fixed size.

                if (view != nullptr) {
                    // clip to min and max size
                    size.applyMinimum(view->preferredSizeMinimum());
                    size.applyMaximum(view->preferredSizeMaximum());
                }

                return size;
            }

            void generateClick()
            {
                P<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    ClickEvent evt(view);

                    cast<Toggle>(view)->onClick().notify(evt);
                }
            }

          protected:
            String _label;
            int _labelChangeCount = 0;
            bool _on;
            int _onChangeCount = 0;
        };
    }
}

#endif // BDN_TEST_MockToggleCore_H_
