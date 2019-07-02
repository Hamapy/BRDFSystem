////////////////////////////BRDF���ݽ�ģ������////////////////////////////
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
	//���ڳ�ʼ��
	bool InitCOM();
	//������ʱ
	bool SetSteadyTime(int steadyTime);
	//��ID����	
	//bool LightenById(vector<int> id);
	bool LightenById(UINT id);
	//����Ƭ����λ��˳������
	bool LightenByOrder(int num);
	//��ȡ���ڽ��ܻ������е�����
	//void ReadData(unsigned char* cRecved);

	//��������ָ��
	bool Start();
	//������ͣ/ָֹͣ��
	bool Suspend();
	//ʵ�ְ����Զ����˳�������Դ
	bool LightenByCustomOrder(unsigned char* Order, int length);
	//bool Run(unsigned char* Order, int length);
	//���������߳�
	bool OpenListenThread();
	//��ʽʵ�ֲɼ�
	void Collection(unsigned char* Order, int length);
private:
	//��ȡ���뻺�������ֽ���
	UINT GetBytesInCOM();

	//�򿪴���
	bool OpenCOM();
	//�رռ����߳�
	bool CloseListenTread();
	//��մ������뻺����
	void Clear();
	//�򿪴���
	bool CloseCOM();
	//������д�봮��
	bool WriteData(unsigned char *pData, int length);
	//��ȡ���ڽ��ܻ�������һ���ֽڵ�����
	bool ReadChar(unsigned char& cRecved);
	//ʵ��������㣬�õ�У����
	unsigned char Xor(unsigned char* array, int size);
	//���ڼ����߳�
	static UINT WINAPI ListenThread(void* pParam);
	HANDLE              _hComm;//���ھ��
	UINT                _portNo;//�ڼ���COM��
	//int 				_orderType;//���ü�������˳��ԭ����ֻ��Ҫһ��
	static     int 		_steadyTime;//��Դ��������ʱ��
	//int 				_waitingTime;//��Դ�������ʱ��
	static     bool     _bExit;  //�߳��˳���־����
	volatile HANDLE     _hListenThread;//�߳̾��
	CRITICAL_SECTION    _csCommunicationSync;//ͬ�����⣬�ٽ�������
	static    atomic<bool>      _flag;//�жϹ�Դ���������Ƿ����,Ӧ��Ϊԭ�Ӳ���
};