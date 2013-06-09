package com.aliyun.datasync;

import android.util.Log;

public class NativeManager {
	public static native String nativeGetString();
	public static native void nativeInit();
	public static native void nativeFree();
	public static native void nativeWorker(int id, int iteration);
	public static native void posixThreads(int threads, int iterations);
	static {
		Log.d("dyb", "load library");
		System.loadLibrary("DataSync");
		
	}
}
