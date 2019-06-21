#include "workerCCD.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_capture = 0;
	_measurement = 0;
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
		//_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		//_img = QImage(_mat.data, _mat.cols, _mat.rows, static_cast<int>(_mat.step), QImage::Format_RGB888);
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		if (_capture == 1)
		{
			_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
			_cameraAVT->SaveImage(_mat);
		}
		emit sendingImg(_img);
	}
}

WorkerCCD::~WorkerCCD()
{
	this->killTimer(_timerId);
	delete _cameraAVT;
}