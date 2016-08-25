package com.thebhwgroup.demo;

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
}
