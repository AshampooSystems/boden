#ifndef BDN_WEBEMS_TextFieldCore_H_
#define BDN_WEBEMS_TextFieldCore_H_

#include <emscripten/html5.h>

#include <bdn/webems/ViewCore.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/TextField.h>


namespace bdn
{
namespace webems
{


class TextFieldCore : public ViewCore, BDN_IMPLEMENTS ITextFieldCore
{
public:
    TextFieldCore(TextField* pOuterTextField);

    void setText(const String& text) override
    {
        _domObject.set("value", text.asUtf8());
    }

    static void onInputCallback(void* textFieldCore);
    static EM_BOOL onKeyDownCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);

protected:
    void setDefaultPadding() override
    {
        ViewCore::setDefaultPadding();
    }

    void setNonDefaultPadding(const Margin& padding) override
    {
        ViewCore::setNonDefaultPadding(padding);
    }
};

}    
}

#endif