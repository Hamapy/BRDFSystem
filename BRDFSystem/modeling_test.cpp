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
	//����BRDF���������
	BRDFFitting a;
	
	//����BRDF���Ա����StartFitting���������ļ�������Ϊ������ʼ���
	//���Ϊ1-100�����Ը�����Ҫ���ж����һ�����ʵ����
	for (BRDFSerialNumber = 1; BRDFSerialNumber <= 100; BRDFSerialNumber++)
	{
		a.StartFitting(BRDFSerialNumber);
	}

		
	


	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	system("pause");
	return 0;
}

