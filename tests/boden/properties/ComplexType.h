#pragma once

#include <bdn/property/Property.h>
#include <string>

struct ComplexType
{
    ComplexType() = default;
    ComplexType(const int &someInt_, const std::string &someString_) : someInt(someInt_), someString(someString_) {}

    bool operator==(const ComplexType &other) const
    {
        return someInt == other.someInt && someString == other.someString;
    }

    bool operator!=(const ComplexType &other) const
    {
        return someInt != other.someInt || someString != other.someString;
    }

    int someInt;
    std::string someString;
};

struct ComplexPropertyType
{
    ComplexPropertyType() = default;
    /*    ComplexPropertyType(const bdn::Property<int> &intProperty_, const bdn::Property<std::string> &stringProperty_)
            : intProperty(intProperty_), stringProperty(stringProperty_)
        {}
    */
    bool operator==(const ComplexPropertyType &other) const
    {
        return intProperty == other.intProperty && stringProperty == other.stringProperty;
    }

    bool operator!=(const ComplexPropertyType &other) const
    {
        return intProperty != other.intProperty || stringProperty != other.stringProperty;
    }

    bdn::Property<int> intProperty = 55;
    bdn::Property<std::string> stringProperty = std::string("Default");
};

struct RecursivePropertyType
{
    bool operator==(const RecursivePropertyType &other) const { return complex == other.complex; }

    bool operator!=(const RecursivePropertyType &other) const { return complex != other.complex; }

    bdn::Property<ComplexPropertyType> complex;
};

/*
struct ComplexGetterSetterType {
    bdn::Property<int, GetterSetterMemberBacking<int>> id = {Connect<GetterSetterMemberBacking<int>>{_idGetterSetter}};
    GetterSetterMemberBacking<int> _idGetterSetter = {
        this,
        &ComplexGetterSetterType::getId,
        &ComplexGetterSetterType::setId
    };

    int getId() const {
        return _id;
    }

    void setId(const int& id) {
        _id = id;
    }

//private:
    int _id = 123;
};

struct FilterType {
    bdn::Property<int> id = 1;
    bdn::Property<std::string, FilterMemberBacking<std::string, int>> name = {Connect<FilterMemberBacking<std::string,
int>>{_name}}; FilterMemberBacking<std::string, int> _name = { this, &id, &FilterType::idToName, &FilterType::nameToId
};

    std::string idToName(const int& id) {
        switch (id) {
            case 1: { return "Heinz"; }
            case 2: { return "Franz"; }
        }
        return "unknown";
    }

    int nameToId(const std::string& name) {
        if (name == "Heinz") {
            return 1;
        } else if (name == "Franz") {
            return 2;
        }
        return 0;
    }
};
*/
