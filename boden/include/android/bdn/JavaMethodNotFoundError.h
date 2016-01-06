#ifndef _BDN_JAVAMETHODNOTFOUNDERROR_H_
#define _BDN_JAVAMETHODNOTFOUNDERROR_H_

#include <exception>
#include <string>

namespace bdn
{

class JavaMethodNotFoundError : public std::exception
{
public:
    JavaMethodNotFoundError(const std::string& name, const std::string& signature)
    {
        _msg = "Java method not found: "+name+" (signature: "+signature+")";
    }
    
    virtual const char* what() const noexcept override
    {
        return _msg.c_str();
    }

protected:
    std::string _msg;
};
    
   
}



#endif


