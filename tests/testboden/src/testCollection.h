#ifndef BDN_testCollection_H_
#define BDN_testCollection_H_

#include <bdn/test.h>
#include <bdn/sort.h>

#include <vector>
#include <list>

namespace bdn
{
    namespace test
    {

        template <class CollType> struct CollectionElementOrderUndefined_
        {
            enum
            {
                value = 0
            };
        };

        class TestCollectionElement_UnorderedUncomparable_
        {
          public:
            TestCollectionElement_UnorderedUncomparable_()
            {
                _a = -1;
                _b = -1;
            }

            TestCollectionElement_UnorderedUncomparable_(int a, int b)
            {
                _a = a;
                _b = b;
            }

            TestCollectionElement_UnorderedUncomparable_(const TestCollectionElement_UnorderedUncomparable_ &o)
            {
                _a = o._a;
                _b = o._b;
            }

            TestCollectionElement_UnorderedUncomparable_(TestCollectionElement_UnorderedUncomparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;
            }

            TestCollectionElement_UnorderedUncomparable_ &
            operator=(const TestCollectionElement_UnorderedUncomparable_ &o)
            {
                _a = o._a;
                _b = o._b;

                return *this;
            }

            TestCollectionElement_UnorderedUncomparable_ &operator=(TestCollectionElement_UnorderedUncomparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;

                return *this;
            }

            int _a;
            int _b;
        };

        template <typename CHAR_TYPE, class CHAR_TRAITS>
        std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                               const TestCollectionElement_UnorderedUncomparable_ &el)
        {
            return stream << el._a << ", " << el._b;
        }

        class TestCollectionElement_UnorderedComparable_ : public TestCollectionElement_UnorderedUncomparable_
        {
          public:
            TestCollectionElement_UnorderedComparable_()
            {
                _a = -1;
                _b = -1;
            }

            TestCollectionElement_UnorderedComparable_(int a, int b)
            {
                _a = a;
                _b = b;
            }

            TestCollectionElement_UnorderedComparable_(const TestCollectionElement_UnorderedComparable_ &o)
            {
                _a = o._a;
                _b = o._b;
            }

            TestCollectionElement_UnorderedComparable_(TestCollectionElement_UnorderedComparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;
            }

            TestCollectionElement_UnorderedComparable_ &operator=(const TestCollectionElement_UnorderedComparable_ &o)
            {
                _a = o._a;
                _b = o._b;

                return *this;
            }

            TestCollectionElement_UnorderedComparable_ &operator=(TestCollectionElement_UnorderedComparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;

                return *this;
            }

            bool operator==(const TestCollectionElement_UnorderedComparable_ &o) const
            {
                return (_a == o._a && _b == o._b);
            }

            bool operator!=(const TestCollectionElement_UnorderedComparable_ &o) const { return !operator==(o); }
        };

        template <typename CHAR_TYPE, class CHAR_TRAITS>
        std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                               const TestCollectionElement_UnorderedComparable_ &el)
        {
            return stream << el._a << ", " << el._b;
        }

        class TestCollectionElement_OrderedComparable_ : public TestCollectionElement_UnorderedComparable_
        {
          public:
            TestCollectionElement_OrderedComparable_() {}

            TestCollectionElement_OrderedComparable_(int a, int b) : TestCollectionElement_UnorderedComparable_(a, b) {}

            TestCollectionElement_OrderedComparable_(const TestCollectionElement_OrderedComparable_ &o)
                : TestCollectionElement_UnorderedComparable_(o)
            {}

            TestCollectionElement_OrderedComparable_(TestCollectionElement_OrderedComparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;
            }

            TestCollectionElement_OrderedComparable_ &operator=(const TestCollectionElement_OrderedComparable_ &o)
            {
                _a = o._a;
                _b = o._b;

                return *this;
            }

            TestCollectionElement_OrderedComparable_ &operator=(TestCollectionElement_OrderedComparable_ &&o)
            {
                _a = o._a;
                _b = o._b;

                o._a = -2;
                o._b = -2;

                return *this;
            }

            bool operator<(const TestCollectionElement_OrderedComparable_ &o) const
            {
                // the ordering is only based on the "a" component, so that
                // we can have elements that are not equal, but have the same
                // ordering position
                return (_a < o._a);
            }
        };

        template <typename CHAR_TYPE, class CHAR_TRAITS>
        std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                               const TestCollectionElement_OrderedComparable_ &el)
        {
            return stream << el._a << ", " << el._b;
        }

        inline bool _isCollectionElementEqual(const TestCollectionElement_UnorderedUncomparable_ &l,
                                              const TestCollectionElement_UnorderedUncomparable_ &r)
        {
            return (l._a == r._a && l._b == r._b);
        }

        inline bool _isCollectionElementEqual(int l, int r) { return (l == r); }

        inline bool _isCollectionElementEqual(const std::string &l, const std::string &r) { return (l == r); }

        inline bool _isCollectionElementEqual(const std::pair<const int, double> &l,
                                              const std::pair<const int, double> &r)
        {
            return (l.first == r.first && l.second == r.second);
        }

        inline bool _isCollectionElementEqual(const std::pair<const TestCollectionElement_OrderedComparable_,
                                                              TestCollectionElement_UnorderedComparable_> &l,
                                              const std::pair<const TestCollectionElement_OrderedComparable_,
                                                              TestCollectionElement_UnorderedComparable_> &r)
        {
            return (_isCollectionElementEqual(l.first, r.first) && _isCollectionElementEqual(l.second, r.second));
        }

        inline bool _isCollectionElementEqual(const std::pair<const int, String> &l,
                                              const std::pair<const int, String> &r)
        {
            return (l.first == r.first && l.second == r.second);
        }

        template <class CollType> inline void _verifyEmptyCollection(CollType &coll)
        {
            SECTION("isEmpty")
            REQUIRE(coll.isEmpty());

            SECTION("size")
            {
                REQUIRE(coll.getSize() == 0);
                REQUIRE(coll.size() == 0);
            }

            SECTION("iterators")
            {
                REQUIRE(coll.begin() == coll.end());
                REQUIRE(coll.constBegin() == coll.constBegin());
                REQUIRE(coll.reverseBegin() == coll.reverseEnd());
                REQUIRE(coll.constReverseBegin() == coll.constReverseEnd());
            }

            SECTION("maxSize")
            {
                REQUIRE(coll.getMaxSize() >= 0x7ffffff);
                REQUIRE(coll.max_size() == coll.getMaxSize());
            }
        }

        template <class CollType> inline void _verifyEmptySequence(CollType &coll)
        {
            _verifyEmptyCollection(coll);

            SECTION("getFirst")
            REQUIRE_THROWS_AS(coll.getFirst(), OutOfRangeError);

            SECTION("getLast")
            REQUIRE_THROWS_AS(coll.getLast(), OutOfRangeError);
        }

        template <class ElType, class ItType>
        inline void _verifyIteratorIntervalContents(ItType it, ItType end, const std::list<ElType> &expectedElementList,
                                                    bool mustHaveSameOrder)
        {
            if (mustHaveSameOrder) {
                for (const auto &expectedEl : expectedElementList) {
                    REQUIRE(it != end);
                    REQUIRE(_isCollectionElementEqual((const typename ItType::value_type &)*it, expectedEl));
                    ++it;
                }

                REQUIRE(it == end);
            } else {
                std::list<ElType> remainingExpectedEls = expectedElementList;

                while (it != end) {
                    auto expectedIt =
                        std::find_if(remainingExpectedEls.begin(), remainingExpectedEls.end(),
                                     [it](const ElType &el) { return _isCollectionElementEqual(el, *it); });
                    REQUIRE(expectedIt != remainingExpectedEls.end());
                    REQUIRE(_isCollectionElementEqual(*expectedIt, *it));

                    remainingExpectedEls.erase(expectedIt);

                    ++it;
                }

                REQUIRE(remainingExpectedEls.empty());
            }
        }

        template <class CollType> class CollectionSupportsBiDirIteration_
        {
            // this function will be ignord if FuncCollType::reverse_iterator
            // does not exist.
            template <typename FuncCollType> static typename FuncCollType::reverse_iterator test(int);

            template <typename FuncCollType> static char test(double);

          public:
            enum
            {
                // test(0) can match either function.
                // The first one will be preferred to the second one since 0 is
                // an int. So if rbegin is there then test(0) returns an
                // iterator, otherwise it returns char.
                value = sizeof(test<CollType>(0)) != sizeof(char)

                // note that we could also check the type of the Collection
                // iterators, BUT that does not work for all collection. In
                // VS2015 std::unordered_map uses std::list iterators, which are
                // bidirectional. But that is an implementation detail --t
                // std::unordered_map still does not have an rbegin function. If
                // we could depend on the iterator type then we could check it
                // with: std::is_base_of
                // <
                //		std::bidirectional_iterator_tag,
                //		typename std::iterator_traits<typename
                // CollType::iterator>::iterator_category
                //	>::value
            };
        };

        template <class CollType, bool supportsBiDir> class CollectionIterationVerifier_
        {
          public:
            static void verify(CollType &coll, const std::list<typename CollType::Element> &expectedElementList)
            {
                // verify that the correct version of verify was called
                static_assert(!CollectionSupportsBiDirIteration_<CollType>::value,
                              "incorrect CollectionIterationVerifier_ chosen by "
                              "compiler!");

                bool mustHaveSameOrder = !CollectionElementOrderUndefined_<CollType>::value;

                // this is for iterators that do not support
                // bidirectional_iterator_tag
                SECTION("normal")
                _verifyIteratorIntervalContents<typename CollType::Element>(coll.begin(), coll.end(),
                                                                            expectedElementList, mustHaveSameOrder);

                SECTION("const coll")
                _verifyIteratorIntervalContents<typename CollType::Element>(((const CollType &)coll).begin(),
                                                                            ((const CollType &)coll).end(),
                                                                            expectedElementList, mustHaveSameOrder);

                SECTION("constBegin/End")
                _verifyIteratorIntervalContents<typename CollType::Element>(coll.constBegin(), coll.constEnd(),
                                                                            expectedElementList, mustHaveSameOrder);
            }
        };

        template <class CollType> class CollectionIterationVerifier_<CollType, true>
        {
          public:
            static void verify(CollType &coll, const std::list<typename CollType::Element> &expectedElementList)
            {
                // verify that the correct version of verify was called
                static_assert(CollectionSupportsBiDirIteration_<CollType>::value,
                              "incorrect CollectionIterationVerifier_ chosen by "
                              "compiler!");

                bool mustHaveSameOrder = !CollectionElementOrderUndefined_<CollType>::value;

                SECTION("normal")
                _verifyIteratorIntervalContents<typename CollType::Element>(coll.begin(), coll.end(),
                                                                            expectedElementList, mustHaveSameOrder);

                SECTION("const coll")
                _verifyIteratorIntervalContents<typename CollType::Element>(((const CollType &)coll).begin(),
                                                                            ((const CollType &)coll).end(),
                                                                            expectedElementList, mustHaveSameOrder);

                SECTION("constBegin/End")
                _verifyIteratorIntervalContents<typename CollType::Element>(coll.constBegin(), coll.constEnd(),
                                                                            expectedElementList, mustHaveSameOrder);

                std::list<typename CollType::Element> reversedExpectedElementList(expectedElementList);
                reversedExpectedElementList.reverse();

                SECTION("reverse normal")
                _verifyIteratorIntervalContents<typename CollType::Element>(
                    coll.reverseBegin(), coll.reverseEnd(), reversedExpectedElementList, mustHaveSameOrder);

                SECTION("reverse const coll")
                _verifyIteratorIntervalContents<typename CollType::Element>(
                    ((const CollType &)coll).reverseBegin(), ((const CollType &)coll).reverseEnd(),
                    reversedExpectedElementList, mustHaveSameOrder);

                SECTION("constReverseBegin/End")
                _verifyIteratorIntervalContents<typename CollType::Element>(
                    coll.constReverseBegin(), coll.constReverseEnd(), reversedExpectedElementList, mustHaveSameOrder);
            }
        };

        template <class CollType>
        inline void _verifyCollectionIteration(CollType &coll,
                                               const std::list<typename CollType::Element> &expectedElementList)
        {
            CollectionIterationVerifier_<CollType, CollectionSupportsBiDirIteration_<CollType>::value>::verify(
                coll, expectedElementList);
        }

        template <class CollType>
        inline void _verifyGenericCollectionReadOnly(CollType &coll,
                                                     std::list<typename CollType::Element> expectedElementList)
        {
            SECTION("size")
            {
                REQUIRE(coll.size() == expectedElementList.size());
                REQUIRE(coll.getSize() == expectedElementList.size());
            }

            SECTION("maxSize")
            {
                // max size should be "reasonably large". It might depend on the
                // size of the elements, though, So this test might not work for
                // all element types. But it works for the types we use in our
                // test.
                REQUIRE(coll.max_size() >= 0x00ffffff);
                REQUIRE(coll.getMaxSize() >= 0x00ffffff);
            }

            SECTION("empty")
            {
                REQUIRE(coll.empty() == (expectedElementList.size() == 0));
                REQUIRE(coll.isEmpty() == (expectedElementList.size() == 0));
            }

            SECTION("iteration")
            _verifyCollectionIteration(coll, expectedElementList);
        }

        template <class CollType>
        inline void _verifyAdditionalPositionalCollectionFunctionalityReadOnly(
            CollType &coll, std::list<typename CollType::Element> expectedElementList)
        {
            SECTION("getFirst")
            {
                if (expectedElementList.size() == 0)
                    REQUIRE_THROWS_AS(coll.getFirst(), OutOfRangeError);
                else
                    REQUIRE(_isCollectionElementEqual(coll.getFirst(), expectedElementList.front()));
            }

            SECTION("getLast")
            {
                if (expectedElementList.size() == 0)
                    REQUIRE_THROWS_AS(coll.getLast(), OutOfRangeError);
                else
                    REQUIRE(_isCollectionElementEqual(coll.getLast(), expectedElementList.back()));
            }
        }

        template <class CollType>
        inline void _verifyPositionalCollectionReadOnly(CollType &coll,
                                                        std::list<typename CollType::Element> expectedElementList)
        {
            _verifyGenericCollectionReadOnly(coll, expectedElementList);

            _verifyAdditionalPositionalCollectionFunctionalityReadOnly(coll, expectedElementList);
        }

        template <class CollType, typename... ConstructArgs>
        inline void _verifyCollectionInsertAt(CollType &coll, int insertIndex,
                                              std::list<typename CollType::Element> expectedElementList,
                                              std::initializer_list<typename CollType::Element> newElList,
                                              std::function<bool(const typename CollType::Element &)> isMovedRemnant,
                                              typename CollType::Element expectedConstructedEl,
                                              ConstructArgs... constructArgs)
        {
            std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

            typename CollType::Iterator insertIt = coll.begin();
            typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.begin();
            for (int i = 0; i < insertIndex; i++) {
                ++insertIt;
                ++expectedInsertIt;
            }

            typename CollType::Element elToAdd = *newElList.begin();

            SECTION("ref")
            {
                coll.insertAt(insertIt, elToAdd);
                newExpectedElementList.insert(expectedInsertIt, elToAdd);

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }

            SECTION("move")
            {
                newExpectedElementList.insert(expectedInsertIt, elToAdd);
                coll.insertAt(insertIt, std::move(elToAdd));

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);

                // elToAdd should not have the same value anymore
                REQUIRE(isMovedRemnant(elToAdd));
            }

            SECTION("sequence")
            {
                SECTION("iterators")
                {
                    SECTION("empty")
                    {
                        coll.insertSequenceAt(insertIt, newElList.begin(), newElList.begin());
                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.insertSequenceAt(insertIt, newElList.begin(), newElList.end());
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }
                }

                SECTION("initializer_list")
                {
                    SECTION("empty")
                    {
                        coll.insertSequenceAt(insertIt, {});

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.insertSequenceAt(insertIt, newElList);
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }
                }
            }

            SECTION("multiple copies")
            {
                SECTION("0 times")
                {
                    coll.insertMultipleCopiesAt(insertIt, 0, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                SECTION("1 times")
                {
                    coll.insertMultipleCopiesAt(insertIt, 1, *newElList.begin());
                    newExpectedElementList.insert(expectedInsertIt, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                SECTION("3 times")
                {
                    coll.insertMultipleCopiesAt(insertIt, 3, *newElList.begin());
                    newExpectedElementList.insert(expectedInsertIt, 3, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }
            }

            SECTION("new")
            {
                coll.template insertNewAt<ConstructArgs...>(insertIt, std::forward<ConstructArgs>(constructArgs)...);
                newExpectedElementList.insert(expectedInsertIt, expectedConstructedEl);

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }
        }

        template <class CollType, typename... ConstructArgs>
        inline void _verifyCollectionAdd(CollType &coll, std::list<typename CollType::Element> expectedElementList,
                                         std::initializer_list<typename CollType::Element> newElList,
                                         std::function<bool(const typename CollType::Element &)> isMovedRemnant,
                                         typename CollType::Element expectedConstructedEl,
                                         ConstructArgs... constructArgs)
        {
            std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

            typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.end();

            typename CollType::Element elToAdd = *newElList.begin();

            SECTION("ref")
            {
                coll.add(elToAdd);
                newExpectedElementList.insert(expectedInsertIt, elToAdd);

                _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
            }

            SECTION("move")
            {
                newExpectedElementList.insert(expectedInsertIt, elToAdd);
                coll.add(std::move(elToAdd));

                _verifyGenericCollectionReadOnly(coll, newExpectedElementList);

                // elToAdd should not have the same value anymore
                REQUIRE(isMovedRemnant(elToAdd));
            }

            SECTION("sequence")
            {
                SECTION("iterators")
                {
                    SECTION("empty")
                    {
                        coll.addSequence(newElList.begin(), newElList.begin());
                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.addSequence(newElList.begin(), newElList.end());
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }
                }

                SECTION("initializer_list")
                {
                    SECTION("empty")
                    {
                        coll.addSequence({});

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.addSequence(newElList);
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }
                }

                SECTION("std::list")
                {
                    SECTION("empty")
                    {
                        std::list<typename CollType::Element> sourceList;

                        coll.addSequence(sourceList);
                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        std::list<typename CollType::Element> sourceList(newElList);

                        coll.addSequence(sourceList);
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }
                }
            }

            SECTION("new")
            {
                const typename CollType::Element &addedEl = coll.addNew(std::forward<ConstructArgs>(constructArgs)...);

                REQUIRE(_isCollectionElementEqual(addedEl, expectedConstructedEl));

                newExpectedElementList.insert(expectedInsertIt, expectedConstructedEl);

                _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
            }
        }

        template <class CollType>
        inline void _verifyCollectionAddMultipleCopies(CollType &coll,
                                                       std::list<typename CollType::Element> expectedElementList,
                                                       std::initializer_list<typename CollType::Element> newElList)
        {
            std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

            typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.end();

            typename CollType::Element elToAdd = *newElList.begin();

            SECTION("0 times")
            {
                coll.addMultipleCopies(0, *newElList.begin());

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }

            SECTION("1 times")
            {
                coll.addMultipleCopies(1, *newElList.begin());
                newExpectedElementList.insert(expectedInsertIt, *newElList.begin());

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }

            SECTION("3 times")
            {
                coll.addMultipleCopies(3, *newElList.begin());
                newExpectedElementList.insert(expectedInsertIt, 3, *newElList.begin());

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }
        }

        template <class CollType, typename... ConstructArgs>
        inline void
        _verifyCollectionInsertAtBegin(CollType &coll, std::list<typename CollType::Element> expectedElementList,
                                       std::initializer_list<typename CollType::Element> newElList,
                                       std::function<bool(const typename CollType::Element &)> isMovedRemnant,
                                       typename CollType::Element expectedConstructedEl, ConstructArgs... constructArgs)
        {
            std::list<typename CollType::Element> newExpectedElementList = expectedElementList;

            typename std::list<typename CollType::Element>::iterator expectedInsertIt = newExpectedElementList.begin();

            typename CollType::Element elToAdd = *newElList.begin();

            SECTION("ref")
            {
                coll.insertAtBegin(elToAdd);
                newExpectedElementList.insert(expectedInsertIt, elToAdd);

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }

            SECTION("move")
            {
                newExpectedElementList.insert(expectedInsertIt, elToAdd);
                coll.insertAtBegin(std::move(elToAdd));

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);

                // elToAdd should not have the same value anymore
                REQUIRE(isMovedRemnant(elToAdd));
            }

            SECTION("sequence")
            {
                SECTION("iterators")
                {
                    SECTION("empty")
                    {
                        coll.insertSequenceAtBegin(newElList.begin(), newElList.begin());
                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.insertSequenceAtBegin(newElList.begin(), newElList.end());
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }
                }

                SECTION("initializer_list")
                {
                    SECTION("empty")
                    {
                        coll.insertSequenceAtBegin({});

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("non-empty")
                    {
                        coll.insertSequenceAtBegin(newElList);
                        newExpectedElementList.insert(expectedInsertIt, newElList.begin(), newElList.end());

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }
                }
            }

            SECTION("multiple copies")
            {
                SECTION("0 times")
                {
                    coll.insertMultipleCopiesAtBegin(0, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                SECTION("1 times")
                {
                    coll.insertMultipleCopiesAtBegin(1, *newElList.begin());
                    newExpectedElementList.insert(expectedInsertIt, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                SECTION("3 times")
                {
                    coll.insertMultipleCopiesAtBegin(3, *newElList.begin());
                    newExpectedElementList.insert(expectedInsertIt, 3, *newElList.begin());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }
            }

            SECTION("new")
            {
                coll.insertNewAtBegin(std::forward<ConstructArgs>(constructArgs)...);
                newExpectedElementList.insert(expectedInsertIt, expectedConstructedEl);

                _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
            }
        }

        template <class IteratorType, typename ElementType>
        IteratorType _findCollectionElement(IteratorType beginIt, IteratorType endIt, const ElementType &el)
        {
            for (auto it = beginIt; it != endIt; ++it) {
                if (_isCollectionElementEqual(*it, el))
                    return it;
            }

            return endIt;
        }

        template <class CollType> void _removeCollectionElement(CollType &coll, const typename CollType::value_type &el)
        {
            int removedCount = 0;

            while (true) {
                auto it = _findCollectionElement(coll.begin(), coll.end(), el);
                if (it == coll.end())
                    break;

                coll.erase(it);

                removedCount++;
            }

            REQUIRE(removedCount > 0);
        }

        template <class CollType>
        inline void _verifyCollectionRemoveSpecificSection(CollType &coll, int removeBeginIndex, int removeCount,
                                                           std::list<typename CollType::Element> expectedElementList)
        {
            auto removeBegin = coll.begin();
            for (int i = 0; i < removeBeginIndex && removeBegin != coll.end(); i++)
                ++removeBegin;

            auto removeEnd = removeBegin;
            for (int i = 0; i < removeCount && removeEnd != coll.end(); i++)
                ++removeEnd;

            for (auto it = removeBegin; it != removeEnd; ++it)
                _removeCollectionElement(expectedElementList, *it);

            coll.removeSection(removeBegin, removeEnd);

            _verifyGenericCollectionReadOnly(coll, expectedElementList);
        }

        template <class CollType>
        inline void
        _verifyCollectionRemoveSectionOfSize(CollType &coll, int removeCount,
                                             const std::list<typename CollType::Element> &expectedElementList)
        {
            SECTION("from begin")
            _verifyCollectionRemoveSpecificSection(coll, 0, removeCount, expectedElementList);

            if (expectedElementList.size() >= (size_t)removeCount) {
                SECTION("to end")
                _verifyCollectionRemoveSpecificSection(coll, ((int)coll.size()) - removeCount, removeCount,
                                                       expectedElementList);
            }

            if (expectedElementList.size() >= (size_t)removeCount + 1) {
                SECTION("from middle")
                _verifyCollectionRemoveSpecificSection(coll, 1, removeCount, expectedElementList);
            }
        }

        template <class CollType>
        inline void _verifyCollectionRemoveSection(CollType &coll,
                                                   const std::list<typename CollType::Element> &expectedElementList)
        {
            SECTION("0 elements")
            _verifyCollectionRemoveSectionOfSize(coll, 0, expectedElementList);

            if (expectedElementList.size() >= 1) {
                SECTION("1 element")
                _verifyCollectionRemoveSectionOfSize(coll, 1, expectedElementList);
            }

            if (expectedElementList.size() >= 2) {
                SECTION("2 elements")
                _verifyCollectionRemoveSectionOfSize(coll, 2, expectedElementList);
            }

            SECTION("all elements")
            _verifyCollectionRemoveSectionOfSize(coll, (int)coll.size(), expectedElementList);
        }

        template <class CollType>
        inline void verifyCollectionFindAndRemoveAtIndex(CollType &coll, int removeIndex,
                                                         std::list<typename CollType::Element> expectedElementList)
        {
            auto removeIt = coll.begin();

            for (int i = 0; removeIt != coll.end() && i < removeIndex; i++)
                ++removeIt;

            // sanity check
            REQUIRE(removeIt != coll.end());

            typename CollType::Element elToRemove = *removeIt;

            _removeCollectionElement(expectedElementList, elToRemove);

            coll.findAndRemove(elToRemove);

            _verifyGenericCollectionReadOnly(coll, expectedElementList);
        }

        template <class CollType>
        inline void _verifyCollectionFindAndRemove(CollType &coll,
                                                   const std::list<typename CollType::Element> &expectedElementList,
                                                   typename CollType::Element elNotInList)
        {
            if (!expectedElementList.empty()) {
                SECTION("first")
                verifyCollectionFindAndRemoveAtIndex(coll, 0, expectedElementList);

                SECTION("last")
                verifyCollectionFindAndRemoveAtIndex(coll, ((int)expectedElementList.size()) - 1, expectedElementList);
            }

            if (expectedElementList.size() >= 2) {
                SECTION("middle")
                verifyCollectionFindAndRemoveAtIndex(coll, 1, expectedElementList);
            }

            SECTION("not found")
            {
                coll.findAndRemove(elNotInList);

                _verifyGenericCollectionReadOnly(coll, expectedElementList);
            }
        }

        template <class CollType>
        inline void
        verifyCollectionFindCustomAndRemoveAtIndex(CollType &coll, int removeIndex,
                                                   std::list<typename CollType::Element> expectedElementList)
        {
            auto removeIt = coll.begin();

            for (int i = 0; removeIt != coll.end() && i < removeIndex; i++)
                ++removeIt;

            typename CollType::Element elToRemove = *removeIt;

            _removeCollectionElement(expectedElementList, elToRemove);

            coll.findCustomAndRemove([elToRemove](const typename CollType::Iterator &it) {
                return _isCollectionElementEqual(*it, elToRemove);
            });

            _verifyGenericCollectionReadOnly(coll, expectedElementList);
        }

        template <class CollType>
        inline void
        verifyCollectionFindCustomAndRemove(CollType &coll,
                                            const std::list<typename CollType::Element> &expectedElementList)
        {
            if (!expectedElementList.empty()) {
                SECTION("first")
                verifyCollectionFindCustomAndRemoveAtIndex(coll, 0, expectedElementList);

                SECTION("last")
                verifyCollectionFindCustomAndRemoveAtIndex(coll, ((int)expectedElementList.size()) - 1,
                                                           expectedElementList);
            }

            if (expectedElementList.size() >= 2) {
                SECTION("middle")
                verifyCollectionFindCustomAndRemoveAtIndex(coll, 1, expectedElementList);
            }

            SECTION("not found")
            {
                coll.findCustomAndRemove([](const typename CollType::Iterator &it) { return false; });

                _verifyGenericCollectionReadOnly(coll, expectedElementList);
            }
        }

        template <class CollType> typename CollType::iterator _getLastElementIterator(CollType &coll)
        {
            // find the last element. Note that we cannot use --end() here
            // because the collection might not support backwards iteration.
            auto lastElementIt = coll.begin();
            if (lastElementIt != coll.end()) {
                auto nextIt = lastElementIt;
                while (true) {
                    ++nextIt;
                    if (nextIt == coll.end())
                        break;

                    lastElementIt = nextIt;
                }
            }

            return lastElementIt;
        }

        template <class CollType, typename... ConstructArgs>
        inline void _verifyGenericCollection(CollType &coll, std::list<typename CollType::Element> expectedElementList,
                                             std::initializer_list<typename CollType::Element> newElList,
                                             std::function<bool(const typename CollType::Element &)> isMovedRemnant,
                                             typename CollType::Element expectedConstructedEl,
                                             ConstructArgs... constructArgs)
        {
            _verifyGenericCollectionReadOnly(coll, expectedElementList);

            std::list<typename CollType::Element> newExpectedElementList(expectedElementList);

            SECTION("clear")
            {
                coll.clear();

                _verifyGenericCollectionReadOnly(coll, {});
            }

            SECTION("operator=")
            {
                SECTION("other collection")
                {
                    SECTION("empty")
                    {
                        CollType otherColl;

                        SECTION("ref")
                        {
                            coll = otherColl;
                            _verifyGenericCollectionReadOnly(coll, {});
                        }

                        SECTION("move")
                        {
                            coll = std::move(otherColl);
                            _verifyGenericCollectionReadOnly(coll, {});

                            REQUIRE(otherColl.size() == 0);
                        }
                    }

                    SECTION("non-empty")
                    {
                        CollType otherColl;
                        otherColl.add(expectedConstructedEl);

                        SECTION("ref")
                        {
                            coll = otherColl;
                            _verifyGenericCollectionReadOnly(coll, {expectedConstructedEl});
                        }

                        SECTION("move")
                        {
                            coll = std::move(otherColl);
                            _verifyGenericCollectionReadOnly(coll, {expectedConstructedEl});

                            REQUIRE(otherColl.size() == 0);
                        }
                    }
                }

                SECTION("initializer_list")
                {
                    SECTION("empty")
                    {
                        coll = {};
                        _verifyGenericCollectionReadOnly(coll, {});
                    }

                    SECTION("non-empty")
                    {
                        coll = {expectedConstructedEl};

                        _verifyGenericCollectionReadOnly(coll, {expectedConstructedEl});
                    }
                }
            }

            if (expectedElementList.size() > 0) {
                SECTION("removeAt")
                {
                    SECTION("first")
                    {
                        _removeCollectionElement(newExpectedElementList, *coll.begin());

                        coll.removeAt(coll.begin());

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }

                    SECTION("last")
                    {
                        auto removeIt = _getLastElementIterator(coll);
                        _removeCollectionElement(newExpectedElementList, *removeIt);
                        coll.removeAt(removeIt);

                        _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                    }

                    if (expectedElementList.size() > 1) {
                        SECTION("mid")
                        {
                            auto removeIt = coll.begin();
                            ++removeIt;

                            _removeCollectionElement(newExpectedElementList, *removeIt);
                            coll.removeAt(removeIt);

                            _verifyGenericCollectionReadOnly(coll, newExpectedElementList);
                        }
                    }
                }
            }

            SECTION("removeSection")
            _verifyCollectionRemoveSection(coll, expectedElementList);

            SECTION("findCustomAndRemove")
            verifyCollectionFindCustomAndRemove(coll, expectedElementList);

            SECTION("add")
            _verifyCollectionAdd(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl,
                                 std::forward<ConstructArgs>(constructArgs)...);
        }

        template <class CollType, typename... ConstructArgs>
        inline void
        _verifyPositionalCollection(CollType &coll, std::list<typename CollType::Element> expectedElementList,
                                    std::initializer_list<typename CollType::Element> newElList,
                                    std::function<bool(const typename CollType::Element &)> isMovedRemnant,
                                    typename CollType::Element expectedConstructedEl, ConstructArgs... constructArgs)
        {
            _verifyGenericCollection(coll, expectedElementList, newElList, isMovedRemnant, expectedConstructedEl,
                                     std::forward<ConstructArgs>(constructArgs)...);

            _verifyAdditionalPositionalCollectionFunctionalityReadOnly(coll, expectedElementList);

            std::list<typename CollType::Element> newExpectedElementList(expectedElementList);

            if (expectedElementList.size() > 0) {
                SECTION("removeFirst")
                {
                    coll.removeFirst();
                    newExpectedElementList.erase(newExpectedElementList.begin());
                }

                SECTION("removeLast")
                {
                    coll.removeLast();
                    newExpectedElementList.erase(--newExpectedElementList.end());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }
            }

            SECTION("addMultipleCopies") { _verifyCollectionAddMultipleCopies(coll, expectedElementList, newElList); }

            SECTION("insertAt")
            {
                SECTION("at begin")
                _verifyCollectionInsertAt(coll, 0, expectedElementList, newElList, isMovedRemnant,
                                          expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)...);

                if (expectedElementList.size() > 1) {
                    SECTION("in middle")
                    _verifyCollectionInsertAt(coll, 1, expectedElementList, newElList, isMovedRemnant,
                                              expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)...);
                }

                SECTION("at end")
                _verifyCollectionInsertAt(coll, (int)expectedElementList.size(), expectedElementList, newElList,
                                          isMovedRemnant, expectedConstructedEl,
                                          std::forward<ConstructArgs>(constructArgs)...);
            }

            SECTION("insertAtBegin")
            {
                _verifyCollectionInsertAtBegin(coll, expectedElementList, newElList, isMovedRemnant,
                                               expectedConstructedEl, std::forward<ConstructArgs>(constructArgs)...);
            }

            SECTION("setSize")
            {
                SECTION("to 0")
                {
                    coll.setSize(0);
                    newExpectedElementList.clear();

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                if (expectedElementList.size() > 0) {
                    SECTION("to one less")
                    {
                        coll.setSize(coll.size() - 1);
                        newExpectedElementList.erase(--newExpectedElementList.end());

                        _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                    }
                }

                SECTION("to one more")
                {
                    coll.setSize(coll.size() + 1);
                    newExpectedElementList.push_back(typename CollType::Element());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }

                SECTION("to 100 more")
                {
                    coll.setSize(coll.size() + 100);

                    for (int i = 0; i < 100; i++)
                        newExpectedElementList.push_back(typename CollType::Element());

                    _verifyPositionalCollectionReadOnly(coll, newExpectedElementList);
                }
            }

            SECTION("toString")
            {
                String expected;

                if (coll.isEmpty())
                    expected = "[]";
                else {
                    expected = "[ ";

                    bool first = true;
                    for (auto &el : coll) {
                        if (!first)
                            expected += ",\n  ";
                        expected += bdn::toString(el);

                        first = false;
                    }

                    expected += " ]";
                }

                String actual = toString(coll);
                REQUIRE(actual == expected);
            }
        }

        template <class CollType, class ItType>
        inline void _verifyIndexedCollectionAccess(CollType &coll, int index, ItType expectedElementIt)
        {
            if (index != coll.size()) {
                SECTION("atIndex")
                REQUIRE(&coll.atIndex(index) == &*expectedElementIt);

                SECTION("operator[]")
                REQUIRE(&coll[index] == &*expectedElementIt);
            }

            SECTION("indexToIterator")
            REQUIRE(coll.indexToIterator(index) == expectedElementIt);

            SECTION("iteratorToIndex")
            REQUIRE(coll.iteratorToIndex(expectedElementIt) == index);
        }

        template <class CollType> inline void _testCollectionIndexedAccess(CollType &coll)
        {
            SECTION("0")
            _verifyIndexedCollectionAccess(coll, 0, coll.begin());

            SECTION("1")
            _verifyIndexedCollectionAccess(coll, 1, ++coll.begin());

            SECTION("last")
            _verifyIndexedCollectionAccess(coll, ((int)coll.size()) - 1, --coll.end());

            SECTION("end")
            _verifyIndexedCollectionAccess(coll, (int)coll.size(), coll.end());
        }

        template <class CollType> inline void _testGenericCollectionPrepareForSize(CollType &coll)
        {
            std::list<typename CollType::Element> origElements(coll.begin(), coll.end());

            typename CollType::Size prepareFor = 0;

            SECTION("0")
            prepareFor = 0;

            SECTION("1")
            prepareFor = 1;

            SECTION("size")
            prepareFor = coll.size();

            SECTION("size+1")
            prepareFor = coll.size() + 1;

            SECTION("size+1000")
            prepareFor = coll.size() + 1000;

            coll.prepareForSize(prepareFor);

            // prepareForSize should NEVER change the collection contents
            _verifyGenericCollectionReadOnly(coll, origElements);
        }

        template <class CollType, class ItType>
        inline void _verifyCollectionFindVariant(CollType &coll, typename CollType::Element toFind,
                                                 ItType expectedResult)
        {
            SECTION("find")
            {
                ItType it = coll.find(toFind);
                REQUIRE(it == expectedResult);
            }

            SECTION("contains")
            {
                bool expectedContains = (expectedResult != coll.end());

                REQUIRE(coll.contains(toFind) == expectedContains);
            }
        }

        template <class CollType>
        inline void _verifyCollectionFind(CollType &coll, typename CollType::Element toFind,
                                          typename CollType::Iterator expectedResult)
        {
            SECTION("normal")
            _verifyCollectionFindVariant<CollType, typename CollType::Iterator>(coll, toFind, expectedResult);

            SECTION("const")
            {
                const CollType &constColl(coll);

                // convert the const iterator to non-const
                typename CollType::ConstIterator constExpectedResult = constColl.begin();
                std::advance(constExpectedResult, std::distance(coll.begin(), expectedResult));

                _verifyCollectionFindVariant<const CollType, typename CollType::ConstIterator>(constColl, toFind,
                                                                                               constExpectedResult);
            }
        }

        template <class CollType, class ItType>
        inline void _verifyCollectionFindWithStartPosVariant(CollType &coll, typename CollType::Element toFind,
                                                             ItType expectedResult)
        {
            SECTION("no startPos")
            {
                ItType it = coll.find(toFind);
                REQUIRE(it == expectedResult);
            }

            SECTION("with startPos")
            {
                SECTION("begin")
                {
                    ItType it = coll.find(toFind, coll.begin());
                    REQUIRE(it == expectedResult);
                }

                SECTION("expected find position")
                {
                    ItType it = coll.find(toFind, expectedResult);
                    REQUIRE(it == expectedResult);
                }

                if (expectedResult != coll.end()) {
                    SECTION("one after expected find position")
                    {
                        ItType startPos = expectedResult;
                        ++startPos;

                        ItType it = coll.find(toFind, startPos);
                        REQUIRE(it != expectedResult);
                    }
                }

                SECTION("end")
                {
                    ItType it = coll.find(toFind, coll.end());
                    REQUIRE(it == coll.end());
                }
            }

            SECTION("contains")
            {
                bool expectedContains = (expectedResult != coll.end());

                REQUIRE(coll.contains(toFind) == expectedContains);
            }
        }

        template <class CollType>
        inline void _verifyCollectionFindWithStartPos(CollType &coll, typename CollType::Element toFind,
                                                      typename CollType::Iterator expectedResult)
        {
            SECTION("normal")
            _verifyCollectionFindWithStartPosVariant<CollType, typename CollType::Iterator>(coll, toFind,
                                                                                            expectedResult);

            SECTION("const")
            {
                const CollType &constColl(coll);

                // convert the const iterator to non-const
                typename CollType::ConstIterator constExpectedResult = constColl.begin();
                std::advance(constExpectedResult, std::distance(coll.begin(), expectedResult));

                _verifyCollectionFindWithStartPosVariant<const CollType, typename CollType::ConstIterator>(
                    constColl, toFind, constExpectedResult);
            }
        }

        template <class CollType, class ItType>
        inline void verifyReverseFindForSpecificVariant(CollType &coll, typename CollType::Element toFind,
                                                        ItType expectedResult, ItType posAfterExpectedPos)
        {
            SECTION("no startPos")
            {
                ItType it = coll.reverseFind(toFind);
                REQUIRE(it == expectedResult);
            }

            SECTION("with startPos")
            {
                SECTION("begin")
                {
                    ItType it = coll.reverseFind(toFind, coll.begin());

                    if (!coll.isEmpty() && coll.getFirst() == toFind)
                        REQUIRE(it == coll.begin());
                    else
                        REQUIRE(it == coll.end());
                }

                SECTION("expected find position")
                {
                    ItType it = coll.reverseFind(toFind, expectedResult);
                    REQUIRE(it == expectedResult);
                }

                if (expectedResult != coll.begin()) {
                    SECTION("after expected find position")
                    {
                        ItType it = coll.reverseFind(toFind, posAfterExpectedPos);

                        if (expectedResult == coll.end())
                            REQUIRE(it == expectedResult);
                        else
                            REQUIRE(it != expectedResult);
                    }
                }

                if (!coll.isEmpty()) {
                    SECTION("last")
                    {
                        ItType it = coll.reverseFind(toFind, --coll.end());
                        REQUIRE(it == expectedResult);
                    }
                }

                SECTION("end")
                {
                    ItType it = coll.reverseFind(toFind, coll.end());
                    REQUIRE(it == expectedResult);
                }
            }
        }

        template <class CollType>
        inline void verifyReverseFind(CollType &coll, typename CollType::Element toFind,
                                      typename CollType::Iterator expectedResult)
        {
            SECTION("normal")
            {
                typename CollType::Iterator posAfterExpectedPos = expectedResult;
                if (expectedResult != coll.begin())
                    --posAfterExpectedPos;

                verifyReverseFindForSpecificVariant<CollType, typename CollType::Iterator>(coll, toFind, expectedResult,
                                                                                           posAfterExpectedPos);
            }

            SECTION("const")
            {
                const CollType &constColl(coll);

                // convert the const iterator to non-const
                typename CollType::ConstIterator constExpectedResult = constColl.begin();
                std::advance(constExpectedResult, std::distance(coll.begin(), expectedResult));

                typename CollType::ConstIterator posAfterExpectedPos = constExpectedResult;
                if (constExpectedResult != constColl.begin())
                    --posAfterExpectedPos;

                verifyReverseFindForSpecificVariant<const CollType, typename CollType::ConstIterator>(
                    constColl, toFind, constExpectedResult, posAfterExpectedPos);
            }
        }

        template <class COLL, typename FIND_FUNC, typename CONST_FIND_FUNC>
        inline void verifyFindCustomWithFindFunc(COLL &coll, FIND_FUNC findFunc, CONST_FIND_FUNC constFindFunc,
                                                 typename COLL::Iterator expectedResult)
        {
            SECTION("normal")
            {
                typename COLL::Iterator it = findFunc(coll);
                REQUIRE(it == expectedResult);
            }

            SECTION("const")
            {
                const COLL &constColl(coll);

                typename COLL::ConstIterator it = constFindFunc(constColl);

                if (expectedResult == coll.end())
                    REQUIRE(it == constColl.end());
                else {
                    REQUIRE(&*it == &*expectedResult);
                }
            }
        }

        template <class CollType>
        inline void verifyFindCustom(CollType &coll,
                                     std::function<bool(const typename CollType::ConstIterator &)> conditionFunc,
                                     typename CollType::Iterator expectedResult)
        {
            SECTION("without startPos")
            {
                verifyFindCustomWithFindFunc(
                    coll, [conditionFunc](CollType &coll) { return coll.findCustom(conditionFunc); },
                    [conditionFunc](const CollType &constColl) { return constColl.findCustom(conditionFunc); },
                    expectedResult);
            }

            SECTION("with startPos")
            {
                SECTION("begin")
                {
                    verifyFindCustomWithFindFunc(
                        coll, [conditionFunc](CollType &coll) { return coll.findCustom(conditionFunc, coll.begin()); },
                        [conditionFunc](const CollType &constColl) {
                            return constColl.findCustom(conditionFunc, constColl.begin());
                        },
                        expectedResult);
                }

                SECTION("hitPos")
                {
                    verifyFindCustomWithFindFunc(coll,
                                                 [conditionFunc, expectedResult](CollType &coll) {
                                                     return coll.findCustom(conditionFunc, expectedResult);
                                                 },
                                                 [conditionFunc, expectedResult](const CollType &constColl) {
                                                     return constColl.findCustom(conditionFunc, expectedResult);
                                                 },
                                                 expectedResult);
                }

                if (expectedResult != coll.end()) {
                    SECTION("after hitPos")
                    {
                        typename CollType::Iterator startPos = expectedResult;
                        ++startPos;

                        typename CollType::Iterator expectedResultAfterStartPos = startPos;
                        while (expectedResultAfterStartPos != coll.end()) {
                            if (conditionFunc(expectedResultAfterStartPos))
                                break;

                            ++expectedResultAfterStartPos;
                        }

                        verifyFindCustomWithFindFunc(coll,
                                                     [conditionFunc, startPos](CollType &coll) {
                                                         return coll.findCustom(conditionFunc, startPos);
                                                     },
                                                     [conditionFunc, startPos](const CollType &constColl) {
                                                         return constColl.findCustom(conditionFunc, startPos);
                                                     },
                                                     expectedResultAfterStartPos);
                    }
                }
            }
        }

        template <class CollType>
        inline void verifyReverseFindCustom(CollType &coll,
                                            std::function<bool(const typename CollType::ConstIterator &)> conditionFunc,
                                            typename CollType::Iterator expectedResult)
        {
            SECTION("normal")
            {
                typename CollType::Iterator it = coll.reverseFindCustom(conditionFunc);
                REQUIRE(it == expectedResult);
            }

            SECTION("const")
            {
                const CollType &constColl(coll);

                typename CollType::ConstIterator it = constColl.reverseFindCustom(conditionFunc);

                if (expectedResult == coll.end())
                    REQUIRE(it == constColl.end());
                else {
                    REQUIRE(&*it == &*expectedResult);
                }
            }
        }

        template <class CollType, typename FindCaller>
        inline void _verifyCollectionFindXWithCaller(CollType &coll, const typename CollType::Element &elNotInColl,
                                                     FindCaller findCaller)
        {
            for (auto &elToFind : coll) {
                auto finder = findCaller(coll, elToFind);

                REQUIRE(finder.begin() != finder.end());

                auto finderIt = finder.begin();

                auto baseIt = coll.begin();
                while (baseIt != coll.end()) {
                    auto &el = *baseIt;

                    if (elToFind == el) {
                        REQUIRE(finderIt != finder.end());
                        REQUIRE(*finderIt == el);
                        REQUIRE(finderIt.getBaseIterator() == baseIt);

                        // verify that the BaseIterator typedef is either
                        // Iterator or ConstIterator
                        REQUIRE(
                            (typeid(typename decltype(finder)::BaseIterator) == typeid(typename CollType::Iterator) ||
                             typeid(typename decltype(finder)::BaseIterator) ==
                                 typeid(typename CollType::ConstIterator)));

                        // verify that implicit conversion works
                        typename decltype(finder)::BaseIterator conversionResultBaseIt = finderIt;
                        REQUIRE(conversionResultBaseIt == baseIt);

                        ++finderIt;
                    }

                    ++baseIt;
                }

                REQUIRE(finderIt == finder.end());

                // verify that the iterators of the returned finder objects
                // support assignment of a base iterator.
                for (int testPosition = 0;; testPosition++) {
                    finderIt = finder.begin();

                    for (int position = 0; position < testPosition; position++) {
                        if (finderIt == finder.end())
                            break;

                        ++finderIt;
                    }

                    if (finderIt == finder.end())
                        break;

                    // now we have a finder iterator at the desired position.
                    // Get its base iterator.
                    auto baseIt = finderIt.getBaseIterator();

                    // advance to the following base element.
                    ++baseIt;

                    // assign this element to the finder element.
                    finderIt.advanceAfterRemoval(baseIt);

                    // get the value of the base iterator that we expect the
                    // finder iterator to have
                    auto expectedBaseIt = baseIt;
                    while (expectedBaseIt != coll.end() && *expectedBaseIt != elToFind)
                        ++expectedBaseIt;

                    REQUIRE(finderIt.getBaseIterator() == expectedBaseIt);

                    if (expectedBaseIt == coll.end())
                        REQUIRE(finderIt == finder.end());
                    else
                        REQUIRE(finderIt != finder.end());
                }
            }

            auto finder = findCaller(coll, elNotInColl);
            REQUIRE(finder.begin() == finder.end());
        }

        template <class CollType, typename FindCaller>
        inline void _testCollectionFindXWithCaller(std::initializer_list<typename CollType::Element> elements,
                                                   const typename CollType::Element &elNotInColl, FindCaller findCaller)
        {
            SECTION("empty")
            {
                CollType coll;

                _verifyCollectionFindXWithCaller(coll, elNotInColl, findCaller);
            }

            SECTION("not empty")
            {
                CollType coll{elements};

                _verifyCollectionFindXWithCaller(coll, elNotInColl, findCaller);
            }

            SECTION("not empty with duplicates")
            {
                // double each second element
                bool doubleEl = false;
                std::list<typename CollType::Element> consecutiveDuplicates;
                for (auto &el : elements) {
                    consecutiveDuplicates.push_back(el);
                    if (doubleEl)
                        consecutiveDuplicates.push_back(el);

                    doubleEl = !doubleEl;
                }

                // now we have consecutive duplicates
                // Append the whole list to itself to
                // add some non-consecutive duplicates.
                std::list<typename CollType::Element> consecutiveAndNonConsecutiveDuplicates(consecutiveDuplicates);
                consecutiveAndNonConsecutiveDuplicates.insert(consecutiveAndNonConsecutiveDuplicates.end(),
                                                              consecutiveDuplicates.begin(),
                                                              consecutiveDuplicates.end());

                // note that collections that do not support duplicate elements
                // will simply filter out the duplicates and have only unique
                // elements as a result. That is fine since for those
                // collections there is no duplicate handling to test.

                CollType coll(consecutiveAndNonConsecutiveDuplicates.begin(),
                              consecutiveAndNonConsecutiveDuplicates.end());

                _verifyCollectionFindXWithCaller(coll, elNotInColl, findCaller);
            }
        }

        template <class CollType>
        inline void _testCollectionFind(std::initializer_list<typename CollType::Element> elements,
                                        const typename CollType::Element &elNotInColl)
        {
            SECTION("find")
            {
                CollType coll;

                SECTION("empty")
                {
                    SECTION("not found")
                    _verifyCollectionFind(coll, elNotInColl, coll.end());
                }

                SECTION("not empty")
                {
                    coll.addSequence(elements);

                    SECTION("first")
                    _verifyCollectionFind(coll, *coll.begin(), coll.begin());

                    SECTION("last")
                    _verifyCollectionFind(coll, *_getLastElementIterator(coll), _getLastElementIterator(coll));

                    SECTION("second")
                    _verifyCollectionFind(coll, *++coll.begin(), ++coll.begin());

                    SECTION("not found")
                    _verifyCollectionFind(coll, elNotInColl, coll.end());
                }
            }

            SECTION("findCustom")
            {
                CollType coll;

                SECTION("empty")
                {
                    SECTION("not found")
                    {
                        verifyFindCustom(coll, [](const typename CollType::ConstIterator &it) { return false; },
                                         coll.end());
                    }
                }

                SECTION("not empty")
                {
                    coll.addSequence(elements);

                    SECTION("first")
                    {
                        typename CollType::Element toFind = *coll.begin();

                        verifyFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            coll.begin());
                    }

                    SECTION("last")
                    {
                        typename CollType::Element toFind = *_getLastElementIterator(coll);

                        verifyFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            _getLastElementIterator(coll));
                    }

                    SECTION("second")
                    {
                        typename CollType::Element toFind = *++coll.begin();

                        verifyFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            ++coll.begin());
                    }

                    SECTION("not found")
                    {
                        verifyFindCustom(coll, [](const typename CollType::ConstIterator &it) { return false; },
                                         coll.end());
                    }
                }
            }

            SECTION("findAll(element)")
            {
                SECTION("non const")
                {
                    _testCollectionFindXWithCaller<CollType>(
                        elements, elNotInColl, [](CollType &coll, const typename CollType::Element &elToFind) {
                            return coll.findAll(elToFind);
                        });
                }

                SECTION("const")
                {
                    _testCollectionFindXWithCaller<CollType>(
                        elements, elNotInColl, [](CollType &coll, const typename CollType::Element &elToFind) {
                            return ((const CollType &)coll).findAll(elToFind);
                        });
                }
            }

            SECTION("findAllCustom")
            {
                SECTION("non const")
                {
                    _testCollectionFindXWithCaller<CollType>(
                        elements, elNotInColl, [](CollType &coll, const typename CollType::Element &elToFind) {
                            return coll.findAllCustom(
                                [elToFind](const typename CollType::Iterator &it) { return (*it) == elToFind; });
                        });
                }

                SECTION("const")
                {
                    _testCollectionFindXWithCaller<CollType>(
                        elements, elNotInColl, [](CollType &coll, const typename CollType::Element &elToFind) {
                            return ((const CollType &)coll)
                                .findAllCustom([elToFind](const typename CollType::ConstIterator &it) {
                                    return (*it) == elToFind;
                                });
                        });
                }
            }
        }

        template <class CollType>
        inline void _testCollectionFindWithStartPos(std::initializer_list<typename CollType::Element> elements,
                                                    const typename CollType::Element &elNotInColl)
        {
            _testCollectionFind<CollType>(elements, elNotInColl);

            SECTION("find with start pos")
            {
                CollType coll;

                SECTION("empty")
                {
                    SECTION("not found")
                    _verifyCollectionFindWithStartPos(coll, elNotInColl, coll.end());
                }

                SECTION("not empty")
                {
                    coll.addSequence(elements);

                    // the first element must equal the third one for these
                    // tests to work
                    REQUIRE(*coll.begin() == *(++(++coll.begin())));

                    SECTION("first")
                    _verifyCollectionFindWithStartPos(coll, *coll.begin(), coll.begin());

                    SECTION("last")
                    _verifyCollectionFindWithStartPos(coll, *--coll.end(), --coll.end());

                    SECTION("second")
                    _verifyCollectionFindWithStartPos(coll, *++coll.begin(), ++coll.begin());

                    SECTION("not found")
                    _verifyCollectionFindWithStartPos(coll, elNotInColl, coll.end());
                }
            }
        }

        template <class CollType>
        inline void _testCollectionReverseFind(std::initializer_list<typename CollType::Element> elements,
                                               const typename CollType::Element &elNotInColl)
        {
            SECTION("reverseFind")
            {
                CollType coll;

                SECTION("empty")
                {
                    SECTION("not found")
                    verifyReverseFind(coll, elNotInColl, coll.end());
                }

                SECTION("not empty")
                {
                    coll.addSequence(elements);

                    // the first element must equal the third one for these
                    // tests to work
                    REQUIRE(*coll.begin() == *(++(++coll.begin())));

                    SECTION("first")
                    {
                        // the first and the third element compare equal. So we
                        // expect to find the third one
                        verifyReverseFind(coll, *coll.begin(), ++(++coll.begin()));
                    }

                    SECTION("last")
                    verifyReverseFind(coll, *--coll.end(), --coll.end());

                    SECTION("second")
                    verifyReverseFind(coll, *++coll.begin(), ++coll.begin());

                    SECTION("not found")
                    verifyReverseFind(coll, elNotInColl, coll.end());
                }
            }

            SECTION("reverseFindCustom")
            {
                CollType coll;

                SECTION("empty")
                {
                    SECTION("not found")
                    {
                        verifyReverseFindCustom(coll, [](const typename CollType::ConstIterator &it) { return false; },
                                                coll.end());
                    }
                }

                SECTION("not empty")
                {
                    coll.addSequence(elements);

                    // the first element must equal the third one for these
                    // tests to work
                    REQUIRE(*coll.begin() == *(++(++coll.begin())));

                    SECTION("first")
                    {
                        typename CollType::Element toFind = *coll.begin();

                        verifyReverseFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            // the first element in the sequence is also the
                            // third one. So we should find that
                            ++(++coll.begin()));
                    }

                    SECTION("last")
                    {
                        typename CollType::Element toFind = *--coll.end();

                        verifyReverseFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            --coll.end());
                    }

                    SECTION("second")
                    {
                        typename CollType::Element toFind = *++coll.begin();

                        verifyReverseFindCustom(
                            coll, [toFind](const typename CollType::ConstIterator &it) { return ((*it) == toFind); },
                            ++coll.begin());
                    }

                    SECTION("not found")
                    {
                        verifyReverseFindCustom(coll, [](const typename CollType::ConstIterator &it) { return false; },
                                                coll.end());
                    }
                }
            }
        }

        template <class CollType>
        inline void _verifyCollectionSortResult(CollType &coll,
                                                std::initializer_list<typename CollType::Element> elements,
                                                bool expectInvertedOrder = false)
        {
            // verify that the elements are all in the correct order
            auto it = coll.begin();
            while (it != coll.end()) {
                auto nextIt = it;
                ++nextIt;

                if (nextIt == coll.end())
                    break;

                if (expectInvertedOrder)
                    REQUIRE(!(*it < *nextIt));
                else
                    REQUIRE(!(*nextIt < *it));

                ++it;
            }

            // verify that all elements are still in the collection
            REQUIRE(coll.size() == elements.size());

            for (auto &el : elements)
                REQUIRE(coll.find(el) != coll.end());
        }

        template <class CollType>
        inline void _testCollectionSort(std::initializer_list<typename CollType::Element> elements,
                                        std::initializer_list<typename CollType::Element> stableSortedElements)
        {
            SECTION("empty")
            {
                CollType coll;

                SECTION("sort")
                {
                    coll.sort();
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("sort(ascending)")
                {
                    coll.sort(bdn::ascending<typename CollType::Element>);
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("sort(descending)")
                {
                    coll.sort(bdn::descending<typename CollType::Element>);
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("sort with compare func")
                {
                    coll.sort([](typename CollType::Element &a, typename CollType::Element &b) {
                        // use inverted comparison
                        return (b < a);
                    });

                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("stableSort")
                {
                    coll.stableSort();
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("stableSort(ascending)")
                {
                    coll.stableSort(bdn::ascending<typename CollType::Element>);
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("stableSort(descending)")
                {
                    coll.stableSort(bdn::descending<typename CollType::Element>);
                    _verifyPositionalCollectionReadOnly(coll, {});
                }

                SECTION("stableSort with compare func")
                {
                    coll.stableSort([](const typename CollType::Element &a, const typename CollType::Element &b) {
                        // use inverted comparison
                        return (b < a);
                    });

                    _verifyPositionalCollectionReadOnly(coll, {});
                }
            }

            SECTION("not empty")
            {
                CollType coll(elements);

                SECTION("sort")
                {
                    coll.sort();

                    _verifyCollectionSortResult(coll, elements);
                }

                SECTION("sort(ascending)")
                {
                    coll.sort(bdn::ascending<typename CollType::Element>);

                    _verifyCollectionSortResult(coll, elements);
                }

                SECTION("sort(descending)")
                {
                    coll.sort(bdn::descending<typename CollType::Element>);

                    _verifyCollectionSortResult(coll, elements, true);
                }

                SECTION("sort with compare func")
                {
                    coll.sort([](typename CollType::Element &a, typename CollType::Element &b) {
                        // use inverted comparison
                        return (b < a);
                    });

                    _verifyCollectionSortResult(coll, elements, true);
                }

                SECTION("stableSort")
                {
                    coll.stableSort();

                    _verifyCollectionSortResult(coll, elements);

                    _verifyPositionalCollectionReadOnly(coll, stableSortedElements);
                }

                SECTION("stableSort(ascending)")
                {
                    coll.stableSort(bdn::ascending<typename CollType::Element>);

                    _verifyCollectionSortResult(coll, elements);

                    _verifyPositionalCollectionReadOnly(coll, stableSortedElements);
                }

                SECTION("stableSort(descending)")
                {
                    coll.stableSort(descending<typename CollType::Element>);

                    _verifyCollectionSortResult(coll, elements, true);
                }

                SECTION("stableSort with compare func")
                {
                    coll.stableSort([](const typename CollType::Element &a, const typename CollType::Element &b) {
                        // use inverted comparison
                        return (b < a);
                    });

                    _verifyCollectionSortResult(coll, elements, true);
                }
            }
        }
    }
}

namespace std
{

    template <> struct hash<bdn::test::TestCollectionElement_OrderedComparable_>
    {
        size_t operator()(const bdn::test::TestCollectionElement_OrderedComparable_ &o) const
        {
            return std::hash<int>()(o._a) ^ std::hash<int>()(o._b);
        }
    };
}

#endif
