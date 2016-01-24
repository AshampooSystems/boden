#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

TEST_CASE("newObj")
{
	SECTION("refCount")
	{
		P<Base> p = newObj<Base>();

		REQUIRE(p->getRefCount()==1);
	}

	SECTION("instantDelete")
	{
		class Helper : public Base
		{
		public:
			Helper(bool* pDeleted)
			{
				_pDeleted = pDeleted;
			}

			~Helper()
			{
				*_pDeleted = true;
			}

		protected:
			bool* _pDeleted;
		};

		bool deleted = false;

		newObj<Helper>(&deleted);

		REQUIRE(deleted);
	}
}

