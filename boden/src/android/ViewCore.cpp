#include <bdn/init.h>
#include <bdn/android/ViewCore.h>

#include <bdn/android/UiProvider.h>


#include <cmath>

namespace bdn
{
namespace android
{


double ViewCore::uiLengthToDips(const UiLength& uiLength) const
{
    if(uiLength.unit==UiLength::sem)
        return uiLength.value * UiProvider::get().getSemDips(*const_cast<ViewCore*>(this));

    else if(uiLength.unit==UiLength::dip)
        return uiLength.value;

    else
        throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
}


}
}






