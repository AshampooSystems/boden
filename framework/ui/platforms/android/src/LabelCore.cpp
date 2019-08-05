#include <bdn/android/AttributedString.h>
#include <bdn/android/LabelCore.h>
#include <bdn/android/wrapper/TextUtils.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Label, bdn::ui::android::LabelCore, Label)
}

namespace bdn::ui::android
{
    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeLabel>(viewCoreFactory)),
          _jLabel(getJViewAS<bdn::android::wrapper::NativeLabel>())
    {
        updateTruncateAndWrapMode();

        text.onChange() += [=](auto &property) { textChanged(property.get()); };

        wrap.onChange() += [=](auto &) { updateTruncateAndWrapMode(); };

        geometry.onChange() += [=](auto &property) {
            int widthPixels = property.get().width * getUIScaleFactor();

            if (wrap) {
                _jLabel.setMaxWidth(widthPixels);
            }
        };

        textOverflow.onChange() += [=](const auto &) { updateTruncateAndWrapMode(); };
    }

    Size LabelCore::sizeForSpace(Size availableSpace) const
    {
        if (wrap) {
            _jLabel.setMaxWidth((int)(availableSpace.width * getUIScaleFactor()));
        }

        Size result = ViewCore::sizeForSpace(availableSpace);

        if (wrap) {
            _jLabel.setMaxWidth((int)(geometry->width * getUIScaleFactor()));
        }

        return result;
    }

    void LabelCore::textChanged(const Text &text)
    {
        if (_currentAttributedString) {
            _currentAttributedString->linkClicked().unsubscribe(_linkSubscription);
        }

        std::visit(
            [this](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, std::string>) {
                    // Remove '\r' as android treats them as a space
                    std::string textToSet = arg;
                    textToSet.erase(
                        std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                        textToSet.end());
                    _jLabel.setText(textToSet);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto attrString = std::dynamic_pointer_cast<bdn::android::AttributedString>(arg)) {
                        _currentAttributedString = attrString;
                        _linkSubscription = _currentAttributedString->linkClicked().subscribe(
                            [this](auto url) { this->_linkClickCallback.fire(url); });
                        _jLabel.setText(attrString->spanned());
                    }
                }
            },
            text);

        markDirty();
        scheduleLayout();
    }

    void LabelCore::updateTruncateAndWrapMode()
    {
        if (!wrap) {
            bool useSingleLine = true;

            switch (textOverflow.get()) {
            case TextOverflow::EllipsisHead:
                _jLabel.setEllipsize(bdn::android::wrapper::TextUtils::TruncateAt::START);
                break;
            case TextOverflow::EllipsisTail:
                _jLabel.setEllipsize(bdn::android::wrapper::TextUtils::TruncateAt::END);
                break;
            case TextOverflow::EllipsisMiddle:
                _jLabel.setEllipsize(bdn::android::wrapper::TextUtils::TruncateAt::MIDDLE);
                break;
            case TextOverflow::Clip:
                _jLabel.setEllipsize(bdn::android::wrapper::TextUtils::TruncateAt(bdn::java::Reference()));
                break;
            case TextOverflow::ClipWord:
                useSingleLine = false;
                _jLabel.setEllipsize(bdn::android::wrapper::TextUtils::TruncateAt(bdn::java::Reference()));
                break;
            }

            if (useSingleLine) {
                _jLabel.setSingleLine(true);
            } else {
                _jLabel.setSingleLine(false);
                _jLabel.setMaxLines(1);
            }
        } else {
            _jLabel.setSingleLine(false);
            _jLabel.setMaxLines(std::numeric_limits<int>::max());
        }

        scheduleLayout();
    }
}
