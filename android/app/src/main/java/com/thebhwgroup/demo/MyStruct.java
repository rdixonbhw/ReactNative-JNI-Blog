package com.thebhwgroup.demo;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.WritableMap;

public class MyStruct {
    int vector_count;
    MyVector[] vectors;
    MyVector position;

    public MyStruct() {}

    public MyStruct(int vector_count, MyVector[] vectors, MyVector position) {
        this.vector_count = vector_count;
        this.vectors = vectors;
        this.position = position;
    }

    public MyStruct(ReadableMap map) {
        this( //call the (int, MyVector[], MyVector) constructor
                map.getInt("vector_count"), //get the int for vector_count from map
                MyVectorArrayFromReadableArray(map.getArray("vectors")), //get the ReadableArray for vectors and convert to MyVector[]
                new MyVector(map.getMap("position")) //get the ReadableMap of position and pass it to the MyVector(ReadableMap) constructor
        );
    }

    public WritableMap toWritableMap() {
        WritableMap map = Arguments.createMap(); //make new map

        map.putInt("vector_count", vector_count); //put integer for vector_count in map

        WritableArray arr = Arguments.createArray(); //make WritableArray for vectors
        for (MyVector v : vectors) {
            arr.pushMap(v.toWritableMap()); //convert every MyVector to a WritableMap and add it to the WritableArray
        }
        map.putArray("vectors", arr); //add the now complete WritableArray to map

        map.putMap("position", position.toWritableMap()); //convert position to a WritableMap and add it to map

        return map;
    }

    //this could also be put in a separate utility class, especially if it's going to used elsewhere
    private static MyVector[] MyVectorArrayFromReadableArray(ReadableArray arr) {
        MyVector[] vectors = new MyVector[arr.size()]; //allocate new MyVector[]
        for (int i = 0; i < arr.size(); ++i) {
            vectors[i] = new MyVector(arr.getMap(i)); //get a ReadableMap for each element of arr, and give it to the MyVector(ReadableMap) constructor
        }
        return vectors;
    }
}
