package io.boden.android;


import android.os.Looper;
import android.os.MessageQueue;
import android.content.Intent;

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
     *
     *  @param nativeLibName name of the native code library that contains the
     *  native code of the app and provides the app controller.
     *
     */
    static void baseInit(String nativeLibName)
    {
        if(!mBaseInitialized)
        {
            System.loadLibrary(nativeLibName);

            mBaseInitialized = true;
        }
    }

    /** Launches the native side.
     */
    public static void launch( Intent intent )
    {
        if(!mLaunched)
        {
            mLaunched = true;

            nativeLaunch(intent);
        }
    }

    private static boolean mBaseInitialized = false;
    private static boolean mLaunched = false;

    private static native void nativeLaunch(Intent intent);




};

