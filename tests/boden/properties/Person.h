#pragma once

#include "ComplexType.h"
#include <bdn/property/Property.h>
#include <string>

using namespace std::string_literals;

class Person
{
  public:
    // ValueBacking
    bdn::Property<std::string> name = "Jacqueline"s;
    bdn::Property<int> age = 15;
    bdn::Property<ComplexType> complex = ComplexType{42, "running out of ideas"s};

    // GetterSetterBacking
    bdn::Property<int> height{bdn::GetterSetterBacking<int>{this, &Person::getHeight, &Person::setHeight, &_height}};

    int getHeight() const { return _height; }
    bool setHeight(int height)
    {
        if (_height != height) {
            _height = height;
            return true;
        }
        return false;
    }
    int _height = 155;

    // Transform backing
    bdn::Property<double> heightInInches = {
        bdn::TransformBacking<double, int>{height, &Person::heightToInches, &Person::inchesToHeight}};

    static double heightToInches(int height) { return 2.54 * double(height); }
    static int inchesToHeight(double inches) { return int(inches / 2.54); }

    int id = 0; // for debugging purposes only
};
