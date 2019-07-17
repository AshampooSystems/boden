#include <bdn/android/AttributedString.h>
#include <bdn/android/wrapper/Build.h>
#include <bdn/android/wrapper/Html.h>

#include <bdn/log.h>

namespace bdn::android
{
    AttributedString::AttributedString() : _spanned(bdn::java::Reference()) {}

    bool AttributedString::fromHtml(const String &html)
    {
        if ((int)wrapper::Build::VERSION::SDK_INT >= wrapper::Build::VERSION_CODES::M) {
            _spanned = wrapper::Html::fromHtmlWithFlags(html, wrapper::Html::FROM_HTML_MODE_COMPACT);
        } else {
            _spanned = wrapper::Html::fromHtml(html);
        }

        return true;
    }

    String AttributedString::toHtml() const
    {
        if ((int)wrapper::Build::VERSION::SDK_INT > wrapper::Build::VERSION_CODES::M) {
            return wrapper::Html::toHtmlWithFlags(_spanned, wrapper::Html::TO_HTML_PARAGRAPH_LINES_INDIVIDUAL);
        } else {
            return wrapper::Html::toHtml(_spanned);
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
