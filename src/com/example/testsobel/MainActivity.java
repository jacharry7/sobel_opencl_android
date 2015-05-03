package com.example.testsobel;



import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {

	private Button sobelGPU_CPU=null;
	private nativeSobelFilter t;
	private String s1,s2;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		sobelGPU_CPU = (Button) findViewById(R.id.sobel_gpu_cpu);
		sobelGPU_CPU.setOnClickListener(new MyButtonListener());

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	class MyButtonListener implements OnClickListener{
		
		public void onClick(View v)
		{
			
			
			t = new nativeSobelFilter();//启动线程来执行sobel滤波
			s1=t.test();
			s2=t.sobelFilter();
			Toast.makeText(
					getApplicationContext(),
					s1,
					Toast.LENGTH_SHORT).show();
			Toast.makeText(
					getApplicationContext(),
					s2,
					Toast.LENGTH_SHORT).show();
		}
	}
}
