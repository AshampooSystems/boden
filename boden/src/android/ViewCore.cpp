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
    switch( uiLength.unit )
    {
    case UiLength::Unit::none:
        return 0;

    case UiLength::Unit::dip:
        return uiLength.value;

    case UiLength::Unit::em:
        return uiLength.value * getEmSizeDips();

    case UiLength::Unit::sem:
		return uiLength.value * UiProvider::get().getSemDips(*const_cast<ViewCore*>(this);

    default:
		throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
    }
}

   
Margin ViewCore::uiMarginToDipMargin(const UiMargin& margin) const override
{
    return Margin(
        uiLengthToDips(margin.top),
        uiLengthToDips(margin.right),
        uiLengthToDips(margin.bottom),
        uiLengthToDips(margin.left) );
}



}
}






