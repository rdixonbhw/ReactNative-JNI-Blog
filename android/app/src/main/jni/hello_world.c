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

    my_struct.vector_count = (*env)->GetIntField( //gets an int from a jobject field
            env, //current JNI interface pointer
            jstruct, //the jobject (MyStruct) we're getting the field from
            MY_STRUCT_VECTOR_COUNT //the jfieldID for "vector_count"
    );
    my_struct.position = make_native_my_vector( //function we have to make MyVector from jobject
            env, //current JNI interface pointer
            (*env)->GetObjectField( //gets jobject from a jobject field
                    env, //current JNI interface pointer
                    jstruct, //jobject we're getting the jobject from
                    MY_STRUCT_POSITION //jfieldID for "position"
            ));

    //Allocate vectors
    my_struct.vectors = (MyVector *) malloc(sizeof(MyVector) * my_struct.vector_count);
    jobjectArray jvectors = (*env)->GetObjectField( //get jobject from a jobject field
            env, //current JNI interface pointer
            jstruct, //jobject from which we're getting the jobject
            MY_STRUCT_VECTORS //jfieldID for "vectors"
    );
    for (int i = 0; i < my_struct.vector_count; ++i) { //copy objects from jobjectArray to C array
        jobject jvec = (*env)->GetObjectArrayElement( //get jobject from jobjectArray
                env, //current JNI interface pointer
                jvectors, //jobjectArray to get jobject from
                i //index of jobject
        );
        my_struct.vectors[i] = make_native_my_vector( //function we have to make MyVector from jobject
                env, //current JNI interface pointer
                jvec //jobject to make MyVector from
        );
        (*env)->DeleteLocalRef(env, jvec); // DON'T FORGET THIS essentially a free() call but for a JNI local reference
    }

    return my_struct;
}

//Takes a MyVector and returns a jobject
jobject make_jni_my_vector(JNIEnv *env, MyVector vector) {
    return (*env)->NewObject(env, MY_VECTOR, MY_VECTOR_CONS, vector.x, vector.y);
}

//Take a MyStruct and returns a jobject
jobject make_jni_my_struct(JNIEnv *env, MyStruct my_struct) {
    jobjectArray ja_vectors = (*env)->NewObjectArray( //make new jobjectArray
            env,
            my_struct.vector_count, //size of array
            MY_VECTOR, //type of array, jclass of MyVector
            NULL //initial elements of jobjectArray
    );
    for (int i = 0; i < my_struct.vector_count; ++i) { //copy MyVector array into jobjectArray
        (*env)->SetObjectArrayElement( //set element in jobjectArray
                env,
                ja_vectors, //jobjectArray in which to set element
                i, //index to set
                make_jni_my_vector(env, my_struct.vectors[i]) //element to set index to, helper function we have to make a jobject from a MyVector
        );
    }

    free(my_struct.vectors); //we assume we're done with the MyStruct at this point

    return (*env)->NewObject( //make jobject given jclass, jmethodID of constructors, and parameters
            env,
            MY_STRUCT, //object type we're making, jclass for MyStruct
            MY_STRUCT_CONS, //method used to make object, jmethodID of MyStruct constructor
            my_struct.vector_count, //first parameter, an int of vector_count
            ja_vectors, //second parameter, a jobjectArray of vectors
            make_jni_my_vector(env, my_struct.position) //third parameter, a jobject of position
    );
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
