#ifndef BDN_TEST_ViewWithTestExtensions_H_
#define BDN_TEST_ViewWithTestExtensions_H_


namespace bdn
{
namespace test
{

/** A mixin that adds some test extensions to a class that derives from view.
    
    Exposes some statistics to test how often certain functions have been called.
    */
template<class BaseViewClass>
class ViewWithTestExtensions : public BaseViewClass
{
public:
    template<typename... Arguments>
	ViewWithTestExtensions(Arguments... args)
        : BaseViewClass(args...)
	{		
	}

};





}
}

#endif