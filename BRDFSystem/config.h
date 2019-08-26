#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include "VimbaSystem.h"
#include <QObject>
#include <QSettings>
#include <string>
#include <io.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace AVT::VmbAPI;

extern bool device_on;
extern vector<Point2f> referPts;
extern QMutex _ispMutex;
extern QMutex _fileMutex;
extern QSettings* ini;
struct CameraParameters
{
	//设备注册参数没写，每台相机不同
	//多帧采集设置，本程序用不到，可忽略
	int acquisitionFrameCount;
	float acquisitionFrameRate;
	//光学检测区域/拍摄视野
	bool autofunctionAOIEnable;
	int autofunctionAOIHeight;
	int autofunctionAOIOffsetX;
	int autofunctionAOIOffsetY;
	bool autofunctionAOIShowArea;
	int autofunctionAOIWidth;
	//自动检测目标强度，设置为最小
	int autofunctionTargetIntensity;
	//暗电平设置，CMOS为了提高暗部细节加的基值，设置为0
	float blackLevel;
	//颜色变换，暂时不用SDK此功能，程序自带颜色变换后面待完善
	bool colorTransformationEnable;
	//曝光时间，初始设为30ms
	float exposureTime;
	//Gain值
	float gain;
	//Gamma值
	float gamma;
	//图像尺寸取最大
	int height;
	int width;
	//色相和饱和度
	float hue;
	float saturation;
	//AVT Enumeration类型参数
	char* pixelFormat;//直接用彩色图像格式 一般RGB
	char* exposureAuto;
	//string acquisitionMode = "Continuous";
	char* gainAuto;
};

struct CameraCalibrationParamaters
{
	/* 参与校正的图片的总数量 */
	int Count;
	/* 摄像机内参数矩阵 */
	Mat cameraMatrix;
	/* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */
	Mat distCoeffs;
	/* 对应的每张图片的旋转向量 */
	vector<Mat> rvecsMats;
	/* 对应的每张图片的旋转矩阵 */
	vector<Mat> rotationMatrixs;
	/* 对应的每张图片的平移向量 */
	vector<Mat> tvecsMats;
	/* 校正后图片的平均误差 */
	double error;
};



//光源参数
struct LightParameters
{
	int serialPortSelection;
	int baudRate;
	int delaySetting;
	int lightingSequence;
	UINT order[196];
};


//步进电机参数
struct StepperMotorParameters
{
	int stepperMotorPortSelection;
	float stepperMotorSpeed;
	int stepperMotorAcceleration;
	int stepperMotorDeceleration;
	int stepperMotorResolution;
	int stepperMotorToHome;
};


//伺服电机参数
struct ServoMotorParameters
{
	int servoMotorPortSelection;
	int	sampleRotationAngle;
	int	servoMotorSpeed;
	int	servoMotorAcceleration;
	int	servoMotorDeceleration;
	int	servoMotorResolution;
	int	slideTableMovingDistance;
};


//采集参数
struct AcquisitionParameters
{
	string  save_calibration;
	string	save_brdfiso;
	string	save_brdfiniso;
	string	imageSaveFormat;
	string	imageSavePath;
};

struct MainWindowParameters{
	float gain;
	float blackLevel;
	QString imageSaveFormat;
	QString imageSavePath;

	QString serialPortSelection;
	QString baudRate;
	int	delaySetting;
	int	lightingSequence;


	QString stepperMotorPortSelection;
	QString stepperMotorSpeed;
	QString stepperMotorAcceleration;
	QString stepperMotorDeceleration;
	QString stepperMotorResolution;
	QString sampleRotationAngle;

	QString servoMotorPortSelection;
	QString servoMotorSpeed;
	QString servoMotorAcceleration;
	QString servoMotorDeceleration;
	QString servoMotorResolution;
	int slideTableMovingDistance;
};
extern MainWindowParameters* mainWindowParameters;
extern CameraParameters* cameraParameters;
extern CameraCalibrationParamaters* cameraCalibrationParamaters;
//extern FileStorage cvfs;
//extern vector<Point2f> pCorners;//正视角下四个标记点
//extern Point2f pCenter;//正视角下中心点
extern LightParameters* lightParameters;
extern StepperMotorParameters* stepperMotorParameters;
extern ServoMotorParameters* servoMotorParameters;
extern AcquisitionParameters* acquisitionParameters;
extern CameraParameters* cameraParameters;
//extern CameraCalibrationParamaters* camCalParas;


#endif