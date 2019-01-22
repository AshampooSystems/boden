
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosTextViewCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestTextViewCore>
{
  protected:
    void initCore() override
    {
        TestIosViewCoreMixin<bdn::test::TestTextViewCore>::initCore();

        _uILabel = (UILabel *)_uIView;
        REQUIRE(_uILabel != nullptr);
    }

    bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const override
    {
        // unfortunately the ios text view will wrap single words into
        // individual characters. This is not the recommended behaviour for
        // boden, but there is apparently no setting to change that.
        return true;
    }

    void verifyCoreText() override
    {
        String expectedText = _textView->text;

        String text = bdn::ios::nsStringToString(_uILabel.text);

        REQUIRE(text == expectedText);
    }

  protected:
    UILabel *_uILabel;
};

TEST_CASE("ios.TextViewCore")
{
    std::shared_ptr<TestIosTextViewCore> test = std::make_shared<TestIosTextViewCore>();

    test->runTests();
}
