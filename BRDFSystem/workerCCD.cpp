#include "workerCCD.h"

////////////////////////////////AVT����߳��ඨ��////////////////////////////
WorkerCCD::WorkerCCD(int workerID, VimbaSystem&	system, QObject *parent) :
_workerID(workerID),
_system(system),
QObject(parent) 
{
	_exposureTime = 50;
	//_saveName = 0;
	_capture = 0;
	_measureFlag = 0;
	_mutex.lock();//��ֹ9̨�����ռVimbaϵͳ����
	cameraAVT = new AVTCamera(_workerID, _system);
	_mutex.unlock();

	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));

	//�߳�Ƕ�� moveToThread: Cannot move objects with a parent
	//_mutex.lock();//��ֹ9̨�����ռVimbaϵͳ����
	//workerMeasurement = new WorkerMeasurement(_workerID, _system);//ָ��ÿһ���ɼ��̵߳ĸ�ָ��
	//_mutex.unlock();
	//threadMeasurement = new QThread();
	//workerMeasurement->moveToThread(threadMeasurement);
	//Sleep(500);//�ȴ������ʼ��
	//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));
}

void WorkerCCD::StartTimer(/*int measureFlag*/)
{
	//_measureFlag = measureFlag;
	_timerId = this->startTimer(1000);//���ö�ʱ���������߳�capture  ��λ����
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		//_matWB = cameraAVT->WhiteBalance(_mat);
		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		//_img = QImage((const unsigned char*)(_matWB.data), _matWB.cols, _matWB.rows, QImage::Format_RGB888);

		//�����ɼ�
		if (_capture == 1)
			cameraAVT->CaptureImages(_mat, _imageSavingPath3);

		emit sendingMat(_workerID, /*_mat*/_img);//���ɼ��̴߳���
		emit sendingImg(_workerID, _img);//��������ʾ�̴߳���
	}
}

void WorkerCCD::SetExposureTime()
{
	_exposureTime = AVTCamera::GetExposureTime(_mat);//�����ع�ʱ��
	cameraAVT->CameraSettings(_exposureTime * 1000);
	Sleep(500);//���ع�ʱ����Ч
}

void WorkerCCD::GetMaterialName(QString materialName)
{
	_materialName = materialName.toStdString();
}

void WorkerCCD::Grab()
{
	//SetExposureTime();//�����ع�ʱ��
	string path;
	if (_measureFlag == 1)
		path = _imageSavingPath1 + _materialName + "//camera" + to_string(_workerID);
	if (_measureFlag == 2)
		path = _imageSavingPath2 + _materialName + "//camera" + to_string(_workerID);
	if (_measureFlag == 3)
		path = _imageSavingPath3 + "//camera" + to_string(_workerID);

	cameraAVT->SaveAnImage(_mat, path, _workerID);

	emit grabDone(_workerID);
}

void WorkerCCD::CloseWorker()
{
	this->killTimer(_timerId);//���ټ�ʱ��д������������ᱨ��
}

WorkerCCD::~WorkerCCD()
{
	//this->killTimer(_timerId);//���ټ�ʱ��д������������ᱨ��
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