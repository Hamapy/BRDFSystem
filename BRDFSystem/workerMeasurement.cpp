#include "workerMeasurement.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerMeasurement::WorkerMeasurement(QObject *parent) :
QObject(parent)
{
	illuminant = new Illuminant(6);
	_saveName = 0;
}
WorkerMeasurement::~WorkerMeasurement()
{
	this->killTimer(_timerId);
}

void WorkerMeasurement::StartTimer()
{
	_timerId = this->startTimer(5000);//设置定时器触发子线程capture
}

void WorkerMeasurement::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		//illuminant.NextIlluminant(int exposureTimes);
		connect(this->parent(), SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));		
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

	return;
}

