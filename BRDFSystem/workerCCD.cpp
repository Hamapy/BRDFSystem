#include "workerCCD.h"

////////////////////////////////AVT����߳��ඨ��////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_capture = 0;
	_measurement = 0;
	
	//�߳�Ƕ�ף�
	workerMeasurement = new WorkerMeasurement(this);//ָ��ÿһ���ɼ��̵߳ĸ�ָ��
	threadMeasurement = new QThread();
	workerMeasurement->moveToThread(threadMeasurement);

	connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
}

void WorkerCCD::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	_cameraAVT = new AVTCamera(_workerID, _system);
	_timerId = this->startTimer(500);//���ö�ʱ���������߳�capture
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		_cameraAVT->GetImageSize(_width, _height);
		_cameraAVT->CameraSettings(workerMeasurement->_exposureTime * 1000);
		_pImageFrame = _cameraAVT->CaptureImage();
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		
		_mutex.lock();
		if (_capture == 1)
		{
			_cameraAVT->SaveImage(_mat);
		}
		if (_measurement == 1)
		{
			if (!threadMeasurement->isRunning())
			{
				threadMeasurement->start();
				//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
				emit startMeasurement(_measureFlag);
				_measurement = 0;
			}
		}
		if (_measurement == 2)
		{
			if (!threadMeasurement->isRunning())
			{
				threadMeasurement->start();
				//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
				emit startMeasurement(_measureFlag);
				_measurement = 0;
			}
		}
		_mutex.unlock();
		
		emit sendingMat(_workerID, _mat);
		emit sendingImg(_workerID, _img);
	}
}

WorkerCCD::~WorkerCCD()
{
	this->killTimer(_timerId);
	delete _cameraAVT;

	if (threadMeasurement->isFinished())
		return;
	threadMeasurement->quit();
	threadMeasurement->wait();

	delete workerMeasurement;
	workerMeasurement = NULL;

	delete threadMeasurement;
	threadMeasurement = NULL;
}