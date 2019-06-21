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
// ������Init(EdsCameraRef &camera)
// ������SDK��ʼ�����������
// ���룺Null
// ������Ƿ�ɹ���ɲ���
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::Init(EdsCameraRef& camera)
{
	//��ʼ��SDK
	err = EdsInitializeSDK();
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true;
	}
	//��ȡ���
	if (err == EDS_ERR_OK)
	{
		//err = GetFirstCamera(&Camera);
		EdsCameraListRef cameraList = NULL;
		EdsUInt32 count = 0;
		err = EdsGetCameraList(&cameraList);
		//��ȡ�������
		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildCount(cameraList, &count);
			if (count == 0)
			{
				err = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}
		//��ȡ���
		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildAtIndex(cameraList, 0, &camera);
		}
		//�ͷ�����б�
		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}
	}
	////û������
	if (err == EDS_ERR_OK)
	{
		_controller = new CameraController();
		_controller->setCameraModel(_model);
		//���objects�¼��仯�ص�������volumes representing memory cards, files and directories, and shot images stored in memory��
		if (err == EDS_ERR_OK)
		{
			err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, CameraEventListener::handleObjectEvent, (EdsVoid *)_controller);
			//err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
		}
		//��������¼��仯�ص�����
		if (err == EDS_ERR_OK)
		{
			err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, CameraEventListener::handlePropertyEvent, (EdsVoid *)_controller);
			//err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, handlePropertyEvent, NULL);
		}
		//���״̬�¼��仯�ص�����
		if (err == EDS_ERR_OK)
		{
			err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, CameraEventListener::handleStateEvent, (EdsVoid *)_controller);
			//err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, handleStateEvent, NULL);
		}
	}
	//������Ự
	if (err == EDS_ERR_OK)
	{
		err = EdsOpenSession(camera);
		if (err == EDS_ERR_OK)
		{
			cout << "��������Ự" << endl;
		}
		else
			return false;
	}
	//�������豸��ʶ
	if (err == EDS_ERR_OK)
	{
		EdsDeviceInfo DeviceInfo;
		err = EdsGetDeviceInfo(camera, &DeviceInfo);
		if (err == EDS_ERR_OK)
		{
			cout << "����豸���ƣ�" << DeviceInfo.szDeviceDescription << endl;
		}
		else
			return false;
	}
	bool bContinue = false;
	//��������洢���ԣ�������Ƭֱ������������
	EdsUInt32 SaveTo = kEdsSaveTo_Host;
	err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(SaveTo), &SaveTo);//����cameraΪ��ֵ��
	//���������ı�������
	EdsCapacity Capacity = { 0x7FFFFFFF, 0x1000, 1 };
	err = EdsSetCapacity(camera, Capacity);
	//����ֱ�Ӵ���ģʽ
	//err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_ExitDirectTransfer, 0);

	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������SetCameraSettings
// �����������������
// ���룺ʱ�䣺time(us)
// ������Ƿ�ɹ�ִ��
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::SetCameraSettings()
{
	return 1;

}
////////////////////////////////////////////////////////////////////////////
// ������TakePhoto
// ����������һ����Ƭ
// ���룺Null
// �����һ֡ͼ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::TakePhoto(EdsCameraRef camera, int count, int interv_millsec)
{
	for (int i = 0; i < count; ++i) 
	{
		cout << "����һ����Ƭ" << endl;
		//EOS 50D����EOS 5D Mark II��֮������ʹ��PressShutterButton�������� 
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
// ������DownloadImage(EdsDirectoryItemRef directoryItem)
// ����������ͼ����PC
// ���룺Null
// �����
// ���أ�
// ��ע��
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

	//�ڴ濽��,�洢Ŀ¼�ַ��ϲ�
	//char* path = "../photos/";
	//const size_t len1 = strlen(path);
	//const size_t len2 = strlen(dirItemInfo.szFileName);
	//char *dest = (char*)malloc(len1 + len2 + 1);
	//memcpy(dest, path, len1);
	//memcpy(dest + len1, dirItemInfo.szFileName, len2 + 1);

	//���������ļ���
	if (err == EDS_ERR_OK)
	{
		err = EdsCreateFileStream(path, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
	}
	//����ͼƬ
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
// ������Fini(EdsCameraRef camera)
// �����������Ự���ͷ���Դ
// ���룺Null
// ������Ƿ���ȷ��ɲ���
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SLR::Fini(EdsCameraRef camera)
{
	//��������Ự
	if (err == EDS_ERR_OK)
	{
		err = EdsCloseSession(camera);
		if (err != EDS_ERR_OK)
			return false;
	}
	//�ͷ�����ڴ�
	if (camera != NULL)
	{
		EdsRelease(camera);
		if (camera != NULL)
			return false;
	}
	//����SDK
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

	//����������ͼ������ȡָ�����Ե��ֽڴ�С���������͡�
	if (err == EDS_ERR_OK)
	{
		err = EdsGetPropertySize(camera, propertyID, 0, &dataType, &dataSize);
	}

	if (err == EDS_ERR_OK)
	{

		if (dataType == kEdsDataType_UInt32)
		{
			EdsUInt32 data;

			//��ȡ��������
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
	case kEdsObjectEvent_DirItemRequestTransfer://����ͼ��PC
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