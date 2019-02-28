
#include <bdn/TextView.h>

namespace bdn
{

    TextView::TextView()
    {
        wrap = true;

        text.onChange() += View::CorePropertyUpdater<String, ITextViewCore>{this, &ITextViewCore::setText};
        wrap.onChange() += View::CorePropertyUpdater<bool, ITextViewCore>{this, &ITextViewCore::setWrap};
    }
}
