package com.thebhwgroup.demo; //copy the package nane in MainApplication.java

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

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
    public void helloWorld(Promise promise) {
        try {
            MyVector[] vectors = {new MyVector(1, 1), new MyVector(2, 2), new MyVector(3, 3)};
            MyStruct myStruct = new MyStruct(
                    3,
                    vectors,
                    new MyVector(10, 15));
            MyStruct newStruct = helloStructJNI(myStruct, 47);

            String hello = helloWorldJNI();
            promise.resolve(hello);
        } catch (Exception e) {
            promise.reject("ERR", e);
        }
    }

    public native String helloWorldJNI();

    public native MyStruct helloStructJNI(MyStruct struct, double d);
}
