#ifndef BDN_ANDROID_IParentViewCore_H_
#define BDN_ANDROID_IParentViewCore_H_


namespace bdn
{
namespace android
{

/** Interface for view cores that can act as parents for other view cores.*/
class IParentViewCore : BDN_IMPLEMENTS IBase
{
public:

    /** Adds a child Ui element to the parent.*/
    virtual void addChildJView( JView view )=0;


    /** Returns the current UI scale factor. This depends on the pixel density
     *  of the current display. On high DPI displays the scale factor is >1 to scale
     *  up UI elements to use more physical pixels.
     *
     *  Scale factor 1 means a "medium" DPI setting of 160 dpi.
     *
     *  Note that the scale factor can also be <1 if the display has a very low dpi (lower than 160).
     *
     *  Also note that the scale factor can change at runtime if the view switches to another display.
     *
     **/
    virtual double getUiScaleFactor() const=0;

};


}
}

#endif


