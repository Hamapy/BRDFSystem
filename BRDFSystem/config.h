#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include "stdafx.h"
#include <QObject>
#include <QSettings>
#include <string>

using namespace std;

extern QSettings* ini;
//相机参数
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
	string acquisitionMode;
	char* gainAuto;

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
extern LightParameters* lightParameters;
extern StepperMotorParameters* stepperMotorParameters;
extern ServoMotorParameters* servoMotorParameters;
extern AcquisitionParameters* acquisitionParameters;
#endif