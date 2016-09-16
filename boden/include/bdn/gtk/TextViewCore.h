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
private:
    static GtkWidget* createLabel( TextView* pOuter )
    {
        GtkWidget* pLabel = gtk_label_new( pOuter->text().get().asUtf8Ptr() );
        
        gtk_label_set_line_wrap( GTK_LABEL(pLabel), TRUE );        
        
        return pLabel;
    }
    
public:
    TextViewCore(View* pOuter)
    : ViewCore( pOuter,
                createLabel( cast<TextView>(pOuter) ) )
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
        
        
protected:
    bool canAdjustWidthToAvailableSpace() const override
    {
        return true;
    }

};


}
}

#endif
