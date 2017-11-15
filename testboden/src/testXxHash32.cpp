#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Array.h>
#include <bdn/XxHash32.h>

using namespace bdn;

TEST_CASE("XxHash32")
{
	struct TestData
	{
		std::string inData;
		uint32_t    hashNoSeed;
		uint32_t    hashWithSeed17;
	};

	Array< TestData > allTestData 
		{
			{"", 0x02cc5d05, 0xacf758fb},
			{"a", 0x550d7456, 0xc0b37d72 },
			{"ba", 0xf98284bd, 0xdad39d75 },
			{"cba", 0x088ba7d7, 0x6cd8a858 },
			{"dcba", 0x90cab330, 0x89f1e75e },
			{"edcba", 0x235c35cc, 0x122a6f4a },
			{"mlkjihgfedcba", 0x64e05355, 0x814cc161 },
			{"nmlkjihgfedcba", 0xa0c134da, 0x9ea88661 },
			{"onmlkjihgfedcba", 0xc24c8f84, 0x2351847b },
			//{"ponmlkjihgfedcba", 0x1853c84e, 0x5cac4f55 },
			{"qponmlkjihgfedcba", 0x27f9bf3e, 0x9b0d767c },
			{"rqponmlkjihgfedcba", 0x770d7d85, 0xe069e2db },
			{"srqponmlkjihgfedcba", 0x24fa0f51, 0xc145e111 },
			{"tsrqponmlkjihgfedcba", 0x7ddd095b, 0xded98377 },
			{"utsrqponmlkjihgfedcba", 0x5c8cf7d6, 0x9bad0d52 },
			{"BAzyxwvutsrqponmlkjihgfedcba", 0x07a5a31f, 0x0202e10b },
			{"CBAzyxwvutsrqponmlkjihgfedcba", 0xfe9221f0, 0x63d7cf2f },
			{"DCBAzyxwvutsrqponmlkjihgfedcba", 0xcc8a10fc, 0x35fb577a },
			{"EDCBAzyxwvutsrqponmlkjihgfedcba", 0x26cf1ed0, 0x402031d1 },
			{"FEDCBAzyxwvutsrqponmlkjihgfedcba", 0x7445737e, 0x4873cd94 },
			{"GFEDCBAzyxwvutsrqponmlkjihgfedcba", 0x34d1a326, 0xf545fbcc }
		};

	// index based loop for simpler breakpointing
	for(size_t testIndex=0; testIndex < allTestData.size(); testIndex++ )
	{
		auto& testData = allTestData[testIndex];

		SECTION( testData.inData )
		{
			uint32_t hashNoSeed = XxHash32::calcHash( testData.inData.c_str(), testData.inData.length() );
			REQUIRE( hashNoSeed == testData.hashNoSeed );

			uint32_t hashWithSeed17 = XxHash32::calcHash( testData.inData.c_str(), testData.inData.length(), 17 );
			REQUIRE( hashWithSeed17 == testData.hashWithSeed17 );
		}
	}
}


