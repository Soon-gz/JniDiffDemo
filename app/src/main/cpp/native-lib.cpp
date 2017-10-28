#include <jni.h>
#include <android/log.h>
#include <malloc.h>

#define TAG "TAG00"
#define LOGI( ... ) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

long get_file_size ( const char * path ) {
    FILE * fp = fopen ( path , "rb" ); //打开一个文件， 文件必须存在，只运行读
    fseek ( fp , 0 , SEEK_END );
    long ret = ftell ( fp );
    fclose ( fp );
    return ret;
}



extern "C" {
JNIEXPORT void JNICALL
Java_com_example_administrator_jnidiffdemo_MainActivity_splitFile ( JNIEnv * env ,
                                                                    jobject instance ,
                                                                    jstring srcFilePath_ ,
                                                                    jstring dstFilePath_ ,
                                                                    jint file_num ) {
    //    1，首先需要拆分文件的地址，生成拆分后子文件的地址，子文件（可以使用原文件名_%d命名，然后在代码中进行替换，如果嫌麻烦也可以直接用传入多个地址是一样的）。
//2，需要知道拆分的子文件个数
    const char * srcFilePath = env -> GetStringUTFChars ( srcFilePath_ , NULL );
    const char * dstFilePath = env -> GetStringUTFChars ( dstFilePath_ , NULL );

    LOGI( "JNI native diff begin" );
    //申请存放多个文件的内存  二级指针首地址
    char ** patches = ( char ** ) malloc ( sizeof ( char * ) * file_num );

    for ( int i = 0 ; i < file_num ; i ++ ) {
        patches[ i ] = ( char * ) malloc ( sizeof ( char ) * 100 );
        //对每个地址进行拼接
        sprintf ( patches[ i ] , dstFilePath , i );
        LOGI( "patch path : %s" , patches[ i ] );
    }

//  3，源文件的大小，以及是否能整除子文件个数（这里分两种情况处理） 读取源文件大小  根据大小进行拆分
    int fileSize = get_file_size ( srcFilePath );
    LOGI( "fileSize : %d" , fileSize );
//    打开源文件
    FILE * rfp = fopen ( srcFilePath , "rb" );
//   4，读取源文件，循环写入子文件。（这里拆分需要一个个char读取和写入，不然会出问题） 源文件是否正好整除所划分的文件个数  分别处理
    if ( fileSize % file_num == 0 ) {
        int part = fileSize / file_num;
        for ( int i = 0 ; i < file_num ; i ++ ) {
            FILE * fwp = fopen ( patches[ i ] , "wb" );
            for ( int j = 0 ; j < part ; j ++ ) {
                fputc ( fgetc ( rfp ) , fwp );
            }
            fclose ( fwp );
        }
    } else {
        int part = fileSize / ( file_num - 1 );
        for ( int i = 0 ; i < file_num - 1 ; i ++ ) {
            FILE * fwp = fopen ( patches[ i ] , "wb" );
            for ( int j = 0 ; j < part ; j ++ ) {
                fputc ( fgetc ( rfp ) , fwp );
            }
            fclose ( fwp );
        }
        FILE * fwp = fopen ( patches[ file_num - 1 ] , "wb" );
        for ( int i = 0 ; i < fileSize % ( file_num - 1 ) ; i ++ ) {
            fputc ( fgetc ( rfp ) , fwp );
        }
        fclose ( fwp );
    }
//  5，关闭文件流，释放资源
    fclose ( rfp );
    for ( int i = 0 ; i < file_num ; i ++ ) {
        free ( patches[ i ] );
    }
    free ( patches );

    env -> ReleaseStringUTFChars ( srcFilePath_ , srcFilePath );
    env -> ReleaseStringUTFChars ( dstFilePath_ , dstFilePath );
}

JNIEXPORT void JNICALL
Java_com_example_administrator_jnidiffdemo_MainActivity_mergeFile ( JNIEnv * env ,
                                                                    jobject instance ,
                                                                    jstring srcFilePath_ ,
                                                                    jstring dstFilePath_ ,
                                                                    jint fileNum ) {
    const char * srcFilePath = env -> GetStringUTFChars ( srcFilePath_ , 0 );
    const char * dstFilePath = env -> GetStringUTFChars ( dstFilePath_ , 0 );

    char** patches = ( char**)malloc ( sizeof ( char*)*fileNum);
    for ( int i = 0 ; i < fileNum ;  i++ ) {
        patches[i] = (char*)malloc ( sizeof ( char)*100);
        sprintf (patches[i],dstFilePath,i);
        LOGI("patches[i]:%s",patches[i]);
    }

    FILE *fwp = fopen (srcFilePath,"wb");

    for ( int i = 0 ; i < fileNum ; i++ ) {
        int fileSize = get_file_size (patches[i]);
        FILE *frp = fopen (patches[i],"rb");
        LOGI("fileSize:%d",fileSize);
        for ( int j = 0 ; j < fileSize ; j++ ) {
            fputc (fgetc (frp),fwp);
        }
        fclose (frp);
    }
    fclose (fwp);

    for ( int i = 0 ; i < fileNum ;  i++ ) {
        free (patches[i]);
    }
    free (patches);

    env -> ReleaseStringUTFChars ( srcFilePath_ , srcFilePath );
    env -> ReleaseStringUTFChars ( dstFilePath_ , dstFilePath );
}


}
