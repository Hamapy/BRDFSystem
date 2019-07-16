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
	
	//调用BRDF类成员函数StartFitting，并传入文件名称作为参数开始拟合
	//序号为1-100，可以根据需要进行多个或一个材质的拟合
	for (BRDFSerialNumber = 1; BRDFSerialNumber <= 100; BRDFSerialNumber++)
	{
		a.StartFitting(BRDFSerialNumber);
	}

		
	


	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	system("pause");
	return 0;
}

