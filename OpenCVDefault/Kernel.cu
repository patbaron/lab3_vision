#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>

int iDivUp(const int a, const int b) { return (a % b != 0) ? (a / b + 1) : (a / b); };

__global__ static void KernelRect(unsigned char *imgdst,long *X,long *Y, int imgWidth, int imgHeight)
{
	unsigned long index = threadIdx.x + blockIdx.x * blockDim.x;
	int sum = 0;

	//sommation Y
	if(index < imgHeight)
	{
		for(int i = 0; i < imgWidth;i++)
			sum = sum + imgdst[index * imgWidth + i];
		Y[index] = sum;
	}

	sum = 0;

	//sommation X
	if(index < imgWidth)
	{
		for(int i = 0; i < imgHeight; i++)
			sum = sum + imgdst[index + imgWidth * i];
		X[index] = sum;
	}
}

__global__ static void KernelSobel(unsigned char *imgscr,unsigned char *imgscr2, unsigned char *imgdst,int imgWidth,int imgHeight,unsigned char K)
{
	unsigned long index;
	unsigned char pValue;

	index = threadIdx.x + blockIdx.x * blockDim.x;
	pValue = abs(imgscr[index] - imgscr2[index]);
	
	if(pValue < K)
		imgdst[index] = 0;
	else
		imgdst[index] =255;
	
}

extern "C" void Mykernelfunc(unsigned char *c_imgsrc, unsigned char *c_imgsrc2, unsigned char *c_imgdst, long *X, long *Y, int imageWidth, int imageHeight, unsigned long imagesize, unsigned char K)
{
	float tempsex = 0;
	
	// Defining gpu variables
	unsigned char *gpu_imgsrc = 0;
	unsigned char *gpu_imgsrc2 = 0;
	unsigned char *gpu_imgdst = 0;
	long *gpu_X = 0;
	long *gpu_Y = 0;

	// Defining size of memory allocations
	dim3 dimBlock = 16;
	dim3 dimGrid = imagesize / dimBlock.x;
	dim3 dimGridsum = 0;

	// Grabbing the highest value of both imageheight and imagewidth
	if(imageHeight > imageWidth)
		dimGridsum = iDivUp(imageHeight, dimBlock.x);
	else
		dimGridsum = iDivUp(imageWidth, dimBlock.x);

	// Finding the memory size of the image
	size_t memsize = imagesize * sizeof(unsigned char);

	//Allocating memory
	cudaMalloc((void**)&gpu_imgsrc, memsize);
	cudaMemcpy(gpu_imgsrc, c_imgsrc, memsize, cudaMemcpyHostToDevice);
	cudaMalloc((void**)&gpu_imgsrc2, memsize);
	cudaMemcpy(gpu_imgsrc2, c_imgsrc2, memsize, cudaMemcpyHostToDevice);
	cudaMalloc((void**)&gpu_imgdst, memsize);
	cudaMalloc((void**)&gpu_X,imageWidth * sizeof(long));
	cudaMemcpy(gpu_X, X, imageWidth * sizeof(long), cudaMemcpyHostToDevice);
	cudaMalloc((void**)&gpu_Y,imageHeight * sizeof(long));
	cudaMemcpy(gpu_Y, Y, imageHeight * sizeof(long), cudaMemcpyHostToDevice);
	
	// Launching kernel functions
	KernelSobel<<<dimGrid.x, dimBlock.x>>>(gpu_imgsrc, gpu_imgsrc2, gpu_imgdst, imageWidth, imageHeight, K);
	cudaThreadSynchronize();

	KernelRect<<<dimGridsum.x,dimBlock.x>>>(gpu_imgdst, gpu_X, gpu_Y, imageWidth, imageHeight);
	cudaThreadSynchronize();

	// Grabbing data from gpu variables
	cudaMemcpy(c_imgdst,gpu_imgdst, memsize, cudaMemcpyDeviceToHost);
	cudaMemcpy(X,gpu_X,imageWidth * sizeof(long), cudaMemcpyDeviceToHost);
	cudaMemcpy(Y,gpu_Y,imageHeight * sizeof(long), cudaMemcpyDeviceToHost);

	// Freeing memory
	cudaFree(gpu_imgdst);
	cudaFree(gpu_imgsrc);
	cudaFree(gpu_imgsrc2);
	cudaFree(gpu_X);
	cudaFree(gpu_Y);
}
