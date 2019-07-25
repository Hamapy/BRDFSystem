#pragma once
#ifndef CCD_H
#define CCD_H
//#include <windows.h>
#include <io.h>  //�����ļ��в�������ͷ�ļ�
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

#define HDR_NUM 3 //HDR�ع����
#define CAM_NUM 9 //�����������
#define CAPTURE_NUM 150 //�ɼ�ͼ������ 
#define TIMEOUT 5000 //�ɼ�ͼ��ȴ���ʱʱ�� 

//������
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

	//ϵͳ��ʼ��
	static bool IniVimba(VimbaSystem& system);
	//�����ʼ��
	bool IniCamera();
	//�����������
	void CameraSettings(float exposureTime = cameraParameters->exposureTime, CameraParameters* cameraParameters = cameraParameters);
	//����һ֡ͼ��
	VmbUchar_t* CaptureAnImage();//typedef unsigned char VmbUchar_t
	//����ͼ��ߴ�
	bool GetImageSize(int& width, int& height);
	//����һ֡ͼ��
	bool SaveAnImage(Mat mat, string path, int cameraID, int sampleID, int illuminantID, int measureFlag);
	//�����ɼ�ͼ��
	bool SaveImages(Mat& captureMat, string imageSavingPath);
	//�ɼ���ͬ�ع�ʱ���µ�һ��ͼ������HDR��
	map<double, Mat> CaptureByDifTimes(double originalTime, Mat originalMat);
	//������̺߳���
	//static void CameraThread(CameraThreadInfo threadInfo);//�̵߳��õĽӿڱ�����ȫ�ֺ�����̬��Ա����
	//�������9̨��ͬ���ɼ�
	//void CaptureSynchronizally(CameraPtrVector& cameras, string imageSavingPath, CameraThreadInfo threadInfo);
	//�ر����
	bool FiniCamera();
	//�ر�ϵͳ
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
	
	//�ɼ�ǰ���Ŀ¼
	static int EmptyFiles(string dirPath);

signals:
	void sendingMeasureState(int cameraID, int sampleID, int illuminantID);

};
#endif