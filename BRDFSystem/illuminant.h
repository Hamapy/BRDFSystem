////////////////////////////BRDF数据建模类声明////////////////////////////
//Author:ZJB
//Review:ZJX
#pragma once
#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <time.h>
#include <process.h>
#include <iomanip>
#include <algorithm>
#include <atomic>
#include <thread>

using namespace std;
class Illuminant
{
public:
	Illuminant(UINT portNo);
	~Illuminant();
	//串口初始化
	bool InitCOM();
	//设置延时
	bool SetSteadyTime(int steadyTime);
	//按ID亮灯	
	//bool LightenById(vector<int> id);
	bool LightenById(UINT id);
	//按单片机槽位的顺序亮灯
	bool LightenByOrder(int num);
	//读取串口接受缓冲区中的数据
	//void ReadData(unsigned char* cRecved);

	//发送启动指令
	bool Start();
	//发送暂停/停止指令
	bool Suspend();
	//实现按照自定义的顺序点亮光源
	bool LightenByCustomOrder(unsigned char* Order, int length);
	//bool Run(unsigned char* Order, int length);
	//开启监听线程
	bool OpenListenThread();
	//正式实现采集
	void Collection(unsigned char* Order, int length);
private:
	//获取输入缓冲区的字节数
	UINT GetBytesInCOM();

	//打开串口
	bool OpenCOM();
	//关闭监听线程
	bool CloseListenTread();
	//清空串口输入缓冲区
	void Clear();
	//打开串口
	bool CloseCOM();
	//将数据写入串口
	bool WriteData(unsigned char *pData, int length);
	//读取串口接受缓冲区中一个字节的数据
	bool ReadChar(unsigned char& cRecved);
	//实现异或运算，得到校验码
	unsigned char Xor(unsigned char* array, int size);
	//串口监听线程
	static UINT WINAPI ListenThread(void* pParam);
	HANDLE              _hComm;//串口句柄
	UINT                _portNo;//第几个COM口
	//int 				_orderType;//设置几种亮灯顺序，原则上只需要一种
	static     int 		_steadyTime;//光源点亮持续时间
	//int 				_waitingTime;//光源点亮间隔时间
	static     bool     _bExit;  //线程退出标志变量
	volatile HANDLE     _hListenThread;//线程句柄
	CRITICAL_SECTION    _csCommunicationSync;//同步互斥，临界区保护
	static    atomic<bool>      _flag;//判断光源点亮过程是否出错,应该为原子操作
};