#ifndef BDN_AppNotInstantiatedError_H_
#define BDN_AppNotInstantiatedError_H_


namespace bdn
{

class AppNotInstantiatedError : public std::runtime_error
{
public:
	AppNotInstantiatedError()
		: std::runtime_error("No app object was instantiated.")
	{
	}
    
};
    
   
}



#endif


