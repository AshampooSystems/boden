#ifndef BDN_UiAppControllerBase_H_
#define BDN_UiAppControllerBase_H_


#include <bdn/AppControllerBase.h>

namespace bdn
{
	

class UiAppControllerBase : public AppControllerBase
{
public:

	virtual void closeAtNextOpportunityIfPossible(int exitCode) override;
};


}


#endif

