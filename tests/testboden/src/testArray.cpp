#include <bdn/init.h>
#include <bdn/test.h>

#include "testCollection.h"

#include <bdn/Array.h>
#include <bdn/sort.h>

using namespace bdn;
using namespace bdn::test;

template <class CollType> inline void _testArrayPrepareForSize(CollType &coll)
{
    std::list<typename CollType::Element> origElements(coll.begin(),
                                                       coll.end());

    typename CollType::Size origCapacity = coll.capacity();

    typename CollType::Size prepareFor = 0;

    SECTION("0")
    prepareFor = 0;

    SECTION("1")
    prepareFor = 1;

    SECTION("size")
    prepareFor = coll.size();

    SECTION("capacity")
    prepareFor = coll.capacity();

    SECTION("capacity+1")
    prepareFor = coll.capacity() + 1;

    coll.prepareForSize(prepareFor);

    // prepareForSize should NEVER change the collection contents
    _verifyPositionalCollectionReadOnly(coll, origElements);

    typename CollType::Size newCapacity = coll.capacity();

    REQUIRE(newCapacity >= coll.size());

    if (prepareFor <= origCapacity)
        REQUIRE(newCapacity <= origCapacity);

    REQUIRE(newCapacity >= prepareFor);
}

template <typename ElType, typename... ConstructArgs>
static void testArray(std::initializer_list<ElType> initElList,
                      std::initializer_list<ElType> newElList,
                      std::function<bool(const ElType &)> isMovedRemnant,
                      ElType expectedConstructedEl,
                      ConstructArgs... constructArgs)
{
    SECTION("test traits")
    {
        REQUIRE(CollectionSupportsBiDirIteration_<Array<ElType>>::value);
    }

    SECTION("construct")
    {
        std::list<ElType> expectedElements;

        SECTION("n copies")
        {
            SECTION("0")
            {
                Array<ElType> coll(0, *newElList.begin());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }

            SECTION("3")
            {
                Array<ElType> coll(3, *newElList.begin());

                for (int i = 0; i < 3; i++)
                    expectedElements.push_back(*newElList.begin());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }
        }

        SECTION("n default constructed")
        {
            SECTION("0")
            {
                Array<ElType> coll(0);
                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }

            SECTION("3")
            {
                Array<ElType> coll(3);

                for (int i = 0; i < 3; i++)
                    expectedElements.push_back(ElType());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }
        }

        SECTION("iterators")
        {
            SECTION("empty")
            {
                Array<ElType> coll(newElList.begin(), newElList.begin());
                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }

            SECTION("non-empty")
            {
                Array<ElType> coll(newElList.begin(), newElList.end());

                expectedElements.insert(expectedElements.begin(),
                                        newElList.begin(), newElList.end());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }
        }

        SECTION("copy")
        {
            SECTION("Array")
            {
                Array<ElType> src(newElList);

                Array<ElType> coll(src);

                expectedElements.insert(expectedElements.begin(),
                                        newElList.begin(), newElList.end());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }

            SECTION("vector")
            {
                std::vector<ElType> src(newElList);

                Array<ElType> coll(src);

                expectedElements.insert(expectedElements.begin(),
                                        newElList.begin(), newElList.end());

                _verifyPositionalCollectionReadOnly(coll, expectedElements);
            }
        }

        SECTION("move")
        {
            SECTION("Array")
            {
                Array<ElType> src(newElList);

                Array<ElType> coll(std::move(src));

                expectedElements.insert(expectedElements.begin(),
                                        newElList.begin(), newElList.end());
                _verifyPositionalCollectionReadOnly(coll, expectedElements);

                REQUIRE(src.size() == 0);
            }

            SECTION("vector")
            {
                std::vector<ElType> src(newElList);

                Array<ElType> coll(std::move(src));

                expectedElements.insert(expectedElements.begin(),
                                        newElList.begin(), newElList.end());
                _verifyPositionalCollectionReadOnly(coll, expectedElements);

                REQUIRE(src.size() == 0);
            }
        }

        SECTION("initializer_list")
        {
            Array<ElType> coll(newElList);

            expectedElements.insert(expectedElements.begin(), newElList.begin(),
                                    newElList.end());
            _verifyPositionalCollectionReadOnly(coll, expectedElements);
        }
    }

    Array<ElType> coll;

    SECTION("empty")
    {
        _verifyPositionalCollection(
            coll, std::list<ElType>({}), newElList, isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)...);

        SECTION("prepareForSize")
        {
            SECTION("generic")
            _testGenericCollectionPrepareForSize(coll);

            SECTION("array")
            _testArrayPrepareForSize(coll);
        }
    }

    SECTION("non-empty")
    {
        for (auto &el : initElList)
            coll.add(el);

        _verifyPositionalCollection(
            coll, std::list<ElType>(initElList), newElList, isMovedRemnant,
            expectedConstructedEl,
            std::forward<ConstructArgs>(constructArgs)...);

        SECTION("indexed access")
        {
            SECTION("normal")
            _testCollectionIndexedAccess(coll);

            SECTION("const")
            _testCollectionIndexedAccess((const Array<ElType> &)coll);
        }

        SECTION("getData")
        {
            SECTION("normal")
            REQUIRE(coll.getData() == &coll[0]);

            SECTION("const")
            REQUIRE(((const Array<ElType> &)coll).getData() == &coll[0]);
        }

        SECTION("prepareForSize")
        {
            SECTION("generic")
            _testGenericCollectionPrepareForSize(coll);

            SECTION("array")
            _testArrayPrepareForSize(coll);
        }
    }
}

TEST_CASE("Array")
{
    SECTION("simple type")
    {
        testArray<int>({17, 42, 3}, {100, 101, 102},
                       [](const int &el) { return true; }, 345, 345);

        _testCollectionFindWithStartPos<Array<int>>({17, 42, 17, 3}, 88);
        _testCollectionReverseFind<Array<int>>({17, 42, 17, 3}, 88);
        _testCollectionSort<Array<int>>({17, 42, 17, 3}, {3, 17, 17, 42});

        SECTION("findAndRemove")
        {
            Array<int> coll({17, 42, 17, 3});

            _verifyCollectionFindAndRemove(coll, {17, 42, 17, 3}, 99);
        }
    }

    SECTION("addSequence with compatible but different type")
    {
        // String objects can be implicitly converted to std::string.
        // Passing a source sequence with String objects to a collection with
        // std::string elements should work.

        Array<std::string> coll;

        SECTION("initializer_list")
        {
            coll.addSequence({String("hello"), String("world")});
            _verifyGenericCollectionReadOnly(
                coll, {std::string("hello"), std::string("world")});
        }

        SECTION("std::list")
        {
            coll.addSequence(
                std::list<String>({String("hello"), String("world")}));
            _verifyGenericCollectionReadOnly(
                coll, {std::string("hello"), std::string("world")});
        }
    }

    SECTION(
        "addSequence with initializer list of compatible but different type")
    {
        // String objects can be implicitly converted to std::string. Verify
        // that this works when passing an initializer list to addSequence.

        Array<std::string> myArray;

        myArray.addSequence({String("hello"), String("world")});

        _verifyGenericCollectionReadOnly(
            myArray, {std::string("hello"), std::string("world")});
    }

    SECTION("complex type")
    {
        SECTION("ordered")
        {
            testArray<TestCollectionElement_OrderedComparable_>(
                {TestCollectionElement_OrderedComparable_(17, 117),
                 TestCollectionElement_OrderedComparable_(42, 142),
                 TestCollectionElement_OrderedComparable_(3, 103)},
                {TestCollectionElement_OrderedComparable_(100, 201),
                 TestCollectionElement_OrderedComparable_(102, 202),
                 TestCollectionElement_OrderedComparable_(103, 203)},
                [](const TestCollectionElement_OrderedComparable_ &el) {
                    return el._a == -2 && el._b == -2;
                },
                TestCollectionElement_OrderedComparable_(345, 456), 345, 456);

            _testCollectionFindWithStartPos<
                Array<TestCollectionElement_OrderedComparable_>>(
                {
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                TestCollectionElement_OrderedComparable_(400, 401));
            _testCollectionReverseFind<
                Array<TestCollectionElement_OrderedComparable_>>(
                {
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 117),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                TestCollectionElement_OrderedComparable_(400, 401));

            _testCollectionSort<
                Array<TestCollectionElement_OrderedComparable_>>(
                {
                    TestCollectionElement_OrderedComparable_(17, 1),
                    TestCollectionElement_OrderedComparable_(42, 142),
                    TestCollectionElement_OrderedComparable_(17, 2),
                    TestCollectionElement_OrderedComparable_(3, 103),
                },
                {
                    TestCollectionElement_OrderedComparable_(3, 103),
                    TestCollectionElement_OrderedComparable_(17, 1),
                    TestCollectionElement_OrderedComparable_(17, 2),
                    TestCollectionElement_OrderedComparable_(42, 142),
                });

            SECTION("findAndRemove")
            {
                Array<TestCollectionElement_OrderedComparable_> coll(
                    {TestCollectionElement_OrderedComparable_(17, 1),
                     TestCollectionElement_OrderedComparable_(42, 142),
                     TestCollectionElement_OrderedComparable_(17, 2),
                     TestCollectionElement_OrderedComparable_(3, 103)});

                _verifyCollectionFindAndRemove(
                    coll,
                    {TestCollectionElement_OrderedComparable_(17, 1),
                     TestCollectionElement_OrderedComparable_(42, 142),
                     TestCollectionElement_OrderedComparable_(17, 2),
                     TestCollectionElement_OrderedComparable_(3, 103)},
                    TestCollectionElement_OrderedComparable_(99, 99));
            }
        }

        SECTION("unordered comparable")
        {
            testArray<TestCollectionElement_UnorderedComparable_>(
                {TestCollectionElement_UnorderedComparable_(17, 117),
                 TestCollectionElement_UnorderedComparable_(42, 142),
                 TestCollectionElement_UnorderedComparable_(3, 103)},
                {TestCollectionElement_UnorderedComparable_(100, 201),
                 TestCollectionElement_UnorderedComparable_(102, 202),
                 TestCollectionElement_UnorderedComparable_(103, 203)},
                [](const TestCollectionElement_UnorderedComparable_ &el) {
                    return el._a == -2 && el._b == -2;
                },
                TestCollectionElement_UnorderedComparable_(345, 456), 345, 456);

            _testCollectionFindWithStartPos<
                Array<TestCollectionElement_UnorderedComparable_>>(
                {
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(42, 142),
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(3, 103),
                },
                TestCollectionElement_UnorderedComparable_(400, 401));
            _testCollectionReverseFind<
                Array<TestCollectionElement_UnorderedComparable_>>(
                {
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(42, 142),
                    TestCollectionElement_UnorderedComparable_(17, 117),
                    TestCollectionElement_UnorderedComparable_(3, 103),
                },
                TestCollectionElement_UnorderedComparable_(400, 401));

            SECTION("findAndRemove")
            {
                Array<TestCollectionElement_UnorderedComparable_> coll(
                    {TestCollectionElement_UnorderedComparable_(17, 1),
                     TestCollectionElement_UnorderedComparable_(42, 142),
                     TestCollectionElement_UnorderedComparable_(17, 2),
                     TestCollectionElement_UnorderedComparable_(3, 103)});

                _verifyCollectionFindAndRemove(
                    coll,
                    {TestCollectionElement_UnorderedComparable_(17, 1),
                     TestCollectionElement_UnorderedComparable_(42, 142),
                     TestCollectionElement_UnorderedComparable_(17, 2),
                     TestCollectionElement_UnorderedComparable_(3, 103)},
                    TestCollectionElement_UnorderedComparable_(99, 99));
            }

            // cannot sort because not ordered
        }

        SECTION("unordered uncomparable")
        {
            testArray<TestCollectionElement_UnorderedUncomparable_>(
                {TestCollectionElement_UnorderedUncomparable_(17, 117),
                 TestCollectionElement_UnorderedUncomparable_(42, 142),
                 TestCollectionElement_UnorderedUncomparable_(3, 103)},
                {TestCollectionElement_UnorderedUncomparable_(100, 201),
                 TestCollectionElement_UnorderedUncomparable_(102, 202),
                 TestCollectionElement_UnorderedUncomparable_(103, 203)},
                [](const TestCollectionElement_UnorderedUncomparable_ &el) {
                    return el._a == -2 && el._b == -2;
                },
                TestCollectionElement_UnorderedUncomparable_(345, 456), 345,
                456);

            // cannot use Array::find, since elements are not comparable
        }
    }
}
