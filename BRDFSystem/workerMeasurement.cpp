#include "workerMeasurement.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerMeasurement::WorkerMeasurement(VimbaSystem&	system, QObject *parent) :
_system(system),
QObject(parent)
{
	_mutex.lock();
	//cameraAVT = new AVTCamera(_workerID, _system);
	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));
	connect(this, SIGNAL(done()), this, SLOT(ContributeBRDF()));

	sampleComm = new SampleComm();
	sampleComm->Init(2);
	slideComm = new SlideComm();
	slideComm->Init(9, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	illuminant = new Illuminant(5);
	illuminant->InitCOM();
	//illuminant->SetSteadyTime(30);
	//光源排布顺序
	_illuminantID = new UINT[196] { 0, 1, 2, 3,    25, 26, 27, 28, 29,    50,51,52,53,       75,76,77,78,79,   100,101,102,103,      125,126,127,128,129,    150,151,152,153,      175,176,177,178,179,\
									  4,5,6,7,8,     30,31,32,33,           54,55,56,57,58,    80,81,82,83,      104,105,106,107,108,  130,131,132,133,        154,155,156,157,158,  180,181,182,183,\
									  9,10,11,12,    34,35,36,37,           59,60,61,62,       84,85,86,87,      109,110,111,112,      134,135,136,137,        159,160,161,162,      184,185,186,187,\
									  13,14,15,      38,39,40,41,           63,64,65,          88,89,90,91,      113,114,115,          138,139,140,141,        163,164,165,          188,189,190,191,\
									  16,17,18,      42,43,44,              66,67,68,          92,93,94,         116,117,118,          142,143,144,            166,167,168,          192,193,194,\
									  19,20,21,      45,46,                 69,70,             95,96,            119,120,121,          145,146,                169,170,              195,196,\
									  22,23,         47,                    71,72,             97,               122,123,              147,                    171,172,              197,\
									  24,            48,                    73,                                  124,                  148,                    173,                  \
										    		 49,                                       98,                                     149,                                          198 };
	_iID = 0;
	_sID = 0;
	_isReady = 0;
	_captureDone = 0;
	_measureFlag = 0;
	_sampleFlag = 0;
	//_exposureTime = 50;
	//_saveName = 0;
	//_saveName = new int[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	_seriesCAM = new bool[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	/*
	//_measureFlag = measureFlag;
	illuminant->InitCOM();
	if (_measureFlag == 1)
		illuminant->SetSteadyTime(10);
	if (_measureFlag == 2)
		illuminant->SetSteadyTime(360);
	_timerId = this->startTimer(10000);//设置定时器触发子线程capture
	*/
}
WorkerMeasurement::~WorkerMeasurement()
{
	delete illuminant;
	delete _illuminantID;
	delete sampleComm;
	delete slideComm;
	//delete cameraAVT;

}

void WorkerMeasurement::StartTimer(int measureFlag)
{
	//slideComm->MoveToX2();//滑轨就位
	_measureFlag = measureFlag;
	//if (_measureFlag == 1)
	//	illuminant->SetSteadyTime(30);
	//if (_measureFlag == 2)
	//	illuminant->SetSteadyTime(200);
	_timerId = this->startTimer(8000);
}

void WorkerMeasurement::timerEvent(QTimerEvent *event)
{
	if (!_isReady)
	{
		if (_measureFlag == 1)
		{
			if (_iID != 196)
			{
				illuminant->Suspend();
				//Sleep(500);
				illuminant->SetSteadyTime(30);
				illuminant->LightenById(_illuminantID[_iID]);
				illuminant->Start();
				Sleep(500);//避免接收到灯亮前的图像
				_iID++;
				_isReady = 1;
				emit readyForGrab();//通过主线程告诉相机咱切换到下一个灯了，你可以试试调整一下你的曝光时间
			}
			else if (_iID == 196)
			{
				emit done();
				_isReady = 1;
			}
		}

		if (_measureFlag == 2)
		{
			if (_iID != 196)
			{
				if (_sampleFlag == 0)
				{
					illuminant->Suspend();
					illuminant->SetSteadyTime(200);
					illuminant->LightenById(_illuminantID[_iID]);
					illuminant->Start();
					Sleep(500);
				}

				if (_sID != 9)//36个角度耗时太长
				{
					sampleComm->GotoNextPos(175);
					Sleep(3000);//等待样品台旋转，设置2秒时指令污染
					emit readyForGrab();
					_sID++;
					_isReady = 1;
					_sampleFlag = 1;
				}
				else
				{
					_sampleFlag = 0;
					_sID = 0;//开始转下一圈
					_iID++;
					_isReady = 1;
				}
			}
			else if (_iID == 196)
			{
				emit done();
				_isReady = 1;
			}
		}
	}
	//else //光源样品就位，保存相机线程发来的采集图像
	//{
		//SaveSeriesMat(workerID, mat);
		//emit readyForGrab();
	//}
}

void WorkerMeasurement::CheckDone(int workerID)
{
#define CAM_NUM 9

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (workerID == i)
			_seriesCAM[i] = 1;
	}

	int s = 0;
	for (int i = 0; i < CAM_NUM; i++)
	{
		s += _seriesCAM[i];
	}
	if (s == CAM_NUM)
	{
		_isReady = 0;
		for (int i = 0; i < CAM_NUM; i++)
		{
			_seriesCAM[i] = 0;
		}
	}
	else
		_isReady = 1;
#undef CAM_NUM
}
////////////////////////////////////////////////////////////////////////////
// 函数：ContributeBRDF()
// 描述：采集原始图像完成后，生成材质数据
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void WorkerMeasurement::ContributeBRDF()
{

}

void WorkerMeasurement::CloseWorker()
{
	this->killTimer(_timerId);
}












