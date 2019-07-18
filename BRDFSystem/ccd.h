#pragma once
#ifndef CCD_H
#define CCD_H
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <thread>
#include <mutex>
#include <algorithm>
#include "Camera.h"
#include "Vimbacpp.h"
#include "VimbaSystem.h"
#include "SharedPointer.h"
#include "VmbCommonTypes.h"
#include "IFrameObserver.h"
//#include "VmbTransformTypes.h"
#include "opencv2/opencv.hpp"
#include "BasicImage.h"
#include "config.h"
#include <QtWidgets/QMainWindow>

using namespace std;
using namespace cv;
using namespace AVT::VmbAPI;

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

typedef struct _tagPIXEL
{
	int i;   // i axis (row)
	int j;   // j axis (column)
}PIXEL;

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
	//打开相机
	//bool OpenCamera(int cameraID);
	//设置相机参数
	void CameraSettings(float exposureTime = cameraParameters->exposureTime, CameraParameters* cameraParameters = cameraParameters);
	//返回一帧图像
	VmbUchar_t* CaptureImage();//typedef unsigned char VmbUchar_t
	//返回图像尺寸
	bool GetImageSize(int& widthOutput, int& heightOutput);
	//保存一帧图像
	bool SaveAnImage(Mat mat, string path, int cameraID, int sampleID, int illuminantID, int mutex);
	//连续采集图像
	bool CaptureImages(Mat& captureMat, string imageSavingPath);
	//连续采集
	void CaptureContinuously(int num, int millisecond);
	//计算合适曝光时间
	static float GetExposureTime(Mat mat);
	//通过直方图方法判断采集图像是否过曝或过暗
	static bool IsOverExposure(Mat src);
	//自动曝光
	vector<int> AutoExposure(int num, FeaturePtr feature);
	//相机多线程函数
	//static void CameraThread(CameraThreadInfo threadInfo);//线程调用的接口必须是全局函数或静态成员函数
	//多相机（9台）同步采集
	//void CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath, CameraThreadInfo threadInfo);
	//关闭相机
	bool FiniCamera();
	//关闭系统
	static bool FiniVimba(VimbaSystem& system);


	//相机标定校正部分
	//辐射度标定
	//白平衡校正（完美全反射）&均匀度校正（事先用白板标定，载物台上标记点仅用于仿射变换及采集过程中的白平衡检验）
	static vector<float> GetWhiteBalanceTrans(vector<Mat> mats);
	Mat WhiteBalance(Mat src);
	//颜色校正（多项式回归） 返回变换矩阵
	//Mat ColorCorrection(CameraPtr& camera, int cameraID);

	//几何标定
	//Zhang棋盘格标定 返回相机参数矩阵
	Mat ChessCorrection(CameraPtr& camera, int cameraID);

	//线性度测试（计算最大曝光时间、自动曝光时间） num曝光次数
	//bool LinearityTest(CameraPtr& camera, int cameraID, int num, float& k, float& tMax, float& tAuto);

	//CCD底噪校正
	//坏点标记 返回坏点位置矩阵
	void DeadPixelDetect(Mat src, int maxNum);
	Mat DeadPixelCorrection();
	//暗电平处理（Vimba SDK中提供了设置BlackLevel的功能，我们需要对其BlackLevel=0时留的pedestal进行处理，输出不能为负数）
	//返回需要减去的基值
	int BlackLevelCorrection(CameraPtr& camera, int cameraID);


	friend class MainWindow;



private:
	VimbaSystem&				_system;
	int							_cameraID;
	//static CameraPtrVector		_cameras;
	//static CameraPtr			_camera;
	CameraPtrVector				_cameras;
	CameraPtr					_camera;
	FramePtr					_pFrame;
	VmbUchar_t*					_pImage;
	VmbUint32_t					_width;
	VmbUint32_t					_height;
	//string						_imageSavingPath = "..\\imgs_calibration";
	int							_saveName;
	FeaturePtr					_feature;
	//VmbError_t					err;
	//VmbHandle_t					hCamera_;
	
	//CameraThreadInfo			threadInfo;
	static int					threadID;
	Mat							captureMat;
	
	
	//相机曝光参数 9组
	float*						tMax;
	float*						tAuto;
	float*						k; //E-t曲线斜率
	vector<float>				_trans;//白平衡校正系数
	vector<vector<float>>		_transs;
	int							_darkResponse;//暗电流响应值
	vector<int*>				_deadPixelPos;//坏点位置

	//采集前清空目录
	static int EmptyFiles(string dirPath);
	//多张连续图像求平均
	static Mat AverageImage(vector<Mat> mats);
	//读取一个文件夹下所有图片
	static vector<Mat> ReadImages(string path);
	//判断该像素点是否被标记
	static bool IsSelected(Mat src, int i, int j);
	//选中标记像素点
	static void Select(Mat src, int i, int j);

signals:
	void sendingMeasureState(int cameraID, int sampleID, int illuminantID);
};
#endif