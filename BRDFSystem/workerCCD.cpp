#include "workerCCD.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_exposureTime = 50;
	//_saveName = 0;
	_capture = 0;
	_measureFlag = 0;
	_mutex.lock();//防止9台相机抢占Vimba系统别名
	cameraAVT = new AVTCamera(_workerID, _system);
	_mutex.unlock();

	//线程嵌套 moveToThread: Cannot move objects with a parent
	//_mutex.lock();//防止9台相机抢占Vimba系统别名
	//workerMeasurement = new WorkerMeasurement(_workerID, _system);//指明每一个采集线程的父指针
	//_mutex.unlock();
	//threadMeasurement = new QThread();
	//workerMeasurement->moveToThread(threadMeasurement);
	//Sleep(500);//等待相机初始化
	//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
}

void WorkerCCD::StartTimer(/*int measureFlag*/)
{
	//_measureFlag = measureFlag;
	_timerId = this->startTimer(1000);//设置定时器触发子线程capture  单位毫秒
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);

		//if (_capture == 1)
		//{
		//	cameraAVT->SaveImage(_mat);
		//}

		//if (_measureFlag != 0)
		//{
			//if (!threadMeasurement->isRunning())
			//{
			//	threadMeasurement->start();
			//	connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
			//	emit startMeasurement(_measureFlag);
			//}
			//_measureFlag = 0;
		//}
		
		//emit next();
		emit sendingMat(_workerID, /*_mat*/_img);//给采集线程传递
		emit sendingImg(_workerID, _img);//给界面显示线程传递
	}
}

void WorkerCCD::SetExposureTime()
{
	_exposureTime = AVTCamera::GetExposureTime(_mat);//更新曝光时间
	cameraAVT->CameraSettings(_exposureTime * 1000);
	Sleep(500);//等曝光时间生效
}

//void WorkerCCD::SaveAMat()
//{
//	char saveName[4] = { 0 };
//	sprintf(saveName, "%4d", _saveName);
//	char sPath[200];
//
//	sprintf(sPath, "//%s.bmp", saveName);
//	string path;
//	if (_measureFlag == 1)
//		path = _imageSavingPath1 + "//camera" + to_string(workerID) + sPath;
//	if (_measureFlag == 2)
//		path = _imageSavingPath2 + "//camera" + to_string(workerID) + sPath;
//	if (_measureFlag == 3)
//		path = _imageSavingPath3 + "//camera" + to_string(workerID) + sPath;
//	imwrite(path, _mat);
//	_saveName++;
//
//	return;
//}

WorkerCCD::~WorkerCCD()
{
	this->killTimer(_timerId);
	delete cameraAVT;
	cameraAVT = NULL;

	//delete workerMeasurement;
	//workerMeasurement = NULL;

	//if (threadMeasurement->isFinished())
	//	return;
	//threadMeasurement->quit();
	//threadMeasurement->wait();

	//delete threadMeasurement;
	//threadMeasurement = NULL;
}