#ifndef BDN_GTK_TextViewCore_H_
#define BDN_GTK_TextViewCore_H_


#include <bdn/gtk/ButtonCoreBase.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
public:
    TextViewCore(View* pOuter)
    : ViewCore( pOuter,
                gtk_label_new( cast<TextView>(pOuter)->text().get().asUtf8Ptr() ) )
    {
    }
    
    
    void setPadding(const Nullable<UiMargin>& uiPadding)
    {
        // not reflected by gtk widget
    }
    
    void setText(const String& text) override
    {
        gtk_label_set_text( GTK_LABEL( getGtkWidget() ), text.asUtf8Ptr() );
    }
        

};


}
}

#endif
