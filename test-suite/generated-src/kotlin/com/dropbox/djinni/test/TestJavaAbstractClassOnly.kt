// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from interface_and_abstract_class.djinni

package com.dropbox.djinni.test;

import com.snapchat.djinni.NativeObjectManager;
import java.util.concurrent.atomic.AtomicBoolean;

abstract class TestJavaAbstractClassOnly {

    companion object {
        external fun testMethod(): Boolean
    }

    class CppProxy(private val nativeRef: Long) : TestJavaAbstractClassOnly() {
        private val destroyed = AtomicBoolean(false)

        init {
            if (nativeRef == 0L) throw RuntimeException("nativeRef is zero")
            NativeObjectManager.register(this, nativeRef)
        }

        companion object {
            @kotlin.jvm.JvmStatic
            external fun nativeDestroy(nativeRef: Long)
        }
    }
}
