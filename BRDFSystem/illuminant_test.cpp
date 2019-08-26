#include<iostream>
//#include<windows.h> 
#include "Illuminant.h"
using namespace std;
int main0()
{
	//此程序的使用方法1.首先调用InitiCOM()，打开和初始化串口 2.调用SetSteadyTime设置所需要的亮灯时间
	//3.使用Collection函数实现正式的采集顺序
	UINT _illuminantID[] = { 0, 1, 2, 3, 25, 26, 27, 28, 29, 50, 51, 52, 53, 75, 76, 77, 78, 79, 100, 101, 102, 103, 125, 126, 127, 128, 129, 150, 151, 152, 153, 175, 176, 177, 178, 179, \
		4, 5, 6, 7, 8, 30, 31, 32, 33, 54, 55, 56, 57, 58, 80, 81, 82, 83, 104, 105, 106, 107, 108, 130, 131, 132, 133, 154, 155, 156, 157, 158, 180, 181, 182, 183, \
		9, 10, 11, 12, 34, 35, 36, 37, 59, 60, 61, 62, 84, 85, 86, 87, 109, 110, 111, 112, 134, 135, 136, 137, 159, 160, 161, 162, 184, 185, 186, 187, \
		13, 14, 15, 38, 39, 40, 41, 63, 64, 65, 88, 89, 90, 91, 113, 114, 115, 138, 139, 140, 141, 163, 164, 165, 188, 189, 190, 191, \
		16, 17, 18, 42, 43, 44, 66, 67, 68, 92, 93, 94, 116, 117, 118, 142, 143, 144, 166, 167, 168, 192, 193, 194, \
		19, 20, 21, 45, 46, 69, 70, 95, 96, 119, 120, 121, 145, 146, 169, 170, 195, 196, \
		22, 23, 47, 71, 72, 97, 122, 123, 147, 171, 172, 197, \
		24, 48, 73, 124, 148, 173, \
		49, 98, 149, 198 };

	//UINT _illuminantID[] = {24, 48, 73, 124, 148, 173, 49, 98, 149, 198 };
	
	bool ret;
	Illuminant a;
	ret = a.InitCOM(5);
	//a.OpenCOM();
	
	bool flag = 0;
	for (int i = 0; i < 196; i++)
	{
		if (flag == 1)
			ret = a.Suspend(_illuminantID[i-1] + 1);
		else
			flag = 1;
		//ret = a.SetSteadyTime(5);
		ret = a.LightenById(_illuminantID[i] + 1);
		
		//ret = a.Start();
		//Sleep(500);
	}

	/*
	unsigned char *temp = new unsigned char[9];//动态创建一个数组
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
	if (!a.OpenListenThread())//是否打开监听线程，开启线程用来传输返回值
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