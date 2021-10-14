// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from yaml-test.djinni

package com.dropbox.djinni.test;

import com.snapchat.djinni.NativeObjectManager;
import java.util.concurrent.atomic.AtomicBoolean;

public abstract class ExternInterface1 {
    public abstract com.dropbox.djinni.test.ClientReturnedRecord foo(com.dropbox.djinni.test.ClientInterface i);

    public abstract com.dropbox.djinni.test.Color bar(com.dropbox.djinni.test.Color e);

    public static final class CppProxy extends ExternInterface1
    {
        private final long nativeRef;
        private final AtomicBoolean destroyed = new AtomicBoolean(false);

        private CppProxy(long nativeRef)
        {
            if (nativeRef == 0) throw new RuntimeException("nativeRef is zero");
            this.nativeRef = nativeRef;
            NativeObjectManager.register(this, nativeRef);
        }
        public static native void nativeDestroy(long nativeRef);

        @Override
        public com.dropbox.djinni.test.ClientReturnedRecord foo(com.dropbox.djinni.test.ClientInterface i)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_foo(this.nativeRef, i);
        }
        private native com.dropbox.djinni.test.ClientReturnedRecord native_foo(long _nativeRef, com.dropbox.djinni.test.ClientInterface i);

        @Override
        public com.dropbox.djinni.test.Color bar(com.dropbox.djinni.test.Color e)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_bar(this.nativeRef, e);
        }
        private native com.dropbox.djinni.test.Color native_bar(long _nativeRef, com.dropbox.djinni.test.Color e);
    }
}
