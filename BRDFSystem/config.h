#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include "stdafx.h"
#include <QObject>
#include <QSettings>
#include <string>

using namespace std;

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
extern CameraParameters* cameraParameters;
#endif