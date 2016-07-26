#include <bdn/init.h>
#include <bdn/android/ViewCore.h>

#include <bdn/android/UiProvider.h>


#include <cmath>

namespace bdn
{
namespace android
{


int ViewCore::uiLengthToPixels(const UiLength& uiLength) const
{
    if(uiLength.unit==UiLength::sem)
        return (int)std::lround( uiLength.value * UiProvider::get().getSemPixelsForView(*const_cast<ViewCore*>(this)) );

    else if(uiLength.unit==UiLength::pixel96)
        return (int)std::lround( uiLength.value * getUiScaleFactor() );

    else if(uiLength.unit==UiLength::realPixel)
        return (int)std::lround( uiLength.value );

    else
        throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
}


}
}






