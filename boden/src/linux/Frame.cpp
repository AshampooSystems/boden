#include <bdn/init.h>
#include <bdn/Frame.h>

namespace bdn
{

P<IFrame> createFrame(const String& title)
{
    return newObj<Frame>(title);    
}


}

