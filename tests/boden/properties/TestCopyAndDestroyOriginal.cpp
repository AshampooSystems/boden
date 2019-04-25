#include "ComplexType.h"
#include "Person.h"
#include <bdn/property/Property.h>
#include <gtest/gtest.h>
#include <memory>

using namespace std::string_literals;

TEST(Old_Property, CopyAndDestroyDefaultConstructed)
{
    auto amy = std::make_unique<Person>();
    auto chantal = std::make_unique<Person>();

    amy->name = "Amy"s;
    amy->age = 20;
    amy->height = 200;

    // This actually works. As both objects have been default constructed before,
    // they have allocated distinct backings and operator=() will just copy values.
    *chantal = *amy;

    EXPECT_EQ(amy->name, "Amy");
    EXPECT_EQ(amy->age, 20);
    EXPECT_EQ(amy->height, 200);

    EXPECT_EQ(chantal->name, "Amy");
    EXPECT_EQ(chantal->age, 20);
    EXPECT_EQ(chantal->height, 200);

    amy.reset();

    EXPECT_EQ(chantal->name, "Amy");
    EXPECT_EQ(chantal->age, 20);
    EXPECT_EQ(chantal->height, 200);

    chantal->name = "Chantal";
    chantal->age = 23;
    chantal->height = 142;

    EXPECT_EQ(chantal->name, "Chantal");
    EXPECT_EQ(chantal->age, 23);
    EXPECT_EQ(chantal->height, 142);

    chantal.reset();
}
