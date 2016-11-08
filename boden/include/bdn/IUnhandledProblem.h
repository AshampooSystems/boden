#ifndef BDN_IUnhandledProblem_H_
#define BDN_IUnhandledProblem_H_

namespace bdn
{


/** Contains information about an unhandled problem.*/
class IUnhandledProblem : BDN_IMPLEMENTS IBase
{
public:

    enum class Type
    {
        /** The problem is an C++ unhandled exception. It might or might not be
            derived from std::exception.*/
        exception,

        /** The problem is a "crash". I.e. an error condition other than an unhandled C++ exception
        that causes the app to terminate.*/
        crash
    };


    /** Returns the problem type.*/
    virtual Type getType() const=0;

        


    /** Rethrows the problem as a C++ exception.

        If the problem is not based on an exception then a suitable exception object   
        that represents the problem is thrown instead.
    */
    virtual void throwAsException() const=0;



    /** Returns a short string representation of the problem that is suitable for displaying to an end user.*/
    virtual String getErrorMessage() const=0;


    /** Returns a detailed string representation of the problem that is suitable for internal diagnostics.*/
    virtual String toString() const=0;

};


}



#endif


