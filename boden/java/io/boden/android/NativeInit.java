package io.boden.android;

/**
 * Handles initialization of the native-code side.
 *
 * If you use NativeRootActivity then the activity already takes care of
 * calling this and you do not have to do anything with this class.
 *
 * If you do not use NativeRootActivity then you should first create the
 * NativeRootView object that you want to host the native UI and then
 * you should call NativeInit.launch().
 *
 */
public class NativeInit
{
    /** Loads the base boden library and ensures that needed global components are initialized.
     *  Does not yet load the native library with the actual app code - that one is loaded during
     *  launc().
     *
     *  baseInit can be called multiple times. Subsequent calls have no effect.
     */
    static void baseInit()
    {
        if(!mBaseInitialized)
        {
            // load the boden library
            System.loadLibrary("boden");

            // ensure that the NativeHandler object exists
            NativeHandler.getMainNativeHandler();

            mBaseInitialized = true;
        }
    }

    /** Launches the native side.
     *
     *  @param nativeLibName name of the native code library that contains the
     *  actual code of the app.
     */
    public static void launch(String nativeLibName)
    {
        if(!mLaunched)
        {
            System.loadLibrary( nativeLibName );

            mLaunched = true;

            launch();
        }
    }

    private static boolean mBaseInitialized = false;
    private static boolean mLaunched = false;

    private static native void launch();


};

