#include<iostream>
//#include<windows.h> 
#include "Illuminant.h"
using namespace std;
int main4()
{
	//�˳����ʹ�÷���1.���ȵ���InitiCOM()���򿪺ͳ�ʼ������ 2.����SetSteadyTime��������Ҫ������ʱ��
	//3.ʹ��Collection����ʵ����ʽ�Ĳɼ�˳��
	Illuminant a;
	//a.OpenCOM();
	a.InitCOM(5);
	a.SetSteadyTime(200);
	a.LightenById(2);
	a.Start();
	Sleep(200);
	/*
	unsigned char *temp = new unsigned char[9];//��̬����һ������
	temp[0] = 0x91;
	temp[1] = 0x92;
	temp[2] = 0x93;
	temp[3] = 0x94;
	temp[4] = 0x95;
	temp[5] = 0x96;
	temp[6] = 0x97;
	temp[7] = 0x98;
	temp[8] = 0x99;
	//a.LightenByCustomOrder(temp, 9);
	//a.Start();
	a.Collection(temp, 9);
	if (!a.OpenListenThread())//�Ƿ�򿪼����̣߳������߳��������䷵��ֵ
    {
		std::cout << "OpenListenThread fail !" << std::endl;
    }
	else
	{
		std::cout << "OpenListenThread success !" << std::endl;
	}
	//a.LightenByCustomOrder(temp, 9);
	//a.Start();
	//a.Suspend();
	system("pause");
	//cout << "ok" << endl;
	*/
	return 0;
	
}