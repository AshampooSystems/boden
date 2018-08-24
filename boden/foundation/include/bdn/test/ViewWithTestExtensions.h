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

    ~ViewWithTestExtensions()
    {
        if(_destructFunc)
            _destructFunc(this);
    }


    /** Sets a function that is executed from the object's destructor.
    
        The function gets a pointer to the object being destructed as its sole parameter.
    */
    void setDestructFunc( std::function< void( ViewWithTestExtensions<BaseViewClass>* ) > func)
    {
        _destructFunc = func;
    }

protected:
    std::function< void( ViewWithTestExtensions<BaseViewClass>* ) > _destructFunc;

};





}
}

#endif