
#include <bdn/test.h>

#include <bdn/func.h>

using namespace bdn;

struct MethodPTestStruct
{
    int i;
    std::string s;
};

class MethodPTestHelperBase : public Base
{
  public:
    virtual int ii(int a)
    {
        _lastCalled = "base int(int)";

        return a;
    }

    String _lastCalled;
};

class IMethodPTestHelper
{
  public:
    virtual int ii(int a) = 0;
};

class MethodPTestHelper : public MethodPTestHelperBase, virtual public IMethodPTestHelper
{
  public:
    int i()
    {
        _lastCalled = "int()";

        return 42;
    }

    int ii(int a) override
    {
        _lastCalled = "int(int)";

        return a * a;
    }

    void vi(int a) { _lastCalled = "void(int)"; }

    MethodPTestStruct sss(const MethodPTestStruct &a, const MethodPTestStruct &b)
    {
        _lastCalled = "struct(struct,struct)";

        return MethodPTestStruct{a.i + b.i, a.s + b.s};
    }
};

template <class MethodType> static void verifyMethodValid(const MethodType &m, bool expectedValid)
{
    REQUIRE(static_cast<bool>(m) == expectedValid);
    REQUIRE((m == nullptr) == !expectedValid);
    REQUIRE((m != nullptr) == expectedValid);
}

class PlainMethodTestHelperNonBase
{
  public:
    virtual int ii(int a)
    {
        _lastCalled = "int(int)";

        return a * a;
    }

    virtual void v() { _lastCalled = "void()"; }

    String _lastCalled;
};
