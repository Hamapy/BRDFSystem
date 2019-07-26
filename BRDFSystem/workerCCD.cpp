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
	_isMasked = 0;
	_hasWhiteTrans = 0;
	_measureFlag = 0;
	_sampleID = 0;
	_mutex.lock();//��ֹ9̨�����ռVimbaϵͳ����
	cameraAVT = new AVTCamera(_workerID, _system);
	cameraAVT->CameraSettings(30000);//���ó�ʼ�ع�ʱ��Ϊ50ms
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

void WorkerCCD::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	_timerId = this->startTimer(60);//���ö�ʱ���������߳�capture  ��λ����
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
	
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureAnImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
			
		//�����ɼ�
		if (_capture == 1)
		{
			string capturePath = ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();
			cameraAVT->SaveImages(_mat, capturePath);
		}
		if (_isMasked == 1)
		{
			Mat mask = ImageProcess::ReadMask(_workerID, _sampleID);
			Mat masked = ImageProcess::ComputeWhiteArea(mask, _mat);
			if (_hasWhiteTrans == 0)
			{
				_whiteTrans = ImageProcess::ComputeWhiteTrans(masked);
				_hasWhiteTrans = 1;
			}
			_mat = ImageProcess::WhiteBalance(masked, _whiteTrans);
		}

		//_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		_img = CvMat2QImage(_mat);
		emit sendingImg(_workerID, _img);//��������ʾ�̴߳���
	}
}

//void WorkerCCD::SetExposureTime()
//{
//	_exposureTime = AVTCamera::GetExposureTime(_mat);//�����ع�ʱ��
//	cameraAVT->CameraSettings(_exposureTime * 1000);
//	Sleep(500);//���ع�ʱ����Ч
//}

void WorkerCCD::GetMaterialName(QString materialName)
{
	_materialName = materialName.toStdString();
}

void WorkerCCD::Grab(int sID, int iID)
{
	_sampleID = sID;
	uchar* pImageFrame = NULL;
	Mat mat = Mat::zeros(_height, _width, CV_8UC3);
	if (_measureFlag != 3)
	{
		cameraAVT->GetImageSize(_width, _height);
		pImageFrame = cameraAVT->CaptureAnImage();
		mat = Mat(_height, _width, CV_8UC3, pImageFrame);

		if (ImageProcess::IsProperExposure(mat))
		{
			_exposureTime = ImageProcess::ComputeExposureTime(_mat, 50);//�����ع�ʱ��
			cameraAVT->CameraSettings(_exposureTime * 1000);
			Sleep(500);//���ع�ʱ����Ч

			//��������
			cameraAVT->GetImageSize(_width, _height);
			pImageFrame = cameraAVT->CaptureAnImage();
			mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		}
	}
	else
	{
		
		cameraAVT->GetImageSize(_width, _height);
		pImageFrame = cameraAVT->CaptureAnImage();
		mat = Mat(_height, _width, CV_8UC3, pImageFrame);
		/*
		map<double, Mat> imgs = cameraAVT->CaptureByDifTimes(_exposureTime, mat);
		Mat hdr = ImageProcess::ContributeHDR(imgs, 0);//�ݶ�����CRF
		*/
	}

	string path;
	if (_measureFlag == 1)
		path = _imageSavingPath1 + _materialName + "\\";
	if (_measureFlag == 2)
		path = _imageSavingPath2 + _materialName + "\\";
	if (_measureFlag == 3)
		path = _imageSavingPath3;

	cameraAVT->SaveAnImage(mat, path, _workerID, sID, iID, _measureFlag);

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

QImage WorkerCCD::CvMat2QImage(const Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		//qDebug() << "CV_8UC4";
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		//qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}