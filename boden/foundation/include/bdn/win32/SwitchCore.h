#ifndef BDN_WIN32_SwitchCore_H_
#define BDN_WIN32_SwitchCore_H_

#include <bdn/ISwitchCore.h>
#include <bdn/Toggle.h>
#include <bdn/Switch.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/Win32WindowClass.h>
#include <bdn/SafeInit.h>
#include <bdn/safeStatic.h>

#include <bdn/win32/WindowDeviceContext.h>

#define BDN_WIN32SWITCH_WIDTH 44
#define BDN_WIN32SWITCH_HEIGHT 20
#define BDN_WIN32SWITCH_HANDLE_WIDTH 14
#define BDN_WIN32SWITCH_HANDLE_HEIGHT 14
#define BDN_WIN32SWITCH_HANDLE_MARGIN 3
#define BDN_WIN32SWITCH_HANDLE_OFFSET_OFF BDN_WIN32SWITCH_HANDLE_MARGIN
#define BDN_WIN32SWITCH_HANDLE_OFFSET_ON                                       \
    BDN_WIN32SWITCH_WIDTH - BDN_WIN32SWITCH_HANDLE_MARGIN -                    \
        BDN_WIN32SWITCH_HANDLE_WIDTH

namespace bdn
{
    namespace win32
    {

        template <class T>
        class SwitchCore : public ViewCore, BDN_IMPLEMENTS ISwitchCore
        {
          public:
            SwitchCore(T *pOuter)
                : ViewCore(pOuter, SwitchCore::SwitchCoreClass::get().getName(),
                           pOuter->label(), WS_CHILD, 0)
            {
                setLabel(pOuter->label());
                setOn(pOuter->on());
            }

            void setLabel(const String &label) override
            {
                setWindowText(getHwnd(), label);
                InvalidateRect(getHwnd(), NULL, TRUE);
            }

            void setOn(const bool &on) override
            {
                SendMessage(getHwnd(), BM_SETCHECK,
                            on ? BST_CHECKED : BST_UNCHECKED, 0);
                InvalidateRect(getHwnd(), NULL, TRUE);
            }

            // Convenience function to retrieve the current "checked" state of
            // the win32 window
            bool _on() const
            {
                UINT state = SendMessage(getHwnd(), BM_GETCHECK, 0, 0);
                return state == BST_CHECKED;
            }

            void generateClick()
            {
                P<View> pView = getOuterViewIfStillAttached();

                if (pView != nullptr) {
                    ClickEvent evt(pView);

                    // Click is triggered also when the user uses the keyboard
                    // to toggle the checkbox, so we set the outer's on state
                    // here to inform the framework about a state change.
                    cast<T>(pView)->setOn(_on());

                    // We guarantee that the on property will be set on the
                    // outer control before a notification is posted to onClick.
                    cast<T>(pView)->onClick().notify(evt);
                }
            }

            Size calcPreferredSize(const Size &availableSpace) const
            {
                Size switchMinSize(BDN_WIN32SWITCH_WIDTH,
                                   BDN_WIN32SWITCH_HEIGHT);
                double switchMargin = 10.; // hardcoded value that looks good

                String label;

                // we completely ignore the available space as there is no way
                // to reduce the size for the switch.

                P<const T> pOuter =
                    cast<const T>(getOuterViewIfStillAttached());
                if (pOuter != nullptr)
                    label = pOuter->label();

                Size prefSize;

                {
                    WindowDeviceContext dc(getHwnd());

                    P<const win32::Font> pFont = getFont();
                    if (pFont != nullptr)
                        dc.setFont(*pFont);
                    prefSize = dc.getTextSize(label);
                }

                // SwitchCore width = switch min width + margin + label
                prefSize.width += switchMinSize.width + switchMargin;
                // SwitchCore height is at least the height of the switch
                // control. If the label's height is greater than the switch
                // control's height, then the core's height is the label's
                // height. In the latter case the switch control will be
                // centered vertically.
                prefSize.height = fmax(prefSize.height, switchMinSize.height);

                Nullable<UiMargin> pad;
                if (pOuter != nullptr)
                    pad = pOuter->padding();

                UiMargin uiPadding;
                if (pad.isNull()) {
                    // Use a default padding that looks good – win32 does not
                    // have a built-in default
                    uiPadding =
                        UiMargin(UiLength::sem(0.12), UiLength::sem(0.5));
                } else
                    uiPadding = pad;

                prefSize += uiMarginToDipMargin(uiPadding);

                if (pOuter != nullptr) {
                    prefSize.applyMaximum(pOuter->preferredSizeMaximum());
                    prefSize.applyMinimum(pOuter->preferredSizeMinimum());
                }

                return prefSize;
            }

          protected:
            void handleMessage(MessageContext &context, HWND windowHandle,
                               UINT message, WPARAM wParam, LPARAM lParam)
            {
                P<T> pOuter = cast<T>(getOuterViewIfStillAttached());

                switch (message) {
                case WM_PAINT:
                    paint();
                    break;

                case WM_SETTEXT:
                    if (pOuter != nullptr) {
                        pOuter->setLabel(String((WCHAR *)lParam));
                    }
                    break;

                case WM_GETTEXT: {
                    String label = pOuter != nullptr ? pOuter->label() : "";
                    wcscpy_s((WCHAR *)lParam, wParam, label.asWidePtr());
                    context.setResult(wcsnlen_s((WCHAR *)lParam, wParam),
                                      false);
                    break;
                }

                case BM_SETCHECK:
                    if (pOuter != nullptr)
                        pOuter->setOn(wParam == BST_CHECKED);
                    break;

                case BM_GETCHECK:
                    context.setResult(pOuter != nullptr && pOuter->on()
                                          ? BST_CHECKED
                                          : BST_UNCHECKED,
                                      false);
                    break;

                case WM_SETREDRAW:
                    InvalidateRect(windowHandle, NULL, TRUE);
                    break;

                case WM_LBUTTONUP: {
                    POINT pos;
                    pos.x = LOWORD(lParam);
                    pos.y = HIWORD(lParam);

                    RECT switchRect = computeSwitchRect();

                    if (PtInRect(&switchRect, pos)) {
                        SendMessage(GetParent(windowHandle), WM_COMMAND, 0,
                                    (LPARAM)windowHandle);
                    }

                    break;
                }
                }
            }

            void handleParentMessage(MessageContext &context, HWND windowHandle,
                                     UINT message, WPARAM wParam,
                                     LPARAM lParam) override
            {
                if (message == WM_COMMAND) {
                    // Set state on HWND
                    setOn(!_on());
                    // we were clicked.
                    generateClick();
                }

                return ViewCore::handleParentMessage(context, windowHandle,
                                                     message, wParam, lParam);
            }

            RECT computeSwitchRect(RECT *clientRect = nullptr)
            {
                RECT rect;
                int dpi = ::GetDeviceCaps(GetDC(getHwnd()), LOGPIXELSX);
                double scaleFactor = dpi / 96.0;
                int clientHeight;

                if (clientRect == nullptr) {
                    GetClientRect(getHwnd(), &rect);
                    clientRect = &rect;
                }

                clientHeight = clientRect->bottom - clientRect->top;

                return {clientRect->right -
                            int(BDN_WIN32SWITCH_WIDTH * scaleFactor),
                        clientRect->top + clientHeight / 2 -
                            BDN_WIN32SWITCH_HEIGHT / 2,
                        clientRect->right,
                        clientRect->top + clientHeight / 2 +
                            BDN_WIN32SWITCH_HEIGHT / 2};
            }

            void paint()
            {
                P<T> pOuter = cast<T>(getOuterViewIfStillAttached());

                PAINTSTRUCT ps;
                HDC hdc;
                RECT rect;
                RECT switchRect;
                HFONT hFont, hOldFont;

                GetClientRect(getHwnd(), &rect);
                switchRect = computeSwitchRect(&rect);

                hdc = BeginPaint(getHwnd(), &ps);

                int dpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
                double scaleFactor = dpi / 96.0;

                DWORD handleBackgroundColor = GetSysColor(COLOR_3DFACE);
                DWORD backgroundColorOff = GetSysColor(COLOR_3DSHADOW);
                DWORD backgroundColorOn = RGB(0, 200, 0);
                DWORD outlineColor = GetSysColor(COLOR_3DDKSHADOW);
                DWORD handleOutlineColor = GetSysColor(COLOR_3DFACE);

                HBRUSH handleBackgroundBrush =
                    CreateSolidBrush(handleBackgroundColor);
                HBRUSH backgroundBrush = CreateSolidBrush(
                    pOuter != nullptr && pOuter->on() ? backgroundColorOn
                                                      : backgroundColorOff);
                HPEN handleOutlinePen =
                    CreatePen(PS_SOLID, 1, handleOutlineColor);
                HPEN outlinePen = CreatePen(PS_SOLID, 1, outlineColor);

                SelectObject(hdc, outlinePen);
                SelectObject(hdc, backgroundBrush);
                Rectangle(hdc, switchRect.left, switchRect.top,
                          switchRect.right, switchRect.bottom);

                SelectObject(hdc, handleBackgroundBrush);
                SelectObject(hdc, handleOutlinePen);

                Rectangle(
                    hdc, switchRect.left + int(handleOffset() * scaleFactor),
                    switchRect.top +
                        int(BDN_WIN32SWITCH_HANDLE_MARGIN * scaleFactor),
                    switchRect.left +
                        int((handleOffset() + BDN_WIN32SWITCH_HANDLE_WIDTH) *
                            scaleFactor),
                    switchRect.top + int((BDN_WIN32SWITCH_HANDLE_MARGIN +
                                          BDN_WIN32SWITCH_HANDLE_HEIGHT) *
                                         scaleFactor));

                DeleteObject(backgroundBrush);
                DeleteObject(outlinePen);
                DeleteObject(handleBackgroundBrush);
                DeleteObject(handleOutlinePen);

                if (pOuter != nullptr) {
                    P<Font> pFont = UiProvider::get().getFontForView(
                        pOuter.getPtr(), scaleFactor);
                    hFont = pFont->getHandle();
                    if (hOldFont = (HFONT)SelectObject(hdc, hFont)) {
                        SetTextColor(hdc, RGB(0, 0, 0));
                        SetBkMode(hdc, TRANSPARENT);
                        DrawText(hdc, pOuter->label().asWidePtr(), -1, &rect,
                                 DT_SINGLELINE | DT_LEFT | DT_VCENTER);

                        SelectObject(hdc, hOldFont);
                    }
                }

                EndPaint(getHwnd(), &ps);
            }

            int handleOffset()
            {
                P<const T> pOuter =
                    cast<const T>(getOuterViewIfStillAttached());
                return pOuter != nullptr && pOuter->on()
                           ? BDN_WIN32SWITCH_HANDLE_OFFSET_ON
                           : BDN_WIN32SWITCH_HANDLE_OFFSET_OFF;
            }

            class SwitchCoreClass : public Win32WindowClass
            {
              public:
                SwitchCoreClass()
                    : Win32WindowClass("bdnSwitch", Win32Window::windowProc)
                {
                    ensureRegistered();
                }

                static SwitchCoreClass &get()
                {
                    static SafeInit<SwitchCoreClass> init;
                    return *init.get();
                }
            };
        };
    }
}

#endif // BDN_WIN32_SwitchCore_H_
