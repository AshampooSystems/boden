package io.boden.android;


import android.os.MessageQueue;

public class NativeIdleHandler implements MessageQueue.IdleHandler
{
    public boolean queueIdle()
    {
        nativeOnIdle();

        return true;
    }


    private static native void nativeOnIdle();
}



