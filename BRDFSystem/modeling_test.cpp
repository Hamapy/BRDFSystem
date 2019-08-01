#pragma once


#include "brdfModeling.h"
#include <time.h>


//using namespace cv;
//using namespace std;



int mainm()
{
	clock_t start, finish;
	double totaltime;
	start = clock();
	int BRDFSerialNumber;

	// 调用BRDF参数拟合类
	BRDFFitting a;
	Mat param = Mat::zeros(1, 7, CV_64FC1);;


	// 调用BRDF类成员函数StartFitting，并传入文件名称作为参数开始拟合


	param = a.StartFitting("blueprint.binary");







	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	system("pause");
	return 0;
}

