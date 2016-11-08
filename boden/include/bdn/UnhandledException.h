#ifndef BDN_UnhandledException_H_
#define BDN_UnhandledException_H_

namespace bdn
{


/** IUnhandledProblem implementation for unhandled C++ exceptions.*/
class UnhandledException : public Base, BDN_IMPLEMENTS IBase
{
public:
    UnhandledException( const std::exception_ptr& p );


    /** Returns the problem type.*/
    Type getType() const override
    {
        return Type::exception;
    }

        
    void throwAsException() const override;

    String getErrorMessage() const override;

    String toString() const override;

private:
    void ensureStringsInitialized();

    std::exception_ptr  _exceptionPtr;

    bool                _stringsInitialized = false;

    String              _errorMessage;
    String              _detailedString;
};


}



#endif