
#include <bdn/TextView.h>

namespace bdn
{

    TextView::TextView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider)) { wrap = true; }

    void TextView::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<TextViewCore>();
        textCore->text.bind(text);
        textCore->wrap.bind(wrap);
    }
}
