#ifndef BDN_TEST_MockTextUi_H_
#define BDN_TEST_MockTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/NotImplementedError.h>

#include <vector>

namespace bdn
{
namespace test
{

    
/** A mock implementation for bdn::ITextUi. This is intended for usage
    in unit tests.*/
class MockTextUi : public Base, BDN_IMPLEMENTS ITextUi
{
public:

    /** Returns the list of text chunks that have been written with the "normal" (=non-error)
        write routines. Each writeXYZ call
        creates a new entry in this list. The writeLine routines add a newline at the
        end of the written text.
        */
    const std::vector< String >& getWrittenChunks() const
    {
        return  _writtenChunks;
    }


    /** Returns the list of text chunks that have been written with the error write routines. Each writeXYZ call
        creates a new entry in this list. The writeLine routines add a newline at the
        end of the written text.
        */
    const std::vector< String >& getWrittenErrorChunks() const
    {
        return  _writtenErrorChunks;
    }
    
    

    P< IAsyncOp<String> > readLine() override
    {
        throw NotImplementedError("MockTextUi::readLine");
    }

	
	void write(const String& s) override
    {
        _writtenChunks.push_back(s);
    }

	
	void writeLine(const String& s) override
    {
        _writtenChunks.push_back(s+"\n");
    }


	void writeError(const String& s) override
    {
        _writtenErrorChunks.push_back(s);
    }
	
    
	void writeErrorLine(const String& s) override
    {
        _writtenErrorChunks.push_back(s+"\n");
    }

private:
    

    std::vector< String > _writtenChunks;
    std::vector< String > _writtenErrorChunks;
};



}
}

#endif
