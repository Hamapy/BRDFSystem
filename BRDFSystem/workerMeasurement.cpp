#include "workerMeasurement.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerMeasurement::WorkerMeasurement(QObject *parent) :
QObject(parent)
{
	illuminant = new Illuminant(5);
	sampleComm = new SampleComm();
	sampleComm->Init(2, 0, 0.25, STEP_ACCELERATE, STEP_DECELERATE, STEP_RESOLUTION, STEP_TOHOME);
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
	_ID = 0;
	_saveName = 0;
	_measureFlag = 0;
}
WorkerMeasurement::~WorkerMeasurement()
{
	delete illuminant;
	delete _illuminantID;
	this->killTimer(_timerId);
}

void WorkerMeasurement::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	illuminant->InitCOM();
	//为什么程序启动会自动跳到这句
	illuminant->SetSteadyTime(30);
	_timerId = this->startTimer(10000);//设置定时器触发子线程capture
}

void WorkerMeasurement::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		if (_measureFlag = 1)
		{
			illuminant->LightenById(_illuminantID[_ID]);
			illuminant->Start();
			_ID++;
			Sleep(1000);//避免收到灯亮前的图像
			connect(this->parent(), SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));//这么写是为了避免与workerCCD抢占相机资源并减去不必要的相机匹配ID参数传递		

		}
		if (_measureFlag = 2)
		{
			sampleComm->GotoNextPos(100);
			illuminant->LightenById(_illuminantID[_ID]);
			illuminant->Start();
			_ID++;
			Sleep(1000);//避免收到灯亮前的图像
			connect(this->parent(), SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));//这么写是为了避免与workerCCD抢占相机资源并减去不必要的相机匹配ID参数传递		

		}

	}
}


void WorkerMeasurement::SaveAMat(int workerID, Mat mat)
{
	disconnect(this->parent(), SIGNAL(sendingImg(QImage)), this, SLOT(SaveAnImage(QImage)));

	char saveName[4] = { 0 };
	sprintf(saveName, "%4d", _saveName);
	char sPath[200];

	sprintf(sPath, "//%s.bmp", saveName);
	string path = _imageSavingPath + "//camera" + to_string(workerID) + sPath;
	imwrite(path, mat);
	_saveName++;

	return;
}


