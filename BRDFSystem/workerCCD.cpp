#include "workerCCD.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerCCD::WorkerCCD(int workerID, QObject *parent) :
_workerID(workerID),
QObject(parent) 
{
	
	//_saveName = 0;
	_capture = 0;
	_isMasked = 0;
	_hasWhiteTrans = 0;
	_hasAffineTrans = 0;
	_measureFlag = 0;
	_sampleID = 0;
	_isHdr = 0;

	cameraAVT = new AVTCamera(_workerID);
	cameraAVT->IniCamera();
	_exposureTime = 600000;
	cameraAVT->CameraSettings(_exposureTime);//设置初始曝光时间为50ms

	//////读取标定的仿射变换矩阵
	//_fileMutex.lock();
	//FileStorage cvfs("affineTrans.xml", FileStorage::READ);
	////cvfs["cam" + to_string(workerID)] >> _affineTrans;
	//////读取正视角下角点
	//Mat corners(4, 2, CV_8UC1);
	//cvfs["corners-cam0"] >> corners;
	//_fileMutex.unlock();

	//////Point2f pCenter;
	//pCenter.x = (corners.at<double>(0, 0) + corners.at<double>(1, 0) + corners.at<double>(2, 0) + corners.at<double>(3, 0)) / 4.0;
	//pCenter.y = (corners.at<double>(0, 1) + corners.at<double>(1, 1) + corners.at<double>(2, 1) + corners.at<double>(3, 1)) / 4.0;
	//cvfs.release();
	
	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));

	//imageProcess = new ImageProcess();
	//线程嵌套 moveToThread: Cannot move objects with a parent
	//_mutex.lock();//防止9台相机抢占Vimba系统别名
	//workerMeasurement = new WorkerMeasurement(_workerID, _system);//指明每一个采集线程的父指针
	//_mutex.unlock();
	//threadMeasurement = new QThread();
	//workerMeasurement->moveToThread(threadMeasurement);
	//Sleep(500);//等待相机初始化
	//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(StartTimer(int)));

	/*
	//正视角相机先保存标定点
	if (_workerID == 0)
	{
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureAnImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);

		Mat mask = ImageProcess::ReadMask(_workerID, 0);
		Mat dst = ImageProcess::ComputeWhiteArea(mask, _mat);
		referPts = ImageProcess::ComputeKeyPoints(dst);
	}
	*/
}

WorkerCCD::~WorkerCCD()
{
	cameraAVT->IniCamera();
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

void WorkerCCD::StartTimer(int measureFlag)
{
	_measureFlag = measureFlag;
	_timerId = this->startTimer(30);//设置定时器触发子线程capture  单位毫秒
}

void WorkerCCD::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == _timerId)
	{
	
		cameraAVT->GetImageSize(_width, _height);
		_pImageFrame = cameraAVT->CaptureAnImage();
		_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
		//cvtColor(_mat, _mat, CV_RGB2BGR);
		//imshow("test", _mat);
		//waitKey();

		//连续采集
		if (_capture == 1)
		{
			string capturePath = ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();
			cameraAVT->SaveImages(_mat, capturePath);
		}
		/*
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
		*/

		_img = QImage(_pImageFrame, _width, _height, QImage::Format_RGB888);
		//_img = CvMat2QImage(_mat);
		emit sendingImg(_workerID, _img);//给界面显示线程传递
	}
}

void WorkerCCD::GetMaterialName(QString materialName)
{
	_materialName = materialName.toStdString();
}

void WorkerCCD::Grab(int sID, int iID)
{
	_sampleID = sID;
	uchar* pImageFrame = NULL;
	Mat mat;
	if (_measureFlag != 3)
	{
		//cameraAVT->GetImageSize(_width, _height);
		pImageFrame = cameraAVT->CaptureAnImage();
		mat = Mat(_height, _width, CV_8UC3, pImageFrame);
		cvtColor(mat, mat, CV_RGB2BGR);
		//imshow("test", mat);
		//waitKey();
				
		_ispMutex.lock();
		//
		//////1.1、自动曝光
		//Mat temp = mat.clone();
		//ImageProcess::CutSampleArea(temp, pCenter);//取中间一小块区域作为参考，整幅图做转换耗时较长
		//cvtColor(temp, _gray, CV_BGR2GRAY);//转灰度图函数提到外面保存下来
		//if (!ImageProcess::IsProperExposure(_gray))
		//{
		//	_exposureTime = ImageProcess::ComputeExposureTime(_gray, _exposureTime);//更新曝光时间
		//	cameraAVT->CameraSettings(_exposureTime);
		//	Sleep(500);//等曝光时间生效

		//	//重新拍摄
		//	//cameraAVT->GetImageSize(_width, _height);
		//	pImageFrame = cameraAVT->CaptureAnImage();
		//	mat = Mat(_height, _width, CV_8UC3, _pImageFrame);

		//	cvtColor(mat, _gray, CV_BGR2GRAY);//改尺寸图片转换比较耗时，统一处理
		//}
		////1.2、HDR采集（opencv2版本下待完善）
		///*
		//if (_isHdr)
		//{
		//	map<double, Mat> imgs = cameraAVT->CaptureByDifTimes(_exposureTime, mat);
		//	Mat hdr = ImageProcess::ContributeHDR(imgs, 0);//暂视为单色区域整体CRF曲线为线性
		//}
		//*/

		//////2、坏点校正（待测试，这一步厂家在demosiac时应该做过，暂可不用）
		////ImageProcess::DeadPixelCorrection(mat, _deadPos);

		//////3、相机内参畸变矫正（同上，暂可不用）
		////ImageProcess::DistortionCorrection(mat, _cameraMatrix, _distCoeffs);

		//////4、采集图像正视角校正
		////高度角仿射变换
		//ImageProcess::AngelCalibration(mat, _affineTrans);
		//方位角角度旋转
		//ImageProcess::RotateImageXoY(mat, sID, { 600, 800 });
		/*
		////7.26日前考虑的定点方案，但低纬度相机下视野非常小，难以计算mask
		//vector<Point2f> pts = ImageProcess::ComputeKeyPoints(dst);
		//if (_hasAffineTrans == 0)
		//{
		//_affineTrans = ImageProcess::ComputeAffineTrans(pts, referPts);
		//_hasAffineTrans = 1;
		//}
		//ImageProcess::AngelCalibration(mat, _affineTrans);
		//mat = ImageProcess::ComputeSampleArea(pts, mat);
		//Point2f center = ImageProcess::ComputeCenterPoint(pts);
		//ImageProcess::RotateImageXoY(mat, sID, center);
		////第一个相机由于安装位置限制，需要额外旋转45°
		//if (_workerID == 0)
		//	ImageProcess::RotateImageXoY(mat, 4.5, center);
		//*/

		//////5、根据标定Mask计算白点标记区域，进行白平衡校正	
		//Mat mask = ImageProcess::ReadMask(_workerID, sID);//统一转换到正视角下后再做mask（仅白平衡用），减少mask的存取数量
		//Mat dst = ImageProcess::ComputeWhiteArea(mask, mat);//获取白色标记区域
		//采集时根据第一帧获取白平衡矩阵，不必先进行标定
		//if (_hasWhiteTrans == 0)
		//{
		//	_whiteTrans = ImageProcess::ComputeWhiteTrans(dst);
		//	_hasWhiteTrans = 1;
		//}
		//ImageProcess::WhiteBalance(mat, _whiteTrans);

		//////6、截取材质中心高质量区域（这一步视采集方式来定，样品多角度采集需要框取整个材质区域）
		//ImageProcess::ComputeSampleArea(mat, pCorners);//框取材质样品区域（svbrdf要求像素精度）
		ImageProcess::CutSampleArea(mat, {600,800});//框取光照中心区域有效值

		_ispMutex.unlock();	
	}
	else
	{
		cameraAVT->GetImageSize(_width, _height);
		pImageFrame = cameraAVT->CaptureAnImage();
		mat = Mat(_height, _width, CV_8UC3, pImageFrame);
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
	this->killTimer(_timerId);//销毁计时器写在析构函数里会报错
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
