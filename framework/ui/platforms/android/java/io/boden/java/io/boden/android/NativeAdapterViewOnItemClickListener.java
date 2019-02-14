package io.boden.android;

import android.view.View;
import android.widget.AdapterView;

public class NativeAdapterViewOnItemClickListener extends Object implements AdapterView.OnItemClickListener
{
    public NativeAdapterViewOnItemClickListener()
    {
    }

    public void	onItemClick(AdapterView<?> parent, View view, int position, long id)
    {
        nativeOnItemClick(parent, view, position, id);
    }

    public native void nativeOnItemClick(AdapterView<?> parent, View view, int position, long id);

}
