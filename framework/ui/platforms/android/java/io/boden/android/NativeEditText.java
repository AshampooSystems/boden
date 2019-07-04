package io.boden.android;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Typeface;
import android.os.Build;
import android.util.TypedValue;

import androidx.appcompat.widget.AppCompatEditText;

import java.lang.annotation.Target;

class NativeEditText extends AppCompatEditText
{
    public NativeEditText(Context context) {
        super(context);
    }


    private static final int Inherit=0;
    private static final int Medium = 1;
    private static final int Small = 2;
    private static final int XSmall = 3;
    private static final int XXSmall = 4;
    private static final int Large = 5;
    private static final int XLarge = 6;
    private static final int XXLarge = 7;
    private static final int Percent = 8;
    private static final int Points = 9;
    private static final int Pixels = 10;

    private static final int Normal = 400;
    private static final int Bold = 700;

    public void setFont(String family, int sizeType, float size, int weight, boolean italic)
    {
        Typeface typeFace = null;

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            if (family.isEmpty()) {
                typeFace = Typeface.defaultFromStyle(Typeface.NORMAL);
            } else {
                typeFace = Typeface.create(family, Typeface.NORMAL);
            }

            if(weight == -1) {
                weight = 400;
            }

            weight = Math.max(0, Math.min(weight, 1000));

            if (italic || (weight != Normal)) {
                typeFace = Typeface.create(typeFace, weight, italic);
            }
        } else {
            int style = 0;

            if(weight >= Bold) {
                style = Typeface.BOLD;
            }

            if(italic) {
                style |= Typeface.ITALIC;
            }

            if(family.isEmpty()) {
                typeFace = Typeface.defaultFromStyle(style);
            } else {
                typeFace = Typeface.create(family, style);
            }
        }

        if(typeFace != null) {
            setTypeface(typeFace);

            float defaultTextSize = new NativeEditText(this.getContext()).getTextSize();

            float fontSize = defaultTextSize;

            switch(sizeType) {
                case Inherit:
                    break;
                case Medium:
                    break;
                case Small:
                    fontSize *= 0.75;
                    break;
                case XSmall:
                    fontSize *= 0.5;
                    break;
                case XXSmall:
                    fontSize *= 0.25;
                    break;
                case Large:
                    fontSize *= 1.25;
                    break;
                case XLarge:
                    fontSize *= 1.5;
                    break;
                case XXLarge:
                    fontSize *= 1.75;
                    break;
                case Percent:
                    fontSize *= size;
                    break;
                default:
                    break;
            }

            if(sizeType == Points) {
                setTextSize(TypedValue.COMPLEX_UNIT_PT, fontSize);
            } else {
                setTextSize(TypedValue.COMPLEX_UNIT_PX, fontSize);
            }
        }
    }
}