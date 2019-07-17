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
	cameraAVT->CameraSettings(30000);//设置初始曝光时间为50ms
	_mutex.unlock();

	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));

	//线程嵌套 moveToThread: Cannot move objects with a parent
	//_mutex.lock();//防止9台相机抢占Vimba系统别名
	//workerMeasurement = new WorkerMeasurement(_workerID, _system);//指明每一个采集线程的父指针
	//_mutex.unlock();
	//threadMeasurement = new QThread();
	//workerMeasurement->moveToThread(threadMeasurement);
	//Sleep(500);//等待相机初始化
	//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
}

void WorkerCCD::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	_timerId = this->startTimer(60);//设置定时器触发子线程capture  单位毫秒
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureAnImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		//_matWB = cameraAVT->WhiteBalance(_mat);
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		//_img = QImage((const unsigned char*)(_matWB.data), _matWB.cols, _matWB.rows, QImage::Format_RGB888);

		//连续采集
		if (_capture == 1)
		{
			string capturePath = ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();
			cameraAVT->SaveImages(_mat, capturePath);
		}

		emit sendingImg(_workerID, _img);//给界面显示线程传递
	}
}

//void WorkerCCD::SetExposureTime()
//{
//	_exposureTime = AVTCamera::GetExposureTime(_mat);//更新曝光时间
//	cameraAVT->CameraSettings(_exposureTime * 1000);
//	Sleep(500);//等曝光时间生效
//}

void WorkerCCD::GetMaterialName(QString materialName)
{
	_materialName = materialName.toStdString();
}

void WorkerCCD::Grab(int sID, int iID)
{
	cameraAVT->GetImageSize(_width, _height);
	uchar* pImageFrame = cameraAVT->CaptureAnImage();
	Mat mat = Mat(_height, _width, CV_8UC3, _pImageFrame);

	if (ImageProcess::IsOverExposure(mat))
	{
		_exposureTime = ImageProcess::ComputeExposureTime(_mat);//更新曝光时间
		cameraAVT->CameraSettings(_exposureTime * 1000);
		Sleep(500);//等曝光时间生效

		//重新拍摄
		cameraAVT->GetImageSize(_width, _height);
		pImageFrame = cameraAVT->CaptureAnImage();
		mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
	}
	
	//extern string _materialName;
	string path;
	if (_measureFlag == 1)
		path = _imageSavingPath1 + _materialName;
	if (_measureFlag == 2)
		path = _imageSavingPath2 + _materialName;
	//if (_measureFlag == 3)
	//	path = _imageSavingPath3;

	cameraAVT->SaveAnImage(_mat, path, _workerID, sID, iID);

	emit grabDone(_workerID);
}

void WorkerCCD::CloseWorker()
{
	this->killTimer(_timerId);//销毁计时器写在析构函数里会报错
}

WorkerCCD::~WorkerCCD()
{
	//this->killTimer(_timerId);//销毁计时器写在析构函数里会报错
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