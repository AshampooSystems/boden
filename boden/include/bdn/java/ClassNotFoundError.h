#ifndef BDN_JAVA_ClassNotFoundError_H_
#define BDN_JAVA_ClassNotFoundError_H_


namespace bdn
{
namespace java
{

class ClassNotFoundError : public std::exception
{
public:
    ClassNotFoundError(const String& name)
    {
        _msg = ("Java class not found: "+name).asUtf8();
    }
    
    virtual const char* what() const noexcept override
    {
        return _msg.c_str();
    }

protected:
    std::string _msg;
};
    

}
}



#endif


