#pragma once
#ifndef CCD_H
#define CCD_H
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <thread>
#include <mutex>
#include "Camera.h"
#include "Vimbacpp.h"
#include "VimbaSystem.h"
#include "SharedPointer.h"
#include "VmbCommonTypes.h"
#include "IFrameObserver.h"
//#include "VmbTransformTypes.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
using namespace AVT::VmbAPI;


#define CAPTURE_NUM 100 //�ɼ�ͼ������ 
#define TIMEOUT 5000 //�ɼ�ͼ��ȴ���ʱʱ�� 

//������
/*A camera can be opened if camera-specific control is required, such as I/O pins
on a frame grabber card.Control is then possible via feature access methods.
"pID" might be one of the following : "169.254.12.13" for an IP address,
"000F314C4BE5" for a MAC address or "DEV_1234567890" for an ID as reported by Vimba*/

#define CAM0 DEV_0xA4701120BA037 
#define CAM1 DEV_0xA4701120BA038
#define CAM2 DEV_0xA4701120BA039
#define CAM3 DEV_0xA4701120BA03A
#define CAM4 DEV_0xA4701120BA03B
#define CAM5 DEV_0xA4701120BA03C
#define CAM6 DEV_0xA4701120BA03D
#define CAM7 DEV_0xA4701120BA03E
#define CAM8 DEV_0xA4701120BA03F


//��������̺߳�������
struct CameraThreadInfo
{
	CameraPtrVector cameras;
	int cameraID;
	double exposureTime;
	double gain;
	double blackLevel;
};

class AVTCamera
{

public:
	//AVTCamera();
	AVTCamera(VimbaSystem& system);
	AVTCamera(int cameraID, VimbaSystem& system);
	virtual ~AVTCamera();

	//ϵͳ��ʼ��
	static bool IniVimba(VimbaSystem& system);
	//�����ʼ��
	bool IniCamera();
	//�����
	//bool OpenCamera(int cameraID);
	//�����������
	void CameraSettings(double exposureTime, double gain = 0, double blackLevel = 0);
	//����һ֡ͼ��
	VmbUchar_t* CaptureImage();//typedef unsigned char VmbUchar_t
	//����ͼ��ߴ�
	bool GetImageSize(int& widthOutput, int& heightOutput);
	//����һ֡ͼ��
	bool SaveImage(Mat& captureMat);
	//�����ɼ�
	void CaptureContinuously(int num, int millisecond);
	//��������ع�ʱ��
	static float GetExposureTime(Mat mat);
	//�Զ��ع�
	vector<int> AutoExposure(int num, FeaturePtr feature);
	//������̺߳���
	//static void CameraThread(CameraThreadInfo threadInfo);//�̵߳��õĽӿڱ�����ȫ�ֺ�����̬��Ա����
	//�������9̨��ͬ���ɼ�
	//void CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath, CameraThreadInfo threadInfo);
	//�ر����
	bool FiniCamera();
	//�ر�ϵͳ
	static bool FiniVimba(VimbaSystem& system);


	//����궨У������
	//����ȱ궨
	//��ƽ��У��������ȫ���䣩&���ȶ�У���������ðװ�궨������̨�ϱ�ǵ�����ڷ���任���ɼ������еİ�ƽ����飩
	Mat WhiteBalance(Mat src, Rect wBlock);
	//��ɫУ��������ʽ�ع飩 ���ر任����
	Mat ColorCorrection(CameraPtr& camera, int cameraID);

	//���α궨
	//Zhang���̸�궨 ���������������
	Mat ChessCorrection(CameraPtr& camera, int cameraID);

	//���ԶȲ��ԣ���������ع�ʱ�䡢�Զ��ع�ʱ�䣩 num�ع����
	bool LinearityTest(CameraPtr& camera, int cameraID, int num, float& k, float& tMax, float& tAuto);

	//CCD����У��
	//������ ���ػ���λ�þ���
	Mat DeadPixelDetect(CameraPtr& camera, int cameraID);
	//����ƽ����Vimba SDK���ṩ������BlackLevel�Ĺ��ܣ�������Ҫ����BlackLevel=0ʱ����pedestal���д����������Ϊ������
	//������Ҫ��ȥ�Ļ�ֵ
	int BlackLevelCorrection(CameraPtr& camera, int cameraID);


	//friend class ImageProcessing;  //�ɼ�ͼ��������Ҫ�õ������У������



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
	string						_imageSavingPath = "..\\imgs_calibration";
	int							_saveName;
	FeaturePtr					_feature;

	//VmbError_t					err;
	//VmbHandle_t					hCamera_;
	
	CameraThreadInfo			threadInfo;
	static int					threadID;
	Mat							captureMat;
	
	
	//����ع���� 9��
	float*						tMax;
	float*						tAuto;
	float*						k; //E-t����б��

	//�ɼ�ǰ���Ŀ¼
	static int EmptyFiles(string dirPath);
	//Ѱ�Ҳ�����Ʒ�ĸ��߽���λ��
	vector<Point> FindCorner(int cameraID);
	//Ѱ�Ұ�ɫ�������
	int AVTCamera::WihteAreaDetection(Mat src, Rect wBlock);
};
#endif