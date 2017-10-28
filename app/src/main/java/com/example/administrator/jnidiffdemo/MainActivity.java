package com.example.administrator.jnidiffdemo;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    public static final String SD_DIR = Environment.getExternalStorageDirectory().getAbsolutePath();

    static {
        System.loadLibrary("native-lib");
    }
    //    拆分文件的native方法
    public native void  splitFile(String srcFilePath,String dstFilePath,int fileNum);
//    合并文件
    public native void  mergeFile(String srcFilePath,String dstFilePath,int fileNum);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void split(View View){
        String srcFile = SD_DIR+"/"+"a.mp4";
        String dstFile = SD_DIR+"/"+"a_%d.mp4";
        splitFile(srcFile,dstFile,4);
    }

    public void merge(View view){
        String srcFile = SD_DIR+"/"+"a.mp4";
        String dstFile = SD_DIR+"/"+"a_%d.mp4";
        mergeFile(srcFile,dstFile,4);
    }


}
