package com.dropbox.djinni.test;

public class SimpleObjectImpl extends SimpleObject {
    private final int value;
    private final String name;

    public SimpleObjectImpl(int value, String name) {
        this.value = value;
        this.name = name;
    }

    @Override
    public int getValue() {
        return value;
    }

    @Override
    public String getName() {
        return name;
    }
}
