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
		class InstantDeleteHelper : public Base
		{
		public:
			InstantDeleteHelper(bool* pDeleted)
			{
				_pDeleted = pDeleted;
			}

			~InstantDeleteHelper()
			{
				*_pDeleted = true;
			}

		protected:
			bool* _pDeleted;
		};

		bool deleted = false;

		newObj<InstantDeleteHelper>(&deleted);

		REQUIRE(deleted);
	}
}

