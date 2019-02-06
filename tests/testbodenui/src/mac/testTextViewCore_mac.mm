
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacTextViewCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestTextViewCore>
{
  protected:
    void initCore() override
    {
        TestMacChildViewCoreMixin<bdn::test::TestTextViewCore>::initCore();

        _nsTextView = (NSTextView *)_nsView;
        REQUIRE(_nsTextView != nullptr);
    }

    bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const override
    {
        // unfortunately the mac text view will wrap single words into
        // individual characters. This is not the recommended behaviour for
        // boden, but there is apparently no setting to change that.
        return true;
    }

    void runPostInitTests() override
    {
        TestMacChildViewCoreMixin<bdn::test::TestTextViewCore>::runPostInitTests();

        SECTION("not editable") { REQUIRE(_nsTextView.editable == false); }

        SECTION("not selectable") { REQUIRE(_nsTextView.selectable == false); }

        SECTION("not rich text") { REQUIRE(_nsTextView.richText == false); }
    }

    void verifyCoreText() override
    {
        String expectedText = _textView->text;

        String text = bdn::mac::nsStringToString(_nsTextView.string);

        REQUIRE(text == expectedText);
    }

  protected:
    NSTextView *_nsTextView;
};

TEST_CASE("mac.TextViewCore")
{
    std::shared_ptr<TestMacTextViewCore> test = std::make_shared<TestMacTextViewCore>();

    test->runTests();
}
