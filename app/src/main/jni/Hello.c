#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>

/*
    在C语言中标准输出的方法是printf，但是打印出来的内容在logcat看不到，需要使用
    __android_log_print()方法打印log，才能在logcat看到，由于该方法名比较长，我们在
    这里需要定义宏，使得在C语言中能够向Android一样打印log。
    注意：该方法还需要在gradle中声明ldLibs "log"，详见build.gradle
*/
#define  LOG_TAG    "【C_LOG】"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/*
    返回类型 Java_包名_类名_方法名
    - 返回类型：jstring,即java格式的String
    - 参数：
        可以在jni.h头文件中查找到各个自定义变量的原类型。
        必带的两个参数：
            - JNIEnv：是结构体JNINativeInterface的一级指针，即JNINativeInterface*，
                    结构体JNINativeInterface：接口函数指针表，该表就是用来Java和C语言之间进行交互的，
                    包含着Java变量和C变量之间的对应关系，可以用于变量之间的转换。
              JNIEnv* env：是JNIEnv的一级指针，
                    是结构体JNINativeInterface的二级指针，即JNINativeInterface**
            - jobject thiz：谁调用了这个本地函数，那么这个thiz就是指的哪个对象，本项目中是MainActicity

*/
JNIEXPORT jstring JNICALL Java_com_dgk_jnitest_MainActivity_getStringFromJNI (JNIEnv *env, jobject thiz)
{

    LOGI("调用 C getStringFromJNI() 方法\n");

    /*
        char* 相当与C语言中的字符串
        char* 指的是字符串str的指针，指向的是该str的内存区域，
            但是在C语言中操作字符串可以直接使用一级指针，来获取字符串的各个元素。
    */
    char* str = "Hello Java! 我是一只来自C世界的Dog，汪!!!";

    /*
        通过在jni.h的结构体JNINativeInterface中查找jstring，可以找到将C语言的字符串转换成
        Java字符串的代码：
                const struct JNINativeInterface* functions;
                jstring NewStringUTF(const char* bytes){
                    return functions->NewStringUTF(this, bytes);
                }
            即：JNINativeInterface*->NewStringUTF(*env, str)
            即：(env*)->NewStringUTF(*env, str)
            将str转换并保存在结构体中，然后使用间接引用运算符->来获得这个jstring成员。

        在这里，结构体是JNINativeInterface，他的一级指针是JNIEnv，即*env(因为env又是JNIEnv的一级指针)。
        所以(*env)->NewStringUTF(env, str)就相当于JNINativeInterface.jstring，表示该结构体内的
        jstring格式的变量。
    */
    return (*env)->NewStringUTF(env, str);
}

JNIEXPORT void JNICALL Java_com_dgk_jnitest_MainActivity_setStringToJNI (JNIEnv* env, jobject thiz, jstring str){

    LOGI("调用 C setStringFromJNI() 方法\n");

    // 将收到的jstring转换成UTF-8格式的C字符串
    char* string = (char*)(*env)->GetStringUTFChars(env, str, NULL);
    LOGI("%s\n", string);

    // 显示释放转换成UTf-8的string空间，如果不显示调用，JVM会一直保存该对象，不回收，容易导致内存溢出
    (*env)->ReleaseStringUTFChars(env, str, string);
}