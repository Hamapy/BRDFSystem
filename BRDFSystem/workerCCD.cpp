#include "workerCCD.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_capture = 0;
	_measureFlag = 0;
	
	//线程嵌套 moveToThread: Cannot move objects with a parent
	workerMeasurement = new WorkerMeasurement(_workerID, _system);//指明每一个采集线程的父指针
	threadMeasurement = new QThread();
	workerMeasurement->moveToThread(threadMeasurement);

	connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
}

void WorkerCCD::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	//_cameraAVT = new AVTCamera(_workerID, _system);
	_timerId = this->startTimer(100);//设置定时器触发子线程capture  单位毫秒
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		this->workerMeasurement->cameraAVT->GetImageSize(_width, _height);
		this->workerMeasurement->cameraAVT->CameraSettings(workerMeasurement->_exposureTime * 1000);
		_pImageFrame = this->workerMeasurement->cameraAVT->CaptureImage();
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		
		_mutex.lock();
		if (_capture == 1)
		{
			this->workerMeasurement->cameraAVT->SaveImage(_mat);
		}

		if (_measureFlag != 0)
		{
			if (!threadMeasurement->isRunning())
			{
				threadMeasurement->start();
				connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
				emit startMeasurement(_measureFlag);
			}
			_measureFlag = 0;
		}

		_mutex.unlock();
		
		emit sendingMat(_workerID, _mat);
		emit sendingImg(_workerID, _img);
	}
}

WorkerCCD::~WorkerCCD()
{
	this->killTimer(_timerId);
	//delete _cameraAVT;
	//_cameraAVT = NULL;

	delete workerMeasurement;
	workerMeasurement = NULL;

	//if (threadMeasurement->isFinished())
	//	return;
	//threadMeasurement->quit();
	//threadMeasurement->wait();

	//delete threadMeasurement;
	//threadMeasurement = NULL;
}