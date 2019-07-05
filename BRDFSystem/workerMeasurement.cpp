#include "workerMeasurement.h"

////////////////////////////////AVT����߳��ඨ��////////////////////////////
WorkerMeasurement::WorkerMeasurement(VimbaSystem&	system, QObject *parent) :
_system(system),
QObject(parent)
{
	_mutex.lock();
	//cameraAVT = new AVTCamera(_workerID, _system);

	sampleComm = new SampleComm();
	sampleComm->Init(2, 0, 0.25, STEP_ACCELERATE, STEP_DECELERATE, STEP_RESOLUTION, STEP_TOHOME);
	illuminant = new Illuminant(5);
	illuminant->InitCOM();
	//illuminant->SetSteadyTime(30);
	//��Դ�Ų�˳��
	_illuminantID = new UINT[196] { 0, 1, 2, 3,    25, 26, 27, 28, 29,    50,51,52,53,       75,76,77,78,79,   100,101,102,103,      125,126,127,128,129,    150,151,152,153,      175,176,177,178,179,\
									  4,5,6,7,8,     30,31,32,33,           54,55,56,57,58,    80,81,82,83,      104,105,106,107,108,  130,131,132,133,        154,155,156,157,158,  180,181,182,183,\
									  9,10,11,12,    34,35,36,37,           59,60,61,62,       84,85,86,87,      109,110,111,112,      134,135,136,137,        159,160,161,162,      184,185,186,187,\
									  13,14,15,      38,39,40,41,           63,64,65,          88,89,90,91,      113,114,115,          138,139,140,141,        163,164,165,          188,189,190,191,\
									  16,17,18,      42,43,44,              66,67,68,          92,93,94,         116,117,118,          142,143,144,            166,167,168,          192,193,194,\
									  19,20,21,      45,46,                 69,70,             95,96,            119,120,121,          145,146,                169,170,              195,196,\
									  22,23,         47,                    71,72,             97,               122,123,              147,                    171,172,              197,\
									  24,            48,                    73,                                  124,                  148,                    173,                  \
										    		 49,                                       98,                                     149,                                          198 };
	_iID = 0;
	_sID = 0;
	_isReady = 0;
	_captureDone = 0;
	_measureFlag = 0;
	_sampleFlag = 0;
	//_exposureTime = 50;
	//_saveName = 0;
	_saveName = new int[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	_seriesCAM = new bool[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	/*
	//_measureFlag = measureFlag;
	illuminant->InitCOM();
	if (_measureFlag == 1)
		illuminant->SetSteadyTime(10);
	if (_measureFlag == 2)
		illuminant->SetSteadyTime(360);
	_timerId = this->startTimer(10000);//���ö�ʱ���������߳�capture
	*/
}
WorkerMeasurement::~WorkerMeasurement()
{
	delete illuminant;
	delete _illuminantID;
	delete sampleComm;
	//delete cameraAVT;
	this->killTimer(_timerId);
}

void WorkerMeasurement::NextMeasureState(int workerID, /*Mat*/QImage mat)
{
	if (!_isReady)
	{
		if (_measureFlag == 1)
		{
			if (_iID != 196)
			{
				illuminant->Suspend();
				//Sleep(500);
				illuminant->SetSteadyTime(50);
				illuminant->LightenById(_illuminantID[_iID]);
				illuminant->Start();
				Sleep(500);//������յ�����ǰ��ͼ��
				_iID++;
				_isReady = 1;
				emit readyForCapture();//ͨ�����̸߳���������л�����һ�����ˣ���������Ե���һ������ع�ʱ��
			}
		}

		if (_measureFlag == 2)
		{
			if (_iID != 196)
			{
				if (_sampleFlag == 0)
				{
					illuminant->Suspend();
					illuminant->SetSteadyTime(200);
					illuminant->LightenById(_illuminantID[_iID]);
					illuminant->Start();
					Sleep(500);
				}

				if (_sID != 9)//36���ǶȺ�ʱ̫��
				{
					sampleComm->GotoNextPos(175);
					Sleep(3000);//�ȴ���Ʒ̨��ת������2��ʱָ����Ⱦ
					_sID++;
					_isReady = 1;
					_sampleFlag = 1;
				}
				else
				{
					_sampleFlag = 0;
					_sID = 0;//��ʼת��һȦ
					_iID++;
					_isReady = 1;
					emit readyForCapture();
				}
			}
		}
	}
	else //��Դ��Ʒ��λ����������̷߳����Ĳɼ�ͼ��
	{
		SaveSeriesMat(workerID, mat);
		//emit readyForCapture();
		//if (_captureDone)
		//	_isReady = 0;
	}

}

inline void WorkerMeasurement::SaveSeriesMat(int workerID, /*Mat*/QImage mat)
{
#define CAM_NUM 9

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (workerID == i)
			_seriesCAM[i] = 1;
	}
	SaveAMat(workerID, mat);

	int s = 0;
	for (int i = 0; i < CAM_NUM; i++)
	{
		s += _seriesCAM[i];
	}
	if (s == CAM_NUM)
	{
		//_captureDone = 1;
		_isReady = 1;
		for (int i = 0; i < CAM_NUM; i++)
		{
			_seriesCAM[i] = 0;
		}
	}
	else
		_isReady = 0;
		//_captureDone = 0;

#undef CAM_NUM
}

inline void WorkerMeasurement::SaveAMat(int workerID, /*Mat*/QImage mat)
{
	char saveName[4] = { 0 };
	sprintf(saveName, "%4d", _saveName[workerID]);
	char sPath[200];

	sprintf(sPath, "//%s.bmp", saveName);
	string path;
	if (_measureFlag == 1)
		path = _imageSavingPath1 + _materialName + "//camera" + to_string(workerID) + sPath;
	if (_measureFlag == 2)
		path = _imageSavingPath2 + _materialName + "//camera" + to_string(workerID) + sPath;
	if (_measureFlag == 3)
		path = _imageSavingPath3 + "//camera" + to_string(workerID) + sPath;
	//imwrite(path, mat);
	mat.save(QString::fromStdString(path), "BMP", 100);
	_saveName[workerID]++;

	return;
}

void WorkerMeasurement::GetMaterialName(QString materialName)
{
	_materialName = materialName.toStdString();
}

/*
void WorkerMeasurement::StartTimer(int measureFlag)
{
	
	_measureFlag = measureFlag;
	illuminant->InitCOM();
	//Ϊʲô�����������Զ��������
	if (_measureFlag == 1)
		illuminant->SetSteadyTime(10);
	if (_measureFlag == 2)
		illuminant->SetSteadyTime(360);
	_timerId = this->startTimer(10000);//���ö�ʱ���������߳�capture
}
*/

/*
void WorkerMeasurement::timerEvent(QTimerEvent *event)
//void WorkerMeasurement::StartMeasure(int measueFlag)
{
	if (event->timerId() == _timerId)
	{
		if (_measureFlag = 1)
		{
			illuminant->Suspend();
			illuminant->LightenById(_illuminantID[_ID]);
			illuminant->Start();
			_ID++;
			Sleep(500);//�����յ�����ǰ��ͼ��
			//connect(this->parent(), SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));//��ôд��Ϊ�˱�����workerCCD��ռ�����Դ����ȥ����Ҫ�����ƥ��ID��������
																									//����ôд̫����ˣ������������������Դ������
			
			//����������ռ���
			_mutex.lock();
			cameraAVT->GetImageSize(_width, _height);
			_pImageFrame = cameraAVT->CaptureImage();
			_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
			_exposureTime = AVTCamera::GetExposureTime(_mat);
			//�����ع�ʱ��
			cameraAVT->CameraSettings(_exposureTime * 1000);
			Sleep(1000);//���ع�ʱ����Ч
			_pImageFrame = cameraAVT->CaptureImage();
			_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
			_mutex.unlock();
		}
		if (_measureFlag = 2)
		{
			illuminant->Suspend();
			illuminant->LightenById(_illuminantID[_ID]);
			illuminant->Start();
			_ID++;
			Sleep(500);//�����յ�����ǰ��ͼ��
			for (int i = 0; i < 36; i++)
			{
				//connect(this, SIGNAL(sendingMat(int, Mat)), this, SLOT(GetExposureTime(int, Mat)));
				//Sleep(1000);//���ع�ʱ����Ч
				//connect(this, SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));
				
				//����������ռ���
				_mutex.lock();
				cameraAVT->GetImageSize(_width, _height);
				_pImageFrame = cameraAVT->CaptureImage();
				_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
				_exposureTime = AVTCamera::GetExposureTime(_mat);
				//�����ع�ʱ��
				cameraAVT->CameraSettings(_exposureTime * 1000);
				Sleep(1000);//���ع�ʱ����Ч
				_pImageFrame = cameraAVT->CaptureImage();
				_mat = Mat(_height, _width, CV_8UC3, _pImageFrame);
				_mutex.unlock();

				sampleComm->GotoNextPos(175);
				Sleep(3000);//�ȴ���Ʒ̨��ת
			}
		}
		if (_ID == 195)
		{
			disconnect(this, SIGNAL(sendingMat(int, Mat)), this, SLOT(GetExposureTime(int, Mat)));
			disconnect(this, SIGNAL(sendingMat(int, Mat)), this, SLOT(SaveAMat(int, Mat)));
			emit done();//���Ͳɼ�����ź�
		}
	}
}
*/
/*
void WorkerMeasurement::GetExposureTime(int workerID, Mat mat)
{
	disconnect(this->parent(), SIGNAL(sendingMat(int, Mat)), this, SLOT(GetExposureTime(int, Mat)));//����ۺ��������̶����������֡Mat��ײ
	//_mutex.lock();
	_exposureTime = AVTCamera::GetExposureTime(mat);
	//_mutex.unlock();
}
*/






