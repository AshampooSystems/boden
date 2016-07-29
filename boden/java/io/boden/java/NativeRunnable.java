package io.boden.java;


/** A runnable that executes a piece of native code.
 *
 *  If you only need the runnable to be executed once then you should consider
 *  using NativeOnceRunnable instead (which immediately releases the native code object
 *  after run is called).
 * */
public class NativeRunnable implements Runnable
{
    public NativeRunnable(NativeStrongPointer nativeCallable)
    {
        _nativeCallable = nativeCallable;
    }

    @Override
    public void run()
    {
        if(_nativeCallable!=null)
            nativeRun(_nativeCallable);
    }


    /** Disposes the runnable and releases the native code object.
     *  Subsequence run() calls will have no effect.*/
    public void dispose()
    {
        _nativeCallable.dispose();
        _nativeCallable = null;
    }

    private native void nativeRun(NativeStrongPointer nativeCallable);

    private NativeStrongPointer _nativeCallable;
}
