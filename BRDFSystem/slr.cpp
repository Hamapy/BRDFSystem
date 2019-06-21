#include "slr.h"

SLR::SLR()
{
	isSDKLoaded = false;
	camera = NULL;
	err = EDS_ERR_OK;
}

SLR::~SLR()
{

}

////////////////////////////////////////////////////////////////////////////
// 函数：Init(EdsCameraRef &camera)
// 描述：SDK初始化、相机连接
// 输入：Null
// 输出：是否成功完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::Init(EdsCameraRef& camera)
{
	//初始化SDK
	err = EdsInitializeSDK();
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true;
	}
	//获取相机
	if (err == EDS_ERR_OK)
	{
		//err = GetFirstCamera(&Camera);
		EdsCameraListRef cameraList = NULL;
		EdsUInt32 count = 0;
		err = EdsGetCameraList(&cameraList);
		//获取相机数量
		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildCount(cameraList, &count);
			if (count == 0)
			{
				err = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}
		//获取相机
		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildAtIndex(cameraList, 0, &camera);
		}
		//释放相机列表
		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}
	}
	////没整明白
	if (err == EDS_ERR_OK)
	{
		_controller = new CameraController();
		_controller->setCameraModel(_model);
		//相机objects事件变化回调函数（volumes representing memory cards, files and directories, and shot images stored in memory）
		if (err == EDS_ERR_OK)
		{
			err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, CameraEventListener::handleObjectEvent, (EdsVoid *)_controller);
			//err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
		}
		//相机属性事件变化回调函数
		if (err == EDS_ERR_OK)
		{
			err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, CameraEventListener::handlePropertyEvent, (EdsVoid *)_controller);
			//err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, handlePropertyEvent, NULL);
		}
		//相机状态事件变化回调函数
		if (err == EDS_ERR_OK)
		{
			err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, CameraEventListener::handleStateEvent, (EdsVoid *)_controller);
			//err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, handleStateEvent, NULL);
		}
	}
	//打开相机会话
	if (err == EDS_ERR_OK)
	{
		err = EdsOpenSession(camera);
		if (err == EDS_ERR_OK)
		{
			cout << "开启相机会话" << endl;
		}
		else
			return false;
	}
	//获得相机设备标识
	if (err == EDS_ERR_OK)
	{
		EdsDeviceInfo DeviceInfo;
		err = EdsGetDeviceInfo(camera, &DeviceInfo);
		if (err == EDS_ERR_OK)
		{
			cout << "相机设备名称：" << DeviceInfo.szDeviceDescription << endl;
		}
		else
			return false;
	}
	bool bContinue = false;
	//设置相机存储属性，保存照片直接下载至主机
	EdsUInt32 SaveTo = kEdsSaveTo_Host;
	err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(SaveTo), &SaveTo);//这里camera为空值了
	//设置主机的保留容量
	EdsCapacity Capacity = { 0x7FFFFFFF, 0x1000, 1 };
	err = EdsSetCapacity(camera, Capacity);
	//结束直接传输模式
	//err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_ExitDirectTransfer, 0);

	return true;
}
////////////////////////////////////////////////////////////////////////////
// 函数：SetCameraSettings
// 描述：设置相机参数
// 输入：时间：time(us)
// 输出：是否成功执行
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::SetCameraSettings()
{
	return 1;

}
////////////////////////////////////////////////////////////////////////////
// 函数：TakePhoto
// 描述：拍摄一张照片
// 输入：Null
// 输出：一帧图像
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::TakePhoto(EdsCameraRef camera, int count, int interv_millsec)
{
	for (int i = 0; i < count; ++i) 
	{
		cout << "拍摄一张照片" << endl;
		//EOS 50D或者EOS 5D Mark II或之后的相机使用PressShutterButton这条命令 
		//err = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely);
		//err = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
		err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);

		if (err != EDS_ERR_OK)
		{
			if (err == EDS_ERR_DEVICE_BUSY)
			{
				CameraEvent e("DeviceBusy");
				_model->notifyObservers(&e);
				return true;
			}
			CameraEvent e("error", &err);
			_model->notifyObservers(&e);
		}
		Sleep(DWORD(interv_millsec));
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////
// 函数：DownloadImage(EdsDirectoryItemRef directoryItem)
// 描述：下载图像至PC
// 输入：Null
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::DownloadImage(EdsDirectoryItemRef directoryItem)
//bool DownloadImage(EdsDirectoryItemRef directoryItem)
{ 
	EdsStreamRef stream = NULL;
	EdsDirectoryItemInfo dirItemInfo;
	EdsError err = EdsGetDirectoryItemInfo(directoryItem, &dirItemInfo);
	char* path = "..\\photos\\";
	strcat(path, dirItemInfo.szFileName);

	//内存拷贝,存储目录字符合并
	//char* path = "../photos/";
	//const size_t len1 = strlen(path);
	//const size_t len2 = strlen(dirItemInfo.szFileName);
	//char *dest = (char*)malloc(len1 + len2 + 1);
	//memcpy(dest, path, len1);
	//memcpy(dest + len1, dirItemInfo.szFileName, len2 + 1);

	//创建传输文件流
	if (err == EDS_ERR_OK)
	{
		err = EdsCreateFileStream(path, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
	}
	//下载图片
	if (err == EDS_ERR_OK)
	{
		err = EdsDownload(directoryItem, dirItemInfo.size, stream);
	}
	if (err == EDS_ERR_OK)
	{
		err = EdsDownloadComplete(directoryItem);
	}
	else
		return false;
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Fini(EdsCameraRef camera)
// 描述：结束会话、释放资源
// 输入：Null
// 输出：是否正确完成操作
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::Fini(EdsCameraRef camera)
{
	//结束相机会话
	if (err == EDS_ERR_OK)
	{
		err = EdsCloseSession(camera);
		if (err != EDS_ERR_OK)
			return false;
	}
	//释放相机内存
	if (camera != NULL)
	{
		EdsRelease(camera);
		if (camera != NULL)
			return false;
	}
	//结束SDK
	if (isSDKLoaded)
	{
		err = EdsTerminateSDK();
		if (err != EDS_ERR_OK)
			return false;
	}

	return true;
}

/*
EdsError SetPropertyCommand(EdsCameraRef camera, EdsUInt32 propertyID, EdsUInt32 value)
{
	EdsError err = EDS_ERR_OK;
	err = EdsSetPropertyData(camera, propertyID, 0, sizeof(value), (EdsVoid *)&value);
	return err;
}

EdsError getPropertyCommand(EdsCameraRef camera, EdsPropertyID propertyID)
{
	EdsError err = EDS_ERR_OK;
	EdsDataType	dataType = kEdsDataType_Unknown;
	EdsUInt32   dataSize = 0;


	if (propertyID == kEdsPropID_Unknown)
	{
		//If unknown is returned for the property ID , the required property must be retrieved again
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_AEModeSelect);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_Tv);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_Av);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_ISOSpeed);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_MeteringMode);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_ExposureCompensation);
		if (err == EDS_ERR_OK) err = getPropertyCommand(camera, kEdsPropID_ImageQuality);

		return err;
	}

	//从相机对象或图像对象获取指定属性的字节大小和数据类型。
	if (err == EDS_ERR_OK)
	{
		err = EdsGetPropertySize(camera, propertyID, 0, &dataType, &dataSize);
	}

	if (err == EDS_ERR_OK)
	{

		if (dataType == kEdsDataType_UInt32)
		{
			EdsUInt32 data;

			//获取属性类型
			err = EdsGetPropertyData(camera, propertyID, 0, dataSize, &data);

			//Acquired property value is set
			if (err == EDS_ERR_OK)
			{
				SetPropertyCommand(camera, propertyID, data);
			}
		}

		if (dataType == kEdsDataType_String)
		{

			EdsChar str[EDS_MAX_NAME];
			//Acquisition of the property
			err = EdsGetPropertyData(camera, propertyID, 0, dataSize, str);

			Sleep(500);

		}
		if (dataType == kEdsDataType_FocusInfo)
		{
			EdsFocusInfo focusInfo;
			//Acquisition of the property
			err = EdsGetPropertyData(camera, propertyID, 0, dataSize, &focusInfo);

			Sleep(500);
		}
	}

	return err;
}


static EdsError EDSCALLBACK  handleObjectEvent(
	EdsUInt32			inEvent,
	EdsBaseRef			inRef,
	EdsVoid *			inContext
	)
{

	CameraController*	controller = (CameraController *)inContext;

	switch (inEvent)
	{
	case kEdsObjectEvent_DirItemRequestTransfer://下载图像到PC
		//fireEvent(controller, "download", inRef);
		DownloadImage(inRef);
		break;

	default:
		//Object without the necessity is released
		if (inRef != NULL)
		{
			EdsRelease(inRef);
		}
		break;
	}

	return EDS_ERR_OK;
}

static EdsError EDSCALLBACK  handlePropertyEvent(
	EdsUInt32			inEvent,
	EdsUInt32			inPropertyID,
	EdsUInt32			inParam,
	EdsVoid *			inContext
	)
{

	CameraController*	controller = (CameraController *)inContext;

	switch (inEvent)
	{
	case kEdsPropertyEvent_PropertyChanged:
		//fireEvent(controller, "get_Property", &inPropertyID);
		getPropertyCommand(camera, inPropertyID);
		break;

	case kEdsPropertyEvent_PropertyDescChanged:
		//fireEvent(controller, "get_PropertyDesc", &inPropertyID);
		break;
	}

	return EDS_ERR_OK;
}

static EdsError EDSCALLBACK  handleStateEvent(
	EdsUInt32			inEvent,
	EdsUInt32			inParam,
	EdsVoid *			inContext
	)
{

	CameraController*	controller = (CameraController *)inContext;

	switch (inEvent)
	{
	case kEdsStateEvent_Shutdown:
		//fireEvent(controller, "shutDown");
		break;
	}

	return EDS_ERR_OK;
}

*/