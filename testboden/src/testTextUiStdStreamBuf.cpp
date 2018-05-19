#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextSinkStdStreamBuf.h>
#include <bdn/NotImplementedError.h>
#include <bdn/localeUtil.h>

#include <bdn/test/MockTextSink.h>


using namespace bdn;




template<typename CharType>
struct TextSinkStreamBufSubTestData_
{
    std::basic_string<CharType>	encoded;
    String	                    expectedDecoded;
    const char*		            desc;
};

template<typename CharType>
static void initTextSinkStreamBufSubTestData( std::list<TextSinkStreamBufSubTestData_<CharType> >& subTestDataList );

template<>
void initTextSinkStreamBufSubTestData<char>( std::list<TextSinkStreamBufSubTestData_<char> >& subTestDataList )
{
    subTestDataList = std::list< TextSinkStreamBufSubTestData_<char> > {
                                { "", U"", "empty" },
                                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                // Use \0 instead.
								{ std::string("\0", 1), std::u32string(U"\0", 1), "zero char" },
								{ "h", U"h", "ascii char" },
								{ "hi", U"hi", "ascii 2 chars" },
								{ u8"\u0181", U"\u0181", "2 byte" },
								{ u8"\u0810", U"\u0810", "3 bytes"},
								{ u8"\U00010010", U"\U00010010", "4 bytes"},
								// C++ literals do not recognize characters in the 5 and 6 byte range.
								// So we specify the encoded version manually
								{ "\xF8\x88\x80\x80\x90", U"\x00200010", "5 bytes"},
								{ "\xFC\x84\x80\x80\x80\x90", U"\x04000010", "6 bytes"},

								{ "\xc2hello", U"\ufffdhello", "byte missing at start of string" },
								{ "hell\xc2o", U"hell\ufffdo", "byte missing in middle of string" },

								{ "\xE0\xA4\x92", U"\u0912", "valid 3 byte sequence" },
								{ "\xE0\xA4\x12", U"\ufffd\ufffd\u0012", "last byte ascii in 3 byte sequence" },
                };

}


template<>
void initTextSinkStreamBufSubTestData<char16_t>( std::list<TextSinkStreamBufSubTestData_<char16_t> >& subTestDataList )
{
    // \uFFFF yields and incorrect string with G++ 4.8. So we work around it.
	char16_t u16ffff[2] = {0xffff, 0};
	char32_t u32ffff[2] = {0xffff, 0};

    subTestDataList = std::list< TextSinkStreamBufSubTestData_<char16_t> > {
                                { u"", U"", "empty" },
                                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                // Use \0 instead.
								{ std::u16string(u"\0", 1), std::u32string(U"\0", 1), "zero char" },
								{ u"h", U"h", "ascii char" },
								{ u"hx", U"hx", "ascii 2 chars" },
								{ u"\u0345", U"\u0345", "non-ascii below surrogate range" },
								{ u"\U00010437", U"\U00010437", "surrogate pair A" },
								{ u"\U00024B62", U"\U00024B62", "surrogate pair B" },
								{ u"\uE000", U"\uE000", "above surrogate range A" },
								{ u"\uF123", U"\uF123", "above surrogate range B" },

								// \uFFFF yields and incorrect string with G++ 4.8. So we work around it.
								{ u16ffff, u32ffff, "above surrogate range C" },
                                
								{ u"\xD801\U00024B62", U"\ufffd\U00024B62", "high surrogate at start, pair follows" },
								{ u"\xD801\u0345", U"\ufffd\u0345", "high surrogate at start, single follows" },
								{ u"\U00024B62\xD801\U00010437", U"\U00024B62\ufffd\U00010437", "high surrogate between pairs" },
								{ u"\u0345\xD801\u0567", U"\u0345\ufffd\u0567", "high surrogate between singles" },

								{ u"\xDC37\U00024B62", U"\ufffd\U00024B62", "low surrogate at start, pair follows" },
								{ u"\xDC37\u0345", U"\ufffd\u0345", "low surrogate at start, single follows" },
								{ u"\U00024B62\xDC37\U00010437", U"\U00024B62\ufffd\U00010437", "low surrogate between pairs" },
								{ u"\u0345\xDC37\u0567", U"\u0345\ufffd\u0567", "low surrogate between singles" } };

}



template<>
void initTextSinkStreamBufSubTestData<char32_t>( std::list<TextSinkStreamBufSubTestData_<char32_t> >& subTestDataList )
{
    subTestDataList = std::list< TextSinkStreamBufSubTestData_<char32_t> > {
                                { U"", U"", "empty" },
                                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                // Use \0 instead.
								{ std::u32string(U"\0", 1), std::u32string(U"\0", 1), "zero char" },
								{ U"h", U"h", "ascii char" },
								{ U"hx", U"hx", "ascii 2 chars" },
								{ U"\U00024B62", U"\U00024B62", "non-ascii" },
								{ U"\U00010437\U00024B62", U"\U00010437\U00024B62", "2 non-ascii" }, };

}


template<>
void initTextSinkStreamBufSubTestData<wchar_t>( std::list<TextSinkStreamBufSubTestData_<wchar_t> >& subTestDataList )
{
    if(sizeof(wchar_t)==2)
    {
        std::list<TextSinkStreamBufSubTestData_<char16_t> > u16List;
        initTextSinkStreamBufSubTestData(u16List);

        for(auto& u16Entry: u16List)
        {
            TextSinkStreamBufSubTestData_<wchar_t> entry;
            entry.encoded = std::wstring((const wchar_t*)u16Entry.encoded.c_str(), u16Entry.encoded.length());
            entry.expectedDecoded = u16Entry.expectedDecoded;
            entry.desc = u16Entry.desc;

            subTestDataList.push_back(entry);
        }
    }
    else
    {
        std::list<TextSinkStreamBufSubTestData_<char32_t> > u32List;
        initTextSinkStreamBufSubTestData(u32List);

        for(auto& u32Entry: u32List)
        {
            TextSinkStreamBufSubTestData_<wchar_t> entry;
            entry.encoded = std::wstring((const wchar_t*)u32Entry.encoded.c_str(), u32Entry.encoded.length());
            entry.expectedDecoded = u32Entry.expectedDecoded;
            entry.desc = u32Entry.desc;

            subTestDataList.push_back(entry);
        }
    }
}

template<class CharType>
static void testTextSinkStreamBuf_Preinitialized(P< bdn::test::MockTextSink> pSink, TextSinkStdStreamBuf<CharType>& buf, bool multiByteIsUtf8 )
{  
    const Array<String>& writtenChunks = pSink->getWrittenChunks();

    SECTION("sputc auto flush")
    {
        // the buffer has an input size of 64 elements.
        String expectedDataA;
        for(int i=0; i<64; i++)
        {
            buf.sputc('a' );
            expectedDataA += "a";
        }

        const Array<String>& writtenChunks = pSink->getWrittenChunks();

        // nothing should have been written yet
        REQUIRE( writtenChunks.size() == 0);

        buf.sputc('b');

        // this should trigger an overflow. All decodable data in the buffer
        // should be flushed

        REQUIRE( writtenChunks.size() == 1);
        REQUIRE( writtenChunks[0] == expectedDataA );

        String expectedDataB = "b";

        // write 63 more b's
        for(int i=0; i<63; i++)
        {
            buf.sputc('b');
            expectedDataB += "b";
        }

        // should not have flushed yet
        REQUIRE( writtenChunks.size() == 1);

        // write one more
        buf.sputc('c');

        // this should trigger another overflow / flush
        REQUIRE( writtenChunks.size() == 2);
        REQUIRE( writtenChunks[0] == expectedDataA );
        REQUIRE( writtenChunks[1] == expectedDataB );
    }

    SECTION("sputn auto flush")
    {
        std::basic_string<CharType> toWrite8;
        for(int i=0; i<8; i++)
            toWrite8 += (CharType)('a'+i);

        // write 7 times toWriteA8
        String expected;
        for(int i=0; i<7; i++)
        {
            buf.sputn(toWrite8.c_str(), 8);
            expected += toWrite8;
        }

        // nothing should have been flushed yet
        REQUIRE( writtenChunks.size() == 0);

        // The following should not trigger an overflow yet, since it still all
        // fits into the buffer.

        SECTION("hit buffer limit exactly, then overflow")
        {            
            buf.sputn(toWrite8.c_str(), 8);
            expected += toWrite8;
                        
            // now trigger the overflow        
            buf.sputn(toWrite8.c_str(), 8);
            
            // this should trigger an overflow. All decodable data previously
            // in the buffer should be flushed, then the new 8 characters
            // should be buffered.

            REQUIRE( writtenChunks.size() == 1);
            REQUIRE( writtenChunks[0] == expected );

            // now do a sync - this should flush the tail data
            buf.pubsync();

            REQUIRE( writtenChunks.size() == 2);
            REQUIRE( writtenChunks[0] == expected );
            REQUIRE( writtenChunks[1] == String(toWrite8) );
        }       

        SECTION("1 less than buffer limit, then overflow")
        {
            buf.sputn(toWrite8.c_str(), 7);
            expected += String(toWrite8.c_str(), 7);
                        
            // now trigger the overflow. The first char should still be
            // added to the buffer, the following 7 are tail data
            buf.sputn(toWrite8.c_str(), 8);
            expected += toWrite8[0];
            
            // this should trigger an overflow. All decodable data previously
            // in the buffer should be flushed, then the new 8 characters
            // should be buffered.

            REQUIRE( writtenChunks.size() == 1);
            REQUIRE( writtenChunks[0] == expected );

            // now do a sync - this should flush the tail data
            buf.pubsync();

            REQUIRE( writtenChunks.size() == 2);
            REQUIRE( writtenChunks[0] == expected );
            REQUIRE( writtenChunks[1] == String(toWrite8.c_str()+1) );
        }
    }

    SECTION("pubsync before buffer is filled")
    {
        String expected;
        for(int i=0;i<8;i++)
        {
            buf.sputc( 'a' + i);
            expected += 'a' + i;
        }

        // should not have been flushed yet
        REQUIRE( writtenChunks.size() == 0);

        int syncResult = buf.pubsync();
        REQUIRE( syncResult == 0);

        REQUIRE( writtenChunks.size() == 1);
        REQUIRE( writtenChunks[0] == expected );
    }

    if( sizeof(CharType)==1 && !multiByteIsUtf8)
    {
        // we should skip the multibyte encoded data tests here, because
        // we cannot be certain that the multibyte encoding is Utf8.

        // So, do nothing
    }
    else
    {
        SECTION("encoded data tests")
        {
            std::list< TextSinkStreamBufSubTestData_<CharType> > subTestDataList;
            initTextSinkStreamBufSubTestData<CharType>(subTestDataList);
        
            int subTestIndex=-1;
	        for(auto& subTestData: subTestDataList)
	        {
                subTestIndex++;

		        SECTION( subTestData.desc )
		        {
                    SECTION("sputn")
                    {
                        buf.sputn( subTestData.encoded.c_str(), subTestData.encoded.length() );
                        buf.pubsync();

                        if(subTestData.encoded.length()==0)
                        {
                            REQUIRE( writtenChunks.size() == 0);
                        }
                        else
                        {
                            REQUIRE( writtenChunks.size() == 1);
                            REQUIRE( writtenChunks[0] == subTestData.expectedDecoded );
                        }
                    }

                    SECTION("sputc with syncs")
                    {
                        for( int charIndex=0; charIndex<(int)subTestData.encoded.length(); charIndex++)
                        {
                            buf.sputc( subTestData.encoded[charIndex] );
                            buf.pubsync();

                            if(charIndex<(int)subTestData.encoded.length()-1 && subTestData.expectedDecoded.length()==1)
                            {
                                // The encoded data represents a single character. So we should not get any
                                // output before the last element is written.
                                REQUIRE( writtenChunks.size() == 0);
                            }
                        }

                        bool anyErrorsExpected = false;
                        for(auto c: subTestData.expectedDecoded)
                        {
                            if(c==0xfffd)
                                anyErrorsExpected = true;
                        }

                        if(anyErrorsExpected)
                        {
                            // invalid data can prevent syncs from writing data (since the decoder does not
                            // know if future data might make it valid).
                            // So if we have expected errors then we only test that the total output data
                            // matches the expected data, rather than verifying each written chunk individually.

                            String written;
                            for(auto& chunk: writtenChunks)
                                written += chunk;

                            REQUIRE( written==subTestData.expectedDecoded );
                        }
                        else
                        {                        
                            // each decoded character should have been written individually.
                            REQUIRE( writtenChunks.size() == subTestData.expectedDecoded.length() );

                            for(int decodedCharIndex=0; decodedCharIndex<(int)subTestData.expectedDecoded.length(); decodedCharIndex++)
                            {   
                                REQUIRE( writtenChunks[decodedCharIndex] == subTestData.expectedDecoded.substr(decodedCharIndex, 1) );
                            }
                        }
                    }

                    if(subTestData.encoded.length()>=2
                        && subTestData.expectedDecoded.length()==1 )
                    {                    
                        SECTION("almost filled buffer - encoded char does not fit")
                        {
                            String expected;

                            // 63 bytes in buffer already
                            for( int i=0; i<63; i++)
                            {
                                buf.sputc( 'a'+ (i % 8) );
                                expected += 'a'+ (i % 8);
                            }

                            // then write the encoded data - note that this is always
                            // at least 2 encoded elements long (see if statement above)
                            buf.sputn( subTestData.encoded.c_str(), subTestData.encoded.length() );
                        
                            // this should have triggered an overflow and flush
                            REQUIRE( writtenChunks.size() == 1);

                            // We only run this test if the encoded data represents a single character
                            // (see if-statement above).
                            // the written data should not include any part of the last character
                            // since that one was incomplete
                            REQUIRE( writtenChunks[0] == expected );

                            // flush explicitly
                            buf.pubsync();

                            // now the utf8 character should have been written
                            REQUIRE( writtenChunks.size() == 2);
                            REQUIRE( writtenChunks[0] == expected );
                            REQUIRE( writtenChunks[1] == subTestData.expectedDecoded );
                        }
                    }
                }
            }
        }
    }
}


template<class CharType>
static void testTextSinkStreamBuf()
{
    P< bdn::test::MockTextSink> pSink = newObj<bdn::test::MockTextSink>();

    TextSinkStdStreamBuf< CharType > buf(pSink);

    SECTION("default locale")
    {
        testTextSinkStreamBuf_Preinitialized(pSink, buf, false);
    }

    SECTION("utf8 locale")
    {
        std::locale loc = deriveUtf8Locale(std::locale::classic() );
        
        buf.pubimbue(loc);

        testTextSinkStreamBuf_Preinitialized(pSink, buf, true);
    }
}


TEST_CASE("TextSinkStdStreamBuf")
{
    SECTION("char")
        testTextSinkStreamBuf<char>();

    SECTION("wchar_t")
        testTextSinkStreamBuf<wchar_t>();

    SECTION("char16_t")
        testTextSinkStreamBuf<char16_t>();

    SECTION("char32_t")
        testTextSinkStreamBuf<char32_t>();
}





