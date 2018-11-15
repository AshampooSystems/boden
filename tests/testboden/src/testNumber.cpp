#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Number.h>

#include <type_traits>

using namespace bdn;

template <typename BaseType, typename ArgNumberType,
          typename std::enable_if<sizeof(ArgNumberType) <= sizeof(BaseType), int>::type = 0>
ArgNumberType _downCastIfNotImplicitlyConvertible(ArgNumberType arg)
{
    return arg;
}

template <typename BaseType, typename ArgNumberType,
          typename std::enable_if<sizeof(BaseType) < sizeof(ArgNumberType), int>::type = 0>
BaseType _downCastIfNotImplicitlyConvertible(ArgNumberType arg)
{
    return (BaseType)arg;
}

template <typename NumberType> struct MakeSigned_
{
    using Type = typename std::make_signed<NumberType>::type;
};

template <> struct MakeSigned_<double>
{
    using Type = double;
};

template <> struct MakeSigned_<float>
{
    using Type = float;
};

template <> struct MakeSigned_<long double>
{
    using Type = long double;
};

template <typename NumberType> struct MakeUnsigned_
{
    using Type = typename std::make_unsigned<NumberType>::type;
};

template <> struct MakeUnsigned_<double>
{
    using Type = double;
};

template <> struct MakeUnsigned_<float>
{
    using Type = float;
};

template <> struct MakeUnsigned_<long double>
{
    using Type = long double;
};

template <typename BaseType, typename ArgNumberType> struct MakeSameSign_
{
    using Type = typename std::conditional<
        std::is_signed<ArgNumberType>::value == std::is_signed<BaseType>::value, ArgNumberType,
        typename std::conditional<std::is_signed<BaseType>::value, typename MakeSigned_<ArgNumberType>::Type,
                                  typename MakeUnsigned_<ArgNumberType>::Type>::type>::type;
};

template <class ObjectType, typename ValueType> static void _verifyNumber(ValueType value)
{
    ObjectType numObject(3);

    if (value < 0 && !std::is_signed<typename ObjectType::SimpleType>::value) {
        // the make_signed cast has no effect since value is already signed.
        // However, it is necessary to avoid a compiler warning in case
        // ValueType is unsigned. While we will not enter this "if" branch in
        // that case, the compiler does not know that and may complain that we
        // are doing a sign inversion on an unsigned type.
        value = -(typename MakeSigned_<ValueType>::Type)value;
    }

    auto arg = _downCastIfNotImplicitlyConvertible<typename ObjectType::SimpleType, ValueType>(value);
    using ArgTypeWithSameSign = typename MakeSameSign_<typename ObjectType::SimpleType, decltype(arg)>::Type;

    ArgTypeWithSameSign argWithSameSign = (ArgTypeWithSameSign)arg;

    Number<decltype(arg)> argObj(arg);
    Number<ArgTypeWithSameSign> argWithSameSignObj(argWithSameSign);

    SECTION("operator=")
    {
        SECTION("simple arg")
        {
            numObject = arg;
            REQUIRE(numObject.getValue() == argWithSameSign);
        }

        SECTION("object arg")
        {
            numObject = argObj;
            REQUIRE(numObject.getValue() == argWithSameSign);
        }
    }

    SECTION("setValue")
    {
        SECTION("simple arg")
        {
            numObject.setValue(arg);
            REQUIRE(numObject.getValue() == (ArgTypeWithSameSign)arg);
        }

        SECTION("object arg")
        {
            numObject.setValue(argObj);
            REQUIRE(numObject.getValue() == (ArgTypeWithSameSign)arg);
        }
    }

    SECTION("implicit conversion")
    {
        numObject = arg;

        typename ObjectType::SimpleType v = numObject;
        REQUIRE(v == (ArgTypeWithSameSign)arg);
    }

    SECTION("operator==")
    {
        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            REQUIRE(numObject == arg);

            SECTION("object arg")
            REQUIRE(numObject == argObj);
        }

        SECTION("not equal")
        {
            SECTION("simple arg")
            REQUIRE(!(numObject == arg));

            SECTION("object arg")
            REQUIRE(!(numObject == argObj));
        }
    }

    SECTION("operator!=")
    {
        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            REQUIRE(!(numObject != arg));

            SECTION("object arg")
            REQUIRE(!(numObject != argObj));
        }

        SECTION("not equal")
        {
            SECTION("simple arg")
            REQUIRE(numObject != arg);

            SECTION("object arg")
            REQUIRE(numObject != argObj);
        }
    }

    SECTION("relative comparison")
    {
        SECTION("smaller")
        {
            numObject = arg - 1;

            SECTION("simple arg")
            {
                REQUIRE(numObject < argWithSameSign);
                REQUIRE(numObject <= argWithSameSign);
                REQUIRE(!(numObject > argWithSameSign));
                REQUIRE(!(numObject >= argWithSameSign));
            }

            SECTION("object arg")
            {
                REQUIRE(numObject < argWithSameSignObj);
                REQUIRE(numObject <= argWithSameSignObj);
                REQUIRE(!(numObject > argWithSameSignObj));
                REQUIRE(!(numObject >= argWithSameSignObj));
            }
        }

        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            {
                REQUIRE(!(numObject < argWithSameSign));
                REQUIRE(numObject <= argWithSameSign);
                REQUIRE(!(numObject > argWithSameSign));
                REQUIRE(numObject >= argWithSameSign);
            }

            SECTION("object arg")
            {
                REQUIRE(!(numObject < argWithSameSignObj));
                REQUIRE(numObject <= argWithSameSignObj);
                REQUIRE(!(numObject > argWithSameSignObj));
                REQUIRE(numObject >= argWithSameSignObj);
            }
        }

        SECTION("bigger")
        {
            numObject = arg + 1;

            SECTION("simple arg")
            {
                REQUIRE(!(numObject < argWithSameSign));
                REQUIRE(!(numObject <= argWithSameSign));
                REQUIRE(numObject > argWithSameSign);
                REQUIRE(numObject >= argWithSameSign);
            }

            SECTION("object arg")
            {
                REQUIRE(!(numObject < argWithSameSignObj));
                REQUIRE(!(numObject <= argWithSameSignObj));
                REQUIRE(numObject > argWithSameSignObj);
                REQUIRE(numObject >= argWithSameSignObj);
            }
        }
    }
}

template <class ObjectType, typename ValueType> static void _verifyIntNumberAndUtilityFunctions(ValueType value)
{
    _verifyNumber<ObjectType, ValueType>(value);

    SECTION("utility functions")
    {
        SECTION("invertByteOrder")
        {
            ValueType result = invertByteOrder(value);

            if (sizeof(ValueType) == 1)
                REQUIRE(result == value);

            if (sizeof(ValueType) == 2) {
                uint16_t a = static_cast<uint16_t>(value);
                uint16_t expectedResult = (a << 8) | (a >> 8);
                REQUIRE(result == (ValueType)expectedResult);
            }

            if (sizeof(ValueType) == 4) {
                uint32_t a = static_cast<uint32_t>(value);

                uint32_t expectedResult =
                    ((a & 0xff) << 24) | ((a & 0xff00) << 8) | ((a & 0xff0000) >> 8) | ((a & 0xff000000) >> 24);

                REQUIRE(result == (ValueType)expectedResult);
            }

            if (sizeof(ValueType) == 8) {
                uint64_t a = static_cast<uint64_t>(value);

                uint64_t expectedResult = ((a & 0xff) << 56) | ((a & 0xff00) << 40) | ((a & 0xff0000) << 24) |
                                          ((a & 0xff000000) << 8) | ((a & 0xff00000000) >> 8) |
                                          ((a & 0xff0000000000) >> 24) | ((a & 0xff000000000000) >> 40) |
                                          ((a & 0xff00000000000000) >> 56);

                REQUIRE(result == (ValueType)expectedResult);
            }
        }

        SECTION("rotateBitsLeft")
        {
            ValueType result = rotateBitsLeft(value, 4);

            ValueType expectedResult = value << 4 | ((value >> (sizeof(ValueType) * 8 - 4)) & 0xf);

            REQUIRE(result == expectedResult);
        }

        SECTION("rotateBitsRight")
        {
            ValueType result = rotateBitsRight(value, 4);

            ValueType mask = 0xf;
            mask <<= sizeof(ValueType) * 8 - 4;
            mask = ~mask;

            ValueType expectedResult = ((value >> 4) & mask) | (value << (sizeof(ValueType) * 8 - 4));

            REQUIRE(result == expectedResult);
        }
    }
}

template <typename BaseSimpleType, typename ObjectType> static void _testNumberBase()
{
    SECTION("subtypes")
    {
        SECTION("SimpleType") { REQUIRE(typeid(typename ObjectType::SimpleType) == typeid(BaseSimpleType)); }
    }

    SECTION("constants")
    {
        auto limits = std::numeric_limits<BaseSimpleType>();

        // note: min should be lowest(). numeric_limits::min is something else
        // entirely
        REQUIRE(ObjectType::minValue() == limits.lowest());
        REQUIRE(ObjectType::maxValue() == limits.max());

        REQUIRE(ObjectType::maxValue() > (typename ObjectType::SimpleType)0);

        REQUIRE(ObjectType::hasInfinity() == limits.has_infinity);
        REQUIRE(ObjectType::hasNan() == limits.has_quiet_NaN);
        REQUIRE(ObjectType::isSigned() == limits.is_signed);
        REQUIRE(ObjectType::isInteger() == limits.is_integer);

        if (ObjectType::isSigned())
            REQUIRE(ObjectType::minValue() < 0);
        else
            REQUIRE(ObjectType::minValue() == 0);

        if (ObjectType::hasNan()) {
            BaseSimpleType nanVal = ObjectType::nan();

            REQUIRE(isNan(nanVal));

            // NaN values do not compare equal to anything,
            // unless the compiler has activated optimizations that
            // violate the standard.

#if !BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS
            REQUIRE(nanVal != nanVal);
#endif
        } else {
            BaseSimpleType nanVal = ObjectType::nan();

            REQUIRE(nanVal == 0);

            REQUIRE(!isNan(nanVal));
        }

        if (ObjectType::hasInfinity()) {
#if !BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS
            REQUIRE(ObjectType::infinity() > ObjectType::maxValue());
            REQUIRE(ObjectType::negativeInfinity() < ObjectType::minValue());
#endif
        } else {
            REQUIRE(ObjectType::infinity() == 0);
            REQUIRE(ObjectType::negativeInfinity() == 0);
        }
    }

    SECTION("construct")
    {
        SECTION("default")
        {
            ObjectType i;

            REQUIRE(i.getValue() == 0);
        }

        SECTION("with value")
        {
            ObjectType i(17);

            REQUIRE(i.getValue() == 17);
        }

        SECTION("max")
        {
            ObjectType i(ObjectType::maxValue());

            REQUIRE(i.getValue() == ObjectType::maxValue());
        }

        SECTION("min")
        {
            ObjectType i(ObjectType::minValue());

            REQUIRE(i.getValue() == ObjectType::minValue());
        }
    }

    SECTION("operator bool")
    {
        ObjectType i;

        SECTION("0")
        {
            SECTION("init bool")
            {
                bool b = !!i;
                REQUIRE(!b);
            }

            SECTION("if")
            {
                if (i)
                    REQUIRE(false);

                if (!i) {
                } else
                    REQUIRE(false);
            }
        }

        SECTION("1")
        {
            i = (BaseSimpleType)1;

            SECTION("init bool")
            {
                bool b = !!i;
                REQUIRE(b);
            }

            SECTION("if")
            {
                if (!i)
                    REQUIRE(false);

                if (i) {
                } else
                    REQUIRE(false);
            }
        }

        SECTION("-1")
        {
            i = (BaseSimpleType)-1;

            SECTION("init bool")
            {
                bool b = !!i;
                REQUIRE(b);
            }

            SECTION("if")
            {
                if (!i)
                    REQUIRE(false);

                if (i) {
                } else
                    REQUIRE(false);
            }
        }
    }

    SECTION("hash")
    {
        ObjectType maxObj(ObjectType::maxValue());
        ObjectType minObj(ObjectType::minValue());

        size_t maxHash = std::hash<ObjectType>()(maxObj);
        size_t minHash = std::hash<ObjectType>()(minObj);
        REQUIRE(minHash != maxHash);

        ObjectType maxWithoutHighBitObj(ObjectType::maxValue() / 2);
        size_t maxWithoutHighBitHash = std::hash<ObjectType>()(maxWithoutHighBitObj);

        REQUIRE(maxWithoutHighBitHash != maxHash);
        REQUIRE(maxWithoutHighBitHash != minHash);
    }
}

template <typename BaseSimpleType, typename ObjectType> static void _testIntNumber()
{
    _testNumberBase<BaseSimpleType, ObjectType>();

    SECTION("different argument types")
    {
        SECTION("uint8_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, uint8_t>(0x12);

        SECTION("int8_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, int8_t>(-0x12);

        SECTION("uint16_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, uint16_t>(0x1234);

        SECTION("int16_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, int16_t>(-0x1234);

        SECTION("uint32_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, uint32_t>(0x12345678);

        SECTION("int32_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, int32_t>(-0x12345678);

        SECTION("uint64_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, uint64_t>(0x1234567812345678ll);

        SECTION("int64_t")
        _verifyIntNumberAndUtilityFunctions<ObjectType, int64_t>(-0x1234567812345678ll);
    }
}

template <class ObjectType, typename ValueType> static void _verifyFloatNumber(ValueType value)
{
    ObjectType numObject(3.456f);

    auto arg = _downCastIfNotImplicitlyConvertible<typename ObjectType::SimpleType, ValueType>(value);
    Number<decltype(arg)> argObj(arg);

    SECTION("operator=")
    {
        SECTION("simple arg")
        {
            numObject = arg;
            REQUIRE(numObject.getValue() == arg);
        }

        SECTION("object arg")
        {
            numObject = argObj;
            REQUIRE(numObject.getValue() == arg);
        }
    }

    SECTION("setValue")
    {
        SECTION("simple arg")
        {
            numObject.setValue(arg);
            REQUIRE(numObject.getValue() == arg);
        }

        SECTION("object arg")
        {
            numObject.setValue(argObj);
            REQUIRE(numObject.getValue() == arg);
        }
    }

    SECTION("implicit conversion")
    {
        numObject = arg;

        typename ObjectType::SimpleType v = numObject;
        REQUIRE(v == arg);
    }

    SECTION("operator==")
    {
        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            REQUIRE(numObject == arg);

            SECTION("object arg")
            REQUIRE(numObject == argObj);
        }

        SECTION("not equal")
        {
            SECTION("simple arg")
            REQUIRE(!(numObject == arg));

            SECTION("object arg")
            REQUIRE(!(numObject == argObj));
        }
    }

    SECTION("operator!=")
    {
        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            REQUIRE(!(numObject != arg));

            SECTION("object arg")
            REQUIRE(!(numObject != argObj));
        }

        SECTION("not equal")
        {
            SECTION("simple arg")
            REQUIRE(numObject != arg);

            SECTION("object arg")
            REQUIRE(numObject != argObj);
        }
    }

    SECTION("relative comparison")
    {
        SECTION("smaller")
        {
            numObject = arg - 1.0f;

            SECTION("simple arg")
            {
                REQUIRE(numObject < arg);
                REQUIRE(numObject <= arg);
                REQUIRE(!(numObject > arg));
                REQUIRE(!(numObject >= arg));
            }

            SECTION("object arg")
            {
                REQUIRE(numObject < argObj);
                REQUIRE(numObject <= argObj);
                REQUIRE(!(numObject > argObj));
                REQUIRE(!(numObject >= argObj));
            }
        }

        SECTION("equal")
        {
            numObject = arg;

            SECTION("simple arg")
            {
                REQUIRE(!(numObject < arg));
                REQUIRE(numObject <= arg);
                REQUIRE(!(numObject > arg));
                REQUIRE(numObject >= arg);
            }

            SECTION("object arg")
            {
                REQUIRE(!(numObject < argObj));
                REQUIRE(numObject <= argObj);
                REQUIRE(!(numObject > argObj));
                REQUIRE(numObject >= argObj);
            }
        }

        SECTION("bigger")
        {
            numObject = arg + 1.0f;

            SECTION("simple arg")
            {
                REQUIRE(!(numObject < arg));
                REQUIRE(!(numObject <= arg));
                REQUIRE(numObject > arg);
                REQUIRE(numObject >= arg);
            }

            SECTION("object arg")
            {
                REQUIRE(!(numObject < argObj));
                REQUIRE(!(numObject <= argObj));
                REQUIRE(numObject > argObj);
                REQUIRE(numObject >= argObj);
            }
        }
    }
}

template <typename BaseSimpleType, typename ObjectType> static void _testFloatNumber()
{
    _testNumberBase<BaseSimpleType, ObjectType>();

    SECTION("different argument types")
    {
        SECTION("int8_t")
        _verifyFloatNumber<ObjectType, int8_t>(-0x12);

        SECTION("int16_t")
        _verifyFloatNumber<ObjectType, int16_t>(-0x1234);

        // we do not check with integers > 16 bit since then we might exceed
        // the range of float and the compiler might produce a warning

        SECTION("float")
        _verifyFloatNumber<ObjectType, float>(-1234);

        SECTION("double")
        _verifyFloatNumber<ObjectType, double>(-1234);

        SECTION("long double")
        _verifyFloatNumber<ObjectType, long double>(-1234);
    }
}

template <typename FloatType> void _testGlobalFloatFunctions()
{
    FloatType nanVal = std::numeric_limits<FloatType>::quiet_NaN();
    FloatType infVal = std::numeric_limits<FloatType>::infinity();
    FloatType negInfVal = -infVal;
    FloatType minVal = std::numeric_limits<FloatType>::lowest();
    FloatType maxVal = std::numeric_limits<FloatType>::max();

    // sanity checks
    REQUIRE(std::isnan(nanVal));
    REQUIRE(!std::isfinite(nanVal));

    REQUIRE(!std::isnan(infVal));
    REQUIRE(!std::isfinite(infVal));

    REQUIRE(!std::isnan(negInfVal));
    REQUIRE(!std::isfinite(negInfVal));

    REQUIRE(!std::isnan(minVal));
    REQUIRE(std::isfinite(minVal));

    REQUIRE(!std::isnan(maxVal));
    REQUIRE(std::isfinite(maxVal));

    // actual tests

    REQUIRE(isNan(nanVal));
    REQUIRE(!isFinite(nanVal));

    REQUIRE(!isNan(infVal));
    REQUIRE(!isFinite(infVal));

    REQUIRE(!isNan(negInfVal));
    REQUIRE(!isFinite(negInfVal));

    REQUIRE(!isNan(minVal));
    REQUIRE(isFinite(minVal));

    REQUIRE(!isNan(maxVal));
    REQUIRE(isFinite(maxVal));
}

template <typename IntType> void _testGlobalFloatFunctionsWithIntegers()
{
    IntType val = -17;

    REQUIRE(!isNan(val));
    REQUIRE(isFinite(val));
}

TEST_CASE("Number")
{
    SECTION("int")
    _testIntNumber<int, Int>();

    SECTION("int8_t")
    _testIntNumber<int8_t, Int8>();

    SECTION("uint8_t")
    _testIntNumber<uint8_t, UInt8>();

    SECTION("int16_t")
    _testIntNumber<int16_t, Int16>();

    SECTION("uint16_t")
    _testIntNumber<uint16_t, UInt16>();

    SECTION("int32_t")
    _testIntNumber<int32_t, Int32>();

    SECTION("uint32_t")
    _testIntNumber<uint32_t, UInt32>();

    SECTION("int64_t")
    _testIntNumber<int64_t, Int64>();

    SECTION("uint64_t")
    _testIntNumber<uint64_t, UInt64>();

    SECTION("int8_t")
    _testIntNumber<int8_t, Int8>();

    SECTION("long")
    _testIntNumber<long, Number<long>>();

    SECTION("unsigned long")
    _testIntNumber<unsigned long, Number<unsigned long>>();

    SECTION("float")
    _testFloatNumber<float, Number<float>>();

    SECTION("double")
    _testFloatNumber<double, Number<double>>();

    SECTION("long double")
    _testFloatNumber<long double, Number<long double>>();

    SECTION("global float functions")
    {
        SECTION("float")
        _testGlobalFloatFunctions<float>();

        SECTION("double")
        _testGlobalFloatFunctions<double>();

        SECTION("long double")
        _testGlobalFloatFunctions<long double>();

        SECTION("with integers")
        {
            SECTION("int")
            _testGlobalFloatFunctionsWithIntegers<int>();

            SECTION("int8_t")
            _testGlobalFloatFunctionsWithIntegers<int8_t>();
        }
    }
}
