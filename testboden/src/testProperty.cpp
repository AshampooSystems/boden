#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/DefaultProperty.h>

#include <bdn/test/property.h>

using namespace bdn;

TEST_CASE("DefaultProperty")
{
	SECTION("String")
	{
		bdn::test::testProperty<DefaultProperty<String>, String>(
			[]() { return newObj<DefaultProperty<String> >(); },
			bdn::test::stringValueGenerator );
	}

	SECTION("int")
	{
		bdn::test::testProperty<DefaultProperty<int>, int>(
			[]() { return newObj<DefaultProperty<int> >(); },
			bdn::test::numberValueGenerator<int> );
	}
}





