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

	//系统初始化
	bool Init(EdsCameraRef& camera);//这里要用引用传递，否则初始化之后丢失相机
	//设置相机参数
	bool SetCameraSettings();
	//拍摄一张照片
	bool TakePhoto(EdsCameraRef camera, int count, int interv_millsec);
	//下载照片到PC
	bool DownloadImage(EdsDirectoryItemRef directoryItem);
	//关闭相机
	bool Fini(EdsCameraRef camera);
	//相机状态、属性、事件等改变触发回调函数

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