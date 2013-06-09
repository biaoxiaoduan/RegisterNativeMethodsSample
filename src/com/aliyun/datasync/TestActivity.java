package com.aliyun.datasync;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class TestActivity extends Activity {
	/** Threads edit. */
	private EditText threadsEdit;
	/** Iterations edit. */
	private EditText iterationsEdit;
	/** Start button. */
	private Button startButton;
	/** Log view. */
	private TextView logView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		//		findViewById(R.id.btn_start_update).setOnClickListener(new OnClickListener(){
		//
		//			@Override
		//			public void onClick(View v) {
		//				String text = NativeManager.nativeGetString();
		//				Toast.makeText(TestActivity.this, text, Toast.LENGTH_LONG).show();
		//			}
		//			
		//		});
		// Initialize the native code
		NativeManager.nativeInit();
		threadsEdit = (EditText) findViewById(R.id.threads_edit);
		iterationsEdit = (EditText) findViewById(R.id.iterations_edit);
		startButton = (Button) findViewById(R.id.start_button);
		logView = (TextView) findViewById(R.id.log_view);
		startButton.setOnClickListener(new OnClickListener() {
			public void onClick(View view) {
				int threads = getNumber(threadsEdit, 0);
				int iterations = getNumber(iterationsEdit, 0);
				if (threads > 0 && iterations > 0) {
					startThreads(threads, iterations);
				} }
		});


	}

	/**
	 * Using Java based threads.
	 *
	 * @param threads thread count.
	 * @param iterations iteration count.
	 */
	private void javaThreads(int threads, final int iterations) {
		// Create a Java based thread for each worker
		for (int i = 0; i < threads; i++) {
			final int id = i;
			Thread thread = new Thread() {
				public void run() {
					NativeManager.nativeWorker(id, iterations);
				}
			};
			thread.start();
		}
	}

	@Override
	protected void onDestroy() {
		// Free the native resources
		NativeManager.nativeFree();
		super.onDestroy();
	}

	/**
	 * On native message callback.
	 *
	 * @param message
	 *            native message.
	 */
	private void onNativeMessage(final String message) {
		Log.d("dyb", message);
//		runOnUiThread(new Runnable() {
//			public void run() {
//
//				logView.append(message);
//				logView.append("\n");
//			} });
	}
	/**
	 * Gets the value of edit text as integer. If the value
	 * is empty or count not be parsed, it returns the
	 * default value.
	 *
	 * @param editText edit text.
	 * @param defaultValue default value.
	 * @return numeric value.
	 */
	private static int getNumber(EditText editText, int defaultValue) {
		int value;
		try {
			value = Integer.parseInt(editText.getText().toString());
		} catch (NumberFormatException e) {
			value = defaultValue;
		}
		return value;
	}
	/**
	 * Starts the given number of threads for iterations.
	 *
	 * @param threads thread count.
	 * @param iterations iteration count.
	 */
	private void startThreads(int threads, int iterations) {
		// We will be implementing this method as we
		// work through the chapter
		NativeManager.posixThreads(threads, iterations);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_test, menu);
		return true;
	}

}
