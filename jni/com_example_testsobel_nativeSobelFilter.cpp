#include "com_example_testsobel_nativeSobelFilter.h"
#include<CL/cl.h>
#include"aopencl.h"
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>


using namespace std;
//using namespace cv;

#define N 20	//这里的N是sobel滤波的阈值
#define  KERNEL_SRC "\n" \
	"			__kernel void Sobel(__global unsigned char *array1, __global unsigned char *array2, __global int *array3)		\n "\
	"			{																							\n "\
	"				size_t gidx = get_global_id(0);															\n "\
	"				size_t gidy = get_global_id(1);															\n "\
	"				unsigned char a00, a01, a02;															\n "\
	"				unsigned char a10, a11, a12;															\n "\
	"				unsigned char a20, a21, a22;															\n "\
	"				int width=array3[0];																	\n "\
	"				int heigh=array3[1];																	\n "\
	"				if(gidy>0&&gidy<heigh-1&&gidx>0&&gidx<width-1)											\n "\
	"				{																						\n "\
	"					a00 = array1[gidx- 1+width*(gidy-1)];											\n "\
	"					a01 = array1[gidx    +width*(gidy-1)];												\n "\
	"					a02 = array1[gidx+1+width*(gidy-1)];											\n "\
	"					a10 = array1[gidx-1 +width*gidy];												\n "\
	"					a11 = array1[gidx     +width*gidy];													\n "\
	"					a12 = array1[gidx+1+width*gidy];												\n "\
	"					a20 = array1[gidx- 1+width*(gidy+1)];											\n "\
	"					a21 = array1[gidx    +width*(gidy+1)];												\n "\
	"					a22 = array1[gidx+1+width*(gidy+1)];											\n "\
	"					float ux=a20+2*a21+a22-a00-2*a01-a02;												\n "\
	"					float uy=a02+2*a12+a22-a00-2*a10-a20;												\n "\
	"					array2[gidx+width*gidy]=sqrt(ux*ux + uy*uy);										\n "\
	"					float u=sqrt(ux*ux+uy*uy);														\n "\
	"					if(u>255) {																			\n "\
	"						u=255;																			\n "\
	"					} else if(u<20) {																	\n "\
	"						u=0;																			\n "\
	"					}																					\n "\
	"					array2[gidx+width*gidy]=u;														\n "\
	"				}																						\n "\
"}"

void CPU_Sobel(unsigned char* inputImg_gray,unsigned char* outputImg,int width,int height);
int GPU_Sobel(unsigned char* inputImg_gray, unsigned char* outputImg,int width,int height);
double difference(unsigned char* input1, unsigned char* input2, unsigned char* output,int width,int height);

extern "C" {
JNIEXPORT jstring JNICALL Java_com_example_testsobel_nativeSobelFilter_test
  (JNIEnv *env, jobject)
{

	return env->NewStringUTF("testsobel");
}
JNIEXPORT jstring JNICALL Java_com_example_testsobel_nativeSobelFilter_sobelFilter(
		JNIEnv* env, jobject) {

 	clock_t start, finish;
	double CPU_time, GPU_time,s;
	char b[20];
	char rr[100] = "Compute result:\nGPU:";
	int width=256;
	int height=256;
	int datasize = height *width;

	unsigned char* inputImg_gray=(unsigned char *)malloc(sizeof(unsigned char)*datasize);
	unsigned char* cpu_outputImg=(unsigned char *)malloc(sizeof(unsigned char)*datasize);
	unsigned char* gpu_outputImg=(unsigned char *)malloc(sizeof(unsigned char)*datasize);
	unsigned char* differenceImg=(unsigned char *)malloc(sizeof(unsigned char)*datasize);
	double f = 0.0;
	FILE *fp = fopen("data/testimg/test01.raw","rb");
	//FILE *fp = fopen("data/testimg/1200X1600_y_1.raw","rb");
	//FILE *fp0 = fopen("data/testimg/imggrey.raw","wb");
	//FILE *fp1 = fopen("data/testimg/cpu_sobel.raw","wb");
	//FILE *fp2 = fopen("data/testimg/gpu_sobel.raw","wb");
	//FILE *fp3 = fopen("data/testimg/differenceImg.raw","wb");

	fread(inputImg_gray,sizeof(unsigned char),datasize,fp);
	//fwrite(inputImg_gray,sizeof(unsigned char),datasize,fp0);
	
	memset(cpu_outputImg,0,datasize);
	memset(gpu_outputImg,0,datasize);

	//GPU
	start = clock();	//gpu计时开始
	//for (int i = 0; i < 200; i++)
		GPU_Sobel(inputImg_gray, gpu_outputImg,width,height);
	finish = clock();	//gpu计时结束
	GPU_time = (double) (finish - start) / CLOCKS_PER_SEC;
	sprintf(b, "%d\n", (int)gpu_outputImg[10]);
	strcat(rr, b);
	//CPU
 	start = clock();	//cpu计时开始
	//for (int i = 0; i < 200; i++)
		CPU_Sobel(inputImg_gray, cpu_outputImg,width,height);
	finish = clock();	//cpu计时结束
	CPU_time = (double) (finish - start) / CLOCKS_PER_SEC;
	sprintf(b, "%d\n", (int)cpu_outputImg[10]);
	strcat(rr, b);
	s = CPU_time / GPU_time;	//加速比

	//差异图，差异百分比
	
	f = difference(cpu_outputImg, gpu_outputImg, differenceImg,width,height);

	//保存图像
	
	//fwrite(cpu_outputImg,sizeof(unsigned char),datasize,fp1);
	
	//fwrite(gpu_outputImg,sizeof(unsigned char),datasize,fp2);

	//fwrite(differenceImg,sizeof(unsigned char),datasize,fp3);

	free(inputImg_gray);
	free(cpu_outputImg);
	free(gpu_outputImg);
	free(differenceImg);
	
	
	sprintf(b, "%.8f", GPU_time);
	strcat(rr, b);
	strcat(rr, " CPU:");
	sprintf(b, "%.8f", CPU_time);
	strcat(rr, b);

	strcat(rr, "\nSpeed-up ratio:");
	sprintf(b, "%.8f", s);
	strcat(rr, b);
	strcat(rr, "\ndifference:");
	sprintf(b, "%.8f", f);
	strcat(rr, b);

	fclose(fp);
	//fclose(fp0);
	//fclose(fp1);
	//fclose(fp2);
	//fclose(fp3);

	return env->NewStringUTF(rr);
	//return env->NewStringUTF("sobelFilter");

   }
}

void CPU_Sobel(unsigned char* inputImg_gray, unsigned char* outputImg,int width,int height) {
	unsigned char a00, a01, a02;
	unsigned char a10, a11, a12;
	unsigned char a20, a21, a22;
	int i,j;
	float ux,uy;
	unsigned char u;
	for(i = 1; i < height - 1; i++) {
		for(j = 1; j < width-1; j++) {
 			a00 = inputImg_gray[j-1+width*(i-1)];
 			a01 = inputImg_gray[j  +width*(i-1)];
			a02 = inputImg_gray[j+1+width*(i-1)];
			a10 = inputImg_gray[j-1+width*i];
			a11 = inputImg_gray[j   +width*i];
			a12 = inputImg_gray[j+1+width*i];
			a20 = inputImg_gray[j-1+width*(i+1)];
			a21 = inputImg_gray[j  +width*(i+1)];
			a22 = inputImg_gray[j+1+width*(i+1)];

			// x方向上的近似导数
			ux=a20*(1)+a21*(2)+a22*(1)+a00*(-1)+a01*(-2)+a02*(-1);
			// y方向上的近似导数
			uy=a02*(1)+a12*(2)+a22*(1)+a00*(-1)+a10*(-2)+a20*(-1);
			//梯度
			
			u =sqrt(ux*ux+uy*uy);

			//阈值法确定边缘
			if (u>255)
			{
			  u = 255;
			}else if(u<N)
			{
			  u = 0;
			}
			
			outputImg[j+width*i] =u;
		}
	}
}

int GPU_Sobel(unsigned char* inputImg_gray, unsigned char* outputImg,int width,int height) {
	/*這裡一定要初始化各个函数*/

	initFns();
	/*宿主机变量*/
	cl_uint numPlatforms; //the NO. of platforms
	cl_platform_id platform = NULL; //the chosen platform
	cl_int status;
	cl_platform_id* platforms;
	cl_uint numDevices = 0;
	cl_device_id *devices;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;
	//size_t global;
	cl_mem a1, a2, a3;

	int IMG_WIDTH = 256;
	int IMG_HEIGHT= 256;

	long IN_DATA_SIZE =IMG_WIDTH * IMG_HEIGHT;
	long OUT_DATA_SIZE = IMG_WIDTH * IMG_HEIGHT;
	//unsigned char *inputData1;
	//inputData1=(unsigned char *)malloc(DATA_SIZE*sizeof(unsigned char));
	unsigned char *inputData1;
	inputData1=(unsigned char *)malloc(IN_DATA_SIZE*sizeof(unsigned char));
	inputData1 = inputImg_gray;

	//float *outputData;
	//outputData=(float *)malloc(DATA_SIZE*sizeof(float));
	unsigned char *outputData;
	outputData=(unsigned char *)malloc(OUT_DATA_SIZE*sizeof(unsigned char));
	outputData = outputImg;

	int inputData2[2] = { IMG_WIDTH, IMG_HEIGHT};

	/*Step1: Getting platforms and choose an available one.*/
	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	//For clarity, choose the first available platform.
	if (numPlatforms > 0) {
		platforms = (cl_platform_id*) malloc(
				numPlatforms * sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}
	char buffer[1024];
	clGetPlatformInfo(platform,CL_PLATFORM_NAME,sizeof(buffer),buffer,NULL);

	//Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
//	if (numDevices == 0) //no GPU available.
//			{
//		printf("No GPU device available.\n");
//		printf("Choose CPU as default device.\n");
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL,
//				&numDevices);
//		devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices,
//				devices, NULL);
//	} else {
//		devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices,
//				devices, NULL);
//	}
	devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices,
			NULL);

	//Step 3: Create context
	context = clCreateContext(NULL, 1, devices, NULL, NULL, &status);

	//Step 4: Creating command queue associate with the context
	commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);

	///Step 5: Create program object
	const char *source = KERNEL_SRC;
	size_t sourceSize[] = { strlen(source) };
	program = clCreateProgramWithSource(context, 1, &source, sourceSize,
			&status);

	//Step 6: Build program.
	status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	a1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(unsigned char) * IN_DATA_SIZE, inputData1, &status);

	a2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(unsigned char) * OUT_DATA_SIZE, outputData, &status);

	a3 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(int) * 3, inputData2, &status);

	//Step 8: Create kernel object
	kernel = clCreateKernel(program, "Sobel", &status);

	// set the argument list for the kernel command
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a1);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &a2);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &a3);

	size_t local[] = { 11, 11 };
	size_t global[2];
	global[0] = (
			IMG_WIDTH % local[0] == 0 ?
					IMG_WIDTH : (IMG_WIDTH + local[0] - IMG_WIDTH % local[0]));
	global[1] =
			(IMG_HEIGHT % local[1] == 0 ?
					IMG_HEIGHT : (IMG_HEIGHT + local[1] - IMG_HEIGHT % local[1]));
	//	size_t local[] = { 2, 2 };
	//	size_t global[]={10,10};

	// enqueue the kernel command for execution
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global,
			local, 0, NULL, NULL);
	if (status != 0)
		return status;
	clFinish(commandQueue);

	clEnqueueReadBuffer(commandQueue, a2, CL_TRUE, 0,
			sizeof(unsigned char) * OUT_DATA_SIZE, outputData, 0, NULL, NULL);

	clReleaseMemObject(a1);
	clReleaseMemObject(a2);
	clReleaseMemObject(a3);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	return 0;
}

double difference(unsigned char* input1, unsigned char* input2, unsigned char* output,int width,int height) {

	unsigned char a;
	double re;
	int k = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			a = input1[j+width*i]- input2[j+width*i];
			output[j+width*i] = a;
			if (a != 0) {
				k++;
			}
		}
	}
	printf("差异点：%d\n", k);
	re = (double) k / (double) (height*width);
	return re;
}
