#ifndef _BDN_JAVACLASSNOTFOUNDERROR_H_
#define _BDN_JAVACLASSNOTFOUNDERROR_H_

#include <exception>
#include <string>

namespace bdn
{

class JavaClassNotFoundError : public std::exception
{
public:
    JavaClassNotFoundError(const std::string& name)
    {
        _msg = "Java class not found: "+name;
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


