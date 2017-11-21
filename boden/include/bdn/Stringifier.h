#ifndef BDN_Stringifier_H_
#define BDN_Stringifier_H_

#include <sstream>
#include <cstddef>

namespace bdn
{
    
template<typename InputType>
inline String toString(InputType&& inputValue);
    

template<
	typename INT_TYPE,		
	int BASE = 10,
	char32_t... PREFIX_CHARS >
class IntegerToCharSequence_
{
public:
	IntegerToCharSequence_(INT_TYPE number)
	{
		_startIndex = maxChars-1;

		if(number==0)
		{
			_chars[ _startIndex ] = U'0';
			_startIndex--;
		}
		else
		{
			// note: the division and % operations below also work for negative numbers.
			// The only difference is that the % operation will give us a negative remainder.
			// To handle that we ensure that we have the same digits in both directions.
			const char32_t* pDigits = U"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" + 35;
			bool negative = (number<0);
						
			do
			{
				// note: since we 
				_chars[ _startIndex ] = pDigits[number % BASE];
				number /= BASE;		
					
				_startIndex--;
			}
			while( number!=0 );

			if(negative)
			{
				_chars[ _startIndex ] = U'-';
				_startIndex--;
			}
		}

		_startIndex++;
		_startIndex -= prefixLength;

		storePrefixChars<0, PREFIX_CHARS...>( _chars+_startIndex );
	}

	const char32_t* begin()
	{
		return &_chars[_startIndex];
	}

	const char32_t* end()
	{
		return &_chars[ maxChars ];
	}

private:

	template<int dummy>
	void storePrefixChars(char32_t* pDest)
	{
	}
	
	template<int dummy, char32_t PREFIX_CHAR, char32_t... MORE_PREFIX_CHARS>
	void storePrefixChars(char32_t* pDest)
	{
		*pDest = PREFIX_CHAR;

		storePrefixChars<0, MORE_PREFIX_CHARS...>( pDest+1 );
	}



	template<int dummy>
	constexpr static int calcPrefixLength()
	{
		return 0;
	}
	
	template<int dummy, char32_t PREFIX_CHAR, char32_t... MORE_PREFIX_CHARS>
	constexpr static int calcPrefixLength()
	{
		return calcPrefixLength<0, MORE_PREFIX_CHARS...>() + 1;
	}
	
	enum
	{
		prefixLength = calcPrefixLength<0, PREFIX_CHARS...>()
	};

	constexpr static int calcMaxDigitsPerByte()
	{
		// the correct way to calculate this would be logBASE( 65536 )
		// But we want this to be a constant expression, so that the necessary
		// memory can be allocated on the stack.
		// It is important to note that only a few number of bytes are possible.
		// And it is important to note that it is ok if this number is slightly
		// too big.
		// The extremes are BASE=2 with 16 digits and BASE=65536 with 1 digit.
		// The number of digits only changes at certain points:

		return  BASE<=2 ? 16 :
			(	BASE==3 ? 11 :
			(	BASE==4 ? 8 :
			(   BASE<=6 ? 7 :
			(	BASE<=9 ? 6 :
			(   BASE<=15 ? 5 :
			(   BASE<=40 ? 4 :
			(   BASE<=255 ? 3 :
			(   BASE<=65535 ? 2 :
			(   1 )))))))));
	}

	constexpr static int calcMaxChars()
	{
		// +1 for a possible minus sign
		return prefixLength + 1 + calcMaxDigitsPerByte()*sizeof(INT_TYPE);
	}

	enum
	{
		maxChars = calcMaxChars()
	};

	char32_t	_chars[ maxChars ];
	int			_startIndex;
};



template<class CLS>
struct HasToString_
{
private:
	// the following _hasToStringTest overload takes a pointer to the
	// return type of toString as its argument.
	// Note that this is not valid if the toString method returns void, but
	// that is exactly what we want. A toString method MUST exist and it MUST
	// return a value, otherwise we want HasToString_::value to be 0.
	template<typename TEST_CLS>
	static uint8_t _hasToStringTest( decltype(std::declval<TEST_CLS>().toString())* );

    template <typename TEST_CLS>
	static uint16_t _hasToStringTest(...);

public:
    enum
	{
		value = sizeof( _hasToStringTest<CLS>(nullptr) ) == sizeof(uint8_t) ? 1 : 0
	};


};


struct UnknownObjectStringifier_
{
	template< typename VALUE_TYPE >
	typename std::enable_if<
		HasToString_<VALUE_TYPE>::value,
		String
		>::type
		operator()( VALUE_TYPE&& value ) const
	{
		return value.toString();
	}

	template< typename VALUE_TYPE >
	typename std::enable_if<
		! HasToString_<VALUE_TYPE>::value,
		String
		>::type
		operator()( VALUE_TYPE&& value ) const
	{
		return String("<") + typeid(VALUE_TYPE).name() + " @ "+  toString(&value) + ">";
	}
};


/** A template class that is used to convert values and objects to strings.

	Also see the global function toString() for a simpler to use alternative.

	Note that the string generation is NOT locale dependent - the result is the same on
	all systems. Internally, the rules of the "C" locale are used - i.e. english
	number formatting.

	The class has a single template parameter: the type of the object or value to convert.	

	To convert a value to a string you create an instance Stringifier and then call it
	like a function:

	\code

	SomeType				value = ...;	// some value to convert
	
	Stringifier< SomeType > stringifier;	// create Stringifier for the value

	String s = stringifier( value );		// call the stringifier instance like a function

	\endcode

	Stringifier automatically supports all objects that have a toString() method.	
	Support for additional custom classes and types can also be added by providing a specialization for the
	Stringifier template.

	There are already specializations for various types. The following are supported out
	of the box:
		
	- integers
	- floating point values: float, double, long double
	- bool (returns either "true" or "false")
	- nullptr (returns "nullptr")
	- all pointers (the string will be the value of the pointer (the memory address) in the format "0x12345678")
	- String instances
	- standard library string objects: std::string, std::wstring, std::u16string, std::u32string
	- C style strings: const char*, const wchar_t*, const char16_t*, const char32_t*
	- individual characters: char, wchar_t, char16_t, char32_t
	- all objects that have a toString() method (it is not necessary to implement any interfaces
	  or have a special base class for this - any object with a toString method will work).

	If there is no specialization for the given input type then the default implementation
	checks if the value is an object with a toString() method. If so then the toString method is called and the result is
	returned. If no toString method exists then a string of the form "<typename @ 0x123456>" is returned, 
	where typename is the name of the input type as returned by typeid(type).name() and 0x123456 is the address of the object.

	If you want your custom types and classes to be supported by Stringifier then you have two options:

	1) Implement a toString() method in your class. Nothing else is necessary - Stringifier will detect the presence
	   of toString automatically.

	2) Or you can provide your own template specialization for Stringifier. The following example demonstrates
	   how to add support for "Pet" instances:

	   \code

		// an example type. This could also be a class instead of a struct
		struct Pet
		{
			enum class Type
			{
				cat,
				dog
			};

			Type type;
		};		

		// now we add a specialization for Stringifier, so that it supports
		// Pet instances.
		template<>
		struct Stringifier< Pet >
		{
			String operator()( const Pet& pet ) const
			{
				switch( pet.type )
				{
				case Pet::Type::cat:	return "cat";
				case Pet::Type::dog:	return "dog";
				default:				return "?";
				}
			}
		};

		\endcode
*/
template<typename VALUE_TYPE>
struct Stringifier : public UnknownObjectStringifier_
{};



struct CharStringifier_
{
	template<typename CHAR_TYPE>
	String operator()( CHAR_TYPE&& chr )  const
	{
		char32_t chr32 = chr;
		return String( 1, chr32 );
	}
};

template<>
struct Stringifier<char> : public CharStringifier_
{};

template<>
struct Stringifier<wchar_t> : public CharStringifier_
{};

template<>
struct Stringifier<char16_t> : public CharStringifier_
{};

template<>
struct Stringifier<char32_t> : public CharStringifier_
{};


struct IntegerStringifier_
{
	template<typename ValueType>
	String operator()(ValueType&& inputValue) const
	{
		IntegerToCharSequence_< typename std::decay<ValueType>::type > seq( std::forward<ValueType>( inputValue ) );

		return String( seq.begin(), seq.end() );
	}
};

// the fundamental integer types are
// - signed/unsigned char (which are always different from char without the signed/unsigned prefix)
// - short
// - int
// - long
// - long long
// The fixed width types are simply typedefs to these.


template<>
struct Stringifier<signed char> : public IntegerStringifier_
{};


template<>
struct Stringifier<unsigned char> : public IntegerStringifier_
{};

template<>
struct Stringifier<short> : public IntegerStringifier_
{};

template<>
struct Stringifier<unsigned short> : public IntegerStringifier_
{};

template<>
struct Stringifier<int> : public IntegerStringifier_
{};

template<>
struct Stringifier<unsigned int> : public IntegerStringifier_
{};

template<>
struct Stringifier<long> : public IntegerStringifier_
{};

template<>
struct Stringifier<unsigned long> : public IntegerStringifier_
{};


template<>
struct Stringifier<long long> : public IntegerStringifier_
{};

template<>
struct Stringifier<unsigned long long> : public IntegerStringifier_
{};


template<>
struct Stringifier<bool>
{
	String operator()(bool inputValue) const
	{
		return inputValue ? U"true" : U"false";
	}
};


template<>
struct Stringifier<std::nullptr_t>
{
    String operator()(std::nullptr_t inputValue) const
	{
		return "nullptr";
	}
};

struct StdStreamStringifier_
{
	template<typename ValueType>
	String operator()(ValueType&& inputValue) const
	{
		std::stringstream s;

		// we must imbue the classic locale since
		// we do not want the result to be locale specific
		s.imbue( std::locale::classic() );

		s << std::forward<ValueType>( inputValue );

		return s.str();
	}
};

template<>
struct Stringifier<float> : public StdStreamStringifier_
{};

template<>
struct Stringifier<double> : public StdStreamStringifier_
{};

template<>
struct Stringifier<long double> : public StdStreamStringifier_
{};



struct StringStringifier_
{
	template<typename ValueType>
	String operator()(ValueType&& inputValue) const
	{
		// just rely on the String constructor to 
		// deal with the input value
		return std::forward<ValueType>(inputValue);
	}
};

template<>
struct Stringifier<std::string> : public StringStringifier_
{};

template<>
struct Stringifier<std::wstring> : public StringStringifier_
{};

template<>
struct Stringifier<std::u16string> : public StringStringifier_
{};

template<>
struct Stringifier<std::u32string> : public StringStringifier_
{};


template<>
struct Stringifier<const char*> : public StringStringifier_
{};

template<>
struct Stringifier<const wchar_t*> : public StringStringifier_
{};

template<>
struct Stringifier<const char16_t*> : public StringStringifier_
{};

template<>
struct Stringifier<const char32_t*> : public StringStringifier_
{};

template<>
struct Stringifier<char*> : public StringStringifier_
{};

template<>
struct Stringifier<wchar_t*> : public StringStringifier_
{};

template<>
struct Stringifier<char16_t*> : public StringStringifier_
{};

template<>
struct Stringifier<char32_t*> : public StringStringifier_
{};


template<class STRING_DATA_TYPE>
struct StringImplStringifier
{
	// This is for StringImpl classes that do NOT use NativeStringData
	String operator()(const StringImpl<STRING_DATA_TYPE>& inputValue) const
	{
		return String( inputValue.begin(), inputValue.end() );
	}
};

template<>
struct StringImplStringifier<NativeStringData>
{
	// this is For StringImpl<NativeStringData> (i.e. for String).

	// Instead of using StringImplStringifier (which would return
	// a copy of the String object) we can just transparently
	// pass the input reference through. The Stringifier call
	// becomes a null-op like this and the input value is used
	// directly as the output value.

	String& operator()(String& inputValue) const
	{
		return inputValue;
	}

	String&& operator()(String&& inputValue) const
	{
		return std::move(inputValue);
	}

	const String& operator()(const String& inputValue) const
	{
		return inputValue;
	}	
};

	
template<>
struct Stringifier< StringImpl<Utf8StringData> > : public StringImplStringifier< Utf8StringData >
{};

template<>
struct Stringifier< StringImpl<Utf16StringData> > : public StringImplStringifier< Utf16StringData >
{};

template<>
struct Stringifier< StringImpl<Utf32StringData> > : public StringImplStringifier< Utf32StringData >
{};

template<>
struct Stringifier< StringImpl<WideStringData> > : public StringImplStringifier< WideStringData >
{};


template<typename InputType>
struct Stringifier<InputType*>
{
	String operator()(InputType* pInValue) const
	{
		IntegerToCharSequence_<
			uintptr_t,
			16,
			U'0',
			U'x' > seq( (uintptr_t)( pInValue ) );

		return String( seq.begin(), seq.end() );
	}
};



/** Returns a string representation of the specified value or object.

	The conversion is NOT locale dependent - the result is the same on
	all systems. Internally, the rules of the "C" locale are used - i.e. english
	number formatting.

	toString uses \ref Stringifier internally, so it supports all types
	supported by that. See \ref Stringifier for a full list.

	Note that the global toString function automatically supports all objects that have a toString()
	method. The global function simply calls the method and returns its result.

	You can also add support for other custom types and classes that do not have a toString method.
	See \ref Stringifier for information on how to do that.

*/
template<typename InputType>
inline String toString(InputType&& inputValue)
{
	// we must use std::decay here, so that if InputType
	// is a reference type we instead get the type the reference points to.
	// This also changes const to non-const and arrays to pointers, which is also what we want.
	return Stringifier< typename std::decay<InputType>::type > ()( std::forward<InputType>( inputValue ) );
}


}


#endif

