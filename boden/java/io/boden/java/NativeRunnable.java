package io.boden.java;


/** A runnable that executes a piece of native code.*/
public class NativeRunnable implements Runnable
{
    public NativeRunnable(Object nativeObject)
    {
        _nativeObject = nativeObject;
    }

    public void run()
    {
        nativeRun(_nativeObject);
    }

    private native void nativeRun(Object nativeObject);

    private Object _nativeObject;
}
