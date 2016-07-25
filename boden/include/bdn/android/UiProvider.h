#ifndef BDN_ANDROID_UiProvider_H_
#define BDN_ANDROID_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <cmath>

namespace bdn
{
namespace android
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider()
    {
        activity->configuration->densityDpi
        JavaTextView    textView;

        JavaPaint       paint( textView.getPaint() );

        double          textSize = paint.getTextSize();

        double          uiScaleFactor = getUiScaleFactorForView(textView);

        _semPixelsAtScaleFactor1 = textSize/uiScaleFactor;
    }   

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;



    double getUiScaleFactorForView(JView& javaView)
    {
        JavaDisplay display = javaView.getDisplay();

        JavaDisplayMetrics displayMetrics;
        display.getMetrics( displayMetrics );

        return displayMetrics.getDensity();
    }

    int uiLengthToPixels(const UiLength& uiLength, double uiScaleFactor) const
    {
        if(uiLength.unit==UiLength::sem)
            return std::lround( uiLength.value * _semPixelsAtScaleFactor1 * uiScaleFactor );

        else if(uiLength.unit==UiLength::pixel96)
        {
            return std::lround( uiLength.value * uiScaleFactor );
        }

        else if(uiLength.unit==UiLength::realPixel)
            return std::lround( uiLength.value );

        else
            throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
    }

    Margin uiMarginToPixelMargin(const UiMargin& margin, double uiScaleFactor) const
    {
        return Margin(
            uiLengthToPixels(margin.top, uiScaleFactor),
            uiLengthToPixels(margin.right, uiScaleFactor),
            uiLengthToPixels(margin.bottom, uiScaleFactor),
            uiLengthToPixels(margin.left, uiScaleFactor) );
    }



    static UiProvider& get();


private:
    double _semPixelsAtScaleFactor1;

};

}
}


#endif
