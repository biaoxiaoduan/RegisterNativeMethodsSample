package com.aliyun.datasync;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

public class TestActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		findViewById(R.id.btn_start_update).setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				String text = NativeManager.nativeGetString();
				Toast.makeText(TestActivity.this, text, Toast.LENGTH_LONG).show();
			}
			
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_test, menu);
		return true;
	}

}
