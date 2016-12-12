package io.boden.java;

import java.nio.ByteBuffer;

/** Wrapper for a strong native code pointer to an bdn::IBase object.*/
public class NativeStrongPointer extends Object
{
    public NativeStrongPointer(ByteBuffer wrappedPointer)
    {
        mWrappedPointer = wrappedPointer;
    }

    public void dispose()
    {
        if(mWrappedPointer!=null)
        {
            disposed(mWrappedPointer);
            mWrappedPointer = null;
        }
    }

    public void finalize() throws Throwable
    {
        dispose();

        super.finalize();
    }

    public java.nio.ByteBuffer getWrappedPointer()
    {
        return mWrappedPointer;
    }


    private native void disposed(java.nio.ByteBuffer wrappedPointer);

    private java.nio.ByteBuffer mWrappedPointer;
}


