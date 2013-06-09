#include <jni.h>
#include <android/log.h>
#include <stdio.h>

extern "C" {
jstring native_get_string(JNIEnv *env, jobject obj);
}

static JNINativeMethod s_methods[] = {
		"nativeGetString", "()Ljava/lang/String;", (void*)native_get_string,
};

jstring native_get_string(JNIEnv *env, jobject obj)
{
	return env->NewStringUTF("hello");
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;

	jint result = -1;
	__android_log_write(ANDROID_LOG_WARN, "dyb", "start load");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		__android_log_write(ANDROID_LOG_WARN, "dyb", "get env failed");
		return JNI_ERR;
	} else {
		__android_log_write(ANDROID_LOG_WARN, "dyb", "get env success");
	}
	__android_log_write(ANDROID_LOG_WARN, "dyb", "before find class");
	jclass cls = env->FindClass("com/aliyun/datasync/NativeManager");
	__android_log_write(ANDROID_LOG_WARN, "dyb", "after find class");
	if (cls == NULL) {
		__android_log_write(ANDROID_LOG_WARN, "dyb", "get class failed");
		return JNI_ERR;
	}

	int len = sizeof(s_methods) / sizeof(s_methods[0]);

	if (env->RegisterNatives(cls, s_methods, len) < 0)
	{
		__android_log_write(ANDROID_LOG_WARN, "dyb", "register method failed");
		return JNI_ERR;
	}
	__android_log_write(ANDROID_LOG_WARN, "dyb", "load success");
	return JNI_VERSION_1_4;
}
