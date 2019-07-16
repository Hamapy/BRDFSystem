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
void AVTCamera::CameraSettings(float exposureTime, CameraParameters* cameraParameters)
{
	_camera->GetFeatureByName("AcquisitionFrameCount", _feature);
	_feature->SetValue(cameraParameters->acquisitionFrameCount);
	_camera->GetFeatureByName("AcquisitionFrameRate", _feature);
	_feature->SetValue(cameraParameters->acquisitionFrameRate);
	_camera->GetFeatureByName("AutofunctionAOIEnable", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIEnable);
	_camera->GetFeatureByName("AutofunctionAOIHeight", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIHeight);
	_camera->GetFeatureByName("AutofunctionAOIOffsetX", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIOffsetX);
	_camera->GetFeatureByName("AutofunctionAOIOffsetY", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIOffsetY);
	_camera->GetFeatureByName("AutofunctionAOIShowArea", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIShowArea);
	_camera->GetFeatureByName("AutofunctionAOIWidth", _feature);
	_feature->SetValue(cameraParameters->autofunctionAOIWidth);
	_camera->GetFeatureByName("AutofunctionTargetIntensity", _feature);
	_feature->SetValue(cameraParameters->autofunctionTargetIntensity);
	_camera->GetFeatureByName("BlackLevel", _feature);
	_feature->SetValue(cameraParameters->blackLevel);
	_camera->GetFeatureByName("ColorTransformationEnable", _feature);
	_feature->SetValue(cameraParameters->colorTransformationEnable);
	_camera->GetFeatureByName("ExposureTime", _feature);
	_feature->SetValue(cameraParameters->exposureTime);
	_camera->GetFeatureByName("Gain", _feature);
	_feature->SetValue(cameraParameters->gain);
	_camera->GetFeatureByName("Gamma", _feature);
	_feature->SetValue(cameraParameters->gamma);
	_camera->GetFeatureByName("Height", _feature);
	_feature->SetValue(cameraParameters->height);
	_camera->GetFeatureByName("Width", _feature);
	_feature->SetValue(cameraParameters->width);
	_camera->GetFeatureByName("Hue", _feature);
	_feature->SetValue(cameraParameters->hue);
	_camera->GetFeatureByName("Saturation", _feature);
	_feature->SetValue(cameraParameters->saturation);
	_camera->GetFeatureByName("PixelFormat", _feature);
	_feature->SetValue(cameraParameters->pixelFormat);
	_camera->GetFeatureByName("ExposureAuto", _feature);
	_feature->SetValue(cameraParameters->exposureAuto);
	_camera->GetFeatureByName("GainAuto ", _feature);
	_feature->SetValue(cameraParameters->gainAuto);

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
// 函数：
// 描述：
// 输入：Null
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::CaptureImages(Mat& captureMat, string imageSavingPath)
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
// 函数：SaveAnImage
// 描述：采集一帧RGB图像
// 输入：Null
// 输出：一帧图像
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool AVTCamera::SaveAnImage(Mat mat, string path, int cameraID, int sampleID, int illuminantID)
{
	//000命名方式
	char saveName[4] = { 0 };
	sprintf(saveName, "%4d", _saveName);
	char name[200];
	sprintf(name, "//%s.bmp", saveName);

	//out_相机角度-材质台角度_in_光源θ-光源σ命名方式
	//光源36 36 32 28 24 18 12 6 4
	/*
	if (_saveName)
	{

	}
	char name[200];
	sprintf(name, "out_%d-%d_in%d-%d.bmp", saveName);
	*/
	string spath = path + name;
	bool isSaved = imwrite(spath, mat);
	//mat.save(QString::fromStdString(path), "BMP", 100);
	_saveName++;
	//emit something() //告诉界面线程该角度下的图像已采集，修改界面的Qlabel

	return isSaved;
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
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<float> AVTCamera::GetWhiteBalanceTrans(vector<Mat> mats)
{
	Mat src = AverageImage(mats);

#define WHITE_NUM 500
	//暂时取全图亮度最高的一定数量像素点均值作为白点
	//...
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	Mat B = srcB.clone();
	Mat G = srcG.clone();
	Mat R = srcR.clone();

	//把Mat矩阵拉成一维数组
	unsigned char *arrayB = new unsigned char[srcB.rows*srcB.cols];
	if (srcB.isContinuous())
		arrayB = B.data;//防止修改原图像素顺序
	unsigned char *arrayG = new unsigned char[srcG.rows*srcG.cols];
	if (srcG.isContinuous())
		arrayG = G.data;
	unsigned char *arrayR = new unsigned char[srcR.rows*srcR.cols];
	if (srcR.isContinuous())
		arrayR = R.data;


	//计算亮度值最大像素点集合平均亮度
	sort(arrayB, arrayB + srcB.rows*srcB.cols);
	reverse(arrayB, arrayB + srcB.rows*srcB.cols);
	//sort(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	//reverse(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	int sumB = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumB += (int)arrayB[i];
	}
	uchar aveB = (float)sumB / (float)WHITE_NUM;

	sort(arrayG, arrayG + srcG.rows*srcG.cols);
	reverse(arrayG, arrayG + srcG.rows*srcG.cols);
	int sumG = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumG += (int)arrayG[i];
	}
	uchar aveG = (float)sumG / (float)WHITE_NUM;

	sort(arrayR, arrayR + srcR.rows*srcR.cols);
	reverse(arrayR, arrayR + srcR.rows*srcR.cols);
	int sumR = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumR += (int)arrayR[i];
	}
	uchar aveR = (float)sumR / (float)WHITE_NUM;

	float transB = 255 / (float)aveB;
	float transG = 255 / (float)aveG;
	float transR = 255 / (float)aveR;

	vector<float> trans;
	trans.push_back(transB);
	trans.push_back(transG);
	trans.push_back(transR);

	return trans;

#undef WHITE_NUM
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat AVTCamera::WhiteBalance(Mat src)
{
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	float transB = _trans[0];
	float transG = _trans[1];
	float transR = _trans[2];

	//对每个点将像素拉到[0,255]之间
	//针对超出1的像素，重新归一化
	vector<vector<float>> fBGR(height,vector<float>(width,0));
	float max = 0;
	//float s = 255 / (float)aveB;
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			fBGR[i][j] = srcB.at<uchar>(i, j) / 255.00 * transB;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			srcB.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveG;
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			fBGR[i][j] = srcG.at<uchar>(i, j) / 255.00 * transG;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			srcG.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveR;
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			fBGR[i][j] = srcR.at<uchar>(i, j) / 255.00 * transR;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			srcR.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	Mat dst(src.rows, src.cols, CV_8UC3, Scalar::all(0));
	merge({ srcB, srcG, srcR }, dst);

	return dst;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：多幅图像求平均
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat AVTCamera::AverageImage(vector<Mat> mats)
{
	Mat aver = Mat::zeros(mats[0].size(), CV_32FC3);
	for (int i = 0; i < mats.size(); i++)
	{
		accumulate(mats[i], aver);
	}
	aver /= mats.size();
	aver.convertTo(aver, CV_8UC3);

	return aver;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<Mat> AVTCamera::ReadImages(string path)
{
	vector<Mat> mats;
	string imgPattern = "//*.jpg";
	vector<cv::String> imgFiles;
	imgPattern = path + imgPattern ;
	glob(imgPattern, imgFiles);
	for (int i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		mats.push_back(mat);
	}

	return mats;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void AVTCamera::DeadPixelDetect(Mat src, int maxNum)
{
#define LAMDA 5.0  //亮度值 > mean + LAMDA * sigma 被视为坏点
#define MASK_SEL 1 
#define MASK_UNSEL 0 //是否已选像素

	//对均匀图像可暂时不用滑动窗口
	/*
	int count = 0;  //记录滑动窗口的数目
	int x_step = cvCeil(x_percent*wndSize.width);
	int y_step = cvCeil(y_percent*wndSize.height);
	//利用窗口对图像进行遍历
	for (int i = 0; i < src.cols - wndSize.width; i += y_step)
	{
		for (int j = 0; j < src.rows - wndSize.height; j += x_step)
		{
			Rect roi(Point(j, i), wndSize);
			Mat ROI = src(roi);
			wnd.push_back(ROI);
			count++;
		}
	}
	*/

	//坏点不受通道影响，直接看灰度图就好
	/*
	//求出图像三通道均值和方差
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float meanB = mean.val[0];
	float stddevB = stddev.val[0];
	float meanG = mean.val[1];
	float stddevG = stddev.val[1];
	float meanR = mean.val[2];
	float stddevR = stddev.val[2];

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);
	int ch = src.channels;
	*/

	PIXEL pSel;
	Mat gray;
	Mat zeroMat = Mat::zeros(gray.size(), CV_8UC1);
	cvtColor(src, gray, CV_BGR2GRAY);
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float m = mean.val[0];
	float sigma = stddev.val[0];

	//通过循环找最大值的方法，确定坏点
	for (int k = 0; k < maxNum; k++)
	{
		int maxVal = 0;
		for (int i = 0; i < src.rows; i++)
		{
			uchar* data = gray.ptr<uchar>(i);
			for (int j = 0; j < src.cols; j++)
			{
				if (IsSelected(zeroMat, i, j) == MASK_UNSEL)
				{
					if (data[j] > maxVal)
					{
						maxVal = data[j];
						pSel.i = i;
						pSel.j = j;
					}
				}
			}
		}

		if (maxVal > m + LAMDA * sigma)  // 若该点满足坏点条件
		{
			int deadPos[2] = {pSel.i, pSel.j};
			_deadPixelPos.push_back(deadPos);
			Select(zeroMat, pSel.i, pSel.j);
		}
		else
		{
			break;
		}
	}

	return;
#undef LAMDA
#undef MASK_SEL
#undef MASK_UNSEL
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
bool AVTCamera::IsSelected(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	if (data[j] == 1)
		return true;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void AVTCamera::Select(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	data[j] = 1;

	return;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
bool AVTCamera::IsOverExposure(Mat src)
{
	//通过计算直方图判断采集图像是否过曝或过暗

	return 0;
}