#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextUiCombiner.h>

#include <bdn/test/MockTextUi.h>

using namespace bdn;

class TestUiCombinerFixture : public Base
{
  public:
    class DummyUi : public bdn::test::MockTextUi
    {
      public:
        int readCallCount = 0;

        P<IAsyncOp<String>> readLine() override
        {
            readCallCount++;

            return nullptr;
        }
    };

    P<TextUiCombiner> _combiner;

    int _doneOkCount;
    int _doneIncorrectCount;

    Array<P<DummyUi>> _subUis;

    void createSubUis(int count)
    {
        for (int i = 0; i < count; i++)
            _subUis.add(newObj<DummyUi>());
    }

    void doTests()
    {
        P<TestUiCombinerFixture> self(this);

        SECTION("0 sub UIs")
        {
            _combiner = newObj<TextUiCombiner>();

            SECTION("read")
            {
                P<IAsyncOp<String>> op = _combiner->readLine();

                REQUIRE(!op->isDone());
                REQUIRE_THROWS_AS(op->getResult(), UnfinishedError);

                // should never finish

                CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, self, this, op)
                {
                    op->onDone().subscribe([op, self, this](P<IAsyncOp<String>> opParam) {
                        if (opParam == op)
                            _doneOkCount++;
                        else
                            _doneIncorrectCount++;
                    });

                    REQUIRE(!op->isDone());
                    REQUIRE_THROWS_AS(op->getResult(), UnfinishedError);

                    REQUIRE(_doneOkCount == 0);
                    REQUIRE(_doneIncorrectCount == 0);

                    CONTINUE_SECTION_WHEN_IDLE(self, this, op)
                    {
                        // notifier should still not have been called

                        REQUIRE(!op->isDone());
                        REQUIRE_THROWS_AS(op->getResult(), UnfinishedError);

                        REQUIRE(_doneOkCount == 0);
                        REQUIRE(_doneIncorrectCount == 0);

                        // when we signal it to stop then it SHOULD finish
                        op->signalStop();

                        REQUIRE(op->isDone());

                        // now we should get AbortedError
                        REQUIRE_THROWS_AS(op->getResult(), AbortedError);

                        // but the notifier is async
                        REQUIRE(_doneOkCount == 0);
                        REQUIRE(_doneIncorrectCount == 0);

                        CONTINUE_SECTION_WHEN_IDLE(self, this, op)
                        {
                            // now the notifier should have been called
                            REQUIRE(_doneOkCount == 1);
                            REQUIRE(_doneIncorrectCount == 0);

                            REQUIRE(op->isDone());

                            // now we should get AbortedError
                            REQUIRE_THROWS_AS(op->getResult(), AbortedError);

                            // when we subscribe another function then it should
                            // also be called (async)
                            op->onDone().subscribe([op, self, this](P<IAsyncOp<String>> opParam) {
                                if (opParam == op)
                                    _doneOkCount++;
                                else
                                    _doneIncorrectCount++;
                            });

                            // call should not have happened yet
                            REQUIRE(_doneOkCount == 1);
                            REQUIRE(_doneIncorrectCount == 0);

                            CONTINUE_SECTION_WHEN_IDLE(self, this, op)
                            {
                                REQUIRE(_doneOkCount == 2);
                                REQUIRE(_doneIncorrectCount == 0);
                            };
                        };
                    };
                };
            }

            SECTION("write")
            {
                // should not crash - otherwise have no effect
                _combiner->output()->write("bla");
                _combiner->statusOrProblem()->write("bla");
            }

            SECTION("writeLine")
            {
                // should not crash - otherwise have no effect
                _combiner->output()->writeLine("bla");
                _combiner->statusOrProblem()->writeLine("bla");
            }
        }

        SECTION("1 sub UI")
        doTestsWithNSubUis(1);

        SECTION("2 sub UI")
        doTestsWithNSubUis(2);

        SECTION("3 sub UI")
        doTestsWithNSubUis(3);

        SECTION("init from two pointers")
        {
            P<DummyUi> subUiA = newObj<DummyUi>();
            P<DummyUi> subUiB = newObj<DummyUi>();

            _combiner = newObj<TextUiCombiner>(subUiA, subUiB);

            _combiner->output()->write("bla");
            REQUIRE(subUiA->getWrittenOutputChunks()[0] == "bla");
            REQUIRE(subUiB->getWrittenOutputChunks()[0] == "bla");
        }

        SECTION("init from list of P")
        {
            List<P<ITextUi>> inList;

            P<DummyUi> subUi = newObj<DummyUi>();
            inList.add(subUi);

            _combiner = newObj<TextUiCombiner>(inList);

            _combiner->output()->write("bla");
            REQUIRE(subUi->getWrittenOutputChunks()[0] == "bla");
        }

        SECTION("init from array of *")
        {
            Array<ITextUi *> inArray;

            P<DummyUi> subUi = newObj<DummyUi>();
            inArray.add(subUi);

            _combiner = newObj<TextUiCombiner>(inArray);

            _combiner->output()->write("bla");
            REQUIRE(subUi->getWrittenOutputChunks()[0] == "bla");
        }
    }

    void doTestsWithNSubUis(int subUiCount)
    {
        createSubUis(subUiCount);

        _combiner = newObj<TextUiCombiner>(_subUis);

        SECTION("read")
        {
            P<IAsyncOp<String>> op = _combiner->readLine();

            REQUIRE(_subUis[0]->readCallCount == 1);

            // the other sub UIs should not have been accessed
            for (int i = 1; i < subUiCount; i++)
                REQUIRE(_subUis[i]->readCallCount == 0);

            // our dummy ui returns a nullptr here
            REQUIRE(op == nullptr);
        }

        SECTION("output")
        {
            String expectedChunk;

            SECTION("write")
            {
                _combiner->output()->write("bla");
                expectedChunk = "bla";
            }

            SECTION("writeLine")
            {
                _combiner->output()->writeLine("bla");
                expectedChunk = "bla\n";
            }

            for (P<DummyUi> &subUi : _subUis) {
                const Array<String> &outputChunks = subUi->getWrittenOutputChunks();
                const Array<String> &statusChunks = subUi->getWrittenStatusOrProblemChunks();

                REQUIRE(outputChunks.size() == 1);
                REQUIRE(outputChunks[0] == expectedChunk);

                REQUIRE(statusChunks.size() == 0);
            }
        }

        SECTION("statusOrProblem")
        {
            String expectedChunk;

            SECTION("write")
            {
                _combiner->statusOrProblem()->write("bla");
                expectedChunk = "bla";
            }

            SECTION("writeLine")
            {
                _combiner->statusOrProblem()->writeLine("bla");
                expectedChunk = "bla\n";
            }

            for (P<DummyUi> &subUi : _subUis) {
                const Array<String> &outputChunks = subUi->getWrittenOutputChunks();
                const Array<String> &statusChunks = subUi->getWrittenStatusOrProblemChunks();

                REQUIRE(outputChunks.size() == 0);

                REQUIRE(statusChunks.size() == 1);
                REQUIRE(statusChunks[0] == expectedChunk);
            }
        }
    }
};

TEST_CASE("TextUiCombiner")
{
    P<TestUiCombinerFixture> fixture = newObj<TestUiCombinerFixture>();

    fixture->doTests();
}
