package com.thebhwgroup.demo; //copy the package nane in MainApplication.java

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;

public class HelloWorldModule extends ReactContextBaseJavaModule {
    static {
        System.loadLibrary("hello_world_jni");
    }

    public HelloWorldModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @Override
    public String getName() {
        return "HelloWorld"; //this is how this module is referred to from React Native
    }

    @ReactMethod
    public void helloWorld(ReadableMap structMap, double num, Promise promise) {
        try {
            MyStruct myStruct = new MyStruct(structMap);
            MyStruct newStruct = helloStructJNI(myStruct, num);
            promise.resolve(newStruct.toWritableMap());
        } catch (Exception e) {
            promise.reject("ERR", e);
        }
    }

    public native String helloWorldJNI();

    public native MyStruct helloStructJNI(MyStruct struct, double d);
}
