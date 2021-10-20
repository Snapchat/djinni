package com.dropbox.djinni.test;

import com.snapchat.djinni.NativeObjectManager;
import com.snapchat.djinni.Future;
import java.util.concurrent.atomic.AtomicBoolean;
import javax.annotation.CheckForNull;
import javax.annotation.Nonnull;

public abstract class AsyncInterface {
    @Nonnull
    public abstract Future<Integer> getAsyncResult();
}
