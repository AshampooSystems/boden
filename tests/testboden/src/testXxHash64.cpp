#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Array.h>
#include <bdn/XxHash64.h>

using namespace bdn;

TEST_CASE("XxHash64")
{
    struct XxHash64TestData
    {
        std::string inData;
        uint64_t hashNoSeed;
        uint64_t hashWithSeed;
    };

    uint64_t seed = 0x1234567890abcdef;

    Array<XxHash64TestData> allTestData{
        {"", 0xef46db3751d8e999ULL, 0xa2f5c44d0f46c872ULL},
        {"0", 0x633457081244afecULL, 0xf47ebf7f5cb6795bULL},
        {"01", 0x6ed10a79b52ef0f5ULL, 0xe7a16c981f4be423ULL},
        {"012", 0x1c2dcb4b9024d73dULL, 0x8516f202e563cd06ULL},
        {"0123", 0x4c33072b45647dcbULL, 0xe19fff0227e3b861ULL},
        {"01234", 0x34cb4c2ee6166f65ULL, 0x915c2f06737a7295ULL},
        {"012345", 0x2e47ff2ef217b048ULL, 0xd03d6eb9260be487ULL},
        {"0123456", 0x97ee4fe4a0ff4dfaULL, 0x281fef0a47387bf4ULL},
        {"01234567", 0xe4ba22a49ad89d3fULL, 0x5981b4a4e0a07d38ULL},
        {"012345678", 0x43ecc9248bc07d20ULL, 0xc2fc855460180ba2ULL},
        {"0123456789", 0x3f5fc178a81867e7ULL, 0x07316d17fec9a257ULL},
        {"0123456789:", 0xb5b721ad70a49b0fULL, 0x0b3f5cd6582c7246ULL},
        {"0123456789:;", 0x884e66bb5a00411dULL, 0x9cc27b0db966c216ULL},
        {"0123456789:;<", 0x46a4c93f9ee1767eULL, 0x1ace702635ab26fdULL},
        {"0123456789:;<=", 0x397f4332b63b0887ULL, 0x64afe055e8fdb1adULL},
        {"0123456789:;<=>", 0x7a85916d8cac500aULL, 0x021c3690fce15b7dULL},
        {"0123456789:;<=>?", 0x6ff4eb62bcec6586ULL, 0xc16a47e0b09708c8ULL},
        {"0123456789:;<=>?@", 0x001ee69e11bf98bfULL, 0x8cfe3a2599cfa38fULL},
        {"0123456789:;<=>?@A", 0xf5bb6ec8eced5bd5ULL, 0x88168dea1e4d686eULL},
        {"0123456789:;<=>?@AB", 0x90c0c9efbb551d9cULL, 0xd2201734a7bc6693ULL},
        {"0123456789:;<=>?@ABC", 0x9fa37187b2cf0e37ULL, 0x6e5e51850032a2c2ULL},
        {"0123456789:;<=>?@ABCD", 0x81ce30c869764671ULL, 0x05737ecdf6ea8881ULL},
        {"0123456789:;<=>?@ABCDE", 0x6450d7c8dc87d901ULL,
         0xca7d9e6cdb60960fULL},
        {"0123456789:;<=>?@ABCDEF", 0x62fe3bac6e812803ULL,
         0xf91aae2a74783fc4ULL},
        {"0123456789:;<=>?@ABCDEFG", 0x27b5ceafc3ff7e6dULL,
         0xf2eb378878772912ULL},
        {"0123456789:;<=>?@ABCDEFGH", 0x19520096ed2cbf74ULL,
         0x29afc8f64e3e2719ULL},
        {"0123456789:;<=>?@ABCDEFGHI", 0x0f783c328fffc863ULL,
         0x031a3d3534410b96ULL},
        {"0123456789:;<=>?@ABCDEFGHIJ", 0x421246dcbd64855cULL,
         0x06e25874965f2ea7ULL},
        {"0123456789:;<=>?@ABCDEFGHIJK", 0x74565fafc4753522ULL,
         0x91dbc6bcb35beaf9ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKL", 0x91b760b63645c8e5ULL,
         0x93862aff10d80ba2ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLM", 0x6e50270346564e37ULL,
         0xea608dc8c1464a78ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMN", 0xc506e23012915790ULL,
         0xa2c0eddf74983bfeULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNO", 0x96ee2fa935fdd556ULL,
         0x1396bc2a00938b77ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOP", 0xa5cdc6940c6fde9eULL,
         0x65cd9ed37dff8753ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQ", 0xfc54c8c178295b09ULL,
         0x979cbacd83d90038ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQR", 0xfd098b56c5c25e83ULL,
         0x502eda968044c113ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRS", 0xa1bc3537c8713b6aULL,
         0x34efc5c85781e473ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST", 0xd692b6a43e68fd9cULL,
         0x8c505df0b8d88ad9ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTU", 0x1c1986a4bf504f11ULL,
         0x7ad67a81b5895ba5ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUV", 0x7403716c4b4d2649ULL,
         0xc3f584151d089243ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVW", 0x1c4301b8b7c5c3f3ULL,
         0x10e15fd01f01a9e7ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWX", 0xbf46222877965856ULL,
         0xb23fe52c2a2d630dULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXY", 0x37fa12793c6883cdULL,
         0x869ffc416238781fULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0xf5f53c79824b7ae5ULL,
         0x2171245c3d851fc5ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[", 0x05c558855548ca3cULL,
         0x559e4ff90ac442e6ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\",
         0x3f55f9f87adb6b04ULL, 0xd648279ebd0dfc59ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]",
         0x65e5f822689f925aULL, 0x2cd380cd1650636dULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^",
         0x846f74716baaf1feULL, 0x9f35e6c3c49f2e8aULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_",
         0x6e1a612b2f6ba099ULL, 0x5260f1f5a6f1ee30ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`",
         0xf9e6d2898648b539ULL, 0x3626f7d09a840248ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`a",
         0x09a9888934c151e2ULL, 0xa23a10114c6ad52dULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ab",
         0x658f35ad4555be50ULL, 0x30fcc80e34c16d22ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abc",
         0xa968cc564b34fc9dULL, 0x0db2b8e45318a4caULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcd",
         0x6c6f55c59c8568fbULL, 0x70b1dab20f94f50fULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcde",
         0x230623801fc29abcULL, 0x82fe1b19706a4e46ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdef",
         0x7948f1375bc42787ULL, 0x9e9f7e867d4086c9ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefg",
         0xff28ad604e31c66eULL, 0x3305e737745cf21eULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgh",
         0x53e7651391b3b370ULL, 0x634169d9a208d2f9ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghi",
         0xe1b03aeb18cba521ULL, 0xd23ad73b66946bacULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghij",
         0x202fd4a53e215692ULL, 0xe55e8ea8bb1855d7ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijk",
         0x29143fe8a8b83799ULL, 0xe584191047adc428ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijkl",
         0x726af2e999e1cd5aULL, 0xf248e28c985a44cbULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm",
         0xd6595acea9d953edULL, 0xbea081193f105af9ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmn",
         0x7daf8f9fdbd4e518ULL, 0x5c0dc6efb801d4fbULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmno",
         0x3fe627fabdb30aeaULL, 0xd2ff1b3750571f15ULL},
        {"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnop",
         0xeddce6ec01f7e595ULL, 0xb0aa81bc38df17b9ULL}};

    // index based loop for simpler breakpointing
    for (size_t testIndex = 0; testIndex < allTestData.size(); testIndex++) {
        auto &testData = allTestData[testIndex];

        SECTION(testData.inData)
        {
            uint64_t hashNoSeed = XxHash64::calcHash(testData.inData.c_str(),
                                                     testData.inData.length());
            REQUIRE(hashNoSeed == testData.hashNoSeed);

            uint64_t hashWithSeed = XxHash64::calcHash(
                testData.inData.c_str(), testData.inData.length(), seed);
            REQUIRE(hashWithSeed == testData.hashWithSeed);
        }
    }
}
