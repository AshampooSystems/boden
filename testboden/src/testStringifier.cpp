#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Stringifier.h>

using namespace bdn;
using namespace bdn::test;

class StringifierTestDummyBaseClass_
{
};

class StringifierTestObjectWithTag_ : public StringifierTestDummyBaseClass_, public ToStringTag
{
public:

	String toString() const
	{
		return "test";
	}
};

class StringifierTestObjectWithBaseClassWithTag_ : public StringifierTestObjectWithTag_
{
public:
};

class StringifierTestObjectWithoutTagButWithToStringMethod_
{
public:

	String toString() const
	{
		return "test";
	}
};


class StringifierTestObjectWithoutTag_
{
public:

};


template<typename VALUE_TYPE>
void _verifyStringify( VALUE_TYPE value, const String& expectedString, bool expectedIsPrefixOnly = false )
{
	String actualString;

	SECTION("Stringifier")
		actualString = Stringifier< typename std::decay<VALUE_TYPE>::type >()( std::forward<VALUE_TYPE>(value) );

	SECTION("toString")
		actualString = toString( std::forward<VALUE_TYPE>(value) );

	if( expectedIsPrefixOnly )
		REQUIRE( actualString.startsWith(expectedString) );
	else
		REQUIRE( actualString == expectedString );
}

template<class CLS>
void _verifyStringifyUnknownObject()
{
	CLS obj;

	String address = toString((void*)&obj);
	REQUIRE( address.getLength()>=3 );
	REQUIRE( address.startsWith("0x") );

	String expectedPrefix = String("<") + typeid(CLS).name() + " @ ";
	String expectedWithAddress = expectedPrefix + address+">";

	SECTION("value")	
	{
		// note that since we pass a copy of the object, the address
		// will be different. So we match only the prefix
		_verifyStringify<CLS>(obj, expectedPrefix, true);
	}

	SECTION("const value")	
	{
		// note that since we pass a copy of the object, the address
		// will be different. So we match only the prefix
		_verifyStringify<const CLS>(obj, expectedPrefix, true);
	}

	SECTION("reference")	
		_verifyStringify<CLS&>(obj, expectedWithAddress);

	SECTION("const reference")	
		_verifyStringify<const CLS&>(obj, expectedWithAddress);
}

void _testStringifyUnknownObject()
{
	SECTION("with toString method")
		_verifyStringifyUnknownObject< StringifierTestObjectWithoutTagButWithToStringMethod_ >();

	SECTION("without toString method")
		_verifyStringifyUnknownObject< StringifierTestObjectWithoutTag_ >();
}

template<class CLS>
void _verifyStringifyUnknownObjectWithTag(CLS& obj)
{
	SECTION("value")
		_verifyStringify<CLS>( obj, "test" );

	SECTION("const value")
		_verifyStringify<const CLS>( obj, "test" );

	SECTION("reference")
		_verifyStringify<CLS&>( obj, "test" );

	SECTION("const reference")
		_verifyStringify<const CLS&>( obj, "test" );
}

void _testStringifyUnknownObjectWithTag()
{
	SECTION("tag on object directly")
	{
		SECTION("existing object")
		{
			StringifierTestObjectWithTag_ obj;
	
			_verifyStringify(obj, "test");
		}

		SECTION("ad hoc object")
			_verifyStringify(StringifierTestObjectWithTag_(), "test");
	}

	SECTION("tag on base class")
	{
		SECTION("existing object")
		{
			StringifierTestObjectWithBaseClassWithTag_ obj;
	
			_verifyStringify(obj, "test");
		}

		SECTION("ad hoc object")
			_verifyStringify(StringifierTestObjectWithBaseClassWithTag_(), "test");
	}
}

template<typename CHAR_TYPE>
void _verifyStringifyChar()
{
	typename std::decay<CHAR_TYPE>::type chr = 'X';

	SECTION("value")
		_verifyStringify<CHAR_TYPE>( chr, "X" );

	SECTION("const value")
		_verifyStringify<const CHAR_TYPE>( chr, "X" );

	SECTION("reference")
		_verifyStringify<CHAR_TYPE&>( chr, "X" );

	SECTION("const reference")
		_verifyStringify<const CHAR_TYPE&>( chr, "X" );
}		

void _testStringifyChars()
{
	SECTION("char")
		_verifyStringifyChar<char>();
	
	SECTION("wchar_t")
		_verifyStringifyChar<wchar_t>();

	SECTION("char16_t")
		_verifyStringifyChar<char16_t>();

	SECTION("char32_t")
		_verifyStringifyChar<char32_t>();
}

template< typename INT_TYPE >
void _verifyStringifyIntWithExactType()
{
	typename std::decay<INT_TYPE>::type val;

	SECTION("0")
	{
		val = 0;
		_verifyStringify<INT_TYPE>( val, "0" );
	}

	SECTION("max")
	{
		val = Number< decltype(val) >::maxValue();

		String expected = std::to_string( val );
		_verifyStringify<INT_TYPE>( val, expected );
	}

	SECTION("min")
	{
		val = Number< decltype(val) >::minValue();

		String expected = std::to_string( val );
		_verifyStringify<INT_TYPE>( val, expected );
	}
}

template< typename INT_TYPE >
void _verifyStringifyInt()
{
	SECTION("value")
		_verifyStringifyIntWithExactType<INT_TYPE>();

	SECTION("const value")
		_verifyStringifyIntWithExactType<const INT_TYPE>();

	SECTION("reference")
		_verifyStringifyIntWithExactType<INT_TYPE&>();

	SECTION("const reference")
		_verifyStringifyIntWithExactType<const INT_TYPE&>();
}

void _testStringifyIntegers()
{
	SECTION("int8_t")
		_verifyStringifyInt<int8_t>();

	SECTION("uint8_t")
		_verifyStringifyInt<uint8_t>();

	SECTION("int16_t")
		_verifyStringifyInt<int16_t>();

	SECTION("uint16_t")
		_verifyStringifyInt<uint16_t>();

	SECTION("int32_t")
		_verifyStringifyInt<int32_t>();

	SECTION("uint32_t")
		_verifyStringifyInt<uint32_t>();
	
	SECTION("int64_t")
		_verifyStringifyInt<int64_t>();

	SECTION("uint64_t")
		_verifyStringifyInt<uint64_t>();

	SECTION("int")
		_verifyStringifyInt<int>();

	SECTION("unsigned")
		_verifyStringifyInt<unsigned>();

	SECTION("long")
		_verifyStringifyInt<long>();

	SECTION("unsigned long")
		_verifyStringifyInt<unsigned long>();

	SECTION("long long")
		_verifyStringifyInt<long long>();

	SECTION("unsigned long")
		_verifyStringifyInt<unsigned long long>();

	SECTION("intptr_t")
		_verifyStringifyInt<intptr_t>();

	SECTION("uintptr_t")
		_verifyStringifyInt<uintptr_t>();

	SECTION("size_t")
		_verifyStringifyInt<size_t>();
}


template<typename BOOL_TYPE>
void _verifyStringifyBoolWithExactType()
{
	bool val;

	SECTION("true")
	{
		val = true;
		_verifyStringify<BOOL_TYPE>( val, "true");
	}

	SECTION("false")
	{
		val = false;
		_verifyStringify<BOOL_TYPE>( val, "false");
	}
}

void _testStringifyBool()
{
	SECTION("value")
		_verifyStringifyBoolWithExactType<bool>();

	SECTION("const value")
		_verifyStringifyBoolWithExactType<const bool>();

	SECTION("reference")
		_verifyStringifyBoolWithExactType<bool&>();

	SECTION("const reference")
		_verifyStringifyBoolWithExactType<const bool&>();
}

void _testStringifyNullptr()
{
	nullptr_t p = nullptr;

	SECTION("value")
		_verifyStringify<nullptr_t>( p, "nullptr");

	SECTION("const value")
		_verifyStringify<const nullptr_t>( p, "nullptr");

	SECTION("reference")
		_verifyStringify<nullptr_t&>( p, "nullptr");

	SECTION("const reference")
		_verifyStringify<const nullptr_t&>( p, "nullptr");
}
	

template< typename FLOAT_TYPE >
void _verifyStringifyFloatingPointVariants(FLOAT_TYPE val, const String& expected)
{	
	SECTION("value")
		_verifyStringify<FLOAT_TYPE>( val, expected );

	SECTION("const value")
		_verifyStringify<const FLOAT_TYPE>( val, expected );

	SECTION("reference")
		_verifyStringify<FLOAT_TYPE&>( val, expected );

	SECTION("const reference")
		_verifyStringify<const FLOAT_TYPE&>( val, expected );
}


template< typename FLOAT_TYPE >
void _verifyStringifyFloatingPoint()
{
	SECTION("0")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( 0, "0" );

	SECTION("1")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( 1, "1" );

	SECTION("-1")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( -1, "-1" );

	SECTION("0.5")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( 0.5, "0.5" );

	SECTION("-0.5")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( -0.5, "-0.5" );

	SECTION("0.125")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( 0.125, "0.125" );

	SECTION("-0.125")
		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( -0.125, "-0.125" );

	SECTION("max")
	{
		FLOAT_TYPE val = Number<FLOAT_TYPE>::maxValue();

		std::stringstream strm;
		strm.imbue( std::locale::classic() );
		strm << val;

		String expected = strm.str();
		// sanity check
		REQUIRE( expected.length()>=8 );
		REQUIRE( expected.contains(".") );
		REQUIRE( expected.contains("e") );

		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( val, expected );
	}

	SECTION("min")
	{
		FLOAT_TYPE val = Number<FLOAT_TYPE>::minValue();

		std::stringstream strm;
		strm.imbue( std::locale::classic() );
		strm << val;

		String expected = strm.str();
		// sanity check
		REQUIRE( expected.length()>=8 );
		REQUIRE( expected.contains(".") );
		REQUIRE( expected.contains("e") );
		REQUIRE( expected.startsWith("-") );

		_verifyStringifyFloatingPointVariants<FLOAT_TYPE>( val, expected );
	}
}

void _testStringifyFloatingPoint()
{
	SECTION("float")
		_verifyStringifyFloatingPoint<float>();

	SECTION("double")
		_verifyStringifyFloatingPoint<double>();

	SECTION("long double")
		_verifyStringifyFloatingPoint<long double>();
}


template<typename STRING_TYPE>
STRING_TYPE makeStringOfType( const String& val)
{
	return static_cast<STRING_TYPE>( val );
}

template<>
char* makeStringOfType< char* >( const String& val)
{
	const char* s = makeStringOfType<const char*>(val);
	return const_cast<char*>(s);
}

template<>
wchar_t* makeStringOfType< wchar_t* >( const String& val)
{
	const wchar_t* s = makeStringOfType<const wchar_t*>(val);
	return const_cast<wchar_t*>(s);
}

template<>
char16_t* makeStringOfType< char16_t* >( const String& val)
{
	const char16_t* s = makeStringOfType<const char16_t*>(val);
	return const_cast<char16_t*>(s);
}

template<>
char32_t* makeStringOfType< char32_t* >( const String& val)
{
	const char32_t* s = makeStringOfType<const char32_t*>(val);
	return const_cast<char32_t*>(s);
}


template<>
StringImpl<Utf8StringData> makeStringOfType< StringImpl<Utf8StringData> >( const String& val)
{
	return StringImpl<Utf8StringData>( val.begin(), val.end() );	
}

template<>
StringImpl<Utf16StringData> makeStringOfType< StringImpl<Utf16StringData> >( const String& val)
{
	return StringImpl<Utf16StringData>( val.begin(), val.end() );	
}

template<>
StringImpl<Utf32StringData> makeStringOfType< StringImpl<Utf32StringData> >( const String& val)
{
	return StringImpl<Utf32StringData>( val.begin(), val.end() );	
}

template<>
StringImpl<WideStringData> makeStringOfType< StringImpl<WideStringData> >( const String& val)
{
	return StringImpl<WideStringData>( val.begin(), val.end() );	
}

template<class STRING_TYPE>
void _verifyStringifyStringValue(const String& val)
{
	STRING_TYPE s = makeStringOfType<STRING_TYPE>( val );

	SECTION("value")
		_verifyStringify<STRING_TYPE>( s, val );

	SECTION("const value")
		_verifyStringify<const STRING_TYPE>( s, val );

	SECTION("reference")
		_verifyStringify<STRING_TYPE&>( s, val );

	SECTION("const reference")
		_verifyStringify<const STRING_TYPE&>( s, val );
}


template<class STRING_CLASS>
void _verifyStringifyString()
{
	SECTION("empty")
		_verifyStringifyStringValue<STRING_CLASS>("");

	SECTION("ascii")
		_verifyStringifyStringValue<STRING_CLASS>("hello");

	SECTION("unicode")
		_verifyStringifyStringValue<STRING_CLASS>(U"hel\U00012345lo");
}


void _testStringifyString()
{
	SECTION("String")
		_verifyStringifyString<String>();

	// note: this one is actually the same as String, since String is an
	// alias for StringImpl<NativeStringData>. But we add an explicit
	// test for it anyway, in case the alias changes in the future
	SECTION("StringImpl<NativeStringData>")
		_verifyStringifyString< StringImpl<NativeStringData> >();

	SECTION("StringImpl<Utf8StringData>")
		_verifyStringifyString< StringImpl<Utf8StringData> >();

	SECTION("StringImpl<Utf16StringData>")
		_verifyStringifyString< StringImpl<Utf16StringData> >();

	SECTION("StringImpl<Utf32StringData>")
		_verifyStringifyString< StringImpl<Utf32StringData> >();

	SECTION("StringImpl<WideStringData>")
		_verifyStringifyString< StringImpl<WideStringData> >();

	

	SECTION("std::string")
		_verifyStringifyString<std::string>();

	SECTION("std::wstring")
		_verifyStringifyString<std::wstring>();

	SECTION("std::u16string")
		_verifyStringifyString<std::u16string>();

	SECTION("std::u32string")
		_verifyStringifyString<std::u32string>();

	SECTION("const char*")
		_verifyStringifyString<const char*>();

	SECTION("const wchar_t*")
		_verifyStringifyString<const wchar_t*>();

	SECTION("const char16_t*")
		_verifyStringifyString<const char16_t*>();

	SECTION("const char32_t*")
		_verifyStringifyString<const char32_t*>();

	SECTION("char*")
		_verifyStringifyString<char*>();

	SECTION("wchar_t*")
		_verifyStringifyString<wchar_t*>();

	SECTION("char16_t*")
		_verifyStringifyString<char16_t*>();

	SECTION("char32_t*")
		_verifyStringifyString<char32_t*>();
}


template<typename POINTER_TYPE>
void _verifyStringifyPointerVariants(POINTER_TYPE val, const String& expected)
{
	SECTION("value")
		_verifyStringify<POINTER_TYPE>( val, expected );

	SECTION("const value")
		_verifyStringify<const POINTER_TYPE>( val, expected );

	SECTION("reference")
		_verifyStringify<POINTER_TYPE&>( val, expected );

	SECTION("const reference")
		_verifyStringify<const POINTER_TYPE&>( val, expected );	
}

template<typename POINTER_TYPE>
void _verifyStringifyPointer()
{
	SECTION("null")
		_verifyStringifyPointerVariants<POINTER_TYPE>(nullptr, "0x0");

	SECTION("non-null")
		_verifyStringifyPointerVariants<POINTER_TYPE>((POINTER_TYPE)0x123456, "0x123456" );

	SECTION("all bits used")
	{
		POINTER_TYPE p = (POINTER_TYPE)( (intptr_t)-1 );

		char expectedBuf[40];

#ifdef _MSC_VER
		// disable the "sprintf is unsafe" warning
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

		sprintf( expectedBuf, "0x%p", p);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

		for(int i=0; expectedBuf[i]!=0; i++)
			expectedBuf[i] = tolower(expectedBuf[i]);

		String expected(expectedBuf);		

		_verifyStringifyPointerVariants<POINTER_TYPE>( p, expected );
	}
}

void _testStringifyPointer()
{
	SECTION("void*")
		_verifyStringifyPointer<void*>();

	SECTION("const void*")
		_verifyStringifyPointer<const void*>();

	SECTION("uint8_t*")
		_verifyStringifyPointer<uint8_t*>();

	SECTION("const uint8_t*")
		_verifyStringifyPointer<const uint8_t*>();

	SECTION("String*")
		_verifyStringifyPointer<const String*>();
}

TEST_CASE("Stringifier")
{
	SECTION("chars")
		_testStringifyChars();

	SECTION("integers")
		_testStringifyIntegers();

	SECTION("bool")
		_testStringifyBool();

	SECTION("nullptr")
		_testStringifyNullptr();

	SECTION("floating point")
		_testStringifyFloatingPoint();

	SECTION("string")
		_testStringifyString();

	SECTION("pointer")
		_testStringifyPointer();

	SECTION("unknown object with tag")
		_testStringifyUnknownObjectWithTag();

	SECTION("unknown object")
		_testStringifyUnknownObject();

}
