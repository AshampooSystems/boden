#include "PropertiesPage.h"
#include <bdn/ui/yoga.h>

namespace bdn
{
    std::shared_ptr<ContainerView> createPropertyDemoPage()
    {
        auto page = std::make_shared<Properties>();
        page->init();
        return page;
    }
}
