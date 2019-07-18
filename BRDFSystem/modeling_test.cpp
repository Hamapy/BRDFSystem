#pragma once


#include "brdfModeling.h"
#include <time.h>


using namespace cv;
using namespace std;



int main3()
{
	
	clock_t start, finish;
	double totaltime;
	start = clock();
	int BRDFSerialNumber;
	//调用BRDF参数拟合类
	BRDFFitting a;
	
	// 调用BRDF类成员函数StartFitting，并传入文件名称作为参数开始拟合
	// 序号为1-100，可以根据需要进行多个或一个材质的拟合
	// 传入参数还包括采样角度信息，入射角度和观测角度中的theta采用一样的信息，theta和phi采用一样的采样间隔Gap，
	// thetaInLowerLimit为theta的采样角度下限，thetaInNumber为theta的采样数量，phiInLowerLimit为入射角度phi的采样角度下限，phiInNumber为入射角度phi的采样数量
	// void StartFitting(int BRDFSerialNumber, int thetaInLowerLimit, int thetaInNumber, int phiInLowerLimit, int phiInNumber, int Gap);
	for (BRDFSerialNumber = 1; BRDFSerialNumber <= 100; BRDFSerialNumber++)
	{
		a.StartFitting(BRDFSerialNumber, 10, 15, 5, 35, 5);
	}

		
	


	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	system("pause");
	return 0;
}

