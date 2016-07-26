#include <bdn/init.h>
#include <bdn/android/UiProvider.h>

namespace bdn
{
namespace android
{

BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

double UiProvider::getSemPixelsForView(ViewCore& viewCore)
{
    if (_semPixelsAtScaleFactor1 == -1)
    {
        // sem size is not yet initialized.

        JTextView textView(viewCore.getJView().getContext());
        JTextPaint paint(textView.getPaint());

        double textSize = paint.getTextSize();

        double uiScaleFactor = viewCore.getUiScaleFactor();

        _semPixelsAtScaleFactor1 = textSize / uiScaleFactor;
    }

    return _semPixelsAtScaleFactor1 * viewCore.getUiScaleFactor();
}


}
}






