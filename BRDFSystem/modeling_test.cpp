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

	// ����BRDF���������
	BRDFFitting a;
	Mat param = Mat::zeros(1, 7, CV_64FC1);;


	// ����BRDF���Ա����StartFitting���������ļ�������Ϊ������ʼ���


	param = a.StartFitting("blueprint.binary");







	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	system("pause");
	return 0;
}

