#include <bdn/android/AttributedString.h>
#include <bdn/android/wrapper/Html.h>

namespace bdn::android
{
    AttributedString::AttributedString() : _spanned(bdn::java::Reference()) {}

    bool AttributedString::fromHtml(const String &html)
    {
        _spanned = bdn::android::wrapper::Html::fromHtml(html, 63);
        return true;
    }
}

namespace bdn
{
    std::function<std::shared_ptr<AttributedString>()> AttributedString::defaultCreateAttributedString()
    {
        return []() -> std::shared_ptr<AttributedString> { return std::make_shared<bdn::android::AttributedString>(); };
    }
}
