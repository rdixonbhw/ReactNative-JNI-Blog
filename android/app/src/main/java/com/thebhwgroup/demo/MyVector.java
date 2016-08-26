package com.thebhwgroup.demo;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.WritableMap;

public class MyVector {
    double x;
    double y;

    public MyVector() {}

    public MyVector(double x, double y) {
        this.x = x;
        this.y = y;
    }

    public MyVector(ReadableMap map) {
        this(
                map.getDouble("x"),
                map.getDouble("y")
        );
    }

    public WritableMap toWritableMap() {
        WritableMap map = Arguments.createMap();

        map.putDouble("x", x);
        map.putDouble("y", y);

        return map;
    }
}
