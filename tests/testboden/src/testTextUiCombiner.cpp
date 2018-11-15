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

    P<TextUiCombiner> _pCombiner;

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
        P<TestUiCombinerFixture> pThis(this);

        SECTION("0 sub UIs")
        {
            _pCombiner = newObj<TextUiCombiner>();

            SECTION("read")
            {
                P<IAsyncOp<String>> pOp = _pCombiner->readLine();

                REQUIRE(!pOp->isDone());
                REQUIRE_THROWS_AS(pOp->getResult(), UnfinishedError);

                // should never finish

                CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, this, pThis, pOp)
                {
                    pOp->onDone().subscribe([pOp, this, pThis](P<IAsyncOp<String>> pOpParam) {
                        if (pOpParam == pOp)
                            _doneOkCount++;
                        else
                            _doneIncorrectCount++;
                    });

                    REQUIRE(!pOp->isDone());
                    REQUIRE_THROWS_AS(pOp->getResult(), UnfinishedError);

                    REQUIRE(_doneOkCount == 0);
                    REQUIRE(_doneIncorrectCount == 0);

                    CONTINUE_SECTION_WHEN_IDLE(this, pThis, pOp)
                    {
                        // notifier should still not have been called

                        REQUIRE(!pOp->isDone());
                        REQUIRE_THROWS_AS(pOp->getResult(), UnfinishedError);

                        REQUIRE(_doneOkCount == 0);
                        REQUIRE(_doneIncorrectCount == 0);

                        // when we signal it to stop then it SHOULD finish
                        pOp->signalStop();

                        REQUIRE(pOp->isDone());

                        // now we should get AbortedError
                        REQUIRE_THROWS_AS(pOp->getResult(), AbortedError);

                        // but the notifier is async
                        REQUIRE(_doneOkCount == 0);
                        REQUIRE(_doneIncorrectCount == 0);

                        CONTINUE_SECTION_WHEN_IDLE(this, pThis, pOp)
                        {
                            // now the notifier should have been called
                            REQUIRE(_doneOkCount == 1);
                            REQUIRE(_doneIncorrectCount == 0);

                            REQUIRE(pOp->isDone());

                            // now we should get AbortedError
                            REQUIRE_THROWS_AS(pOp->getResult(), AbortedError);

                            // when we subscribe another function then it should
                            // also be called (async)
                            pOp->onDone().subscribe([pOp, this, pThis](P<IAsyncOp<String>> pOpParam) {
                                if (pOpParam == pOp)
                                    _doneOkCount++;
                                else
                                    _doneIncorrectCount++;
                            });

                            // call should not have happened yet
                            REQUIRE(_doneOkCount == 1);
                            REQUIRE(_doneIncorrectCount == 0);

                            CONTINUE_SECTION_WHEN_IDLE(this, pThis, pOp)
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
                _pCombiner->output()->write("bla");
                _pCombiner->statusOrProblem()->write("bla");
            }

            SECTION("writeLine")
            {
                // should not crash - otherwise have no effect
                _pCombiner->output()->writeLine("bla");
                _pCombiner->statusOrProblem()->writeLine("bla");
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
            P<DummyUi> pSubUiA = newObj<DummyUi>();
            P<DummyUi> pSubUiB = newObj<DummyUi>();

            _pCombiner = newObj<TextUiCombiner>(pSubUiA, pSubUiB);

            _pCombiner->output()->write("bla");
            REQUIRE(pSubUiA->getWrittenOutputChunks()[0] == "bla");
            REQUIRE(pSubUiB->getWrittenOutputChunks()[0] == "bla");
        }

        SECTION("init from list of P")
        {
            List<P<ITextUi>> inList;

            P<DummyUi> pSubUi = newObj<DummyUi>();
            inList.add(pSubUi);

            _pCombiner = newObj<TextUiCombiner>(inList);

            _pCombiner->output()->write("bla");
            REQUIRE(pSubUi->getWrittenOutputChunks()[0] == "bla");
        }

        SECTION("init from array of *")
        {
            Array<ITextUi *> inArray;

            P<DummyUi> pSubUi = newObj<DummyUi>();
            inArray.add(pSubUi);

            _pCombiner = newObj<TextUiCombiner>(inArray);

            _pCombiner->output()->write("bla");
            REQUIRE(pSubUi->getWrittenOutputChunks()[0] == "bla");
        }
    }

    void doTestsWithNSubUis(int subUiCount)
    {
        createSubUis(subUiCount);

        _pCombiner = newObj<TextUiCombiner>(_subUis);

        SECTION("read")
        {
            P<IAsyncOp<String>> pOp = _pCombiner->readLine();

            REQUIRE(_subUis[0]->readCallCount == 1);

            // the other sub UIs should not have been accessed
            for (int i = 1; i < subUiCount; i++)
                REQUIRE(_subUis[i]->readCallCount == 0);

            // our dummy ui returns a nullptr here
            REQUIRE(pOp == nullptr);
        }

        SECTION("output")
        {
            String expectedChunk;

            SECTION("write")
            {
                _pCombiner->output()->write("bla");
                expectedChunk = "bla";
            }

            SECTION("writeLine")
            {
                _pCombiner->output()->writeLine("bla");
                expectedChunk = "bla\n";
            }

            for (P<DummyUi> &pSubUi : _subUis) {
                const Array<String> &outputChunks = pSubUi->getWrittenOutputChunks();
                const Array<String> &statusChunks = pSubUi->getWrittenStatusOrProblemChunks();

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
                _pCombiner->statusOrProblem()->write("bla");
                expectedChunk = "bla";
            }

            SECTION("writeLine")
            {
                _pCombiner->statusOrProblem()->writeLine("bla");
                expectedChunk = "bla\n";
            }

            for (P<DummyUi> &pSubUi : _subUis) {
                const Array<String> &outputChunks = pSubUi->getWrittenOutputChunks();
                const Array<String> &statusChunks = pSubUi->getWrittenStatusOrProblemChunks();

                REQUIRE(outputChunks.size() == 0);

                REQUIRE(statusChunks.size() == 1);
                REQUIRE(statusChunks[0] == expectedChunk);
            }
        }
    }
};

TEST_CASE("TextUiCombiner")
{
    P<TestUiCombinerFixture> pFixture = newObj<TestUiCombinerFixture>();

    pFixture->doTests();
}
