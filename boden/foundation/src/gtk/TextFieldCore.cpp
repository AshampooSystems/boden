#include <bdn/init.h>
#include <bdn/gtk/TextFieldCore.h>

namespace bdn
{
namespace gtk
{
 
TextFieldCore::TextFieldCore(View* pOuter)
	         : ViewCore(pOuter, gtk_entry_new())
{
    GtkWidget* widget = getGtkWidget();
    g_signal_connect(GTK_WIDGET(widget), "activate",     G_CALLBACK(activateCallback),    this);
    
    connectChangedHandler();
    setText(cast<TextField>(pOuter)->text());
}

void TextFieldCore::setText(const String& text)
{
    const gchar* gtkText = gtk_entry_get_text(GTK_ENTRY(getGtkWidget()));
    if (String(gtkText) != text) {
        // Disconnect and re-connect handlers so as to avoid triggering them
        // by programmatic changes to the text property
        disconnectChangedHandler();
        gtk_entry_set_text(GTK_ENTRY(getGtkWidget()), text.asUtf8Ptr());
        connectChangedHandler();
    }
}

void TextFieldCore::changedCallback(GtkEditable* editable, gpointer user_data)
{
    TextFieldCore* pCore = (TextFieldCore*)user_data;
    
    P<TextField> pOuter = cast<TextField>( pCore->getOuterViewIfStillAttached() );
    if (pOuter!=nullptr)
    {
        const gchar* gtkText = gtk_entry_get_text(GTK_ENTRY(editable));
        pOuter->setText( gtkText );
    }
}

void TextFieldCore::activateCallback(GtkEditable* editable, gpointer user_data)
{
    TextFieldCore* pCore = (TextFieldCore*)user_data;
    
    P<TextField> pOuter = cast<TextField>( pCore->getOuterViewIfStillAttached() );
    if (pOuter!=nullptr)
        pOuter->submit();
}

void TextFieldCore::connectChangedHandler()
{
    GtkWidget* widget = getGtkWidget();
    _changedHandlerId = g_signal_connect(GTK_WIDGET(widget), "changed",     G_CALLBACK(changedCallback),    this);
}

void TextFieldCore::disconnectChangedHandler()
{
    GtkWidget* widget = getGtkWidget();
	g_signal_handler_disconnect(G_OBJECT(widget), _changedHandlerId);
}

}
}
