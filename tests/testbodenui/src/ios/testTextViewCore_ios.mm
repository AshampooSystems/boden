#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosTextViewCore
    : public bdn::test::TestIosViewCoreMixin<bdn::test::TestTextViewCore>
{
  protected:
    void initCore() override
    {
        TestIosViewCoreMixin<bdn::test::TestTextViewCore>::initCore();

        _pUILabel = (UILabel *)_pUIView;
        REQUIRE(_pUILabel != nullptr);
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
        String expectedText = _pTextView->text();

        String text = bdn::ios::iosStringToString(_pUILabel.text);

        REQUIRE(text == expectedText);
    }

  protected:
    UILabel *_pUILabel;
};

TEST_CASE("ios.TextViewCore")
{
    P<TestIosTextViewCore> pTest = newObj<TestIosTextViewCore>();

    pTest->runTests();
}
