#ifndef FEATURE_EXTRACT_H_
#define FEATURE_EXTRACT_H_

#include <iostream>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

typedef int strideSize;
// 面积法计算各个块的均值，再计算特征
bool fastGenFeature(const Mat& src,  unsigned short int & result,  strideSize stride)
{
	//imshow("test", src);
	//waitKey(1000);
	int nc = src.cols;
	int nl = src.rows;
	int stepc = nc / 4;
	int stepl = nl / 4;
	int sum[16] = { 0 };
	int num[16] = { 0 };
	result = 0;

	//面积法计算16块取值
	//if (src.isContinuous())
	//{
	//	nc = nl*nc;
	//	nl = 1;
	//}

	//初始化
	Mat areas = Mat::zeros(src.size(), CV_8UC1);
	areas.at<uchar>(0, 0) = 0;
	for (int i = 1; i < nl; i+=stride)
	{
		const uchar* data1 = src.ptr<uchar>(i);
		uchar* p1 = areas.ptr<uchar>(i-1);
		uchar* p2 = areas.ptr<uchar>(i);
		p2[0] = p1[0] + data1[0];
	}

	uchar* p3 = areas.ptr<uchar>(0);
	const uchar* data2 = src.ptr<uchar>(0);
	for (int j = 1; j < nc; j += stride)
	{
		p3[j] = p3[j-1] + data2[j];
	}

	//动态规划                  //这是这个代码有问题的地方，计算的是stride整除的地方，后面求每块单独面积时用的是整除少1的地方，因而后面会出现初始化的0值，而非想要计算的：值
	for (int i = stride; i < nl; i += stride)
	{
		const uchar* data = src.ptr<uchar>(i);
		uchar* p = areas.ptr<uchar>(i);
		uchar* topP = areas.ptr<uchar>(i - stride);
		for (int j = stride; j < nc; j += stride)
		{
			p[j] = (topP[j] + p[j - stride] - topP[j - stride]) + data[j];
		}
	}

	int vecfea[17] = { 0 };
	int prePartY = 0;
	int firstRowX = stepl / stride * stride;
	for (int i = 1; i < 5; i++)
	{
		int wy = (stepc*(i)) / stride * stride -1;
		vecfea[i] = areas.at<uchar>(firstRowX, wy) - areas.at<uchar>(firstRowX, prePartY);
		int totalNumPix = (wy - prePartY) * firstRowX;
		vecfea[i] /= totalNumPix;
		prePartY = wy;
	}

	int prePartX = 0;
	int firstColumnY = stepc / stride*stride;
	for (int j = 1; j < 5;j++)
	{
		int wx = (stepl*j) / stride * stride -1;
		vecfea[(j - 1) * 4 + 1] = areas.at<uchar>(wx, firstColumnY) - areas.at<uchar>(wx, prePartX);
		int totalNumPix = (wx - prePartX)* firstColumnY;
		vecfea[(j-1)*4 +1] /= totalNumPix;
		prePartX = wx;
	}

	//求每一块的平均值
	for (int k = 6; k < 17; k++)
	{
		if (k %4 == 1)
			continue;
		int wx1 = (stepl* (((k-1) /4) +1)) / stride * stride -1;
		int wy1= (stepc*((k-1) % 4 +1)) / stride * stride -1;
		int preX = (stepl*((k - 1) / 4)) / stride * stride -1;  //上一个part的行数
		int preY = (stepc*((k - 1) % 4)) / stride * stride -1; //左边一个part的列数
		//vecfea[k] = areas.at<uchar>(wx, wy) + vecfea[k-5] -vecfea[k-1] -vecfea[k-4];
		vecfea[k] = areas.at<uchar>(wx1, wy1) + areas.at<uchar>(preX, preY) - areas.at<uchar>(preX, wy1) - areas.at<uchar>(wx1, preY);
		int totalNumPix = (wx1 - preX) * (wy1 - preY);
		vecfea[k] /= totalNumPix;
	}

	//归一化 ，通过与每行最左边的块进行比较
	for (int k = 1; k < 17; k++)
	{
		if (vecfea[k] >= vecfea[((k-1)>>2)*4+1])
		{
			result |= 1 << ((k-1) & 0x0F);
		}
	}
	//cout <<" the result is:"<< result << endl;
	return true;
}

//普通计算feature方法，在计算块的序数时采用除法
bool genFeature(const Mat& src, unsigned short int & result)
{
	int nc = src.cols;
	int nl = src.rows;
	int stepc = nc / 4;
	int columnRange[4] = { 0, stepc, stepc << 1, (stepc << 1) + stepc };
	int stepl = nl / 4;
	int lineRange[4] = { 0, stepl, stepl << 1, (stepl << 1) + stepl };
	int sum[16] = { 0 };
	int num[16] = { 0 };
	result = 0;

	// 分成16块取值
	for (int i = 0; i < nl; i++)
	{
		int lineIndex = 0;
		if (i < lineRange[1])
			lineIndex = 0;
		else if (i >= lineRange[1] && i < lineRange[2])
			lineIndex = 1;
		else if (i >= lineRange[2] && i < lineRange[3])
			lineIndex = 2;
		else
			lineIndex = 3;

		const uchar* p = src.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			int columnIndex = 0;
			if (j < columnRange[1])
				columnIndex = 0;
			else if (j >= columnRange[1] && j < columnRange[2])
				columnIndex = 1;
			else if (j >= columnRange[2] && j < columnRange[3])
				columnIndex = 2;
			else
				columnIndex = 3;
			int index = (lineIndex << 2) + columnIndex;
			//int index = 4* (i / stepl )+ (j / stepc);
			sum[index] += p[j];
			num[index] ++;
		}
	}

	//求每一块的平均值
	int vecfea[16] = { 0 };
	for (int k = 0; k < 16; k++)
		sum[k] /= num[k];
	//cout << sum[13] << endl;

	//归一化 ，通过与每行最左边的块进行比较
	for (int k = 0; k < 16; k++)
	{
		if (sum[k] >= sum[(k >> 2) << 2])
			result |= 1 << (k & 0x0F);
	}
	//cout <<" the result is:"<< result << endl;
	return true;
}

// Extract feature with downsampling
//@ src : source image 
//@ stride: the stride for scanning
//@ result: the 16bit feature which has been stored in a short unsigned int
bool genSparseFeature(const Mat& src, int& stride, unsigned short int & result)
{
	if (src.empty())
		return false;
	int nc = src.cols;
	int nl = src.rows;
	stride = stride * src.channels(); // make it adaptive to multi-channel images

	int stepc = nc / 4;
	int columnRange[4] = { 0, stepc, stepc << 1, (stepc << 1) + stepc };
	int stepl = nl / 4;
	int lineRange[4] = { 0, stepl, stepl << 1, (stepl << 1) + stepl };
	int sum[16] = { 0 };
	int num[16] = { 0 };
	result = 0;

	// 分成16块取值
	for (int i = 0; i < nl; i+= stride)
	{
		int lineIndex = 0;
		if (i < lineRange[1])
			lineIndex = 0;
		else if (i >= lineRange[1] && i < lineRange[2])
			lineIndex = 1;
		else if (i >= lineRange[2] && i < lineRange[3])
			lineIndex = 2;
		else
			lineIndex = 3;

		const uchar* p = src.ptr<uchar>(i);
		for (int j = 0; j < nc; j+=stride)
		{
			int columnIndex = 0;
			if (j < columnRange[1])
				columnIndex = 0;
			else if (j >= columnRange[1] && j < columnRange[2])
				columnIndex = 1;
			else if (j >= columnRange[2] && j < columnRange[3])
				columnIndex = 2;
			else
				columnIndex = 3;
			int index = (lineIndex << 2) + columnIndex;
			//int index = 4* (i / stepl )+ (j / stepc);
			sum[index] += p[j];
			num[index] ++;
		}
	}

	//求每一块的平均值
	int vecfea[16] = { 0 };
	for (int k = 0; k < 16; k++)
		sum[k] /= num[k];
	//cout << sum[13] << endl;

	//归一化 ，通过与每行最左边的块进行比较
	for (int k = 0; k < 16; k++)
	{
		if (sum[k] >= sum[(k >> 2) << 2])
			result |= 1 << (k & 0x0F);
	}
	//cout <<" the result is:"<< result << endl;
	return true;
}

// extracted feature with multithread and downsampling
//@ src : source image 
//@ stride: the stride for scanning
//@ result: the 16bit feature which has been stored in a short unsigned int
bool multiThreadSparseFeature(const Mat& src, int& stride, unsigned short int & result)
{
	if (src.empty())
		return false;
	int nc = src.cols;
	int nl = src.rows;
	int stepc = nc / 4;
	int columnRange[4] = { 0, stepc, stepc << 1, (stepc << 1) + stepc };
	int stepl = nl / 4;
	int lineRange[4] = { 0, stepl, stepl << 1, (stepl << 1) + stepl };
	int sum[16] = { 0 };
	int num[16] = { 0 };
	result = 0;

	// 分成16块取值
	for (int i = 0; i < nl; i+= stride)
	{
		int lineIndex = 0;
		if (i < lineRange[1])
			lineIndex = 0;
		else if (i >= lineRange[1] && i < lineRange[2])
			lineIndex = 1;
		else if (i >= lineRange[2] && i < lineRange[3])
			lineIndex = 2;
		else
			lineIndex = 3;

		const uchar* p = src.ptr<uchar>(i);
		for (int j = 0; j < nc; j+=stride)
		{
			int columnIndex = 0;
			if (j < columnRange[1])
				columnIndex = 0;
			else if (j >= columnRange[1] && j < columnRange[2])
				columnIndex = 1;
			else if (j >= columnRange[2] && j < columnRange[3])
				columnIndex = 2;
			else
				columnIndex = 3;
			int index = (lineIndex << 2) + columnIndex;
			//int index = 4* (i / stepl )+ (j / stepc);
			sum[index] += p[j];
			num[index] ++;
		}
	}

	//求每一块的平均值
	int vecfea[16] = { 0 };
	for (int k = 0; k < 16; k++)
		sum[k] /= num[k];
	//cout << sum[13] << endl;

	//归一化 ，通过与每行最左边的块进行比较
	for (int k = 0; k < 16; k++)
	{
		if (sum[k] >= sum[(k >> 2) << 2])
			result |= 1 << (k & 0x0F);
	}
	//cout <<" the result is:"<< result << endl;
	return true;
}

#endif