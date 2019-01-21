

#include <bdn/test.h>
#include <bdn/config.h>

namespace bdn
{

    TEST_CASE("config")
    {
        SECTION("charSize")
        REQUIRE(BDN_WCHAR_SIZE == sizeof(wchar_t));

        SECTION("endian macros")
        {
#if !defined(BDN_IS_BIG_ENDIAN) || !defined(BDN_IS_LITTLE_ENDIAN)
            // should both be defined
            REQUIRE(false);
#endif

            // this line simply enforces that BDN_IS_BIG_ENDIAN and
            // BDN_IS_LITTLE_ENDIAN can be evaluated at compile time. Otherwise
            // we will get a compile error here
#if BDN_IS_BIG_ENDIAN && BDN_IS_LITTLE_ENDIAN
            REQUIRE(true);
#endif

            bool compileTimeLittleEndian = BDN_IS_LITTLE_ENDIAN;
            bool compileTimeBigEndian = BDN_IS_BIG_ENDIAN;

            uint32_t val = 0xff;
            bool runTimeLittleEndian = (((uint8_t *)&val)[0] == 0xff);

            REQUIRE(compileTimeLittleEndian == runTimeLittleEndian);
            REQUIRE(compileTimeBigEndian == !runTimeLittleEndian);
        }
    }
}
