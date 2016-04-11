#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

class Helper : public Base
{
public:
	Helper()
	{
	}

	void addRef() const override
	{
		_addCounter++;

		Base::addRef();
	}

	void releaseRef() const override
	{
		_releaseCounter++;

		Base::releaseRef();
	}

	int dummy()
	{
		return 42;
	}

	void verifyCounters(int expectedAddCounter, int expectedReleaseCounter)
	{
		REQUIRE( _addCounter == expectedAddCounter);
		REQUIRE( _releaseCounter == expectedReleaseCounter);
	}

protected:
	mutable int		_addCounter = 0;
	mutable int		_releaseCounter = 0;
};


class SubHelper : public Helper
{

};

TEST_CASE("P")
{
	Helper helper;
	
	SECTION("constructDestruct")
	{
		{
			P<Helper> p(&helper);

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 1);
	}

	SECTION("constructDetach")
	{
		{
			P<Helper> p(&helper);

			helper.verifyCounters(1, 0);

			p.detach();

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 0);
	}


	SECTION("constructAssignNull")
	{
		{
			P<Helper> p(&helper);

			helper.verifyCounters(1, 0);

			REQUIRE( p!=nullptr );
			
			p = nullptr;

			REQUIRE( p==nullptr );

			helper.verifyCounters(1, 1);
		}

		helper.verifyCounters(1, 1);
	}


	SECTION("assignNullWhileNull")
	{
		P<Helper> p;

		REQUIRE( p==nullptr );

		p = nullptr;

		REQUIRE( p==nullptr );
	}

	SECTION("assignDestruct")
	{
		{
			P<Helper> p;

			REQUIRE( p==nullptr );
			
			p = &helper;

			helper.verifyCounters(1, 0);

			REQUIRE( p!=nullptr );
		}

		helper.verifyCounters(1, 1);
	}

	SECTION("constructAssignSelfDestruct")
	{
		{
			P<Helper> p(&helper);

			helper.verifyCounters(1, 0);
			
			p = p;

			helper.verifyCounters(2, 1);
		}

		helper.verifyCounters(2, 2);
	}

	SECTION("attachDestruct")
	{
		{
			helper.addRef();

			helper.verifyCounters(1, 0);

			P<Helper> p;
			p.attach(&helper);

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 1);
	}

	SECTION("attachDetach")
	{
		{
			P<Helper> p;

			p.attach(&helper);

			REQUIRE( p!=nullptr );

			helper.verifyCounters(0, 0);

			Helper* pRet = p.detach();
			REQUIRE( pRet==&helper );

			REQUIRE( p==nullptr );
		}

		helper.verifyCounters(0, 0);
	}

	SECTION("detachWhileNull")
	{
		{
			P<Helper> p;

			Helper* pRet = p.detach();
			REQUIRE( pRet==nullptr );
		}

		helper.verifyCounters(0, 0);
	}


	SECTION("constructAssignOtherDestruct")
	{
		Helper helper2;

		{
			P<Helper> p(&helper);

			helper.verifyCounters(1, 0);

			p = &helper2;

			helper.verifyCounters(1, 1);
			helper2.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 1);
		helper2.verifyCounters(1, 1);
	}

	SECTION("constructAssignOtherPDestruct")
	{
		Helper helper2;

		{
			P<Helper> p(&helper);
			P<Helper> p2(&helper2);

			helper.verifyCounters(1, 0);
			helper2.verifyCounters(1, 0);

			p = p2;

			helper.verifyCounters(1, 1);
			helper2.verifyCounters(2, 0);
		}

		helper.verifyCounters(1, 1);
		helper2.verifyCounters(2, 2);
	}

	SECTION("compareSelf")
	{
		P<Helper> p(&helper);

		REQUIRE( p==p );
		REQUIRE( !(p!=p) );

		REQUIRE( p!=nullptr );
	}

	SECTION("compareOther")
	{
		Helper helper2;

		P<Helper> p(&helper);
		P<Helper> p2(&helper2);

		REQUIRE( p!=p2 );
	}

	SECTION("compareOtherSameObject")
	{
		P<Helper> p(&helper);
		P<Helper> p2(&helper);

		REQUIRE( p==p2 );
	}

	SECTION("compareOtherNull")
	{
		P<Helper> p(&helper);
		P<Helper> p2;

		REQUIRE( p!=p2 );
	}

	SECTION("detachCompareOtherNull")
	{
		P<Helper> p(&helper);

		p.detach();

		REQUIRE(p==nullptr);
	}


	SECTION("objectMemberAccess")
	{
		P<Helper> p(&helper);

		REQUIRE( p->dummy()==42 );
	}

	SECTION("dereference")
	{
		P<Helper> p(&helper);

		Helper& h = *p;

		REQUIRE(&h==&helper);
	}

	SECTION("getPtr")
	{
		P<Helper> p(&helper);

		Helper* pH = p.getPtr();

		REQUIRE(pH==&helper);
	}


	SECTION("moveConstructor")
	{
		P<Helper> p(&helper);
		P<Helper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==&helper );

		REQUIRE( helper.getRefCount()==2 );
	}

	SECTION("moveConstructorNull")
	{
		P<Helper> p;
		P<Helper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==nullptr );
	}

	SECTION("moveConstructorFromSubClass")
	{
		SubHelper subHelper;

		P<SubHelper> p(&subHelper);
		P<Helper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==(Helper*)&subHelper );

		REQUIRE( subHelper.getRefCount()==2 );
	}

	SECTION("moveAssignment")
	{
		P<Helper> p(&helper);
		P<Helper> p2;

		SECTION("assign")		
			p2.assign( std::move(p) );

		SECTION("operator=")		
			p2 = std::move(p);

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==&helper );

		REQUIRE( helper.getRefCount()==2 );
	}

	SECTION("moveAssignmentNull")
	{
		P<Helper> p;
		P<Helper> p2;

		SECTION("assign")		
			p2.assign( std::move(p) );

		SECTION("operator=")		
			p2 = std::move(p);

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==nullptr );
	}

	SECTION("moveAssignmentFromSubClass")
	{
		SubHelper subHelper;

		P<SubHelper> p(&subHelper);

		P<Helper>	 p2;

		SECTION("assign")		
			p2.assign( std::move(p) );

		SECTION("operator=")		
			p2 = std::move(p);

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==(Helper*)&subHelper );

		REQUIRE( subHelper.getRefCount()==2 );
	}
	

}