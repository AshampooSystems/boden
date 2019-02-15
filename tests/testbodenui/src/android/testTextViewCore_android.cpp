
#include <bdn/test.h>

#include "TestAndroidViewCoreMixin.h"
#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>

using namespace bdn;

class TestAndroidTextViewCore : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestTextViewCore>
{
  public:
    TestAndroidTextViewCore() : _jTextView(nullptr) {}

  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<bdn::test::TestTextViewCore>::initCore();

        _jTextView = bdn::android::JTextView(_jView.getRef_());
    }

    bool usesAllAvailableWidthWhenWrapped() const override
    {
        // unfortunately the android textview will use up all available width
        // when it reaches it (even if the wrapped text ends up being slightly
        // less wide).
        return true;
    }

    bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const override
    {
        // text views will wrap individual characters if a word does not fit.
        return true;
    }

    void verifyCoreText() override
    {
        String expectedText = _textView->text;

        String text = _jTextView.getText();

        REQUIRE(text == expectedText);
    }

  protected:
    bdn::android::JTextView _jTextView;
};

TEST_CASE("android.TextViewCore")
{
    std::shared_ptr<TestAndroidTextViewCore> test = std::make_shared<TestAndroidTextViewCore>();

    test->runTests();
}
