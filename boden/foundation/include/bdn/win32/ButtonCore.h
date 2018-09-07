#ifndef BDN_WIN32_ButtonCore_H_
#define BDN_WIN32_ButtonCore_H_

#include <bdn/Button.h>
#include <bdn/win32/ViewCore.h>

namespace bdn
{
    namespace win32
    {

        class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
        {
          public:
            ButtonCore(Button *pOuter);

            void setLabel(const String &label) override;

            void generateClick()
            {
                P<View> pView = getOuterViewIfStillAttached();

                if (pView != nullptr) {
                    ClickEvent evt(pView);

                    cast<Button>(pView)->onClick().notify(evt);
                }
            }

            Size calcPreferredSize(
                const Size &availableSpace = Size::none()) const override;

          protected:
            void handleParentMessage(MessageContext &context, HWND windowHandle,
                                     UINT message, WPARAM wParam,
                                     LPARAM lParam) override;
        };
    }
}

#endif
