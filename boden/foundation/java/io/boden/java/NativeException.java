package io.boden.java;

/** Represents an exception on the native code side.
 */
public class NativeException extends RuntimeException {


    /** NativeExceptionData is the wrapper of a native-side object that
     *  contains the actual native exception object.
     */
    public NativeException(String message, NativeStrongPointer nativeExceptionData )    {
        super(message);

        mNativeExceptionData = nativeExceptionData;
    }


    public NativeStrongPointer getNativeExceptionData() {
        return mNativeExceptionData;
    }


    private NativeStrongPointer mNativeExceptionData;

}
