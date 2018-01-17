#include <bdn/init.h>

#include <bdn/StreamFormat.h>
#include <bdn/TextOutStream.h>

#include <bdn/test.h>

using namespace bdn;


template<typename STREAM_TYPE, typename FORMAT_REF, typename GET_STRING_FUNC>
static void _verifyAutoFloat( STREAM_TYPE& stream, FORMAT_REF&& format, GET_STRING_FUNC getStringFunc )
{
	SECTION("123456.125")
	{
		stream << format << 123456.125;
		REQUIRE( getStringFunc() == "123456" );
	}

	SECTION("-123456.125")
	{
		stream << format << -123456.125;
		REQUIRE( getStringFunc() == "-123456" );
	}

	SECTION("1234567.125")
	{
		stream << format << 1234567.125;
		REQUIRE( getStringFunc() == "1.23457e+06" );
	}

	SECTION("-1234567.125")
	{
		stream << format << -1234567.125;
		REQUIRE( getStringFunc() == "-1.23457e+06" );
	}
}

template<typename FORMAT_REF, typename CHAR_TYPE, typename STREAM_TYPE = std::basic_ostream<CHAR_TYPE, std::char_traits<CHAR_TYPE> > >
static void _testStreamFormatEffect( FORMAT_REF&& format )
{
	std::basic_stringbuf<CHAR_TYPE, typename STREAM_TYPE::traits_type>	streamBuffer;
	STREAM_TYPE	stream(&streamBuffer);

	stream.imbue( std::locale::classic() );

	auto flagsBefore = stream.flags();
	auto widthBefore = stream.width();
	auto precisionBefore = stream.precision();
	auto fillCharBefore = stream.fill();

	auto getString = [&streamBuffer]()
		{
			return String( streamBuffer.str().c_str() );
		};

	SECTION("nothing")
	{
		stream << std::forward<FORMAT_REF>(format) << 0x1234;

		REQUIRE( getString() == "4660" );
	}

	SECTION("hex")
	{
		stream << format.hex() << 0x1234;

		REQUIRE( getString() == "1234" );
	}

	SECTION("hex0x")
	{
		stream << format.hex0x() << 0x1234;

		REQUIRE( getString() == "0x1234" );
	}

	SECTION("zeroPad")
	{
		SECTION("dec")
		{
			stream << format.zeroPad(8) << 0x1234;
			REQUIRE( getString() == "00004660" );
		}

		SECTION("negative dec")
		{
			stream << format.zeroPad(8) << -0x1234;
			REQUIRE( getString() == "-0004660" );
		}

		SECTION("hex0x")
		{
			stream << format.hex0x().zeroPad(8) << 0x1234;
			REQUIRE( getString() == "0x001234" );
		}

		SECTION("negative hex0x")
		{
			stream << format.hex0x().zeroPad(8) << (int32_t)-4660;
			// hex numbers are never negative
			REQUIRE( getString() == "0xffffedcc" );
		}

		SECTION("string")
		{
			stream << format.zeroPad(8) << String("hello");
			REQUIRE( getString() == "000hello" );
		}
	}

	SECTION("alignRight")
	{
		SECTION("dec")
		{
			stream << format.alignRight(8) << 0x1234;
			REQUIRE( getString() == "    4660" );
		}

		SECTION("negative dec")
		{
			stream << format.alignRight(8) << -4660;
			REQUIRE( getString() == "   -4660" );
		}

		SECTION("hex0x")
		{
			stream << format.hex0x().alignRight(8) << 0x1234;
			REQUIRE( getString() == "  0x1234" );
		}

		SECTION("negative hex0x")
		{
			stream << format.hex0x().alignRight(8) << (int32_t)-4660;
			// hex numbers are never negative
			REQUIRE( getString() == "0xffffedcc" );
		}

		SECTION("String")
		{
			stream << format.alignRight(8) << String("hello");
			REQUIRE( getString() == "   hello" );
		}

		SECTION("const CHAR_TYPE*")
		{
			std::basic_string<CHAR_TYPE> s = String("hello");

			stream << format.alignRight(8) << s.c_str();
			REQUIRE( getString() == "   hello" );
		}

		SECTION("basic_string")
		{
			std::basic_string<CHAR_TYPE> s = String("hello");

			stream << format.alignRight(8) << s;
			REQUIRE( getString() == "   hello" );
		}
	}

	SECTION("alignLeft")
	{
		SECTION("dec")
		{
			stream << format.alignLeft(8) << 0x1234;
			REQUIRE( getString() == "4660    " );
		}

		SECTION("negative dec")
		{
			stream << format.alignLeft(8) << -0x1234;
			REQUIRE( getString() == "-4660   " );
		}

		SECTION("hex0x")
		{
			stream << format.hex0x().alignLeft(8) << 0x1234;
			REQUIRE( getString() == "0x1234  " );
		}

		SECTION("negative hex0x")
		{
			stream << format.hex0x().alignLeft(8) << (int32_t)-4660;
			// hex numbers are never negative
			REQUIRE( getString() == "0xffffedcc" );
		}

		SECTION("string")
		{
			stream << format.alignLeft(8) << String("hello");
			REQUIRE( getString() == "hello   " );
		}
	}

	SECTION("float format")
	{
		SECTION("auto")
		{
			SECTION("no call")
				_verifyAutoFloat( stream, std::forward<FORMAT_REF>(format), getString );

			SECTION("autoFloat called")
			{
				format.autoFloat(6);
				_verifyAutoFloat( stream, std::forward<FORMAT_REF>(format), getString );
			}

			SECTION("autoFloat after scientificFloat")
			{
				format.scientificFloat(9).autoFloat(6);
				_verifyAutoFloat( stream, std::forward<FORMAT_REF>(format), getString );
			}
			
		}

		SECTION("scientificFloat")
		{
			SECTION("123")
			{
				SECTION("default digits")
				{
					stream << format.scientificFloat() << 123.0;
					REQUIRE( getString() == "1.230000e+02" );
				}

				SECTION("explicit digits")
				{
					stream << format.scientificFloat(3) << 123.0;
					REQUIRE( getString() == "1.230e+02" );
				}
			}

			SECTION("123456.125")
			{
				SECTION("default digits")
				{
					stream << format.scientificFloat() << 123456.125;
					REQUIRE( getString() == "1.234561e+05" );
				}

				SECTION("3 digits")
				{
					stream << format.scientificFloat(3) << 123456.125;
					REQUIRE( getString() == "1.235e+05" );
				}
			}

			SECTION("-123456.125")
			{
				stream << format.scientificFloat() << -123456.125;
				REQUIRE( getString() == "-1.234561e+05" );
			}

			SECTION("1234567.125")
			{
				stream << format.scientificFloat() << 1234567.125;
				REQUIRE( getString() == "1.234567e+06" );
			}

			SECTION("-1234567.125")
			{
				stream << format.scientificFloat() << -1234567.125;
				REQUIRE( getString() == "-1.234567e+06" );
			}
		}

		SECTION("simple")
		{
			SECTION("123456.125")
			{
				SECTION("default digits")
				{
					stream << format.simpleFloat() << 123456.126;
					REQUIRE( getString() == "123456.13" );
				}

				SECTION("6 digits")
				{
					stream << format.simpleFloat(6) << 123456.125;
					REQUIRE( getString() == "123456.125000" );
				}
			}

			SECTION("-123456.125")
			{
				SECTION("default digits")
				{
					stream << format.simpleFloat() << -123456.126;
					REQUIRE( getString() == "-123456.13" );
				}

				SECTION("6 digits")
				{
					stream << format.simpleFloat(6) << -123456.125;
					REQUIRE( getString() == "-123456.125000" );
				}
			}

			SECTION("zero digits")
			{
				SECTION("1234567.125")
				{
					stream << format.simpleFloat(0) << 1234567.125;
					REQUIRE( getString() == "1234567" );
				}

				SECTION("-1234567.125")
				{
					stream << format.simpleFloat(0) << -1234567.125;
					REQUIRE( getString() == "-1234567" );
				}

				SECTION("force point")
				{
					SECTION("1234567.125")
					{
						stream << format.simpleFloat(0).forcePoint() << 1234567.125;
						REQUIRE( getString() == "1234567." );
					}

					SECTION("-1234567.125")
					{
						stream << format.simpleFloat(0).forcePoint() << -1234567.125;
						REQUIRE( getString() == "-1234567." );
					}

				}
			}
		}
	}

	SECTION("on ad hoc stream")
	{
        // XXX disabled. Does not work currently because the
        // generic standard library << operator for converting rvalue stream
        // references to normal references does not support a custom object
        // to be returned (always returns a stream reference). So this does not
        // work with StreamFormat.
        // To solve this we probably have to refactor StreamFormat and wrap the parameters
        // to format in custom objects. See https://systems.atlassian.net/browse/BDN-57
		// REQUIRE( (StringBuffer() << StreamFormat().alignRight(8) << "hello") == "   hello" );
	}


	// all formatting should have been undone again
	REQUIRE( stream.flags() == flagsBefore );
	REQUIRE( stream.width() == widthBefore );
	REQUIRE( stream.precision() == precisionBefore );
	REQUIRE( stream.fill() == fillCharBefore );
}

template<typename FORMAT_REF>
static void _testStreamFormat( FORMAT_REF&& format, bool expectedRValue )
{
	// verify that the test setup is as expected
	bool isRValue = std::is_rvalue_reference< decltype(format) >::value;
	REQUIRE( isRValue == expectedRValue );

	SECTION("initial state")
	{
		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == 0);
		REQUIRE( format.flagsMask() == 0);
	}

	SECTION("hex")
	{
		format.hex();

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::hex);
		REQUIRE( format.flagsMask() == std::ios_base::basefield);
	}

	SECTION("hex0x")
	{
		format.hex0x();

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == (std::ios_base::hex | std::ios_base::showbase) );
		REQUIRE( format.flagsMask() == (std::ios_base::basefield | std::ios_base::showbase) );
	}

	SECTION("zeroPad")
	{
		format.zeroPad(8);

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == 8 );
		REQUIRE( format.padChar() == '0' );
		REQUIRE( format.flags() == std::ios_base::internal);
		REQUIRE( format.flagsMask() == std::ios_base::adjustfield );
	}

	SECTION("alignRight")
	{
		format.alignRight(8);

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == 8 );
		REQUIRE( format.padChar() == ' ' );
		REQUIRE( format.flags() == std::ios_base::right);
		REQUIRE( format.flagsMask() == std::ios_base::adjustfield );
	}

	SECTION("alignLeft")
	{
		format.alignLeft(8);

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == 8 );
		REQUIRE( format.padChar() == ' ' );
		REQUIRE( format.flags() == std::ios_base::left);
		REQUIRE( format.flagsMask() == std::ios_base::adjustfield );
	}

	SECTION("scientificFloat default digits")
	{
		format.scientificFloat();

		REQUIRE( format.precision() == 6 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::scientific);
		REQUIRE( format.flagsMask() == std::ios_base::floatfield );
	}

	SECTION("scientificFloat explicit digits")
	{
		format.scientificFloat(7);

		REQUIRE( format.precision() == 7 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::scientific);
		REQUIRE( format.flagsMask() == std::ios_base::floatfield );
	}

	SECTION("simpleFloat default digits")
	{
		format.simpleFloat();

		REQUIRE( format.precision() == 2 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::fixed);
		REQUIRE( format.flagsMask() == std::ios_base::floatfield );
	}

	SECTION("simpleFloat explicit digits")
	{
		format.simpleFloat(7);

		REQUIRE( format.precision() == 7 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::fixed);
		REQUIRE( format.flagsMask() == std::ios_base::floatfield );
	}

	SECTION("precision")
	{
		format.setPrecision(7);

		REQUIRE( format.precision() == 7 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == 0);
		REQUIRE( format.flagsMask() == 0);
	}

	SECTION("forcePoint")
	{
		format.forcePoint();

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == std::ios_base::showpoint);
		REQUIRE( format.flagsMask() == std::ios_base::showpoint);
	}

	SECTION("setPadTo")
	{
		format.setPadTo(5);

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == 5 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == 0);
		REQUIRE( format.flagsMask() == 0);
	}

	SECTION("setPadChar")
	{
		format.setPadChar('x');

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == 'x' );
		REQUIRE( format.flags() == 0);
		REQUIRE( format.flagsMask() == 0);
	}

	SECTION("setFlags")
	{
		format.setFlags( (std::ios_base::fmtflags)0x12345678, (std::ios_base::fmtflags)0x76543210 );

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == 0x12345678 );
		REQUIRE( format.flagsMask() == 0x76543210 );
	}

	SECTION("setFlag")
	{
		format.setFlag( (std::ios_base::fmtflags)0x12345678 );

		REQUIRE( format.precision() == -1 );
		REQUIRE( format.padTo() == -1 );
		REQUIRE( format.padChar() == StreamFormat::padCharNotSet );
		REQUIRE( format.flags() == 0x12345678 );
		REQUIRE( format.flagsMask() == 0x12345678 );
	}

	SECTION("effect")
	{
		SECTION("char")
			_testStreamFormatEffect<FORMAT_REF, char >( std::forward<FORMAT_REF>(format) );

		SECTION("wchar_t")
			_testStreamFormatEffect<FORMAT_REF, wchar_t >( std::forward<FORMAT_REF>(format) );

		SECTION("char32_t")
		{
			SECTION("basic_ostream")
				_testStreamFormatEffect<FORMAT_REF, char32_t, std::basic_ostream<char32_t, UnicodeCharTraits> >( std::forward<FORMAT_REF>(format) );

			SECTION("TextOutStream")
				_testStreamFormatEffect<FORMAT_REF, char32_t, TextOutStream >( std::forward<FORMAT_REF>(format) );
		}
	}

}

TEST_CASE("StreamFormat")
{
	SECTION("temporary")
	{
		_testStreamFormat( StreamFormat(), true );
	}

	SECTION("local")
	{
		StreamFormat format;
		_testStreamFormat(format, false);
	}
}


