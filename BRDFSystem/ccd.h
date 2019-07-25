#pragma once
#ifndef CCD_H
#define CCD_H
//#include <windows.h>
#include <io.h>  //本地文件夹操作所需头文件
//#include <direct.h>
#include <thread>
#include <mutex>
//#include "Camera.h"
#include "Vimbacpp.h"
#include "VimbaSystem.h"
//#include "SharedPointer.h"
#include "VmbCommonTypes.h"
//#include "IFrameObserver.h"
//#include "VmbTransformTypes.h"
#include "opencv2/opencv.hpp"
//#include "BasicImage.h"
#include "config.h"
#include "stdafx.h"
#include "imageProcess.h"

using namespace std;
using namespace cv;
using namespace AVT::VmbAPI;

#define HDR_NUM 3 //HDR曝光次数
#define CAM_NUM 9 //连接相机数量
#define CAPTURE_NUM 150 //采集图像数量 
#define TIMEOUT 5000 //采集图像等待超时时间 

//相机编号
/*A camera can be opened if camera-specific control is required, such as I/O pins
on a frame grabber card.Control is then possible via feature access methods.
"pID" might be one of the following : "169.254.12.13" for an IP address,
"000F314C4BE5" for a MAC address or "DEV_1234567890" for an ID as reported by Vimba

#define CAM0 DEV_0xA4701120BA037 
#define CAM1 DEV_0xA4701120BA038
#define CAM2 DEV_0xA4701120BA039
#define CAM3 DEV_0xA4701120BA03A
#define CAM4 DEV_0xA4701120BA03B
#define CAM5 DEV_0xA4701120BA03C
#define CAM6 DEV_0xA4701120BA03D
#define CAM7 DEV_0xA4701120BA03E
#define CAM8 DEV_0xA4701120BA03F
*/


class AVTCamera : public QObject
{
	Q_OBJECT

public:
	//AVTCamera();
	AVTCamera(VimbaSystem& system);
	AVTCamera(int cameraID, VimbaSystem& system);
	virtual ~AVTCamera();

	//系统初始化
	static bool IniVimba(VimbaSystem& system);
	//相机初始化
	bool IniCamera();
	//设置相机参数
	void CameraSettings(float exposureTime = cameraParameters->exposureTime, CameraParameters* cameraParameters = cameraParameters);
	//返回一帧图像
	VmbUchar_t* CaptureAnImage();//typedef unsigned char VmbUchar_t
	//返回图像尺寸
	bool GetImageSize(int& width, int& height);
	//保存一帧图像
	bool SaveAnImage(Mat mat, string path, int cameraID, int sampleID, int illuminantID, int measureFlag);
	//连续采集图像
	bool SaveImages(Mat& captureMat, string imageSavingPath);
	//采集不同曝光时间下的一组图像（用于HDR）
	map<double, Mat> CaptureByDifTimes(double originalTime, Mat originalMat);
	//相机多线程函数
	//static void CameraThread(CameraThreadInfo threadInfo);//线程调用的接口必须是全局函数或静态成员函数
	//多相机（9台）同步采集
	//void CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath, CameraThreadInfo threadInfo);
	//关闭相机
	bool FiniCamera();
	//关闭系统
	static bool FiniVimba(VimbaSystem& system);

	friend class MainWindow;

private:
	VimbaSystem&				_system;
	int							_cameraID;
	CameraPtrVector				_cameras;
	CameraPtr					_camera;
	FramePtr					_pFrame;
	VmbUchar_t*					_pImage;
	VmbUint32_t					_width;
	VmbUint32_t					_height;
	int							_saveName;
	FeaturePtr					_feature;
	//VmbError_t					err;
	//VmbHandle_t					hCamera_;	
	//CameraThreadInfo			threadInfo;
	Mat							captureMat;
	
	//采集前清空目录
	static int EmptyFiles(string dirPath);

signals:
	void sendingMeasureState(int cameraID, int sampleID, int illuminantID);

};
#endif