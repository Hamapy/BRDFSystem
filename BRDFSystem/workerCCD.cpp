#include "workerCCD.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_capture = 0;
	_measurement = 0;
	//connect(this, SIGNAL(startMeasureMent(Mat)), _workerMeasurement, SLOT(StartMesurement()));
	
	workerMeasurement = new WorkerMeasurement(this);//指明每一个采集线程的父指针
	threadMeasurement = new QThread();
	workerMeasurement->moveToThread(threadMeasurement);
}

void WorkerCCD::StartTimer()
{
	_cameraAVT = new AVTCamera(_workerID, _system);
	_timerId = this->startTimer(100);//设置定时器触发子线程capture
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		_cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = _cameraAVT->CaptureImage();
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		if (_capture == 1)
		{
			_cameraAVT->SaveImage(_mat);
		}
		if (_measurement == 1)
		{
			if (!threadMeasurement->isRunning())
			{
				threadMeasurement->start();
				connect(this, SIGNAL(startMeasurement()), workerMeasurement, SLOT(StartTimer()));
				emit startMeasurement();
			}
		}
		emit sendingMat(_workerID, _mat);
		emit sendingImg(_workerID, _img);
	}
}

WorkerCCD::~WorkerCCD()
{
	this->killTimer(_timerId);
	delete _cameraAVT;
}