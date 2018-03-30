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
        _pUiProvider = newObj<bdn::test::MockUiProvider>();

        _pUi = newObj< ViewTextUi >( _pUiProvider );        
    }

    void doTest()
    {
        SECTION("writeLine")
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeLine), "\n" );

        SECTION("write")
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::write), "" );

        SECTION("writeErrorLine")
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeErrorLine), "\n" );

        SECTION("writeError")
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeError), "" );
    }
    
    String getWrittenText()
    {    
        P<Window> pWindow = _pUi->getWindow();

        List<P<View> > childList;
        pWindow->getChildViews( childList );

        REQUIRE( childList.size() == 1);

        P<ScrollView>   pScrollView = cast<ScrollView>( childList.front() );
        P<View>         pContainer = pScrollView->getContentView();

        String text;
        bool   firstPara = true;
        
        childList.clear();
        pContainer->getChildViews( childList );
        for( auto& pParaView: childList)
        {
            P<TextView> pParaTextView = cast<TextView>( pParaView );

            String para = pParaTextView->text();

            // the paragraph should NOT contain any line breaks
            REQUIRE( !para.contains("\n") );

            if(!firstPara)
                text += "\n";           
            text += para;

            firstPara = false;
        }

        return text;
    }

    void verifyWrittenText(const String& expectedText)
    {
        String text = getWrittenText();

        REQUIRE( text == expectedText );
    }

    
    void testWrite(std::function< void(String) > writeFunc, String expectedWriteSuffix)
    {
        P<TestViewTextUiFixture> pThis = this;

        SECTION("sequential")
        {
            writeFunc("first");

            // ViewTextUI updates the written text only 10 times per second.
            // So we need to wait until we can check it.
            CONTINUE_SECTION_AFTER_SECONDS(1, pThis, this, writeFunc, expectedWriteSuffix)
            {
                // note that the expectedWriteSuffix (either linebreak or empty)
                // only takes effect when the next line is begun.
                String expectedText = "first";
                verifyWrittenText( expectedText );

                expectedText += expectedWriteSuffix;
            
                writeFunc("second");

                CONTINUE_SECTION_AFTER_SECONDS(1, pThis, this, expectedText, writeFunc, expectedWriteSuffix)
                {
                    String expectedText2 = expectedText + "second";
                    verifyWrittenText(expectedText2);
                
                    expectedText2 += expectedWriteSuffix;
            
                    writeFunc("third");

                    expectedText2 += "third";

                    CONTINUE_SECTION_AFTER_SECONDS(1, pThis, this, expectedText2, writeFunc)
                    {                        
                        verifyWrittenText(expectedText2);      
                    };
                };
            };
        }

        SECTION("with linebreaks")
        {
            writeFunc("hello\n\n\nworld\n");

            // the last linebreak at the end is not printed until the next text is written.
            // So we write another dummy string to force the write.
            _pUi->write("X");

            CONTINUE_SECTION_AFTER_SECONDS(1, pThis, this, expectedWriteSuffix, writeFunc)
            {  
                verifyWrittenText( "hello\n\n\nworld\n"+expectedWriteSuffix+"X" );
            };
        }

#if BDN_HAVE_THREADS
        SECTION("multithreaded")
        {
            P<Signal> pSignal = newObj<Signal>();
            
            std::future<void> thread1Result = Thread::exec(
                [writeFunc, pSignal]()
                {
                    pSignal->wait();
                    writeFunc(",first");
                } );

            std::future<void> thread2Result = Thread::exec(
                [writeFunc, pSignal]()
                {
                    pSignal->wait();
                    writeFunc(",second");
                } );

            std::future<void> thread3Result = Thread::exec(
                [writeFunc, pSignal]()
                {
                    pSignal->wait();
                    writeFunc(",third");
                } );

            // wait a little to ensure that all three threads are waiting on
            // out signal.
            Thread::sleepMillis(1000);

            // set the signal to start all three operations at the same time
            pSignal->set();

            // wait for all threads to finish
            thread1Result.get();
            thread2Result.get();
            thread3Result.get();

            CONTINUE_SECTION_AFTER_SECONDS(1, pThis, this, expectedWriteSuffix)
            {
                String result = getWrittenText();

                REQUIRE( result.startsWith(",") );
                result = result.subString(1);

                bool gotFirst = false;
                bool gotSecond = false;
                bool gotThird = false;

                char32_t lastSep=1;
                int      tokenNum=1;

                while(!result.isEmpty())
                {
                    String token = result.splitOffToken(",", true, &lastSep);

                    if(tokenNum==3)
                    {
                        // the expected write suffix of each token only takes effect when
                        // the next line is started. So the last one will not have it.
                        // Add it to simplify the test
                        token += expectedWriteSuffix;
                    }

                    if(token=="first"+expectedWriteSuffix)
                    {
                        REQUIRE( !gotFirst );
                        gotFirst = true;
                    }
                    if(token=="second"+expectedWriteSuffix)
                    {
                        REQUIRE( !gotSecond );
                        gotSecond = true;
                    }

                    if(token=="third"+expectedWriteSuffix)
                    {
                        REQUIRE( !gotThird );
                        gotThird = true;
                    }

                    tokenNum++;
                }

                REQUIRE( gotFirst );
                REQUIRE( gotSecond );
                REQUIRE( gotThird );

                REQUIRE( lastSep==0 );
            };
        }
#endif
    }
    
private:
    P<bdn::test::MockUiProvider>        _pUiProvider;
    P<ViewTextUi>                       _pUi;
};


TEST_CASE("ViewTextUi")
{
    P<TestViewTextUiFixture> pFixture = newObj<TestViewTextUiFixture>();

    pFixture->doTest();
}






