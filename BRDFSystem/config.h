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
	//�豸ע�����ûд��ÿ̨�����ͬ
	//��֡�ɼ����ã��������ò������ɺ���
	int acquisitionFrameCount;
	float acquisitionFrameRate;
	//��ѧ�������/������Ұ
	bool autofunctionAOIEnable;
	int autofunctionAOIHeight;
	int autofunctionAOIOffsetX;
	int autofunctionAOIOffsetY;
	bool autofunctionAOIShowArea;
	int autofunctionAOIWidth;
	//�Զ����Ŀ��ǿ�ȣ�����Ϊ��С
	int autofunctionTargetIntensity;
	//����ƽ���ã�CMOSΪ����߰���ϸ�ڼӵĻ�ֵ������Ϊ0
	float blackLevel;
	//��ɫ�任����ʱ����SDK�˹��ܣ������Դ���ɫ�任���������
	bool colorTransformationEnable;
	//�ع�ʱ�䣬��ʼ��Ϊ30ms
	float exposureTime;
	//Gainֵ
	float gain;
	//Gammaֵ
	float gamma;
	//ͼ��ߴ�ȡ���
	int height;
	int width;
	//ɫ��ͱ��Ͷ�
	float hue;
	float saturation;
	//AVT Enumeration���Ͳ���
	char* pixelFormat;//ֱ���ò�ɫͼ���ʽ һ��RGB
	char* exposureAuto;
	//string acquisitionMode = "Continuous";
	char* gainAuto;
};

struct CameraCalibrationParamaters
{
	/* ����У����ͼƬ�������� */
	int Count;
	/* ������ڲ������� */
	Mat cameraMatrix;
	/* �������5������ϵ����k1,k2,p1,p2,k3 */
	Mat distCoeffs;
	/* ��Ӧ��ÿ��ͼƬ����ת���� */
	vector<Mat> rvecsMats;
	/* ��Ӧ��ÿ��ͼƬ����ת���� */
	vector<Mat> rotationMatrixs;
	/* ��Ӧ��ÿ��ͼƬ��ƽ������ */
	vector<Mat> tvecsMats;
	/* У����ͼƬ��ƽ����� */
	double error;
};



//��Դ����
struct LightParameters
{
	int serialPortSelection;
	int baudRate;
	int delaySetting;
	int lightingSequence;
	UINT order[196];
};


//�����������
struct StepperMotorParameters
{
	int stepperMotorPortSelection;
	float stepperMotorSpeed;
	int stepperMotorAcceleration;
	int stepperMotorDeceleration;
	int stepperMotorResolution;
	int stepperMotorToHome;
};


//�ŷ��������
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


//�ɼ�����
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
//extern vector<Point2f> pCorners;//���ӽ����ĸ���ǵ�
//extern Point2f pCenter;//���ӽ������ĵ�
extern LightParameters* lightParameters;
extern StepperMotorParameters* stepperMotorParameters;
extern ServoMotorParameters* servoMotorParameters;
extern AcquisitionParameters* acquisitionParameters;
extern CameraParameters* cameraParameters;
//extern CameraCalibrationParamaters* camCalParas;


#endif