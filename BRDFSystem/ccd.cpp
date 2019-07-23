#include "ccd.h"

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
// 函数：CameraSettings
// 描述：
// 输入：
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
	//_feature->SetValue(cameraParameters->exposureTime);
	_feature->SetValue(exposureTime);
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
VmbUchar_t* AVTCamera::CaptureAnImage()
{
	if (VmbErrorSuccess == _camera->AcquireSingleImage(_pFrame, TIMEOUT))
	{
		//pFrame->GetHeight(height);
		//pFrame->GetWidth(width);
		_pFrame->GetImage(_pImage);
	}
	
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
bool AVTCamera::SaveImages(Mat& captureMat, string imageSavingPath)
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
bool AVTCamera::SaveAnImage(Mat mat, string path, int cameraID, int sampleID, int illuminantID, int measureFlag)
{

	char name[200];
	double outTheta;
	//出射角Theta
	if (cameraID == 0)  outTheta = 0.0;
	else   outTheta = 5.0 + cameraID * 10.0;

	//出射角Phi
	double outPhi = sampleID * 30.0;
		
	double inTheta, inPhi;
	int i;

	//光源每层数量分布36 36 32 28 24 18 12 6 4
	//光源排布顺序
	int illuminant[196] = { 0, 1, 2, 3, 25, 26, 27, 28, 29, 50, 51, 52, 53, 75, 76, 77, 78, 79, 100, 101, 102, 103, 125, 126, 127, 128, 129, 150, 151, 152, 153, 175, 176, 177, 178, 179, \
		4, 5, 6, 7, 8, 30, 31, 32, 33, 54, 55, 56, 57, 58, 80, 81, 82, 83, 104, 105, 106, 107, 108, 130, 131, 132, 133, 154, 155, 156, 157, 158, 180, 181, 182, 183, \
		9, 10, 11, 12, 34, 35, 36, 37, 59, 60, 61, 62, 84, 85, 86, 87, 109, 110, 111, 112, 134, 135, 136, 137, 159, 160, 161, 162, 184, 185, 186, 187, \
		13, 14, 15, 38, 39, 40, 41, 63, 64, 65, 88, 89, 90, 91, 113, 114, 115, 138, 139, 140, 141, 163, 164, 165, 188, 189, 190, 191, \
		16, 17, 18, 42, 43, 44, 66, 67, 68, 92, 93, 94, 116, 117, 118, 142, 143, 144, 166, 167, 168, 192, 193, 194, \
		19, 20, 21, 45, 46, 69, 70, 95, 96, 119, 120, 121, 145, 146, 169, 170, 195, 196, \
		22, 23, 47, 71, 72, 97, 122, 123, 147, 171, 172, 197, \
		24, 48, 73, 124, 148, 173, \
		49, 98, 149, 198 };
	//入射角Theta和Phi
	for (i = 0; i < 196; i++)
	{
		if (illuminantID == illuminant[i])
		{
			if (i < 36)
			{
				inTheta = 80.0;
				inPhi = i * 10.0;

			}
			else if (i >= 36 && i < 72)
			{
				inTheta = 70.0;
				inPhi = (i - 36) * 10.0;
			}
			else if (i >= 72 && i < 104)
			{
				inTheta = 60.0;
				inPhi = (i - 72) * 360.0 / 32;
			}
			else if (i >= 104 && i < 132)
			{
				inTheta = 50.0;
				inPhi = (i - 104) * 360.0 / 28;
			}
			else if (i >= 132 && i < 156)
			{
				inTheta = 40.0;
				inPhi = (i - 132) * 15.0;
			}
			else if (i >= 156 && i < 174)
			{
				inTheta = 30.0;
				inPhi = (i - 156) * 20.0;
			}
			else if (i >= 174 && i < 186)
			{
				inTheta = 20.0;
				inPhi = (i - 174) * 30.0;
			}
			else if (i >= 186 && i < 192)
			{
				inTheta = 10.0;
				inPhi = (i - 186) * 60.0;
			}
			else
			{
				inTheta = 5.0;
				inPhi = (i - 192) * 90.0;
			}
		}
	}

	//各向同性材质出射角Phi为0
	if (measureFlag == 1)
		outPhi = 0.0;
	//采集一周不用考虑光源角度
	if (measureFlag == 3)
	{
		inTheta = 0.0;
		inPhi = 0.0;
	}

	//命名方式：out_相机角度-材质台角度_in_光源θ-光源σ
	sprintf(name, "out_%.2f-%.2f_in_%.2f-%.2f.bmp", outTheta, outPhi, inTheta, inPhi);
	string spath = path + name;
	bool isSaved = imwrite(spath, mat);
	//mat.save(QString::fromStdString(path), "BMP", 100);
	//_saveName++;


	emit sendingMeasureState(cameraID, sampleID, illuminantID); //告诉界面线程该角度下的图像已采集，修改界面的Qlabel

	return isSaved;
}
////////////////////////////////////////////////////////////////////////////
// 函数：GetExposureTime
// 描述：计算相机在一定角度光源下的合适曝光时间
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
map<double, Mat> AVTCamera::CaptureByDifTimes(double originalTime, Mat originalMat)
{
	map<double, Mat> imgs;
	imgs[originalTime] = originalMat;
	double ped = ImageProcess::ComputeAverage(originalMat);
	double max = 255.00 / ped * originalTime;
	double step = max / HDR_NUM;
	double exposureTime = 0.00;
	for (int i = 0; i < HDR_NUM; i++)
	{
		exposureTime += step;
		CameraSettings(exposureTime);
		//AVTCamera::GetImageSize(_width, _height);
		uchar* pImageFrame = CaptureAnImage();
		Mat mat = Mat(_height, _width, CV_8UC3, pImageFrame);
		imgs[exposureTime] = mat;

		Sleep(exposureTime);
	}

	return imgs;
}

////////////////////////////////////////////////////////////////////////////
// 函数：CameraThread(CameraThreadInfo threadInfo)
// 描述：
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
/*
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

////////////////////////////////////私有函数//////////////////////////////////////////
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
