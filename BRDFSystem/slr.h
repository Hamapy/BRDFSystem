#pragma once
#include "stdafx.h"
#include <iostream>
#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"

#include "CameraController.h"
#include "CameraEventListener.h"
//#include "opencv2/opencv.hpp"

using namespace std;
//using namespace cv;

class SLR
{

public:
	SLR();
	virtual ~SLR();

	//ϵͳ��ʼ��
	bool Init(EdsCameraRef& camera);//����Ҫ�����ô��ݣ������ʼ��֮��ʧ���
	//�����������
	bool SetCameraSettings();
	//����һ����Ƭ
	bool TakePhoto(EdsCameraRef camera, int count, int interv_millsec);
	//������Ƭ��PC
	bool DownloadImage(EdsDirectoryItemRef directoryItem);
	//�ر����
	bool Fini(EdsCameraRef camera);
	//���״̬�����ԡ��¼��ȸı䴥���ص�����

	EdsCameraRef		camera;
	CameraModel*		_model;
	CameraController*	_controller;

private:
	bool isSDKLoaded;
	EdsError err;
};


/*
bool DownloadImage(EdsDirectoryItemRef directoryItem);
EdsError SetPropertyCommand(EdsCameraRef camera, EdsUInt32 propertyID, EdsUInt32 value);
EdsError getPropertyCommand(EdsCameraRef camera, EdsPropertyID propertyID);

static EdsError EDSCALLBACK  handleObjectEvent(
	EdsUInt32			inEvent,
	EdsBaseRef			inRef,
	EdsVoid *			inContext
	);

static EdsError EDSCALLBACK  handlePropertyEvent(
	EdsUInt32			inEvent,
	EdsUInt32			inPropertyID,
	EdsUInt32			inParam,
	EdsVoid *			inContext
	);

static EdsError EDSCALLBACK  handleStateEvent(
	EdsUInt32			inEvent,
	EdsUInt32			inParam,
	EdsVoid *			inContext
	);

	*/