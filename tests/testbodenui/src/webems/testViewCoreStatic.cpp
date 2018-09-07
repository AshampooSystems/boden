#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/webems/ViewCore.h>

using namespace bdn;
using namespace bdn::webems;

TEST_CASE("webems.ViewCoreStatic")
{
    SECTION("htmlEscape")
    {
        REQUIRE(ViewCore::htmlEscape("") == "");
        REQUIRE(ViewCore::htmlEscape("&") == "&amp;");
        REQUIRE(ViewCore::htmlEscape("\'") == "&apos;");
        REQUIRE(ViewCore::htmlEscape("\"") == "&quot;");
        REQUIRE(ViewCore::htmlEscape("<") == "&lt;");
        REQUIRE(ViewCore::htmlEscape(">") == "&gt;");

        REQUIRE(ViewCore::htmlEscape("&&") == "&amp;&amp;");

        REQUIRE(ViewCore::htmlEscape("hi<&>\"\'&world") ==
                "hi&lt;&amp;&gt;&quot;&apos;&amp;world");

        // linebreak should stay as they are
        REQUIRE(ViewCore::htmlEscape("hi\nworld") == "hi\nworld");
    }

    SECTION("textToHtmlContent")
    {
        REQUIRE(ViewCore::textToHtmlContent("") == "");
        REQUIRE(ViewCore::textToHtmlContent("&") == "&amp;");
        REQUIRE(ViewCore::textToHtmlContent("\'") == "&apos;");
        REQUIRE(ViewCore::textToHtmlContent("\"") == "&quot;");
        REQUIRE(ViewCore::textToHtmlContent("<") == "&lt;");
        REQUIRE(ViewCore::textToHtmlContent(">") == "&gt;");

        REQUIRE(ViewCore::textToHtmlContent("&&") == "&amp;&amp;");

        REQUIRE(ViewCore::textToHtmlContent("hi<&>\"\'&world") ==
                "hi&lt;&amp;&gt;&quot;&apos;&amp;world");

        REQUIRE(ViewCore::textToHtmlContent("hi\nworld") == "hi<br>world");
        REQUIRE(ViewCore::textToHtmlContent("hi\r\nworld") == "hi<br>world");
        REQUIRE(ViewCore::textToHtmlContent("hi\r\n\nworld") ==
                "hi<br><br>world");
    }
}
