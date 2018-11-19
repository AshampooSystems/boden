#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/property.h>
#include <bdn/Array.h>
#include <bdn/View.h>

using namespace bdn;

template <typename VALUE_TYPE> class TestPropertyOwner : public Base
{
  public:
    bool *deleted = nullptr;

    ~TestPropertyOwner()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    BDN_PROPERTY(VALUE_TYPE, myProp, setMyProp);
};

template <typename VALUE_TYPE> class TestPropertyOwner_Custom : public Base
{
  public:
    bool *deleted = nullptr;

    ~TestPropertyOwner_Custom()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    VALUE_TYPE myProp() const { return _customMyProp; }

    void setMyProp(const VALUE_TYPE &val)
    {
        if (val != _customMyProp) {
            _customMyProp = val;
            BDN_NOTIFY_PROPERTY_CHANGED(*this, myProp);
        }
    }

    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(VALUE_TYPE, myProp);
    BDN_FINALIZE_CUSTOM_PROPERTY(VALUE_TYPE, myProp, setMyProp);

  private:
    VALUE_TYPE _customMyProp;
};

template <typename VALUE_TYPE> class TestPropertyOwner_Custom_ConstRefGetter_CopySetter : public Base
{
  public:
    bool *deleted = nullptr;

    ~TestPropertyOwner_Custom_ConstRefGetter_CopySetter()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    const VALUE_TYPE &myProp() const { return _customMyProp; }

    void setMyProp(VALUE_TYPE val)
    {
        if (val != _customMyProp) {
            _customMyProp = val;
            BDN_NOTIFY_PROPERTY_CHANGED(*this, myProp);
        }
    }

    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(VALUE_TYPE, myProp);
    BDN_FINALIZE_CUSTOM_PROPERTY(VALUE_TYPE, myProp, setMyProp);

  private:
    VALUE_TYPE _customMyProp;
};

template <typename VALUE_TYPE> class TestPropertyOtherOwner : public Base
{
  public:
    bool *deleted = nullptr;

    ~TestPropertyOtherOwner()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    BDN_PROPERTY(VALUE_TYPE, otherProp, setOtherProp);
};

template <typename VALUE_TYPE> class TestPropertyOtherOwner_Custom : public Base
{
  public:
    bool *deleted = nullptr;

    ~TestPropertyOtherOwner_Custom()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    VALUE_TYPE otherProp() const { return _customOtherProp; }

    void setOtherProp(const VALUE_TYPE &val)
    {
        if (val != _customOtherProp) {
            _customOtherProp = val;
            BDN_NOTIFY_PROPERTY_CHANGED(*this, otherProp);
        }
    }

    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(VALUE_TYPE, otherProp);
    BDN_FINALIZE_CUSTOM_PROPERTY(VALUE_TYPE, myProp, setOtherProp);

  private:
    VALUE_TYPE _customOtherProp;
};

template <typename VALUE_TYPE> class TestPropertySetNotAllowed
{
  public:
    // this version of verifySetNotAllowed should NOT compile and should be
    // ignored by the compiler. If it DOES compile then we will get a test
    // failure here.
    template <typename OWNER_TYPE>
    static void verifySetNotAllowed(
        int dummyInt, decltype((*((OWNER_TYPE *)nullptr)).setMyProp(*((VALUE_TYPE *)nullptr))) *dummyPointer = nullptr)
    {
        REQUIRE(false);
    }

    template <typename OWNER_TYPE> static void verifySetNotAllowed(...)
    {
        // do nothing here
    }

    // same as above, just with a different property name
    template <typename OWNER_TYPE>
    static void verifySetProtectedWritePropNotAllowed(
        int dummyInt,
        decltype((*((OWNER_TYPE *)nullptr)).setProtectedWriteProp(*((VALUE_TYPE *)nullptr))) *dummyPointer = nullptr)
    {
        REQUIRE(false);
    }

    template <typename OWNER_TYPE> static void verifySetProtectedWritePropNotAllowed(...)
    {
        // do nothing here
    }

    // same as above, just with a different property name
    template <typename OWNER_TYPE>
    static void verifySetNoForwardProtectedWritePropNotAllowed(
        int dummyInt,
        decltype((*((OWNER_TYPE *)nullptr)).setNoForwardProtectedWriteProp(*((VALUE_TYPE *)nullptr))) *dummyPointer =
            nullptr)
    {
        REQUIRE(false);
    }

    template <typename OWNER_TYPE> static void verifySetNoForwardProtectedWritePropNotAllowed(...)
    {
        // do nothing here
    }
};

template <typename VALUE_TYPE, typename OWNER_A, typename OWNER_B>
static void testProperty(const VALUE_TYPE &expectedInitialValue, const VALUE_TYPE &valA, const VALUE_TYPE &valB)
{
    P<TestPropertyOwner<VALUE_TYPE>> owner = newObj<TestPropertyOwner<VALUE_TYPE>>();
    P<TestPropertyOtherOwner<VALUE_TYPE>> otherOwner = newObj<TestPropertyOtherOwner<VALUE_TYPE>>();

    SECTION("initial value")
    REQUIRE(owner->myProp() == expectedInitialValue);

    SECTION("modify")
    {
        owner->setMyProp(valA);
        REQUIRE(owner->myProp() == valA);
    }

    SECTION("change notification")
    {
        Array<VALUE_TYPE> callbackValues;

        owner->myPropChanged() += [&callbackValues, owner](const VALUE_TYPE &newValue) {
            callbackValues.add(newValue);
            REQUIRE(owner->myProp() == newValue); // property should already be modified when
                                                  // the notification happens
        };

        owner->setMyProp(valA);

        REQUIRE(callbackValues.size() == 1);
        REQUIRE(callbackValues[0] == valA);

        // change again => should notify again
        owner->setMyProp(valB);

        REQUIRE(callbackValues.size() == 2);
        REQUIRE(callbackValues[0] == valA);
        REQUIRE(callbackValues[1] == valB);
    }

    SECTION("no change notification when value not changed")
    {
        owner->setMyProp(valA);

        Array<VALUE_TYPE> callbackValues;

        owner->myPropChanged() += [&callbackValues, owner](const VALUE_TYPE &newValue) {
            callbackValues.add(newValue);
            REQUIRE(owner->myProp() == newValue); // property should already be modified when
                                                  // the notification happens
        };

        owner->setMyProp(valA);

        // should NOT have caused callbacks
        REQUIRE(callbackValues.size() == 0);
    }

    SECTION("change notification with property change in between")
    {
        Array<std::pair<int, VALUE_TYPE>> callbackData;

        owner->myPropChanged() += [&callbackData, owner, &valB](const VALUE_TYPE &newValue) {
            VALUE_TYPE newValueCopy(newValue);

            callbackData.add(std::make_pair(1, newValue));
            REQUIRE(owner->myProp() == newValue); // property should already be modified when the
                                                  // notification happens

            // now we change the property from within the handler
            owner->setMyProp(valB);
            REQUIRE(owner->myProp() == valB); // property should be at new value immediately
                                              // already be modified when the notification happens

            // the parameter should NOT have changed
            REQUIRE(newValue == newValueCopy);
        };

        owner->myPropChanged() += [&callbackData, owner](const VALUE_TYPE &newValue) {
            callbackData.add(std::make_pair(2, newValue));
            REQUIRE(owner->myProp() == newValue); // property should already be modified when
                                                  // the notification happens
        };

        owner->myPropChanged() += [&callbackData, owner](const VALUE_TYPE &newValue) {
            callbackData.add(std::make_pair(3, newValue));
            REQUIRE(owner->myProp() == newValue); // property should already be modified when
                                                  // the notification happens
        };

        owner->setMyProp(valA);

        REQUIRE(callbackData.size() == 6);
        REQUIRE(callbackData[0] == std::make_pair(1, valA));

        // in the first callback, the value is changed
        // So we immediately get three new notifications from that operation
        REQUIRE(callbackData[1] == std::make_pair(1, valB));
        // on the second callback no further change happens (since the property
        // already has valB).

        // So the second handler is called. First from the second change.
        REQUIRE(callbackData[2] == std::make_pair(2, valB));

        // So the second handler is called. First from the second change.
        REQUIRE(callbackData[3] == std::make_pair(3, valB));

        // and then the still pending call of the second handler from the first
        // change happens. It is very important that this call actually gets the
        // NEW value as its parameter (i.e. the current property value at the
        // time of the callback)
        REQUIRE(callbackData[4] == std::make_pair(2, valB));

        // and then we get the third handler from the first change. Again, we
        // should get the current value.
        REQUIRE(callbackData[5] == std::make_pair(3, valB));
    }

    SECTION("BDN_NOTIFY_PROPERTY_CHANGED")
    {
        Array<VALUE_TYPE> callbackValues;

        owner->myPropChanged() += [&callbackValues, owner](const VALUE_TYPE &newValue) {
            callbackValues.add(newValue);
            REQUIRE(owner->myProp() == newValue); // property should already be modified when
                                                  // the notification happens
        };

        BDN_NOTIFY_PROPERTY_CHANGED(*owner, myProp);

        REQUIRE(callbackValues.size() == 1);
        REQUIRE(callbackValues[0] == expectedInitialValue);
    }

    SECTION("binding")
    {
        BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, *owner, myProp);

        owner->setMyProp(valA);

        REQUIRE(owner->myProp() == valA);
        REQUIRE(otherOwner->otherProp() == valA);

        // the binding should only work in one direction

        otherOwner->setOtherProp(valB);
        REQUIRE(owner->myProp() == valA);
        REQUIRE(otherOwner->otherProp() == valB);
    }

    SECTION("binding transfers current value")
    {
        owner->setMyProp(valA);

        BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, *owner, myProp);

        // source's property should not have changed
        REQUIRE(owner->myProp() == valA);
        // bound property should have been initialized
        REQUIRE(otherOwner->otherProp() == valA);

        owner->setMyProp(valB);

        REQUIRE(owner->myProp() == valB);
        REQUIRE(otherOwner->otherProp() == valB);
    }

    SECTION("bidirectional binding")
    {
        SECTION("two unidir")
        {
            BDN_BIND_TO_PROPERTY(*owner, setMyProp, *otherOwner, otherProp);
            BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, *owner, myProp);
        }

        SECTION("bidir") { BDN_BIND_PROPERTIES(*owner, myProp, setMyProp, *otherOwner, otherProp, setOtherProp); }

        owner->setMyProp(valA);

        REQUIRE(owner->myProp() == valA);
        REQUIRE(otherOwner->otherProp() == valA);

        otherOwner->setOtherProp(valB);
        REQUIRE(owner->myProp() == valB);
        REQUIRE(otherOwner->otherProp() == valB);
    }

    SECTION("bidirectional binding transfers current value")
    {
        otherOwner->setOtherProp(valB);

        SECTION("two unidir")
        {
            BDN_BIND_TO_PROPERTY(*owner, setMyProp, *otherOwner, otherProp);
            BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, *owner, myProp);
        }

        SECTION("bidir") { BDN_BIND_PROPERTIES(*owner, myProp, setMyProp, *otherOwner, otherProp, setOtherProp); }

        REQUIRE(owner->myProp() == valB);
        REQUIRE(otherOwner->otherProp() == valB);
    }

    SECTION("binding with filter")
    {
        SECTION("std::function")
        {
            std::function<VALUE_TYPE(const VALUE_TYPE &)> filterFunc = [](const VALUE_TYPE &value) {
                return value + value;
            };

            BDN_BIND_TO_PROPERTY_WITH_FILTER(*otherOwner, setOtherProp, *owner, myProp, filterFunc);
        }

        SECTION("inline lambda")
        {
            BDN_BIND_TO_PROPERTY_WITH_FILTER(*otherOwner, setOtherProp, *owner, myProp,
                                             [](const VALUE_TYPE &value) { return value + value; });
        }

        owner->setMyProp(valA);

        auto expectedOtherValue = valA + valA;

        REQUIRE(owner->myProp() == valA);
        REQUIRE(otherOwner->otherProp() == expectedOtherValue);

        // the binding should only work in one direction

        otherOwner->setOtherProp(valB);
        REQUIRE(owner->myProp() == valA);
        REQUIRE(otherOwner->otherProp() == valB);
    }

    SECTION("binding with filter transfers current value")
    {
        owner->setMyProp(valA);

        SECTION("std::function")
        {
            std::function<VALUE_TYPE(const VALUE_TYPE &)> filterFunc = [](const VALUE_TYPE &value) {
                return value + value;
            };

            BDN_BIND_TO_PROPERTY_WITH_FILTER(*otherOwner, setOtherProp, *owner, myProp, filterFunc);
        }

        SECTION("inline lambda")
        {
            BDN_BIND_TO_PROPERTY_WITH_FILTER(*otherOwner, setOtherProp, *owner, myProp,
                                             [](const VALUE_TYPE &value) { return value + value; });
        }

        // source's property should not have changed
        REQUIRE(owner->myProp() == valA);
        // bound property should have been initialized
        REQUIRE(otherOwner->otherProp() == valA + valA);

        owner->setMyProp(valB);

        REQUIRE(owner->myProp() == valB);
        REQUIRE(otherOwner->otherProp() == valB + valB);
    }

    SECTION("break binding")
    {
        bool senderDeleted = false;
        bool receiverDeleted = false;
        owner->deleted = &senderDeleted;
        otherOwner->deleted = &receiverDeleted;

        BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, *owner, myProp);

        SECTION("delete sender")
        {
            owner = nullptr;
            // verify that the owner has really been deleted (i.e. that the
            // binding does NOT keep it alive)
            REQUIRE(senderDeleted);

            // verify that the receiver has not yet been deleted (since we still
            // hold a pointer)
            REQUIRE(!receiverDeleted);

            SECTION("do nothing with receiver")
            {
                // do nothing here
            }

            SECTION("modify receiver after deleting sender")
            {
                otherOwner->setOtherProp(valA);
                REQUIRE(otherOwner->otherProp() == valA);
            }

            // verify that the receiver has no dangling references
            otherOwner = nullptr;
            REQUIRE(receiverDeleted);
        }

        SECTION("delete receiver")
        {
            otherOwner = nullptr;
            // verify that the receiver has really been deleted (i.e. that the
            // binding does NOT keep it alive)
            REQUIRE(receiverDeleted);

            // verify that the sender has not yet been deleted (since we still
            // hold a pointer)
            REQUIRE(!senderDeleted);

            SECTION("do nothing with sender")
            {
                // do nothing here
            }

            SECTION("modify sender after deleting sender")
            {
                owner->setMyProp(valA);
                REQUIRE(owner->myProp() == valA);
            }

            // verify that the sender has no dangling references
            owner = nullptr;
            REQUIRE(senderDeleted);
        }
    }

    SECTION("const access")
    {
        owner->setMyProp(valA);

        const TestPropertyOwner<VALUE_TYPE> &constOwner = *owner;

        SECTION("get") { REQUIRE(constOwner.myProp() == valA); }

        SECTION("set not allowed")
        {
            TestPropertySetNotAllowed<VALUE_TYPE>::template verifySetNotAllowed<const TestPropertyOwner<VALUE_TYPE>>(0);
        }

        SECTION("change notification")
        {
            Array<VALUE_TYPE> callbackValues;

            constOwner.myPropChanged() += [&callbackValues, owner](const VALUE_TYPE &newValue) {
                callbackValues.add(newValue);
                REQUIRE(owner->myProp() == newValue); // property should already be modified
                                                      // when the notification happens
            };

            owner->setMyProp(valB);

            REQUIRE(callbackValues.size() == 1);
            REQUIRE(callbackValues[0] == valB);
        }

        SECTION("binding as sender")
        {
            BDN_BIND_TO_PROPERTY(*otherOwner, setOtherProp, constOwner, myProp);

            owner->setMyProp(valB);

            REQUIRE(owner->myProp() == valB);
            REQUIRE(otherOwner->otherProp() == valB);
        }
    }
}

template <typename VALUE_TYPE> class IAbstractTestPropertyOwner : BDN_IMPLEMENTS IBase
{
  public:
    BDN_ABSTRACT_PROPERTY(VALUE_TYPE, myProp, setMyProp);
};

template <typename VALUE_TYPE> class IAbstractTestPropertyOwner2 : BDN_IMPLEMENTS IBase
{
  public:
    BDN_ABSTRACT_PROPERTY(VALUE_TYPE, myProp, setMyProp);
};

template <typename VALUE_TYPE>
class TestPropertyOwnerWithInterface : public Base, BDN_IMPLEMENTS IAbstractTestPropertyOwner<VALUE_TYPE>
{
  public:
    bool *deleted = nullptr;

    TestPropertyOwnerWithInterface()
    {
        if (deleted != nullptr)
            *deleted = true;
    }

    BDN_PROPERTY(VALUE_TYPE, myProp, setMyProp);
};

template <typename VALUE_TYPE>
class TestPropertyOwnerWithInterfaceDerivedReimplementProperty : public TestPropertyOwnerWithInterface<VALUE_TYPE>,
                                                                 BDN_IMPLEMENTS IAbstractTestPropertyOwner2<VALUE_TYPE>
{
  public:
    BDN_DISAMBIGUATE_PROPERTY(VALUE_TYPE, myProp, setMyProp, TestPropertyOwnerWithInterface<VALUE_TYPE>);
};

template <typename VALUE_TYPE>
void testAbstractProperty(VALUE_TYPE expectedInitialValue, VALUE_TYPE valA, VALUE_TYPE valB)
{
    P<TestPropertyOwnerWithInterface<VALUE_TYPE>> owner = newObj<TestPropertyOwnerWithInterface<VALUE_TYPE>>();

    P<IAbstractTestPropertyOwner<VALUE_TYPE>> interface = owner;

    SECTION("initial value")
    {
        REQUIRE(interface->myProp() == expectedInitialValue);
        REQUIRE(owner->myProp() == expectedInitialValue);
    }

    SECTION("set")
    {
        interface->setMyProp(valA);

        REQUIRE(interface->myProp() == valA);
        REQUIRE(owner->myProp() == valA);
    }

    SECTION("changed notifier identical")
    {
        interface->setMyProp(valA);

        REQUIRE(&interface->myPropChanged() == &owner->myPropChanged());
    }

    SECTION("BDN_DISAMBIGUATE_PROPERTY")
    {
        // here we test the case that a derived class adds a new interface that
        // also has the same property.

        P<TestPropertyOwnerWithInterfaceDerivedReimplementProperty<VALUE_TYPE>> derivedOwner =
            newObj<TestPropertyOwnerWithInterfaceDerivedReimplementProperty<VALUE_TYPE>>();

        P<IAbstractTestPropertyOwner2<VALUE_TYPE>> interface2 = derivedOwner;
        P<IAbstractTestPropertyOwner<VALUE_TYPE>> interface = derivedOwner;

        REQUIRE(interface->myProp() == expectedInitialValue);
        REQUIRE(interface2->myProp() == expectedInitialValue);
        REQUIRE(owner->myProp() == expectedInitialValue);

        interface2->setMyProp(valA);

        REQUIRE(interface->myProp() == valA);
        REQUIRE(interface2->myProp() == valA);
        REQUIRE(derivedOwner->myProp() == valA);
    }
}

class TestPropOwnerWithProtectedSetter : public Base
{
  public:
    TestPropOwnerWithProtectedSetter() { setMyProp(42); }

    BDN_PROPERTY_WITH_CUSTOM_ACCESS(int, public, myProp, protected, setMyProp);
};

class TestPropOwnerWithProtectedSetter_Custom : public Base
{
  public:
    TestPropOwnerWithProtectedSetter_Custom() { setMyProp(42); }

    int myProp() const { return _val; }

    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION(int, myProp);
    BDN_FINALIZE_CUSTOM_PROPERTY_WITH_CUSTOM_ACCESS(int, public, myProp, protected, setMyProp);

  protected:
    void setMyProp(int val)
    {
        if (_val != val) {
            _val = val;

            BDN_NOTIFY_PROPERTY_CHANGED(*this, myProp);
        }
    }

  private:
    int _val;
};

template <class BASE> class TestPropOwnerWithProtectedSetterDerived : public BASE
{
  public:
    void derivedSet(int val) { this->setMyProp(val); }
};

template <typename OWNER_WITH_PROTECTED_SETTER> void testPropertyWithProtectedSetter()
{
    P<OWNER_WITH_PROTECTED_SETTER> owner = newObj<OWNER_WITH_PROTECTED_SETTER>();

    SECTION("getter public") { REQUIRE(owner->myProp() == 42); }

    SECTION("setter not public")
    {
        TestPropertySetNotAllowed<int>::template verifySetNotAllowed<const OWNER_WITH_PROTECTED_SETTER>(0);
    }

    SECTION("setter accessible in derived classes")
    {
        TestPropOwnerWithProtectedSetterDerived<OWNER_WITH_PROTECTED_SETTER> derivedOwner;

        derivedOwner.derivedSet(17);

        REQUIRE(derivedOwner.myProp() == 17);
    }
}

template <typename VALUE_TYPE> class TestViewPropOwnerCore : public Base
{
  public:
    VALUE_TYPE nothing;

    void setNothingProp(const VALUE_TYPE &val) { nothing = val; }

    VALUE_TYPE contentLayout;

    void setContentLayoutProp(const VALUE_TYPE &val) { contentLayout = val; }

    VALUE_TYPE prefSize;

    void setPrefSizeProp(const VALUE_TYPE &val) { prefSize = val; }

    VALUE_TYPE parentPrefSize;

    void setParentPrefSizeProp(const VALUE_TYPE &val) { parentPrefSize = val; }

    VALUE_TYPE parentPrefSizeAndContentLayout;

    void setParentPrefSizeAndContentLayoutProp(const VALUE_TYPE &val) { parentPrefSizeAndContentLayout = val; }

    VALUE_TYPE parentLayout;

    void setParentLayoutProp(const VALUE_TYPE &val) { parentLayout = val; }

    VALUE_TYPE protectedWriteProp;

    void setProtectedWriteProp(const VALUE_TYPE &val) { protectedWriteProp = val; }

    void verifyValues(const VALUE_TYPE &nothing, const VALUE_TYPE &contentLayout, const VALUE_TYPE &prefSize,
                      const VALUE_TYPE &parentPrefSize, const VALUE_TYPE &parentPrefSizeAndContentLayout,
                      const VALUE_TYPE &parentLayout)
    {
        REQUIRE(this->nothing == nothing);
        REQUIRE(this->contentLayout == contentLayout);
        REQUIRE(this->prefSize == prefSize);
        REQUIRE(this->parentPrefSize == parentPrefSize);
        REQUIRE(this->parentPrefSizeAndContentLayout == parentPrefSizeAndContentLayout);
        REQUIRE(this->parentLayout == parentLayout);
    }
};

template <typename VALUE_TYPE> class TestViewPropOwner : public Base
{
  public:
    TestViewPropOwner() { _testCore = newObj<TestViewPropOwnerCore<VALUE_TYPE>>(); }

    BDN_VIEW_PROPERTY(VALUE_TYPE, nothingProp, setNothingProp, TestViewPropOwnerCore<VALUE_TYPE>, influencesNothing());

    BDN_VIEW_PROPERTY(VALUE_TYPE, contentLayoutProp, setContentLayoutProp, TestViewPropOwnerCore<VALUE_TYPE>,
                      influencesContentLayout());

    BDN_VIEW_PROPERTY(VALUE_TYPE, prefSizeProp, setPrefSizeProp, TestViewPropOwnerCore<VALUE_TYPE>,
                      influencesPreferredSize());

    BDN_VIEW_PROPERTY(VALUE_TYPE, parentPrefSizeProp, setParentPrefSizeProp, TestViewPropOwnerCore<VALUE_TYPE>,
                      influencesParentPreferredSize());

    BDN_VIEW_PROPERTY(VALUE_TYPE, parentPrefSizeAndContentLayoutProp, setParentPrefSizeAndContentLayoutProp,
                      TestViewPropOwnerCore<VALUE_TYPE>, influencesParentPreferredSize().influencesContentLayout());

    BDN_VIEW_PROPERTY(VALUE_TYPE, parentLayoutProp, setParentLayoutProp, TestViewPropOwnerCore<VALUE_TYPE>,
                      influencesParentLayout());

    BDN_VIEW_PROPERTY_WITHOUT_CORE_FORWARDING(VALUE_TYPE, noForwardProp, setNoForwardProp, influencesParentLayout());

    BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS(VALUE_TYPE, public, protectedWriteProp, protected, setProtectedWriteProp,
                                         TestViewPropOwnerCore<VALUE_TYPE>, influencesParentLayout());

    BDN_VIEW_PROPERTY_WITH_CUSTOM_ACCESS_WITHOUT_CORE_FORWARDING(VALUE_TYPE, public, noForwardProtectedWriteProp,
                                                                 protected, setNoForwardProtectedWriteProp,
                                                                 influencesParentLayout());

    P<IBase> getViewCore() { return _testCore; }

    P<TestViewPropOwnerCore<VALUE_TYPE>> getTestCore() { return _testCore; }

    void verifyInfluences(int nothingCounter, int contentLayoutCounter, int prefSizeCounter, int parentPrefSizeCounter,
                          int parentLayoutCounter)
    {
        REQUIRE(influencesNothingCounter == nothingCounter);
        REQUIRE(influencesContentLayoutCounter == contentLayoutCounter);
        REQUIRE(influencesPreferredSizeCounter == prefSizeCounter);
        REQUIRE(influencesParentPreferredSizeCounter == parentPrefSizeCounter);
        REQUIRE(influencesParentLayoutCounter == parentLayoutCounter);
    }

    int influencesNothingCounter = 0;
    int influencesContentLayoutCounter = 0;
    int influencesPreferredSizeCounter = 0;
    int influencesParentPreferredSizeCounter = 0;
    int influencesParentLayoutCounter = 0;

    class Influences_
    {
      public:
        Influences_(TestViewPropOwner *owner) : _owner(owner) {}

        const Influences_ &influencesNothing() const
        {
            _owner->influencesNothingCounter++;

            return *this;
        }

        const Influences_ &influencesPreferredSize() const
        {
            _owner->influencesPreferredSizeCounter++;

            return *this;
        }

        const Influences_ &influencesContentLayout() const
        {
            _owner->influencesContentLayoutCounter++;

            return *this;
        }

        const Influences_ &influencesParentPreferredSize() const
        {
            _owner->influencesParentPreferredSizeCounter++;

            return *this;
        }

        const Influences_ &influencesParentLayout() const
        {
            _owner->influencesParentLayoutCounter++;

            return *this;
        }

      private:
        TestViewPropOwner *_owner;
    };
    friend class Influences_;

  private:
    P<TestViewPropOwnerCore<VALUE_TYPE>> _testCore;
};

template <class VALUE_TYPE> class TestViewPropOwnerDerived : public TestViewPropOwner<VALUE_TYPE>
{
  public:
    void derivedSetProtectedWriteProp(const VALUE_TYPE &value) { this->setProtectedWriteProp(value); }

    void derivedSetNoForwardProtectedWriteProp(const VALUE_TYPE &value) { this->setNoForwardProtectedWriteProp(value); }
};

template <class VALUE_TYPE> static void testViewProperty(const VALUE_TYPE &initialValue, const VALUE_TYPE &valA)
{
    P<TestViewPropOwner<VALUE_TYPE>> owner = newObj<TestViewPropOwner<VALUE_TYPE>>();

    SECTION("influencesNothing")
    {
        owner->setNothingProp(valA);
        REQUIRE(owner->nothingProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(valA, initialValue, initialValue, initialValue, initialValue, initialValue);

        // correct influence calls made?
        owner->verifyInfluences(1, 0, 0, 0, 0);
    }

    SECTION("influencesContentLayout")
    {
        owner->setContentLayoutProp(valA);
        REQUIRE(owner->contentLayoutProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(initialValue, valA, initialValue, initialValue, initialValue, initialValue);

        // correct influence calls made?
        owner->verifyInfluences(0, 1, 0, 0, 0);
    }

    SECTION("influencesPrefSize")
    {
        owner->setPrefSizeProp(valA);
        REQUIRE(owner->prefSizeProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(initialValue, initialValue, valA, initialValue, initialValue, initialValue);

        // correct influence calls made?
        owner->verifyInfluences(0, 0, 1, 0, 0);
    }

    SECTION("influencesParentPrefSize")
    {
        owner->setParentPrefSizeProp(valA);
        REQUIRE(owner->parentPrefSizeProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(initialValue, initialValue, initialValue, valA, initialValue, initialValue);

        // correct influence calls made?
        owner->verifyInfluences(0, 0, 0, 1, 0);
    }

    SECTION("influencesParentPrefSizeAndContentLayout")
    {
        owner->setParentPrefSizeAndContentLayoutProp(valA);
        REQUIRE(owner->parentPrefSizeAndContentLayoutProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(initialValue, initialValue, initialValue, initialValue, valA, initialValue);

        // correct influence calls made?
        owner->verifyInfluences(0, 1, 0, 1, 0);
    }

    SECTION("influencesParentLayout")
    {
        owner->setParentLayoutProp(valA);
        REQUIRE(owner->parentLayoutProp() == valA);

        // correct setter called?
        owner->getTestCore()->verifyValues(initialValue, initialValue, initialValue, initialValue, initialValue, valA);

        // correct influence calls made?
        owner->verifyInfluences(0, 0, 0, 0, 1);
    }

    SECTION("without core forwarding")
    {
        owner->setNoForwardProp(valA);
        REQUIRE(owner->noForwardProp() == valA);

        // no setter in core should have been called
        owner->getTestCore()->verifyValues(initialValue, initialValue, initialValue, initialValue, initialValue,
                                           initialValue);

        // correct influence calls made?
        owner->verifyInfluences(0, 0, 0, 0, 1);
    }

    SECTION("custom access")
    {
        SECTION("getter")
        owner->protectedWriteProp();

        SECTION("setter")
        TestPropertySetNotAllowed<VALUE_TYPE>::template verifySetProtectedWritePropNotAllowed<
            const TestViewPropOwner<VALUE_TYPE>>(0);

        SECTION("setter accessible in derived classes")
        {
            TestViewPropOwnerDerived<VALUE_TYPE> derivedOwner;

            derivedOwner.derivedSetProtectedWriteProp(valA);

            REQUIRE(derivedOwner.protectedWriteProp() == valA);
            REQUIRE(derivedOwner.getTestCore()->protectedWriteProp == valA);
        }
    }

    SECTION("custom access without core forwarding")
    {
        SECTION("getter")
        owner->noForwardProtectedWriteProp();

        SECTION("setter")
        TestPropertySetNotAllowed<VALUE_TYPE>::template verifySetNoForwardProtectedWritePropNotAllowed<
            const TestViewPropOwner<VALUE_TYPE>>(0);

        SECTION("setter accessible in derived classes")
        {
            TestViewPropOwnerDerived<VALUE_TYPE> derivedOwner;

            derivedOwner.derivedSetNoForwardProtectedWriteProp(valA);

            REQUIRE(derivedOwner.noForwardProtectedWriteProp() == valA);
        }
    }
}

TEST_CASE("properties")
{
    SECTION("BDN_PROPERTY")
    {
        SECTION("int")
        testProperty<int, TestPropertyOwner<int>, TestPropertyOtherOwner<int>>(0, 42, 1234);

        SECTION("String")
        testProperty<String, TestPropertyOwner<String>, TestPropertyOtherOwner<String>>("", "hello", "world");
    }

    SECTION("custom property")
    {
        SECTION("int")
        testProperty<int, TestPropertyOwner_Custom<int>, TestPropertyOtherOwner_Custom<int>>(0, 42, 1234);

        SECTION("String")
        testProperty<String, TestPropertyOwner_Custom<String>, TestPropertyOtherOwner_Custom<String>>("", "hello",
                                                                                                      "world");
    }

    SECTION("custom property with const reference getter and copy setter")
    {
        SECTION("int")
        testProperty<int, TestPropertyOwner_Custom_ConstRefGetter_CopySetter<int>, TestPropertyOtherOwner_Custom<int>>(
            0, 42, 1234);

        SECTION("String")
        testProperty<String, TestPropertyOwner_Custom_ConstRefGetter_CopySetter<String>,
                     TestPropertyOtherOwner_Custom<String>>("", "hello", "world");
    }

    SECTION("BDN_ABSTRACT_PROPERTY")
    {
        SECTION("int")
        testAbstractProperty<int>(0, 42, 1234);

        SECTION("String")
        testAbstractProperty<String>("", "hello", "world");
    }

    SECTION("BDN_PROPERTY_WITH_CUSTOM_ACCESS") { testPropertyWithProtectedSetter<TestPropOwnerWithProtectedSetter>(); }

    SECTION("Custom property with custom access")
    {
        testPropertyWithProtectedSetter<TestPropOwnerWithProtectedSetter_Custom>();
    }

    SECTION("Property binding with different types")
    {
        TestPropertyOwner<int> intOwner;
        TestPropertyOtherOwner<double> doubleOwner;

        BDN_BIND_TO_PROPERTY(doubleOwner, setOtherProp, intOwner, myProp);

        intOwner.setMyProp(1234);

        REQUIRE(doubleOwner.otherProp() == 1234);
    }

    /* note that this does not work because the filterFunc parameter must be
       convertible to a function object. An overloaded function name by itself
    cannot be converted in this way. SECTION("Property binding with overloaded
    filter function")
    {
        TestPropertyOwner<int>          intOwner;
        TestPropertyOtherOwner<String>  stringOwner;

        // note that bdn::toString has many overloads. We want to ensure that we
    can still pass
        // it to bind.
        BDN_BIND_TO_PROPERTY_WITH_FILTER( stringOwner, setOtherProp, intOwner,
    myProp, toString );

        intOwner.setMyProp( 1234 );

        REQUIRE( stringOwner.otherProp() == "1234" );
    }*/

    SECTION("view properties")
    {
        SECTION("int")
        testViewProperty<int>(0, 1234);
        SECTION("String")
        testViewProperty<String>("", "hello");
    }
}
