#include <jni.h>

jstring Java_com_thebhwgroup_demo_HelloWorldModule_helloWorldJNI(JNIEnv* env, jobject thiz) {
  return (*env)->NewStringUTF(env, "Hello World!");
}
