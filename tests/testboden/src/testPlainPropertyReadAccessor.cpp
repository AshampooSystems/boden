#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PlainPropertyReadAccessor.h>
#include <bdn/PropertyNotifier.h>

using namespace bdn;

template <typename VALUE_TYPE> class TestPlainPropertyReadAccessorOwner : public Base
{
  public:
    TestPlainPropertyReadAccessorOwner(bool *deleted, const VALUE_TYPE &initialValue) : _myProp(initialValue)
    {
        _deleted = deleted;
    }

    ~TestPlainPropertyReadAccessorOwner() { *_deleted = true; }

    VALUE_TYPE myProp() const { return _myProp; }

    IPropertyNotifier<VALUE_TYPE> &myPropChanged() const { return *_myPropChanged; }

    using PropertyValueType_myProp = VALUE_TYPE;

  private:
    VALUE_TYPE _myProp;
    mutable P<PropertyNotifier<VALUE_TYPE>> _myPropChanged = newObj<PropertyNotifier<VALUE_TYPE>>();
    bool *_deleted;
};

TEST_CASE("PlainPropertyReadAccessor")
{
    bool ownerDeleted = false;
    P<TestPlainPropertyReadAccessorOwner<String>> owner =
        newObj<TestPlainPropertyReadAccessorOwner<String>>(&ownerDeleted, "hello");

    auto accessor = BDN_PROPERTY_READ_ACCESSOR(*owner, myProp);
    REQUIRE(dynamic_cast<IPropertyReadAccessor<String> *>(&accessor) != nullptr);

    REQUIRE(!ownerDeleted);

    SECTION("does not keep owner alive")
    {
        owner = nullptr;

        REQUIRE(ownerDeleted);
    }

    SECTION("calls getter correctly") { REQUIRE(accessor.get() == "hello"); }

    SECTION("returns correct changed notifier") { REQUIRE(&accessor.changed() == &owner->myPropChanged()); }
}
