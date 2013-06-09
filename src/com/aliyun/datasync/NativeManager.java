package com.aliyun.datasync;

import android.util.Log;

public class NativeManager {
	public static native String nativeGetString();
	static {
		Log.d("dyb", "load library");
		System.loadLibrary("DataSync");
		
	}
}
