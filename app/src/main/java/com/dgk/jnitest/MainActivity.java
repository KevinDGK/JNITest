package com.dgk.jnitest;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity implements View.OnClickListener{

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
