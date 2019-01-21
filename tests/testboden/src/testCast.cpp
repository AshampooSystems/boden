
#include <bdn/test.h>

using namespace bdn;

class A : public Base
{
};

class SubA : public A
{
};

class SubSubA : public SubA
{
};

class B
{
};

void verifyConst(void *p, bool constExpected) { REQUIRE(!constExpected); }

void verifyConst(const void *p, bool constExpected) { REQUIRE(constExpected); }

// TODO REMOVE
