#include <bdn/Color.h>
#include <bdn/android/AttributedString.h>
#include <bdn/android/wrapper/AbsoluteSizeSpan.h>
#include <bdn/android/wrapper/BaselineShiftSpan.h>
#include <bdn/android/wrapper/Build.h>
#include <bdn/android/wrapper/ForegroundColorSpan.h>
#include <bdn/android/wrapper/Html.h>
#include <bdn/android/wrapper/LinkClickSpan.h>
#include <bdn/android/wrapper/RelativeSizeSpan.h>
#include <bdn/android/wrapper/StyleSpan.h>
#include <bdn/android/wrapper/Typeface.h>
#include <bdn/android/wrapper/TypefaceSpan.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

#include <bdn/Font.h>
#include <bdn/log.h>

namespace bdn::android
{
    AttributedString::AttributedString() : _spannedBuilder(bdn::java::Reference()) {}

    void AttributedString::addAttribute(std::string attributeName, std::any value, AttributedString::Range range)
    {
        visitWithObjectFromAttribute(std::make_pair(attributeName, value),
                                     [range, &builder = _spannedBuilder](auto obj) {
                                         auto end = range.start + range.length;
                                         if (range.length == -1) {
                                             end = builder.length();
                                         }

                                         builder.setSpan(obj, range.start, end, 0);
                                     });
    }

    void AttributedString::addAttributes(AttributedString::AttributeMap attributes, AttributedString::Range range)
    {
        for (auto attribute : attributes) {
            visitWithObjectFromAttribute(attribute, [range, &builder = _spannedBuilder](auto obj) {
                auto end = range.start + range.length;
                if (range.length == -1) {
                    end = builder.length();
                }

                builder.setSpan(obj, range.start, end, 0);
            });
        }
    }

    void AttributedString::fromString(const std::string &text)
    {
        _spannedBuilder = wrapper::SpannableStringBuilder(text);
    }

    bool AttributedString::fromHTML(const std::string &html)
    {
        if ((int)wrapper::Build::VERSION::SDK_INT > wrapper::Build::VERSION_CODES::M) {
            _spannedBuilder = wrapper::Html::fromHtmlWithFlags(html, wrapper::Html::FROM_HTML_MODE_COMPACT);
        } else {
            _spannedBuilder = wrapper::Html::fromHtml(html);
        }

        return true;
    }

    std::string AttributedString::toHTML() const
    {
        if ((int)wrapper::Build::VERSION::SDK_INT > wrapper::Build::VERSION_CODES::M) {
            return wrapper::Html::toHtmlWithFlags(_spannedBuilder, wrapper::Html::TO_HTML_PARAGRAPH_LINES_INDIVIDUAL);
        } else {
            return wrapper::Html::toHtml(_spannedBuilder);
        }
    }

    void AttributedString::visitWithObjectFromAttribute(AttributeMap::const_reference attribute,
                                                        std::function<void(java::wrapper::Object)> func)
    {
        if (attribute.first == "foreground-color") {
            auto color = Color::fromAny(attribute.second);
            func(wrapper::ForegroundColorSpan(color));
        } else if (attribute.first == "font") {
            auto font = Font::fromAny(attribute.second);
            if (font.size.type != Font::Size::Type::Inherit) {
                if (font.size.type == Font::Size::Type::Pixels || font.size.type == Font::Size::Type::Points) {
                    wrapper::AbsoluteSizeSpan absoluteSizeSpan(font.size.value,
                                                               font.size.type == Font::Size::Type::Points);
                    func(absoluteSizeSpan);
                } else {
                    float relative = 1.0f;

                    switch (font.size.type) {
                    case Font::Size::Type::Medium:
                        break;
                    case Font::Size::Type::Small:
                        relative = 0.75;
                        break;
                    case Font::Size::Type::XSmall:
                        relative = 0.5;
                        break;
                    case Font::Size::Type::XXSmall:
                        relative = 0.25;
                        break;
                    case Font::Size::Type::Large:
                        relative = 1.25;
                        break;
                    case Font::Size::Type::XLarge:
                        relative = 1.5;
                        break;
                    case Font::Size::Type::XXLarge:
                        relative = 1.75;
                        break;
                    case Font::Size::Type::Percent:
                        relative = font.size.value;
                        break;
                    default:
                        break;
                    }

                    if (relative != 1.0) {
                        wrapper::RelativeSizeSpan relativeSizeSpan(relative);
                        func(relativeSizeSpan);
                    }
                }
            }
            if (font.family.empty()) {
                int style = 0;

                if (font.style == Font::Style::Italic && font.weight >= Font::Bold) {
                    style = wrapper::Typeface::BOLD_ITALIC;
                } else if (font.weight >= Font::Bold) {
                    style |= wrapper::Typeface::BOLD;
                } else if (font.style == Font::Style::Italic) {
                    style |= wrapper::Typeface::ITALIC;
                }

                if (style != 0) {
                    func(wrapper::StyleSpan(style));
                }
            } else {
                auto typeFace = wrapper::Typeface::createWithFamilyAndStyle(font.family, wrapper::Typeface::NORMAL);
                if (font.weight != Font::Inherit || font.style == Font::Style::Italic) {
                    int weight = font.weight != Font::Weight::Inherit ? font.weight : Font::Weight::Normal;
                    weight = std::max(0, std::min(weight, 1000));
                    typeFace = wrapper::Typeface::createWithWeightAndItalic(typeFace, weight,
                                                                            font.style == Font::Style::Italic);
                }
                func(wrapper::TypefaceSpan(typeFace));
            }
        } else if (attribute.first == "link") {
            auto string = stringFromAny(attribute.second);

            func(wrapper::LinkClickSpan(java::wrapper::NativeWeakPointer(shared_from_this()), string));
        } else if (attribute.first == "baseline-offset") {
            int offset = 0;
            if (attribute.second.type() == typeid(bdn::json)) {
                offset = (float)std::any_cast<bdn::json>(attribute.second);
            } else if (attribute.second.type() == typeid(float)) {
                offset = std::any_cast<float>(attribute.second);
            }

            func(wrapper::BaselineShiftSpan(-offset));
        }
    }
}

namespace bdn
{
    std::function<std::shared_ptr<AttributedString>()> AttributedString::defaultCreateAttributedString()
    {
        return []() -> std::shared_ptr<AttributedString> { return std::make_shared<bdn::android::AttributedString>(); };
    }
}
