#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include <stdint.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "featureExtract.h"

string setFilename(const int& index, const char * pre, const char *pro)
{
	char fullname[100] = "";
	strcat_s(fullname, pre);
	char tmpStr[10] = { "" };
	sprintf_s(tmpStr, "%d", index);
	strcat_s(fullname, tmpStr);
	strcat_s(fullname, pro);
	string xfull(fullname);
	return xfull;
}

using namespace std;
using namespace cv;

 unsigned short int hammingDist(short unsigned int x, short unsigned int y)
{
	short unsigned int table[256] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 };
	short unsigned int diff = x ^ y;
	short unsigned int result = 0;
	result = table[diff >> 8] + table[diff & 0xFF];
	return result;
}

 // note that here the length of each array must be in order, the length of p must be shorter than q
 int diff(unsigned short int *p, unsigned short int* q, int l, int r, int& startIndex)
 {
	 if (l > r)
		 return -1;

	 int sum = 10000;
	 for (int i = 0; i <= r - l;  i++)
	 {
		 //cout << p[9] << endl;
		 //cout << q[9] << endl;
		 int tmpSum = 0;
		 for (int j = 0; j < l; j++)
		 {
			 int tmp = hammingDist(p[j], q[j]);
			 //if (p[j] != q[j])
				// cout << tmp << endl;
			 tmpSum += hammingDist(p[j], q[j]);
		 }
		 if (tmpSum < sum)
		 {
			 sum = tmpSum;
			 startIndex = i;
		 }
	 }
	 return sum;
 }


unsigned short int* processVideos(const char* filename, int& byteNum)
{
	VideoCapture videoFrame;
	Mat frame;

	videoFrame.open(filename);
	if (!videoFrame.isOpened())
	{
		cout << "failed to open the video!" << endl;
		return NULL;
	}

	double rate = videoFrame.get(CV_CAP_PROP_FPS);
	int frameStep = static_cast<int>(rate) / 10;
	int flag = 0;
	int interval = 1000 / rate;
	bool isNot = false;
	double frameNum = videoFrame.get(CV_CAP_PROP_FRAME_COUNT);
	int totalNum = static_cast<int>(frameNum);
	cout << "the frame number is:\t" << totalNum << endl;
	int index = 0;

	unsigned short int* mdata = new unsigned short int[totalNum];
	while (!isNot)
	{
		if (index < totalNum)
		{
			//videoFrame.grab();
			//videoFrame.retrieve(frame, CV_8UC1);
			videoFrame.read(frame);
			//index += frameStep;
			//videoFrame.set(CV_CAP_PROP_POS_FRAMES, index);
		}
		else
			break;

		//帧采样
		if (flag != frameStep)  //帧采样间隔
		{
			flag++;
			continue;
		}
		flag = 0;	//flag = frameStep时将当前帧进行特征提取，并且将flag重置为0;

		if (frame.empty())
			break;

		//cout << index << endl;
		unsigned short int tmpResult = 0;
		int strideSize = 2;
		//genFeature(frame, tmpResult);
		genSparseFeature(frame, strideSize, tmpResult);
		//cout << tmpResult << endl;
		mdata[index] = tmpResult;
		index++;
		//cout << index << endl;

		if (waitKey(interval) > 0)
		{
			isNot = true;
		}
	}

	byteNum = index;

	videoFrame.release();
	return mdata;
}

bool setVideoCaptureDonotWork(char* filename)
{
	VideoCapture videoFrame;
	Mat frame;

	videoFrame.open(filename);
	if (!videoFrame.isOpened())
	{
		cout << "failed to open the video!" << endl;
		return NULL;
	}

	double rate = videoFrame.get(CV_CAP_PROP_FPS);
	double frameNum = videoFrame.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "before setting the fps:" << frameNum << "\t" << rate << endl;
	rate = videoFrame.get(CV_CAP_PROP_FPS);
	videoFrame.set(CV_CAP_PROP_FPS, 10);
	frameNum = videoFrame.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "after setting the fps:" << frameNum << "\t" << rate << endl;
	return true;
}

int test(char*filename, string& prefix,  int& result)
{
	string inputFile =  filename;
	string inputFile1 = "G:/datasets/videos/" + inputFile;
	string inputFile2 = "G:/datasets/videos/mid_" + inputFile;
	string inputFile3 = "G:/datasets/videos/pre_" + inputFile;
	

	unsigned short int  *f1, *f2, *f3;
	int num1, num2, num3;
	num1 = num2 = num3 = 0;
	f1 = processVideos(inputFile1.c_str(), num1);
	f2 = processVideos(inputFile2.c_str(), num2);
	f3 = processVideos(inputFile3.c_str(), num3);

	int startIndex1 = 0;
	int distance1 = 0;
	if (num2 > num1)
	{
		distance1 = diff(f1,  f2, num1, num2, startIndex1);
	}
	else
	{
		distance1 = diff(f2, f1, num2, num1, startIndex1);
	}

	int distance2 = 0;
	int startIndex2 = 0;
	if (num3 > num1)
	{
		distance2 = diff(f1,  f3, num1, num3, startIndex2);
	}
	else
	{
		distance2 = diff(f3, f1, num3, num1, startIndex2);
	}

	cout << "mid diff is:" << distance1 << "\t" << startIndex1 << endl;
	cout << "pre diff is:" << distance2 << "\t" << startIndex2 << endl;
	return 0;
}

int main(int argc, char** argv)
{
	//@ test the match method
	//int result;
	//string pre = "pre";
	//int errInfo = test("ironhide-sample-10240.mp4", pre, result);

	//@test the extraction method
	time_t t_start, t_mid, t_check, t_end;

	// extract binary features
	t_start = time(NULL);
	unsigned short int* result;
	int nums = 0;

	result = processVideos("G://datasets/videos/ironhide-sample-3627.avi", nums);
	if (result == NULL)
	{
		return 0;
	}
	t_mid = time(NULL);
	cout << "feature extraction cost time:\t"<<difftime(t_mid, t_start) << endl;
	short unsigned int* tmp = new short unsigned int[nums];
	int i = 0;
	while (i < nums)
	{
		tmp[i] = result[i];
		i++;
	}
	tmp[9] = 14;
	cout << result[9] << endl;
	cout << tmp[9] << endl;

	// features matching
	int mresult = 10000;
	int index = 0;
	t_check = time(NULL);
	mresult = diff(result,tmp, nums, nums, index);
	t_end = time(NULL);
	cout << "matching time cost:\t"<<difftime(t_end, t_check) << endl;

	cout << result[9] << endl;
	cout << tmp[9] << endl;
	cout << mresult<< endl;
	delete[] tmp;

	return 0;
}
