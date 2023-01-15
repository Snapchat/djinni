// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from outcome.djinni

package com.dropbox.djinni.test;

import com.snapchat.djinni.NativeObjectManager;
import java.util.concurrent.atomic.AtomicBoolean;

abstract class TestOutcome {

    companion object {
        external fun getSuccessOutcome(): com.snapchat.djinni.Outcome<String, Int>

        external fun getErrorOutcome(): com.snapchat.djinni.Outcome<String, Int>

        external fun putSuccessOutcome(x: com.snapchat.djinni.Outcome<String, Int>): String

        external fun putErrorOutcome(x: com.snapchat.djinni.Outcome<String, Int>): Int

        external fun getNestedSuccessOutcome(): NestedOutcome

        external fun getNestedErrorOutcome(): NestedOutcome

        external fun putNestedSuccessOutcome(x: NestedOutcome): Int

        external fun putNestedErrorOutcome(x: NestedOutcome): String
    }

    class CppProxy(private val nativeRef: Long) : TestOutcome() {
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
