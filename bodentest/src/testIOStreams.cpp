#include <bdn/init.h>
#include <bdn/test.h>

#include <codecvt>

using namespace bdn;


String getStreamString( std::ostringstream& stream)
{
	return String::fromLocaleEncoding( stream.str(), stream.getloc());	
}

String getStreamString( std::wostringstream& stream)
{
	return String( stream.str() );
}

template<class StreamType, class StringType>
void writeStringToStream(StreamType& stream, StringType in )
{
	stream << in;	
}

template<>
void writeStringToStream<std::ostringstream, std::string>(std::ostringstream& stream, std::string in )
{
	// this is a special case. The stream will assume that a std::string is in its locale encoding, but ours is in UTF-8.
	// We need to pass it in in locale encoding.
	stream << String(in).toLocaleEncoding( stream.getloc() );	
}

template<>
void writeStringToStream<std::ostringstream, const char*>(std::ostringstream& stream, const char* in )
{
	// this is a special case. The stream will assume that a const char* is in its locale encoding, but ours is in UTF-8.
	// We need to pass it in in locale encoding.
	stream << String(in).toLocaleEncoding( stream.getloc() );	
}

template<class StreamType, class StringType>
inline void testStreamStringOutput(bool useUtf8Locale)
{
	String			inObj(U"he\U00002345llo");
	StringType		in = (StringType)inObj;

	StreamType		stream;

	if(useUtf8Locale)
	{
		std::locale utf8Locale( std::locale(), new std::codecvt_utf8<wchar_t> );

		stream.imbue(utf8Locale);	
	}

	writeStringToStream(stream, in);
	
	String result = getStreamString(stream);

	if(useUtf8Locale)
	{
		// must match exactly
		REQUIRE( result == inObj  );
	}
	else
	{
		// the non-ascii char may have been replaced with a unicode replacement character or a question mark.
				
		REQUIRE( (result==inObj || result==U"he\xfffdllo" || result=="he?llo") );
	}	
}


template<class StreamType, class StringType>
inline void testStreamStringOutput()
{
	SECTION("utf8Encoding")
		testStreamStringOutput<StreamType, StringType>(true);

	SECTION("defaultEncoding")
		testStreamStringOutput<StreamType, StringType>(false);
}

template<class StreamType>
inline void testStreamOutput()
{
	SECTION("String")
		testStreamStringOutput<StreamType, String>();

	SECTION("std::string")
		testStreamStringOutput<StreamType, std::string>();

	SECTION("std::wstring")
		testStreamStringOutput<StreamType, std::wstring>();

	SECTION("std::u16string")
		testStreamStringOutput<StreamType, std::u16string>();

	SECTION("std::u32string")
		testStreamStringOutput<StreamType, std::u32string>();


	SECTION("const CharT*")
		testStreamStringOutput<StreamType, const typename StreamType::char_type*>();
}

TEST_CASE("std stream string output")
{
	SECTION("ostringstream")
		testStreamOutput<std::ostringstream>();

	SECTION("wostringstream")
		testStreamOutput<std::wostringstream>();	
}