
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

        _nSTextView = (NSTextView *)_nSView;
        REQUIRE(_nSTextView != nullptr);
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

        SECTION("not editable") { REQUIRE(_nSTextView.editable == false); }

        SECTION("not selectable") { REQUIRE(_nSTextView.selectable == false); }

        SECTION("not rich text") { REQUIRE(_nSTextView.richText == false); }
    }

    void verifyCoreText() override
    {
        String expectedText = _textView->text;

        String text = bdn::mac::macStringToString(_nSTextView.string);

        REQUIRE(text == expectedText);
    }

  protected:
    NSTextView *_nSTextView;
};

TEST_CASE("mac.TextViewCore")
{
    std::shared_ptr<TestMacTextViewCore> test = std::make_shared<TestMacTextViewCore>();

    test->runTests();
}
