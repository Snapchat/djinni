// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from yaml-test.djinni

package com.dropbox.djinni.test;

import com.snapchat.djinni.NativeObjectManager;
import java.util.concurrent.atomic.AtomicBoolean;

abstract class ExternInterface1 {
    abstract fun foo(i: com.dropbox.djinni.test.ClientInterface): com.dropbox.djinni.test.ClientReturnedRecord

    abstract fun bar(e: com.dropbox.djinni.test.Color): com.dropbox.djinni.test.Color

    class CppProxy(private val nativeRef: Long) : ExternInterface1() {
        private val destroyed = AtomicBoolean(false)

        init {
            if (nativeRef == 0L) throw RuntimeException("nativeRef is zero")
            NativeObjectManager.register(this, nativeRef)
        }

        companion object {
            @kotlin.jvm.JvmStatic
            external fun nativeDestroy(nativeRef: Long)
        }

        override fun foo(i: com.dropbox.djinni.test.ClientInterface): com.dropbox.djinni.test.ClientReturnedRecord  {
            assert(!destroyed.get()) { "trying to use a destroyed object" }
            return native_foo(nativeRef, i)
        }
        private external fun native_foo(_nativeRef: Long, i: com.dropbox.djinni.test.ClientInterface): com.dropbox.djinni.test.ClientReturnedRecord

        override fun bar(e: com.dropbox.djinni.test.Color): com.dropbox.djinni.test.Color  {
            assert(!destroyed.get()) { "trying to use a destroyed object" }
            return native_bar(nativeRef, e)
        }
        private external fun native_bar(_nativeRef: Long, e: com.dropbox.djinni.test.Color): com.dropbox.djinni.test.Color
    }
}
