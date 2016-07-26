package io.boden.android;

import android.os.Handler;
import android.os.Looper;


public class NativeHandler extends Handler
{
    private static NativeHandler _instance;

    public NativeHandler(Looper looper)
    {
        super(looper);
    }

    public static NativeHandler get()
    {
        if(_instance==null)
            _instance = new NativeHandler( Looper.getMainLooper() );

        return _instance;
    }


}
