#include "workerMeasurement.h"

////////////////////////////////AVT����߳��ඨ��////////////////////////////
WorkerMeasurement::WorkerMeasurement(QObject *parent) :
QObject(parent)
{
	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));
	connect(this, SIGNAL(done()), this, SLOT(ContributeBRDF()));

	sampleComm = new SampleComm();
	sampleComm->Init(2);
	slideComm = new SlideComm();
	slideComm->Init(9);
	illuminant = new Illuminant();
	illuminant->InitCOM(5);

	//��Դ�Ų�˳��
	//���д���ˣ���Ӧ�ô�0��ʼд
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
	//_captureDone = 0;
	_measureFlag = 0;
	//_sampleFlag = 0;
	_seriesCAM = new bool[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
}
WorkerMeasurement::~WorkerMeasurement()
{
	delete illuminant;
	delete _illuminantID;
	delete sampleComm;
	delete slideComm;
	delete _seriesCAM;
}

void WorkerMeasurement::StartTimer(int measureFlag)
{
	slideComm->MoveToX2();//�����λ
	Sleep(10000);//�ȴ������λ
	_measureFlag = measureFlag;
	//if (_measureFlag == 1)
		_timerId = this->startTimer(300);
	//if (_measureFlag == 2)
		//_timerId = this->startTimer(2100);
}

void WorkerMeasurement::timerEvent(QTimerEvent *event)
{
	if (!_isReady)
	{
		if (_measureFlag == 1)
		{
			if (_iID != ILLUMINANT_NUM)
			{
				_isReady = 1;
				illuminant->Suspend();
				illuminant->SetSteadyTime(20);
				illuminant->LightenById(_illuminantID[_iID]+1);//��Դ�����Ǵ�0��ʼд��
				illuminant->Start();
				Sleep(200);
				_iID++;

				emit readyForGrab(_sID, _iID);//ͨ�����̸߳���������л�����һ�����ˣ���������Ե���һ������ع�ʱ��
			}
			else if (_iID == ILLUMINANT_NUM)
			{
				emit done();
				_isReady = 1;
			}
		}

		//ÿ����Դ��ת��Ʒ̨18���Ƕ�
		/*
		if (_measureFlag == 2)
		{
			if (_iID != 196)
			{
				if (_sampleFlag == 0)
				{
					_isReady = 1;
					illuminant->Suspend();
					illuminant->SetSteadyTime(240);//�����ʱ��25.5s  18���ɼ��Ƕ�ʱ�䲻̫��
					illuminant->LightenById(_illuminantID[_iID]+1);
					illuminant->Start();
					Sleep(200);
				}
				if (_sID != 12)//36���ǶȺ�ʱ̫��
				{
					sampleComm->GotoNextPos(5250);
					Sleep(200);//�������������ʱ��
					emit readyForGrab(_sID, _iID);
					_sID++;
					_isReady = 1;
					_sampleFlag = 1;
				}
				else
				{
					_sampleFlag = 0;
					_sID = 0;//��ʼת��һȦ
					_iID++;
					_isReady = 0;
				}
			}
			else if (_iID == 196)
			{
				emit done();
				_isReady = 1;
			}
		}
		*/

		//ÿ����Ʒ̨�Ƕȵ���196����Դ
		if (_measureFlag == 2)
		{
			if (_sID != SAMPLE_NUM)
			{
				if (_iID != ILLUMINANT_NUM)
				{
					_isReady = 1;
					illuminant->Suspend();
					illuminant->SetSteadyTime(20);//�����ʱ��25.5s  18���ɼ��Ƕ�ʱ�䲻̫��
					illuminant->LightenById(_illuminantID[_iID] + 1);
					illuminant->Start();
					Sleep(200);
					_iID++;

					emit readyForGrab(_sID, _iID);
				}
				else if (_iID == ILLUMINANT_NUM)//36���ǶȺ�ʱ̫��
				{
					sampleComm->GotoNextPos(5250);
					//Sleep(200);//�������������ʱ��			
					_iID = 0;
					_sID++;
				}
			}
			else if (_sID == 36)
			{
				emit done();
				_isReady = 1;
			}
		}
	}
}

void WorkerMeasurement::CheckDone(int workerID)
{
	_seriesCAM[workerID] = 1;
	int s = 0;
	for (int i = 0; i < CAM_NUM; i++)
	{
		s += _seriesCAM[i];
	}
	if (s == CAM_NUM)
	{
		_isReady = 0;
		for (int i = 0; i < CAM_NUM; i++)
		{
			_seriesCAM[i] = 0;
		}
	}
	else
		_isReady = 1;
}
////////////////////////////////////////////////////////////////////////////
// ������ContributeBRDF()
// �������ɼ�ԭʼͼ����ɺ����ɲ�������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void WorkerMeasurement::ContributeBRDF()
{

}

void WorkerMeasurement::CloseWorker()
{
	this->killTimer(_timerId);
}












