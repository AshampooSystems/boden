#ifndef BDN_WINUWP_CheckboxCore_H_
#define BDN_WINUWP_CheckboxCore_H_

#include <bdn/Checkbox.h>
#include <bdn/Toggle.h>
#include <bdn/ICheckboxCore.h>
#include <bdn/ISwitchCore.h>
#include <bdn/winuwp/ChildViewCore.h>

namespace bdn
{
    namespace winuwp
    {

        template <class T>
        class CheckboxCore : public ChildViewCore,
                             BDN_IMPLEMENTS ICheckboxCore,
                             BDN_IMPLEMENTS ISwitchCore
        {
          private:
            static ::Windows::UI::Xaml::Controls::StackPanel ^
                _createCheckbox(T *pOuter) {
                    BDN_WINUWP_TO_STDEXC_BEGIN;

                    // We have to use this wonderful StackPanel container to
                    // implement correct padding on the checkbox. If padding is
                    // applied to the CheckBox control directly, the checkbox
                    // rect is not padded correctly yielding weird layouts with
                    // displaced text labels.
                    ::Windows::UI::Xaml::Controls::StackPanel ^ container =
                        ref new ::Windows::UI::Xaml::Controls::StackPanel();
                    ::Windows::UI::Xaml::Controls::CheckBox ^ checkbox =
                        ref new ::Windows::UI::Xaml::Controls::CheckBox();
                    checkbox->MinWidth = 0;

                    P<Checkbox> pCheckbox = tryCast<Checkbox>(pOuter);
                    if (pCheckbox)
                        checkbox->IsThreeState = true;

                    container->Children->Append(checkbox);

                    return container;

                    BDN_WINUWP_TO_STDEXC_END;
                }

                public :

                /** Used internally.*/
                ref class CheckboxCoreEventForwarder
                : public ViewCoreEventForwarder
            {
                internal : CheckboxCoreEventForwarder(CheckboxCore *pParent)
                    : ViewCoreEventForwarder(pParent)
                {}

                CheckboxCore *getCheckboxCoreIfAlive()
                {
                    return dynamic_cast<CheckboxCore *>(getViewCoreIfAlive());
                }

              public:
                void clicked(Object ^ sender,
                             ::Windows::UI::Xaml::RoutedEventArgs ^ pArgs)
                {
                    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                    CheckboxCore *pCheckboxCore = getCheckboxCoreIfAlive();
                    if (pCheckboxCore != nullptr)
                        pCheckboxCore->_clicked();

                    BDN_WINUWP_TO_PLATFORMEXC_END
                }

              private:
                ChildViewCore *_pParentWeak;
            };

            CheckboxCore(T *pOuter)
                : ChildViewCore(pOuter, _createCheckbox(pOuter),
                                ref new CheckboxCoreEventForwarder(this))
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                _pContainer =
                    dynamic_cast<::Windows::UI::Xaml::Controls::StackPanel ^>(
                        getFrameworkElement());
                _pCheckbox =
                    dynamic_cast<::Windows::UI::Xaml::Controls::CheckBox ^>(
                        _pContainer->Children->GetAt(0));

                CheckboxCoreEventForwarder ^ pEventForwarder =
                    dynamic_cast<CheckboxCoreEventForwarder ^>(
                        getViewCoreEventForwarder());
                _pCheckbox->Click +=
                    ref new ::Windows::UI::Xaml::RoutedEventHandler(
                        pEventForwarder, &CheckboxCoreEventForwarder::clicked);

                setPadding(pOuter->padding());
                setLabel(pOuter->label());
                setState(pOuter->state());

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setPadding(const Nullable<UiMargin> &pad) override
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                // Apply the padding to the control, so that the content is
                // positioned accordingly.
                UiMargin uiPadding;
                if (pad.isNull()) {
                    // we should use a default padding that looks good.
                    // Xaml uses zero padding as the default, so we cannot use
                    // their default value. So we choose our own default that
                    // matches the normal aesthetic of Windows apps.
                    uiPadding = UiMargin(UiLength::sem(0.4), UiLength::sem(1));
                } else
                    uiPadding = pad;

                Margin padding = uiMarginToDipMargin(uiPadding);

                // UWP also uses DIPs => no conversion necessary

                // Note that the padding must be set on the button Content to
                // have an effect. The button class itself seems to completely
                // ignore it.
                _containerPadding = ::Windows::UI::Xaml::Thickness(
                    padding.left, padding.top, padding.right, padding.bottom);

                _pContainer->Padding = _containerPadding;

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setLabel(const String &label)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                ::Windows::UI::Xaml::Controls::TextBlock ^ pLabel =
                    ref new ::Windows::UI::Xaml::Controls::TextBlock();

                pLabel->Text = ref new ::Platform::String(label.asWidePtr());
                // we cannot simply schedule a sizing info update here. The
                // desired size of the control will still be outdated when the
                // sizing happens. Instead we wait for the "layout updated"
                // event that will happen soon after we set the content. That is
                // when we update our sizing info.

                _pCheckbox->Content = pLabel;

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setState(const TriState &state)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                switch (state) {
                case TriState::on:
                    _pCheckbox->IsChecked = true;
                    break;
                case TriState::off:
                    _pCheckbox->IsChecked = false;
                    break;
                case TriState::mixed:
                    _pCheckbox->IsChecked = nullptr;
                    break;
                }

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setOn(const bool &on)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                _pCheckbox->IsChecked = on;

                BDN_WINUWP_TO_STDEXC_END;
            }

          protected:
            void _clicked()
            {
                P<View> pOuterView = getOuterViewIfStillAttached();
                if (pOuterView != nullptr) {
                    P<Toggle> pToggle = tryCast<Toggle>(pOuterView);
                    P<Checkbox> pCheckbox = tryCast<Checkbox>(pOuterView);
                    if (pCheckbox) {
                        TriState state;

                        // Prohibit users from selecting mixed state by clicking
                        // the checkbox. MS themselves say this is a non-use
                        // case, but then still allow users to select mixed
                        // state by default and make it quite hard to prevent:
                        // https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.checkbox
                        // ¯\_(ツ)_/¯
                        if (_pCheckbox->IsChecked != nullptr &&
                            _pCheckbox->IsChecked->Value == true) {
                            state = TriState::on;
                            _pCheckbox->IsThreeState = false;
                        } else if (_pCheckbox->IsChecked != nullptr &&
                                   _pCheckbox->IsChecked->Value == false) {
                            state = TriState::off;
                            _pCheckbox->IsThreeState = true;
                        } else if (_pCheckbox->IsChecked == nullptr) {
                            state = TriState::mixed;
                        }

                        pCheckbox->setState(state);
                    } else if (pToggle) {
                        pToggle->setOn(_pCheckbox->IsChecked->Value);
                    }

                    ClickEvent evt(pOuterView);
                    cast<T>(pOuterView)->onClick().notify(evt);
                }
            }

            ::Windows::UI::Xaml::Controls::StackPanel ^ _pContainer;
            ::Windows::UI::Xaml::Controls::CheckBox ^ _pCheckbox;

            ::Windows::UI::Xaml::Thickness _containerPadding;
        };
    }
}

#endif
