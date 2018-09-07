#include <bdn/init.h>
#include <bdn/webems/TextFieldCore.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

extern "C" {

void bdn_webems_TextFieldCore_onInput(void *textFieldCore)
{
    bdn::webems::TextFieldCore::onInputCallback(textFieldCore);
}
}

namespace bdn
{
    namespace webems
    {

        TextFieldCore::TextFieldCore(TextField *pOuterTextField)
            : ViewCore(pOuterTextField, "input")
        {
            EM_ASM_(
                {
                    bdn_webems_TextFieldCore_onInput = Module.cwrap(
                        'bdn_webems_TextFieldCore_onInput', 'null', ['number']);

                    document.getElementById(Pointer_stringify($1)).oninput =
                        function()
                    {
                        bdn_webems_TextFieldCore_onInput($0);
                    }
                },
                this, _domObject["id"].as<std::string>().c_str());

            emscripten_set_keydown_callback(
                _domObject["id"].as<std::string>().c_str(), this, false,
                onKeyDownCallback);

            setText(pOuterTextField->text());
        }

        void TextFieldCore::onInputCallback(void *textFieldCore)
        {
            TextFieldCore *core = (TextFieldCore *)textFieldCore;
            P<TextField> outer =
                cast<TextField>(core->getOuterViewIfStillAttached());
            if (outer) {
                outer->setText(core->_domObject["value"].as<std::string>());
            }
        }

        EM_BOOL TextFieldCore::onKeyDownCallback(
            int eventType, const EmscriptenKeyboardEvent *keyEvent,
            void *userData)
        {
            if (keyEvent->keyCode == 13) {
                TextFieldCore *core = (TextFieldCore *)userData;
                P<TextField> outer =
                    cast<TextField>(core->getOuterViewIfStillAttached());
                if (outer) {
                    outer->submit();
                }
                return true;
            }
            return false;
        }
    }
}
