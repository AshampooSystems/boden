#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace iface
{

    enum TriState
    {
        on,
        off,
        mixed
    };

    class ICheckbox
    {
      public:
        bdn::Property<string> label;
        bdn::Property<TriState> state;
    };

    class ISwitch
    {
      public:
        bdn::Property<string> label;
        bdn::Property<bool> on;
    };

    class Toggle : virtual public ISwitch, virtual public ICheckbox
    {
      public:
        Toggle() { ICheckbox::label.bind(ISwitch::label); }

        bdn::Property<string> &label = ISwitch::label; // = {Connect<Property<string>>{ISwitch::label}};
    };
}

using namespace iface;

TEST(Old_Property, Interface)
{
    Toggle t;

    t.label = "Test";

    string checkboxLabel = ((ICheckbox *)&t)->label;
    string switchLabel = ((ISwitch *)&t)->label;

    EXPECT_EQ(t.label, "Test");
    EXPECT_EQ(checkboxLabel, "Test");
    EXPECT_EQ(switchLabel, "Test");
}
