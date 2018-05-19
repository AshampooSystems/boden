#ifndef BDN_TEST_MockTextUi_H_
#define BDN_TEST_MockTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/NotImplementedError.h>

#include <bdn/test/MockTextSink.h>

namespace bdn
{
namespace test
{

    
/** A mock implementation for bdn::ITextUi. This is intended for usage
    in unit tests.*/
class MockTextUi : public Base, BDN_IMPLEMENTS ITextUi
{
public:
    
    MockTextUi()
    {
        _pStatusOrProblemSink = newObj<MockTextSink>();
        _pOutputSink = newObj<MockTextSink>();
    }

    /** Returns the list of text chunks that have been written to the output text sink.
        Each write call creates a new entry in this list. The writeLine routines add a newline at the
        end of the written text.
        */
    const Array< String >& getWrittenOutputChunks() const
    {
        return  _pOutputSink->getWrittenChunks();
    }


    /** Returns the list of text chunks that have been written to the statusOrProblem text sink.
        Each write call creates a new entry in this list. The writeLine routines add a newline at the
        end of the written text.
        */
    const Array< String >& getWrittenStatusOrProblemChunks() const
    {
        return  _pStatusOrProblemSink->getWrittenChunks();
    }
    
    

    P< IAsyncOp<String> > readLine() override
    {
        throw NotImplementedError("MockTextUi::readLine");
    }

    
    P<ITextSink> statusOrProblem() override
    {
        return _pStatusOrProblemSink;
    }
    
    P<ITextSink> output() override
    {
        return _pOutputSink;
    }
	
        

private:
    
    
    P<MockTextSink> _pOutputSink;
    P<MockTextSink> _pStatusOrProblemSink;
};



}
}

#endif
