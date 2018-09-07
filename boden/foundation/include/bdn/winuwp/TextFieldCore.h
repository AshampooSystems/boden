#ifndef BDN_WINUWP_TextFieldCore_H_
#define BDN_WINUWP_TextFieldCore_H_

#include <bdn/TextField.h>
#include <bdn/winuwp/ChildViewCore.h>

namespace bdn
{
    namespace winuwp
    {

        class TextFieldCore : public ChildViewCore,
                              BDN_IMPLEMENTS ITextFieldCore
        {
          private:
            static ::Windows::UI::Xaml::Controls::TextBox ^
                _createTextField(TextField *pOuter) {
                    BDN_WINUWP_TO_STDEXC_BEGIN;

                    return ref new ::Windows::UI::Xaml::Controls::TextBox();

                    BDN_WINUWP_TO_STDEXC_END;
                }

                public :

                /** Used internally.*/
                ref class TextFieldCoreEventForwarder
                : public ViewCoreEventForwarder
            {
                internal : TextFieldCoreEventForwarder(TextFieldCore *pParent)
                    : ViewCoreEventForwarder(pParent)
                {}

                TextFieldCore *getTextFieldCoreIfAlive()
                {
                    return dynamic_cast<TextFieldCore *>(getViewCoreIfAlive());
                }

              public:
                void textChanged(
                    Object ^ sender,
                    ::Windows::UI::Xaml::Controls::TextChangedEventArgs ^ pArgs)
                {
                    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                    TextFieldCore *pTextFieldCore = getTextFieldCoreIfAlive();
                    if (pTextFieldCore != nullptr)
                        pTextFieldCore->_textChanged();

                    BDN_WINUWP_TO_PLATFORMEXC_END
                }

                void keyDown(Object ^ sender,
                             ::Windows::UI::Xaml::Input::KeyRoutedEventArgs ^
                                 args)
                {
                    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                    TextFieldCore *pTextFieldCore = getTextFieldCoreIfAlive();
                    if (pTextFieldCore != nullptr &&
                        args->Key == ::Windows::System::VirtualKey::Enter)
                        pTextFieldCore->_returnKeyPressed();

                    BDN_WINUWP_TO_PLATFORMEXC_END
                }

              private:
                ChildViewCore *_pParentWeak;
            };

            TextFieldCore(TextField *pOuter)
                : ChildViewCore(pOuter, _createTextField(pOuter),
                                ref new TextFieldCoreEventForwarder(this))
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                _pTextBox =
                    dynamic_cast<::Windows::UI::Xaml::Controls::TextBox ^>(
                        getFrameworkElement());
                TextFieldCoreEventForwarder ^ pEventForwarder =
                    dynamic_cast<TextFieldCoreEventForwarder ^>(
                        getViewCoreEventForwarder());

                // Rly, MS?
                _handlerToken = _pTextBox->TextChanged += ref new ::Windows::
                    UI::Xaml::Controls::TextChangedEventHandler(
                        pEventForwarder,
                        &TextFieldCoreEventForwarder::textChanged);
                _pTextBox->KeyDown +=
                    ref new ::Windows::UI::Xaml::Input::KeyEventHandler(
                        pEventForwarder, &TextFieldCoreEventForwarder::keyDown);

                setText(pOuter->text());
                setPadding(pOuter->padding());

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setText(const String &text)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                String currentText = String(_pTextBox->Text->Data());
                if (currentText != text) {
                    _pTextBox->Text =
                        ref new ::Platform::String(text.asWidePtr());
                }

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setPadding(const Nullable<UiMargin> &pad) override
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                UiMargin uiPadding;
                if (pad.isNull()) {
                    uiPadding =
                        UiMargin(UiLength::sem(0.4), UiLength::sem(0.4));
                } else
                    uiPadding = pad;

                Margin padding = uiMarginToDipMargin(uiPadding);

                // UWP also uses DIPs => no conversion necessary

                _contentPadding = ::Windows::UI::Xaml::Thickness(
                    padding.left, padding.top, padding.right, padding.bottom);
                _applyContentPadding = true;

                if (_pTextBox != nullptr)
                    _pTextBox->Padding = _contentPadding;

                BDN_WINUWP_TO_STDEXC_END;
            }

          protected:
            void _textChanged()
            {
                P<TextField> pOuter =
                    cast<TextField>(getOuterViewIfStillAttached());
                if (pOuter != nullptr) {
                    pOuter->setText(String(_pTextBox->Text->Data()));
                }
            }

            void _returnKeyPressed()
            {
                P<TextField> pOuter =
                    cast<TextField>(getOuterViewIfStillAttached());
                if (pOuter != nullptr) {
                    pOuter->submit();
                }
            }

            ::Windows::UI::Xaml::Controls::TextBox ^ _pTextBox;
            ::Windows::Foundation::EventRegistrationToken _handlerToken;

            ::Windows::UI::Xaml::Thickness _contentPadding;
            bool _applyContentPadding = false;
        };
    }
}

#endif
