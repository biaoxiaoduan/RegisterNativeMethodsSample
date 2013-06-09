#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Java VM interface pointer
static JavaVM* gVm = NULL;
// Global reference to object
static jobject gObj = NULL;

// Native worker thread arguments
struct NativeWorkerArgs
{
	jint id;
	jint iterations;
};

extern "C" {
jstring native_get_string(JNIEnv *env, jobject obj);
void nativeInit(JNIEnv* env, jobject obj);
void nativeFree ( JNIEnv* env, jobject obj);
void nativeWorker (JNIEnv* env, jobject obj, jint id, jint iterations);
void posixThreads ( JNIEnv* env, jobject obj, jint threads, jint iterations);
}

static JNINativeMethod s_methods[] = {
		"nativeGetString", "()Ljava/lang/String;", (void*)native_get_string,
		"nativeInit", "()V", (void*)nativeInit,
		"nativeFree", "()V", (void*)nativeFree,
		"nativeWorker", "(II)V", (void*)nativeWorker,
		"posixThreads", "(II)V", (void*)posixThreads,
};

// Method ID can be cached
static jmethodID gOnNativeMessage = NULL;

static void* nativeWorkerThread (void* args)
{
	JNIEnv* env = NULL;
	// Attach current thread to Java virtual machine
	// and obrain JNIEnv interface pointer
	if (0 == gVm->AttachCurrentThread(&env, NULL))
	{
		// Get the native worker thread arguments
		NativeWorkerArgs* nativeWorkerArgs = (NativeWorkerArgs*) args;
		// Run the native worker in thread context
		nativeWorker(env,
				gObj,
				nativeWorkerArgs->id,
				nativeWorkerArgs->iterations);
		// Free the native worker thread arguments
		delete nativeWorkerArgs;
		// Detach current thread from Java virtual machine
		gVm->DetachCurrentThread();
	}
	return (void*) 1;
}

jstring native_get_string(JNIEnv *env, jobject obj)
{
	return env->NewStringUTF("hello");
}

void posixThreads ( JNIEnv* env, jobject obj, jint threads, jint iterations)
{
	// Create a POSIX thread for each worker
	for (jint i = 0; i < threads; i++)
	{
		// Native worker thread arguments
		NativeWorkerArgs* nativeWorkerArgs = new NativeWorkerArgs();
		nativeWorkerArgs->id = i;
		nativeWorkerArgs->iterations = iterations;
		// Thread handle
		pthread_t thread;
		// Create a new thread
		int result = pthread_create(
				&thread,
				NULL,
				nativeWorkerThread,
				(void*) nativeWorkerArgs);
		if (0 != result)
		{
			__android_log_write(ANDROID_LOG_WARN, "dyb", "Unable to create thread");
			// Get the exception class
			jclass exceptionClazz = env->FindClass(
					"java/lang/RuntimeException");
			// Throw exception
			env->ThrowNew(exceptionClazz, "Unable to create thread");
		}
		__android_log_write(ANDROID_LOG_WARN, "dyb", "create thread success");
	}
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	gVm = vm;
	JNIEnv* env = NULL;

	jint result = -1;
	//__android_log_write(ANDROID_LOG_WARN, "dyb", "start load");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		//__android_log_write(ANDROID_LOG_WARN, "dyb", "get env failed");
		return JNI_ERR;
	} else {
		//__android_log_write(ANDROID_LOG_WARN, "dyb", "get env success");
	}
	//__android_log_write(ANDROID_LOG_WARN, "dyb", "before find class");
	jclass cls = env->FindClass("com/aliyun/datasync/NativeManager");
	//__android_log_write(ANDROID_LOG_WARN, "dyb", "after find class");
	if (cls == NULL) {
		//__android_log_write(ANDROID_LOG_WARN, "dyb", "get class failed");
		return JNI_ERR;
	}

	int len = sizeof(s_methods) / sizeof(s_methods[0]);

	if (env->RegisterNatives(cls, s_methods, len) < 0)
	{
		//__android_log_write(ANDROID_LOG_WARN, "dyb", "register method failed");
		return JNI_ERR;
	}
	__android_log_write(ANDROID_LOG_WARN, "dyb", "load success");
	return JNI_VERSION_1_4;
}

void nativeInit(JNIEnv* env, jobject obj)
{
	// If method ID is not cached
	if (NULL == gOnNativeMessage)
	{
		// Get the class from the object
		jclass clazz = env->FindClass("com/aliyun/datasync/TestActivity");
		// Get the method id for the callback
		gOnNativeMessage = env->GetMethodID(clazz,
				"onNativeMessage",
				"(Ljava/lang/String;)V");
		// If method could not be found
		if (NULL == gOnNativeMessage)
		{
			__android_log_write(ANDROID_LOG_WARN, "dyb", "Unable to find method");
			// Get the exception class
			jclass exceptionClazz = env->FindClass(
					"java/lang/RuntimeException");
			// Throw exception
			env->ThrowNew(exceptionClazz, "Unable to find method");
		}
		__android_log_write(ANDROID_LOG_WARN, "dyb", "init success");
	}
	// If object global reference is not set
	if (NULL == gObj)
	{// Create a new global reference for the object
		gObj = env->NewGlobalRef(obj);
		if (NULL == gObj)
		{
			return;
		}
	}
}

void nativeFree ( JNIEnv* env, jobject obj)
{
	// If object global reference is set
	if (NULL != gObj)
	{
		// Delete the global reference
		env->DeleteGlobalRef(gObj);
		gObj = NULL;
	}
}
void nativeWorker (JNIEnv* env, jobject obj, jint id, jint iterations)
{
	char logMsg[256];
	sprintf(logMsg, "Worker %d: Iteration %d", id, iterations);
	__android_log_write(ANDROID_LOG_WARN, "dyb", logMsg);
	// Loop for given number of iterations
	for (jint i = 0; i < iterations; i++)
	{
		// Prepare message
		char message[26];
		sprintf(message, "Worker %d: Iteration %d", id, i);
		// Message from the C string
		jstring messageString = env->NewStringUTF(message);
		// Call the on native message method
		env->CallVoidMethod(obj, gOnNativeMessage, messageString);
		// Check if an exception occurred
		if (NULL != env->ExceptionOccurred())
			break;
		// Sleep for a second
		sleep(1);
	}
}
