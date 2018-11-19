#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ViewTextUi.h>
#include <bdn/Signal.h>
#include <bdn/Thread.h>

#include <bdn/test/MockUiProvider.h>

using namespace bdn;

class TestViewTextUiFixture : public Base
{
  public:
    TestViewTextUiFixture()
    {
        _uiProvider = newObj<bdn::test::MockUiProvider>();

        _ui = newObj<ViewTextUi>(_uiProvider);
    }

    void doTest()
    {
        SECTION("output.writeLine")
        testWrite(strongMethod((ITextSink *)_ui->output().getPtr(), &ITextSink::writeLine), "\n");

        SECTION("output.write")
        testWrite(strongMethod((ITextSink *)_ui->output().getPtr(), &ITextSink::write), "");

        SECTION("statusOrProblem.writeLine")
        testWrite(strongMethod((ITextSink *)_ui->statusOrProblem().getPtr(), &ITextSink::writeLine), "\n");

        SECTION("statusOrProblem.writeError")
        testWrite(strongMethod((ITextSink *)_ui->statusOrProblem().getPtr(), &ITextSink::write), "");
    }

    String getWrittenText()
    {
        P<Window> window = _ui->getWindow();

        List<P<View>> childList;
        window->getChildViews(childList);

        REQUIRE(childList.size() == 1);

        P<ScrollView> scrollView = cast<ScrollView>(childList.front());
        P<View> container = scrollView->getContentView();

        String text;
        bool firstPara = true;

        childList.clear();
        container->getChildViews(childList);
        for (auto &paraView : childList) {
            P<TextView> paraTextView = cast<TextView>(paraView);

            String para = paraTextView->text();

            // the paragraph should NOT contain any line breaks
            REQUIRE(!para.contains("\n"));

            if (!firstPara)
                text += "\n";
            text += para;

            firstPara = false;
        }

        return text;
    }

    void verifyWrittenText(const String &expectedText)
    {
        String text = getWrittenText();

        REQUIRE(text == expectedText);
    }

    void testWrite(std::function<void(String)> writeFunc, String expectedWriteSuffix)
    {
        P<TestViewTextUiFixture> self = this;

        SECTION("sequential")
        {
            writeFunc("first");

            // ViewTextUI updates the written text only 10 times per second.
            // So we need to wait until we can check it.
            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, writeFunc, expectedWriteSuffix)
            {
                // note that the expectedWriteSuffix (either linebreak or empty)
                // only takes effect when the next line is begun.
                String expectedText = "first";
                verifyWrittenText(expectedText);

                expectedText += expectedWriteSuffix;

                writeFunc("second");

                CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, expectedText, writeFunc, expectedWriteSuffix)
                {
                    String expectedText2 = expectedText + "second";
                    verifyWrittenText(expectedText2);

                    expectedText2 += expectedWriteSuffix;

                    writeFunc("third");

                    expectedText2 += "third";

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, expectedText2, writeFunc)
                    {
                        verifyWrittenText(expectedText2);
                    };
                };
            };
        }

        SECTION("with linebreaks")
        {
            writeFunc("hello\n\n\nworld\n");

            // the last linebreak at the end is not printed until the next text
            // is written. So we write another dummy string to force the write.
            _ui->output()->write("X");

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, expectedWriteSuffix, writeFunc)
            {
                verifyWrittenText("hello\n\n\nworld\n" + expectedWriteSuffix + "X");
            };
        }

#if BDN_HAVE_THREADS
        SECTION("multithreaded")
        {
            P<Signal> signal = newObj<Signal>();

            std::future<void> thread1Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",first");
            });

            std::future<void> thread2Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",second");
            });

            std::future<void> thread3Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",third");
            });

            // wait a little to ensure that all three threads are waiting on
            // out signal.
            Thread::sleepMillis(1000);

            // set the signal to start all three operations at the same time
            signal->set();

            // wait for all threads to finish
            thread1Result.get();
            thread2Result.get();
            thread3Result.get();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, expectedWriteSuffix)
            {
                String result = getWrittenText();

                REQUIRE(result.startsWith(","));
                result = result.subString(1);

                bool gotFirst = false;
                bool gotSecond = false;
                bool gotThird = false;

                char32_t lastSep = 1;
                int tokenNum = 1;

                while (!result.isEmpty()) {
                    String token = result.splitOffToken(",", true, &lastSep);

                    if (tokenNum == 3) {
                        // the expected write suffix of each token only takes
                        // effect when the next line is started. So the last one
                        // will not have it. Add it to simplify the test
                        token += expectedWriteSuffix;
                    }

                    if (token == "first" + expectedWriteSuffix) {
                        REQUIRE(!gotFirst);
                        gotFirst = true;
                    }
                    if (token == "second" + expectedWriteSuffix) {
                        REQUIRE(!gotSecond);
                        gotSecond = true;
                    }

                    if (token == "third" + expectedWriteSuffix) {
                        REQUIRE(!gotThird);
                        gotThird = true;
                    }

                    tokenNum++;
                }

                REQUIRE(gotFirst);
                REQUIRE(gotSecond);
                REQUIRE(gotThird);

                REQUIRE(lastSep == 0);
            };
        }
#endif
    }

  private:
    P<bdn::test::MockUiProvider> _uiProvider;
    P<ViewTextUi> _ui;
};

TEST_CASE("ViewTextUi")
{
    P<TestViewTextUiFixture> fixture = newObj<TestViewTextUiFixture>();

    fixture->doTest();
}
