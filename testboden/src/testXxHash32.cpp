#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Array.h>
#include <bdn/XxHash32.h>

using namespace bdn;

TEST_CASE("XxHash32")
{	
    struct XxHash32TestData
    {
        std::string inData;
        uint32_t    hashNoSeed;
        uint32_t    hashWithSeed;
    };

    uint64_t seed = 0x12345678;

    Array< XxHash32TestData > allTestData 
        {
            {"", 0x02cc5d05, 0xbd209070 },
            {"0", 0x48454cb2, 0xf53c91ba },
            {"01", 0x034d0471, 0xe51a809f },
            {"012", 0x48009497, 0x2cc61bf2 },
            {"0123", 0x0a0b4c93, 0x405c9efd },
            {"01234", 0x8aa3b71c, 0xba160770 },
            {"012345", 0x994e4577, 0xa4a6ac60 },
            {"0123456", 0x1907ad24, 0xe705552c },
            {"01234567", 0x189bbfbf, 0x47d7b7fe },
            {"012345678", 0x0493d634, 0x86d523ce },
            {"0123456789", 0x950c9c0a, 0x8ed12491 },
            {"0123456789:", 0xe1634e37, 0x1d92ff1e },
            {"0123456789:;", 0x56ad27bd, 0x22826346 },
            {"0123456789:;<", 0x12296e2c, 0x7618a3a0 },
            {"0123456789:;<=", 0x6d8fbd66, 0x789dd041 },
            {"0123456789:;<=>", 0xa089ca17, 0x3d1be4cb },
            {"0123456789:;<=>?", 0xbee4114f, 0xefb49920 },
            {"0123456789:;<=>?@", 0x237a0d72, 0x6fec6ad5 },
            {"0123456789:;<=>?@A", 0xb62ed64a, 0x12303264 },
            {"0123456789:;<=>?@AB", 0x85d6a0e5, 0x4e3512df },
            {"0123456789:;<=>?@ABC", 0x2f42f78b, 0x6a418db1 },
            {"0123456789:;<=>?@ABCD", 0x2069f2d4, 0xaa675387 },
            {"0123456789:;<=>?@ABCDE", 0x5be99f64, 0xef884213 },
            {"0123456789:;<=>?@ABCDEF", 0x0328437d, 0xa335300b },
            {"0123456789:;<=>?@ABCDEFG", 0x6942c6e0, 0x247bd87f },
            {"0123456789:;<=>?@ABCDEFGH", 0x5b67a37c, 0xda9bcd1a },
            {"0123456789:;<=>?@ABCDEFGHI", 0xc8b96905, 0x6564def0 },
            {"0123456789:;<=>?@ABCDEFGHIJ", 0x133bd9c0, 0x4c7c2ed5 },
            {"0123456789:;<=>?@ABCDEFGHIJK", 0x7d6a437c, 0xf082871d },
            {"0123456789:;<=>?@ABCDEFGHIJKL", 0x938edb55, 0x8d395026 },
            {"0123456789:;<=>?@ABCDEFGHIJKLM", 0xd1914da3, 0xc1aa32dd },
            {"0123456789:;<=>?@ABCDEFGHIJKLMN", 0x64470a93, 0xa989382e },
            {"0123456789:;<=>?@ABCDEFGHIJKLMNO", 0x02cb9a54, 0x4af8c15a },
            {"0123456789:;<=>?@ABCDEFGHIJKLMNOP", 0x9d2f52e1, 0xec4c7f31 }
        };


	// index based loop for simpler breakpointing
	for(size_t testIndex=0; testIndex < allTestData.size(); testIndex++ )
	{
		auto& testData = allTestData[testIndex];

		SECTION( testData.inData )
		{
			uint32_t hashNoSeed = XxHash32::calcHash( testData.inData.c_str(), testData.inData.length() );
			REQUIRE( hashNoSeed == testData.hashNoSeed );

			uint32_t hashWithSeed = XxHash32::calcHash( testData.inData.c_str(), testData.inData.length(), seed );
			REQUIRE( hashWithSeed == testData.hashWithSeed );
		}
	}
}


