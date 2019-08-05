#pragma once

#include <bdn/android/wrapper/Spanned.h>
#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kHtmlClassName[] = "android/text/Html";
    class Html : public bdn::java::wrapper::JTObject<kHtmlClassName>
    {
      public:
        using JTObject<kHtmlClassName>::JTObject;

      public:
        constexpr static const java::StaticFinalField<int, Html> TO_HTML_PARAGRAPH_LINES_CONSECUTIVE{
            "TO_HTML_PARAGRAPH_LINES_CONSECUTIVE"};
        constexpr static const java::StaticFinalField<int, Html> TO_HTML_PARAGRAPH_LINES_INDIVIDUAL{
            "TO_HTML_PARAGRAPH_LINES_INDIVIDUAL"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_PARAGRAPH{
            "FROM_HTML_SEPARATOR_LINE_BREAK_PARAGRAPH"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_HEADING{
            "FROM_HTML_SEPARATOR_LINE_BREAK_HEADING"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_LIST_ITEM{
            "FROM_HTML_SEPARATOR_LINE_BREAK_LIST_ITEM"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_LIST{
            "FROM_HTML_SEPARATOR_LINE_BREAK_LIST"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_DIV{
            "FROM_HTML_SEPARATOR_LINE_BREAK_DIV"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_SEPARATOR_LINE_BREAK_BLOCKQUOTE{
            "FROM_HTML_SEPARATOR_LINE_BREAK_BLOCKQUOTE"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_OPTION_USE_CSS_COLORS{
            "FROM_HTML_OPTION_USE_CSS_COLORS"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_MODE_LEGACY{"FROM_HTML_MODE_LEGACY"};
        constexpr static const java::StaticFinalField<int, Html> FROM_HTML_MODE_COMPACT{"FROM_HTML_MODE_COMPACT"};

      public:
        constexpr static StaticMethod<Spanned(std::string, int)> fromHtmlWithFlags{"fromHtml"};
        constexpr static StaticMethod<std::string(Spanned, int)> toHtmlWithFlags{"toHtml"};

        // Deprecated since API 24
        constexpr static StaticMethod<Spanned(std::string)> fromHtml{"fromHtml"};
        constexpr static StaticMethod<std::string(Spanned)> toHtml{"toHtml"};
    };
}
