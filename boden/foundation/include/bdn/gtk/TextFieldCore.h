#ifndef BDN_GTK_TextFieldCore_H_
#define BDN_GTK_TextFieldCore_H_

#include <bdn/TextField.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/gtk/ViewCore.h>
#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{

class TextFieldCore : public ViewCore, BDN_IMPLEMENTS ITextFieldCore
{
public:
    TextFieldCore(View* pOuter);

    void setText(const String& text) override;

    
private:
    void connectChangedHandler();
    void disconnectChangedHandler();
    
    static void changedCallback(GtkEditable* editable, gpointer user_data);
    static void activateCallback(GtkEditable* editable, gpointer user_data);

    gulong _changedHandlerId = 0;

    String _lastText;
};


}
}

#endif // BDN_GTK_TextFieldCore_H_