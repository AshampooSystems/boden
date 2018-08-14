#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;

class TestPHelper : public Base
{
public:
	TestPHelper()
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


class SubTestPHelper : public TestPHelper
{

};

TEST_CASE("P")
{
	TestPHelper helper;
	
	SECTION("constructDestruct")
	{
		{
			P<TestPHelper> p(&helper);

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 1);
	}

	SECTION("constructDetach")
	{
		{
			P<TestPHelper> p(&helper);

			helper.verifyCounters(1, 0);

			p.detachPtr();

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 0);
	}


	SECTION("constructAssignNull")
	{
		{
			P<TestPHelper> p(&helper);

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
		P<TestPHelper> p;

		REQUIRE( p==nullptr );

		p = nullptr;

		REQUIRE( p==nullptr );
	}

	SECTION("assignDestruct")
	{
		{
			P<TestPHelper> p;

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
			P<TestPHelper> p(&helper);

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

			P<TestPHelper> p;
			p.attachPtr(&helper);

			helper.verifyCounters(1, 0);
		}

		helper.verifyCounters(1, 1);
	}

	SECTION("attachDetach")
	{
		{
			P<TestPHelper> p;

			p.attachPtr(&helper);

			REQUIRE( p!=nullptr );

			helper.verifyCounters(0, 0);

			TestPHelper* pRet = p.detachPtr();
			REQUIRE( pRet==&helper );

			REQUIRE( p==nullptr );
		}

		helper.verifyCounters(0, 0);
	}

	SECTION("detachWhileNull")
	{
		{
			P<TestPHelper> p;

			TestPHelper* pRet = p.detachPtr();
			REQUIRE( pRet==nullptr );
		}

		helper.verifyCounters(0, 0);
	}


	SECTION("constructAssignOtherDestruct")
	{
		TestPHelper helper2;

		{
			P<TestPHelper> p(&helper);

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
		TestPHelper helper2;

		{
			P<TestPHelper> p(&helper);
			P<TestPHelper> p2(&helper2);

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
		P<TestPHelper> p(&helper);

		REQUIRE( p==p );
		REQUIRE( !(p!=p) );

		REQUIRE( p!=nullptr );
	}

	SECTION("compareOther")
	{
		TestPHelper helper2;

		P<TestPHelper> p(&helper);
		P<TestPHelper> p2(&helper2);

		REQUIRE( p!=p2 );
	}

	SECTION("compareOtherSameObject")
	{
		P<TestPHelper> p(&helper);
		P<TestPHelper> p2(&helper);

		REQUIRE( p==p2 );
	}

	SECTION("compareOtherNull")
	{
		P<TestPHelper> p(&helper);
		P<TestPHelper> p2;

		REQUIRE( p!=p2 );
	}

	SECTION("detachCompareOtherNull")
	{
		P<TestPHelper> p(&helper);

		p.detachPtr();

		REQUIRE(p==nullptr);
	}


	SECTION("objectMemberAccess")
	{
		P<TestPHelper> p(&helper);

		REQUIRE( p->dummy()==42 );
	}

	SECTION("dereference")
	{
		P<TestPHelper> p(&helper);

		TestPHelper& h = *p;

		REQUIRE(&h==&helper);
	}

	SECTION("getPtr")
	{
		P<TestPHelper> p(&helper);

		TestPHelper* pH = p.getPtr();

		REQUIRE(pH==&helper);
	}


	SECTION("moveConstructor")
	{
		P<TestPHelper> p(&helper);
		P<TestPHelper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==&helper );

		REQUIRE( helper.getRefCount()==2 );
	}

	SECTION("moveConstructorNull")
	{
		P<TestPHelper> p;
		P<TestPHelper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==nullptr );
	}

	SECTION("moveConstructorFromSubClass")
	{
		SubTestPHelper subHelper;

		P<SubTestPHelper> p(&subHelper);
		P<TestPHelper> p2( std::move(p) );

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==(TestPHelper*)&subHelper );

		REQUIRE( subHelper.getRefCount()==2 );
	}

	SECTION("moveAssignment")
	{
		P<TestPHelper> p(&helper);
		P<TestPHelper> p2;

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
		P<TestPHelper> p;
		P<TestPHelper> p2;

		SECTION("assign")		
			p2.assign( std::move(p) );

		SECTION("operator=")		
			p2 = std::move(p);

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==nullptr );
	}

	SECTION("moveAssignmentFromSubClass")
	{
		SubTestPHelper subHelper;

		P<SubTestPHelper> p(&subHelper);

		P<TestPHelper>	 p2;

		SECTION("assign")		
			p2.assign( std::move(p) );

		SECTION("operator=")		
			p2 = std::move(p);

		REQUIRE( p.getPtr()==nullptr );
		REQUIRE( p2.getPtr()==(TestPHelper*)&subHelper );

		REQUIRE( subHelper.getRefCount()==2 );
	}
	

}