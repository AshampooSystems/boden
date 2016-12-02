package io.boden.java;


/** A runnable that executes a piece of native code.
 *
 *  The native code object will automatically be released after the first run and
 *  subsequent run() calls will have no effect.
 *
 *  Using NativeOnceRunnable instead of NativeRunnable has the advantage that
 *  the native code object is released immediately after the runnable is executed.
 *  So the release is not postponed until the Java garbage collector releases the object.
 *
 * */
public class NativeOnceRunnable extends NativeRunnable
{
    public NativeOnceRunnable(NativeStrongPointer nativePointer)
    {
        super(nativePointer);
    }

    @Override
    public void run()
    {
        try {
            super.run();

        }
        finally {
            dispose();
        }
    }
}
