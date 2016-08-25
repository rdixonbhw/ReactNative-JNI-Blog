#include <android/log.h>
#include <jni.h>
#include <stdlib.h>
#include "defs.h"

#define LOG_TAG "jni_hello_world"
//Helper functions for android logging
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


//MyVector
jclass MY_VECTOR;
jmethodID MY_VECTOR_CONS;
jfieldID MY_VECTOR_X;
jfieldID MY_VECTOR_Y;

//MyStruct
jclass MY_STRUCT;
jmethodID MY_STRUCT_CONS;
jfieldID MY_STRUCT_VECTOR_COUNT;
jfieldID MY_STRUCT_VECTORS;
jfieldID MY_STRUCT_POSITION;

jclass JAVA_EXCEPTION;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if ((*vm)->GetEnv(vm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    MY_VECTOR = (jclass) (*env)->NewGlobalRef(env, (*env)->FindClass(env, "com/thebhwgroup/demo/MyVector"));
    MY_VECTOR_CONS = (*env)->GetMethodID(env, MY_VECTOR, "<init>", "(DD)V");
    MY_VECTOR_X = (*env)->GetFieldID(env, MY_VECTOR, "x", "D");
    MY_VECTOR_Y = (*env)->GetFieldID(env, MY_VECTOR, "y", "D");

    MY_STRUCT = (jclass) (*env)->NewGlobalRef(env, (*env)->FindClass(
            env, //the current JNI interface pointer
            "com/thebhwgroup/demo/MyStruct" //the fully qualified class name of MyStruct in Java
    ));
    MY_STRUCT_CONS = (*env)->GetMethodID(
            env, //the current JNI interface pointer
            MY_STRUCT, //a reference to the jclass for MyStruct
            "<init>", //the method name, constructor method names are always "<init>"
            "(I[Lcom/thebhwgroup/demo/MyVector;Lcom/thebhwgroup/demo/MyVector;)V" //the method signature
    );
    MY_STRUCT_VECTOR_COUNT = (*env)->GetFieldID(
            env, //the current JNI interface pointer
            MY_STRUCT, //a reference to the jclass for MyStruct
            "vector_count", //the field name
            "I" //the field signature
    );
    MY_STRUCT_VECTORS = (*env)->GetFieldID(env, MY_STRUCT, "vectors", "[Lcom/thebhwgroup/demo/MyVector;");
    MY_STRUCT_POSITION = (*env)->GetFieldID(env, MY_STRUCT, "position", "Lcom/thebhwgroup/demo/MyVector;");

    JAVA_EXCEPTION = (jclass)(*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Exception"));

    return JNI_VERSION_1_6;
}

//Takes a jobject and returns a MyVector of the data
MyVector make_native_my_vector(JNIEnv *env, jobject jvector) {
    MyVector vector;

    vector.x = (*env)->GetDoubleField(env, jvector, MY_VECTOR_X);
    vector.y = (*env)->GetDoubleField(env, jvector, MY_VECTOR_Y);

    return vector;
}

//Takes a jobject and returns a MyStruct of the data
MyStruct make_native_my_struct(JNIEnv *env, jobject jstruct) {
    MyStruct my_struct;

    my_struct.vector_count = (*env)->GetIntField(env, jstruct, MY_STRUCT_VECTOR_COUNT);
    my_struct.position = make_native_my_vector(env, (*env)->GetObjectField(env, jstruct,
                                                                           MY_STRUCT_POSITION));

    my_struct.vectors = (MyVector *) malloc(sizeof(MyVector) * my_struct.vector_count);
    jobject jvectors = (*env)->GetObjectField(env, jstruct, MY_STRUCT_VECTORS);
    jobjectArray *ja_vectors = (jobjectArray *) (&jvectors);
    for (int i = 0; i < my_struct.vector_count; ++i) {
        my_struct.vectors[i] = make_native_my_vector(env,
                                                     (*env)->GetObjectArrayElement(env, *ja_vectors,
                                                                                   i));
    }

    return my_struct;
}

//Takes a MyVector and returns a jobject
jobject make_jni_my_vector(JNIEnv *env, MyVector vector) {
    return (*env)->NewObject(env, MY_VECTOR, MY_VECTOR_CONS, vector.x, vector.y);
}

//Take a MyStruct and returns a jobject
jobject make_jni_my_struct(JNIEnv *env, MyStruct my_struct) {
    jobjectArray ja_vectors = (*env)->NewObjectArray(
            env,
            my_struct.vector_count,
            MY_VECTOR,
            (*env)->NewObject(
                    env,
                    MY_VECTOR,
                    MY_VECTOR_CONS,
                    0,
                    0));
    for (int i = 0; i < my_struct.vector_count; ++i) {
        (*env)->SetObjectArrayElement(env, ja_vectors, i,
                                      (*env)->NewObject(env, MY_VECTOR, MY_VECTOR_CONS,
                                                        my_struct.vectors[i].x,
                                                        my_struct.vectors[i].y));
    }

    return (*env)->NewObject(env, MY_STRUCT, MY_STRUCT_CONS, my_struct.vector_count,
                             ja_vectors, make_jni_my_vector(env, my_struct.position));
}


jstring Java_com_thebhwgroup_demo_HelloWorldModule_helloWorldJNI(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "Hello World!");
}

jobject Java_com_thebhwgroup_demo_HelloWorldModule_helloStructJNI(
        JNIEnv *env, //The env passed by JNI
        jobject thiz, //"this" passed by JNI
        jobject jstruct, //the MyStruct passed in from Java
        jdouble jd) //the double passed in from Java
{
    MyStruct my_struct = make_native_my_struct(env, jstruct); //get a C struct from the jobject

    LOGD("double %f", jd); //we can treat the jdouble as a double (since it is one)

    //print out the data of our C struct
    LOGD("struct vector_count %d", my_struct.vector_count);
    LOGD("struct vectors");
    for (int i = 0; i < my_struct.vector_count; ++i) {
        LOGD("vector %d (%f, %f)", i, my_struct.vectors[i].x, my_struct.vectors[i].y);
    }
    LOGD("struct pos (%f, %f)", my_struct.position.x, my_struct.position.y);

    //The following line would throw an exception, which would be handled by the reject block in Java
    //return (jobject)(*env)->ThrowNew(env, JAVA_EXCEPTION, "Error in helloStructJNI");

    return make_jni_my_struct(env, my_struct); //turn the C struct back into a jobject and return it
}
