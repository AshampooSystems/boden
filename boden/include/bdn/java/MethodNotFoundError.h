#ifndef BDN_JAVA_METHODNOTFOUNDERROR_H_
#define BDN_JAVA_METHODNOTFOUNDERROR_H_


namespace bdn
{
namespace java
{

class MethodNotFoundError : public std::exception
{
public:
    MethodNotFoundError(const String& className, const String& methodName, const String& signature)
    {
        _msg = String( "Java method not found (class: "+className+", method: "+methodName+", signature: "+signature).asUtf8();
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


