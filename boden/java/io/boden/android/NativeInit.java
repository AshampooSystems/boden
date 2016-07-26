package io.boden.android;

/**
 * Handles initialization of the native-code side.
 *
 * If you use NativeRootActivity then the activity already takes care of
 * calling this and you do not have to do anything with this class.
 *
 * If you do not use NativeRootActivity then you must call AppInit.ensureInitialized
 * at least once before you start using any of the native components.
 *
 *
 */
public class NativeInit
{

    /** Ensures that the native code side is properly initialized,
     *
     *  This can be called multiple times - subsequent calls will have no effect.
     *
     *
     * @param nativeLibName name of the native code library that contains the
     *  actual code of the app.
     */
    public static void ensureInitialized(String nativeLibName)
    {
        if(!mInitialized)
        {
            System.loadLibrary( nativeLibName );

            NativeHandler.get();

            mInitialized = true;

            init();
        }
    }

    private static boolean mInitialized = false;

    private static native void init();


};

