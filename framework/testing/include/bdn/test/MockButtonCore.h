#ifndef BDN_TEST_MockButtonCore_H_
#define BDN_TEST_MockButtonCore_H_

#include <bdn/test/MockViewCore.h>

#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" button core that does not actually show
           anything visible, but behaves otherwise like a normal button core.

            See MockUiProvider.
            */
        class MockButtonCore : public MockViewCore, BDN_IMPLEMENTS IButtonCore
        {
          public:
            MockButtonCore(Button *button) : MockViewCore(button) { _label = button->label(); }

            /** Returns the current label of the button.*/
            String getLabel() const { return _label; }

            /** Returns the number of times that a new label has been set.*/
            int getLabelChangeCount() const { return _labelChangeCount; }

            void setLabel(const String &label) override
            {
                _label = label;
                _labelChangeCount++;
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

                // add some space for the fake button border
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

                    cast<Button>(view)->onClick().notify(evt);
                }
            }

          protected:
            String _label;
            int _labelChangeCount = 0;
        };
    }
}

#endif
