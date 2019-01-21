
#include <bdn/TextView.h>

namespace bdn
{

    TextView::TextView()
    {
        text.onChange() += View::CorePropertyUpdater<String, ITextViewCore>{
            this, &ITextViewCore::setText, [](auto &inf) { inf.influencesPreferredSize(); }};
    }
}
