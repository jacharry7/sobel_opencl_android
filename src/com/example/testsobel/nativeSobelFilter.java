package com.example.testsobel;

public class nativeSobelFilter {
	String resultString;

	static {

		System.loadLibrary("SobelFilter");
	}
	public native String test();
	public native String sobelFilter();

}
