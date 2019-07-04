#include "ccd.h"

//静态变量类外初始化
//CameraPtrVector		_cameras = vector<CameraPtr, allocator<CameraPtr>>();
//CameraPtr			_camera = shared_ptr<Camera>();

//VmbUint32_t	AVTCamera::width = 0;
//VmbUint32_t	AVTCamera::height = 0;
//CameraPtrVector AVTCamera::cameras = ;
//FeaturePtr AVTCamera::feature = ;
int AVTCamera::threadID = 0;
//Mat AVTCamera::captureMat = Mat(height, width, CV_8UC3);

AVTCamera::AVTCamera(VimbaSystem& system) :
_system(system)
{
	_saveName = 0;
	IniCamera();
}

AVTCamera::AVTCamera(int cameraID, VimbaSystem& system) :
_cameraID(cameraID),
_system(system)
{
	_saveName = 0;
	IniCamera();
}

AVTCamera::~AVTCamera()
{
	FiniCamera();
}
////////////////////////////////////////////////////////////////////////////
// 函数：IniVimba
// 描述：相机的SDK-Vimba系统初始化
// 输入：Null
// 输出：是否成功完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::IniVimba(VimbaSystem& system)
{
	if (VmbErrorSuccess == system.Startup())
	{
		cout << "相机系统初始化完成" << endl;
		return 1;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：IniCamera
// 描述：相机初始化
// 输入：Null
// 输出：是否成功完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::IniCamera()
{
	if (VmbErrorSuccess == _system.GetCameras(_cameras))
	{
		//cout << "相机初始化完成" << endl;
		_camera = _cameras[_cameraID];

		if (VmbErrorSuccess == _camera->Open(VmbAccessModeFull))//完全读写相机权限
		{
			//相机参数初始化设置
			_camera->GetFeatureByName("PixelFormat", _feature);
			_feature->SetValue("RGB8Packed");//这个像素源格式很重要，否则后续的QImage、QPixmap包括Mat的数据转换都会报错
			_camera->GetFeatureByName("Gain", _feature);
			_feature->SetValue(0);
			_camera->GetFeatureByName("BlackLevel", _feature);
			_feature->SetValue(0);
			_camera->GetFeatureByName("Gamma", _feature);
			_feature->SetValue(0);
			//_camera->GetFeatureByName("Hue", _feature);
			//_feature->SetValue(0);

			string name;
			_camera->GetName(name);
			string ID;
			_camera->GetID(ID);
			cout << "第" << _cameraID << "个相机" << name << " " << ID << " 打开成功" << endl;

			return 1;
		}
	}	
	return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：SetCameraSettings(CameraPtr& camera, double exposureTime, double gain, double blackLevel)
// 描述：设置曝光时间
// 输入：时间：time(us)
// 输出：是否成功执行
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void AVTCamera::CameraSettings(double exposureTime, double gain, double blackLevel)
{
	_camera->GetFeatureByName("ExposureTime", _feature);
	_feature->SetValue(exposureTime);
	_camera->GetFeatureByName("Gain", _feature);
	_feature->SetValue(gain);
	_camera->GetFeatureByName("BlackLevel", _feature);
	_feature->SetValue(blackLevel);
	
	return;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Capture(CameraPtrVector cameras, int cameraID)
// 描述：采集一帧RGB图像
// 输入：Null
// 输出：一帧图像
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
VmbUchar_t* AVTCamera::CaptureImage()
{
	if (VmbErrorSuccess == _camera->AcquireSingleImage(_pFrame, TIMEOUT))
	//cout << "采集一帧图像" << endl;
		
	//pFrame->GetHeight(height);
	//pFrame->GetWidth(width);
	_pFrame->GetImage(_pImage);
	//captureMat = Mat(height, width, CV_8UC3, pImage);

	return _pImage;

	/*
	VmbError_t err = -2;
	//请求图片
	err = VmbFeatureCommandRun(hCamera_, "AcquisitionStart");
	if (err != VmbErrorSuccess)
	{
		return NULL;
	}
	err = VmbCaptureFrameWait(hCamera_, &frame_, nTimeout_);

	return pImage_;
	*/
}
////////////////////////////////////////////////////////////////////////////
// 函数：GetImageSize(int& widthOutput, int& heightOutput)
// 描述：
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::GetImageSize(int& width, int& height)
{
	if (VmbErrorSuccess == _camera->AcquireSingleImage(_pFrame, TIMEOUT))
	{
		_pFrame->GetHeight(_height);
		_pFrame->GetWidth(_width);
		width = _width;
		height = _height;

		return 1;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：SaveImage(Mat& captureMat)
// 描述：采集一帧RGB图像
// 输入：Null
// 输出：一帧图像
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::SaveImage(Mat& captureMat, string imageSavingPath)
{
	if (_saveName < CAPTURE_NUM)
	{
		char saveName[4] = { 0 };
		sprintf(saveName, "%4d", _saveName);
		char sPath[200];
		//strcpy(sPath, imageSavingPath.c_str());
		sprintf(sPath, "//%s.bmp", saveName);
		string path = imageSavingPath + "//camera" + to_string(_cameraID) + sPath;
		bool isSaved = imwrite(path, captureMat);
		_saveName++;

		return isSaved;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：GetExposureTime(Mat mat, float time)
// 描述：计算相机在一定角度光源下的合适曝光时间
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float AVTCamera::GetExposureTime(Mat mat)
{
	Mat gray;
	cvtColor(mat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	float k = 50.00 / ave;//50ms下拍摄图像
	float t = k * 255 * 0.60;

	return t;
}
////////////////////////////////////////////////////////////////////////////
// 函数：AutoExposure(CameraPtr& camera, int cameraID, int num)
// 描述：计算最大曝光时间
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
vector<int> AVTCamera::AutoExposure(int num, FeaturePtr feature)
{
	//随机曝光时间
	vector<int> t;
	int maxExposureTime;
	int properExposureTime;
	int* exposureTimes;
	exposureTimes = new int[num];
	//vector<Mat> captureMat;
	for (int i = 0; i < num; i++)
	{
		exposureTimes[i] = rand() % 3000; //0~MAX-1
		exposureTimes[i] = 55 + exposureTimes[i] * 20000;

		//每组曝光时间下拍摄测试图像
		_camera->GetFeatureByName("ExposureTime", feature);
		feature->SetValue(exposureTimes[i]);
		//captureMat.push_back(CaptureImage(cameraID));
		//裁剪、对齐、计算平均亮度
		//...
	}
	//拟合直线
	//...
	t.push_back(maxExposureTime);
	t.push_back(properExposureTime);
	return t;
}
/*
////////////////////////////////////////////////////////////////////////////
// 函数：CameraThread(CameraThreadInfo threadInfo)
// 描述：
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void AVTCamera::CameraThread(CameraThreadInfo threadInfo)
{
	mutex cMutex, cMutex_2, cMutex_3;
	cMutex.lock();
	cout << "相机" << threadInfo.cameraID << "线程开始工作" << endl;
	cMutex.unlock();

	AVTCamera::OpenCamera(threadInfo.cameraID);

	for (int i = 0; i < NUM; i++)
	{
		AVTCamera::SetCameraSettings(threadInfo.cameras[threadInfo.cameraID], threadInfo.exposureTime, threadInfo.gain, threadInfo.blackLevel);
		Sleep(5000);//等待一段时间让曝光时间生效
		cMutex_2.lock();
		Mat captureMat = AVTCamera::CaptureImage(threadInfo.cameraID);
		cMutex_2.unlock();
		char filePath[200];
		sprintf(filePath, "./images/camera%d/%d.bmp", threadInfo.cameraID, i);
		cout << filePath << " Acquired" << endl;
		imwrite(filePath, captureMat);
	}
	cMutex_3.lock();
	AVTCamera::threadID++;
	cMutex_3.unlock();

	return;
}
////////////////////////////////////////////////////////////////////////////
// 函数：CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath)
// 描述：
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void AVTCamera::CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath, CameraThreadInfo threadInfo)
{
	cameraID = 0;
	thread task[9];
	//采集一帧图像
	for (CameraPtrVector::iterator iter = cameras.begin(); cameras.end() != iter; ++iter)
	{
		//清空存储路径目录
		char path[200];
		sprintf(path, "./images/camera%d", cameraID);
		imageSavingPath = imageSavingPath + path;
		AVTCamera::EmptyFiles(imageSavingPath);

		threadInfo.cameras = cameras;
		threadInfo.cameraID = cameraID;

		task[cameraID] = thread(AVTCamera::CameraThread, threadInfo);
		task[cameraID].detach();//这样主线程结束子线程还在采集,主线程和子线程分离
		//task[cameraID].join();//这样第一创建的子线程就把主线程堵住了

		cameraID++;
	}
	Sleep(1000000);//等待所有子线程运行结束
}
*/
////////////////////////////////////////////////////////////////////////////
// 函数：FiniCamera()
// 描述：相机停止采集、关闭设备、释放资源
// 输入：Null
// 输出：是否正确完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::FiniCamera()
{
	if (VmbErrorSuccess == _camera->Close())
	{
		std::cout << "Camera closed" << std::endl;
		return 1;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：FiniVimba()
// 描述：相机停止采集、关闭设备、释放资源
// 输入：Null
// 输出：是否正确完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::FiniVimba(VimbaSystem& system)
{
	if (VmbErrorSuccess == system.Shutdown())
	{
		return 1;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Fini()
// 描述：相机停止采集、关闭设备、释放资源
// 输入：Null
// 输出：是否正确完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void AVTCamera::CaptureContinuously(int num, int millisecond)
{

}
//////////////////////////////////////////////////////////////////////////////
//// 函数：EmptyFiles(string dirPath)
//// 描述：采集前清空文件夹
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
int AVTCamera::EmptyFiles(string dirPath)
{
	struct _finddata_t fb;   //find the storage structure of the same properties file.
	string path;
	//long    handle;//win10下数据格式不一样
	intptr_t handle;
	int  resultone;
	int   noFile;            // the tag for the system's hidden files

	noFile = 0;
	handle = 0;

	path = dirPath + "/*";

	handle = _findfirst(path.c_str(), &fb);

	//find the first matching file
	if (handle != -1)
	{
		//find next matching file
		while (0 == _findnext(handle, &fb))
		{
			// "." and ".." are not processed
			noFile = strcmp(fb.name, "..");

			if (0 != noFile)
			{
				path.clear();
				path = dirPath + "/" + fb.name;

				//fb.attrib == 16 means folder
				if (fb.attrib == 16)
				{
					EmptyFiles(path);
				}
				else
				{
					//not folder, delete it. if empty folder, using _rmdir istead.
					remove(path.c_str());
				}
			}
		}
		// close the folder and delete it only if it is closed. For standard c, using closedir instead(findclose -> closedir).
		// when Handle is created, it should be closed at last.  
		_findclose(handle);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//// 函数：EmptyFiles(string dirPath)
//// 描述：采集前清空文件夹
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat AVTCamera::WhiteBalance(Mat src, Rect wBlock)
{
	Mat roi(src, Rect(wBlock.x, wBlock.y, wBlock.width, wBlock.height));
	Mat rR(roi.rows, roi.cols, CV_8UC1, Scalar::all(0));
	Mat rG(roi.rows, roi.cols, CV_8UC1, Scalar::all(0));
	Mat rB(roi.rows, roi.cols, CV_8UC1, Scalar::all(0));
	uchar *pRoi, *prR, *prG, *prB;
	int sumB = 0;
	int sumG = 0;
	int sumR = 0;
	int i, j;
	int amount = 0;
	for (i = 0; i < roi.rows; i++)
	{
		pRoi = roi.ptr<uchar>(i);
		prR = rR.ptr<uchar>(i);
		prG = rG.ptr<uchar>(i);
		prB = rB.ptr<uchar>(i);
		for (j = 0; j < roi.cols; j++)
		{
			//在Mat中是按照BGR的顺序存储的          
			prB[j] = pRoi[j*roi.channels()];
			prG[j] = pRoi[j*roi.channels() + 1];
			prR[j] = pRoi[j*roi.channels() + 2];
			sumB += prB[j];
			sumG += prG[j];
			sumR += prR[j];
			amount++;
		}
	}
	double avgB = (double)sumB / amount;
	double avgG = (double)sumG / amount;
	double avgR = (double)sumR / amount;

	//对每个点将像素量化到[0,255]之间
	uchar blue, green, red;
	double maxVal;
	minMaxLoc(roi, NULL, &maxVal, NULL, NULL);
	uchar *pSrc, *psR, *psG, *psB, *pDst;
	Mat sR(src.rows, src.cols, CV_8UC1, Scalar::all(0));
	Mat sG(src.rows, src.cols, CV_8UC1, Scalar::all(0));
	Mat sB(src.rows, src.cols, CV_8UC1, Scalar::all(0));
	Mat dst(src.rows, src.cols, CV_8UC3, Scalar::all(0));
	for (i = 0; i < src.rows; i++)
	{
		pSrc = src.ptr<uchar>(i);
		pDst = dst.ptr<uchar>(i);
		psR = sR.ptr<uchar>(i);
		psG = sG.ptr<uchar>(i);
		psB = sB.ptr<uchar>(i);
		for (j = 0; j < src.cols; j++)
		{
			psB[j] = pSrc[j*src.channels()];
			psG[j] = pSrc[j*src.channels() + 1];
			psR[j] = pSrc[j*src.channels() + 2];

			if (psB[j] > avgB) blue = maxVal;
			else blue = (double)psB[j] * maxVal / avgB;

			if (psG[j] > avgG) green = maxVal;
			else green = (double)psG[j] * maxVal / avgG;

			if (psR[j] > avgR) red = maxVal;
			else red = (double)psR[j] * maxVal / avgR;

			////if (red > 255) red = 255;
			////else if (red < 0) red = 0;
			////if (green > 255) green = 255;
			////else if (green < 0) green = 0;
			////if (blue > 255)	blue = 255;
			////else if (blue < 0) blue = 0;

			////if (red > 255 || green > 255 || blue > 255)
			////{
			////	red = 255;
			////	green = 255;
			////	blue = 255;
			////}

			//if (red > 255)
			//{
			//	blue = (double)blue * red / 255;
			//	green = (double)green * red / 255;
			//	if (blue > 255 || green > 255)
			//	{
			//		blue = 255;
			//		green = 255;
			//	}
			//	red = 255;
			//}

			//if (blue > 255)
			//{
			//	red = (double)red * blue / 255;
			//	green = (double)green * blue / 255;
			//	if (red > 255 || green > 255)
			//	{
			//		red = 255;
			//		green = 255;
			//	}
			//	blue = 255;
			//}

			//if (green > 255)
			//{
			//	blue = (double)blue * green / 255;
			//	red = (double)red * green / 255;
			//	if (blue > 255 || red > 255)
			//	{
			//		blue = 255;
			//		red = 255;
			//	}
			//	green = 255;
			//}

			pDst[j*src.channels()] = blue;
			pDst[j*src.channels() + 1] = green;
			pDst[j*src.channels() + 2] = red;
		}
	}

	return dst;
}

//////////////////////////////////////////////////////////////////////////////
//// 函数：EmptyFiles(string dirPath)
//// 描述：采集前清空文件夹
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
int AVTCamera::WihteAreaDetection(Mat src, Rect wBlock)
{
	Mat img;
	cvtColor(src, img, CV_BGR2GRAY);
	Mat roi(img, Rect(wBlock.x, wBlock.y, wBlock.width, wBlock.height));
	threshold(roi, roi, 180, 255, CV_THRESH_BINARY);//灰度变二值，阈值180，最大值255
	int counter = 0;
	Mat_<uchar>::iterator it = roi.begin<uchar>();
	Mat_<uchar>::iterator itend = roi.end<uchar>();	//定义迭代器访问像素点  
	for (; it != itend; ++it)
	{
		if ((*it)>0)
			counter += 1;
	}
	int num1 = wBlock.width*wBlock.height;
	int num2 = num1 * 95.00 / 100.00;
	if (counter <= num1 && counter > num2)//Rect框内像素点数，考虑容许误差90%
	{
		cout << "小白块位置正常" << endl;
		system("pause");
	}
	else
	{
		cout << "小白块位置偏移" << endl;
		system("pause");
	}

	return 0;
}