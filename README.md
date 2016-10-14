[TOC]

## 简介

JNI：Java Native Interface(Java 本地接口)，它是为了方便Java调用C、C++等本地代码所封装的一层接口。

NDK：Native Development Kit(本地开发工具包)，通过NDK可以在Android中更加方便的通过JNI来访问本地代码。

## 基于NDK的JNI开发流程

### 1.配置NDK开发环境

打开AS的SDK Manager，安装NDK插件：

### 打开AS的SDK Manager，安装NDK插件：

![001.jpg](https://kevindgk.github.io/android/jni/001.jpg)

整个NDK比较大，解压缩完2个G，自动安装到配置的sdk目录下：

![002.jpg](https://kevindgk.github.io/android/jni/002.jpg)

安装完毕后，点开structure，配置NDK的路径：

![003.jpg](https://kevindgk.github.io/android/jni/003.jpg)

配置NDK的环境变量：

![004.jpg](https://kevindgk.github.io/android/jni/004.jpg)

![](https://kevindgk.github.io/android/jni/005.jpg)

验证是否配置成功：

![](https://kevindgk.github.io/android/jni/006.jpg)

在命令行输入ndk-build，如果显示以上内容，表示成功。



### 2.创建Android项目

项目名称：JNITest

包名：com.dgk.jnitest

实现功能：界面有两个按钮，点击Get从本地方法中获取一个字符串，并toast出来；点击Set向本地方法传递一个字符串，打印到Logcat。

2.1 写Android界面和基本逻辑，并声明两个本地方法。

MainActivity.java

```java
public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private static final String tag = "【MainActivity】";

    private Button btn_get;
    private Button btn_set;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn_get = (Button) findViewById(R.id.btn_get);
        btn_set = (Button) findViewById(R.id.btn_set);

        btn_get.setOnClickListener(this);
        btn_set.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {
            case R.id.btn_get:
                Log.i(tag, "点击Get按钮");
                Toast.makeText(this, getStringFromJNI(), Toast.LENGTH_SHORT).show();
                break;
            case R.id.btn_set:
                Log.i(tag, "点击Set按钮");
                setStringToJNI("Hello C! 我是一只来自Java世界的Cat，喵~~~");
                break;
        }
    }

    /**
     * 声明get方法
     *  - 作用是从本地方法返回一个String
     * @return 返回一个字符串
     */
    public native String getStringFromJNI();

    /**
     * 声明set方法
     *  - 作用是向本地方法传递一个String
     */
    public native void setStringToJNI(String str);

    /**
     * 加载本地代码库
     * - 在应用启动的时候加载名为"libjni-test.so"的代码库，该库在安装Apk的时候就已经
     * 被包管理器拆包放到了/data/data/包名/lib/目录下了。
     */
    static {
        System.loadLibrary("jni-test");
    }
}
```

activity_main.xml

```java
<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    android:padding="16dp">

    <Button
        android:id="@+id/btn_get"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:text="Get" />

    <Button
        android:id="@+id/btn_set"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_marginTop="16dp"
        android:text="Set" />

</LinearLayout>
```

2.2 写C代码

选中main右键选择New->Folder->JNI Folder，会在main路径下创建一个jni的文件夹，用于存放本地源代码，src\main\jni这个是AS默认的jni源代码存放路径，也可以自己手动创建。

创建Hello.c

```c
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "【C_LOG】"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

JNIEXPORT jstring JNICALL Java_com_dgk_jnitest_MainActivity_getStringFromJNI (JNIEnv *env, jobject thiz)
{
    LOGI("调用 C getStringFromJNI() 方法\n");
    char* str = "Hello Java! 我是一只来自C世界的Dog，汪!!!";
    return (*env)->NewStringUTF(env, str);
}

JNIEXPORT void JNICALL Java_com_dgk_jnitest_MainActivity_setStringToJNI (JNIEnv* env, jobject thiz, jstring str){
    LOGI("调用 C setStringFromJNI() 方法\n");
    char* string = (char*)(*env)->GetStringUTFChars(env, str, NULL);
    LOGI("%s\n", string);
    (*env)->ReleaseStringUTFChars(env, str, string);
}

```

C代码的我的理解，可以先大致看一下：

```c
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
```



2.3 配置gradle

![](https://kevindgk.github.io/android/jni/012.jpg)	

配置完gradle后如果直接同步，会提示：

![](https://kevindgk.github.io/android/jni/008.jpg)

如果点击第一行，会转到google开发网站，显示一个实验性的gradle，可以用来集成NDK，在本篇不予介绍。点击第二行，或者直接在gradle.properties中贴上代码，表示使用当前过时的插件：

```java
android.useDeprecatedNdk=true
```



### 3.编译生成.so库

直接Build->Make Project，会自动生成.so库，保存路径为\app\build\intermediates\ndk\debug\lib

![](https://kevindgk.github.io/android/jni/010.jpg)

将生成的lib包下的需要的.so库copy到main/jniLibs目录中即可：

![](https://kevindgk.github.io/android/jni/011.jpg)

该文件夹为AS默认的jni库的目录，可以在gradle中修改。



### 4.运行程序查看结果

运行手机：魅族pro5	Android 5.1

点击GET：

![](https://kevindgk.github.io/android/jni/01.png)

点击SET：

![](https://kevindgk.github.io/android/jni/009.jpg)

到现在为止，已经完成了最简单的JNI开发流程。



## 遇到问题

### 1.自动生成JNI.h头文件

在这里我们的C代码的方法名和参数是自己写的，而实际上完全可以让jdk来帮我们生成，流程是：

编译java代码生成.class文件--->使用javah命令生成C语言的头文件--->将.h文件中的JNI方法声明复制到C文件中，并完善方法的方法体。这样的话，可以加速我们的开发流程，同时避免写错方法名，但是在这个过程中由于对自己的开发环境还有命令了解不够，经常会出问题，所以小编在简洁流程中并未写出。

如果对C语言不了解，就需要自动生成头文件了，在这里给出小编自己的流程，一行代码搞定：

在Make Project之后，在Terminal中输入命令(当前目录为应用的根目录)

![](https://kevindgk.github.io/android/jni/013.jpg)

```
javah -d . -cp android.jar的地址;编译生成的.class目录  全类名
```

格式：
-d . 表示生成文件存放在当前目录
-cp class文件的加载根路径，在这里需要加载两个类，一个是android的基础类库，要不然jdk本身识别不了android的东西，比如Activity\Log等；第二个是MainActivity编译生成的路径 + 全类名，注意中间有个空格，而且不能写全地址，不识别。
运行完该命令后会默认在当前目录(即程序的根目录)下生成一个文件：com_dgk_jnitest_MainActivity.h，该文件就是JNI的头文件，里面定义好了JNI的本地方法声明，可以直接复制来用。

![](https://kevindgk.github.io/android/jni/014.jpg)

对于这一步，网上的命令七花八门，感觉好多复制过来直接用然后就发表了，很多坑，令人很头疼，搞了一天，烦躁！其实主要的问题就是，Android Studio对JNI的兼容性刚开始不是很好，后来又更新的版本比较多，再加上如果对gradle不熟悉的话，就会造成写起来很难受的感觉。

还好，当真正的把整个流程给串起来以后就会感觉还是很清晰的，不过还是希望成熟的插件能够将整个流程串起来~