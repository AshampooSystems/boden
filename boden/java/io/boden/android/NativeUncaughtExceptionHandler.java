package io.boden.android;

import java.lang.Thread;


/** Global uncaught exception handler.*/
public class NativeUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {


    public  static void install()    {

        if(mGlobalInstance==null) {
            mOriginalHandler = Thread.getDefaultUncaughtExceptionHandler();

            mGlobalInstance = new NativeUncaughtExceptionHandler( Thread.currentThread().getId() );
            Thread.setDefaultUncaughtExceptionHandler(mGlobalInstance);
        }
    }

    public static void uninstall() {
        if(mGlobalInstance!=null) {
            Thread.setDefaultUncaughtExceptionHandler(mOriginalHandler);
            mGlobalInstance = null;
        }
    }


    public NativeUncaughtExceptionHandler(long mainThreadId)
    {
        mMainThreadId = mainThreadId;
    }


    /** This can be called from dispatchers when an exception is encountered at a point
     *  before it is let through to the core message loop.
     *
     *  The return value is true if the exception should be ignored and false if the
     *  exception should be let through to the core message loop.
     */
    public static boolean uncaughtExceptionFromDispatcher(Throwable e, boolean canKeepRunning)
    {
        return nativeUncaughtException(e, canKeepRunning);
    }

    public void uncaughtException(Thread t, Throwable e)    {

        // uncaughtException is called when an exception reaches the root of the thread function
        // of the specified thread.
        // If this happens in the main thread then the app cannot continue.
        // The reason is that when an exception reaches the root of the main thread
        // then the message loop is already exited and the app cannot continue.
        // So canKeepRunning must be false there.
        // For other threads the app can theoretically continue, so canKeepRunning must be true.

        // Note that NativeDispatcher also has custom exception handling code that catches exceptions
        // before they reach the root of the main thread. So for those the app will also get a chance
        // to ignore them. But if uncaughtException is called for the main thread then there is no way to continue.

        boolean canKeepRunning = (t.getId()!=mMainThreadId);

        boolean wantsToKeepRunning = nativeUncaughtException(e, canKeepRunning);

        if(wantsToKeepRunning && canKeepRunning)
        {
            // we should ignore the exception.
            // Just do nothing.
            int x=0;
            x+=1;
        }
        else
        {
            // we should crash. Call the original handler if there was one and
            // then exit. Note that the original handler might already schedule an exit.
            if (mOriginalHandler != null)
                mOriginalHandler.uncaughtException(t, e);

            System.exit(1);
        }
    }


    /** Returns true if the app should continue running, false otherwise.*/
    private native static boolean nativeUncaughtException(Throwable e, boolean canKeepRunning);

    private long mMainThreadId;

    private static NativeUncaughtExceptionHandler mGlobalInstance;
    private static Thread.UncaughtExceptionHandler mOriginalHandler;
}
