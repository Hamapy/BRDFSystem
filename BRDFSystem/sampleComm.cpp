#include "sampleComm.h"

////////////////////////////////�����������////////////////////////////////////
/*
// ���õ��Ϊ���������
PM2:  Power - up Mode��Q / SCLģʽ�ϵ磻
SA : Save Parameters����������ݣ�
SS : Send String�����ַ�������SSdone : ��SSִ��ʱ���͡�done����

// ���õ�����������ٶȡ��ٶȵȣ�
AC : Acceleration Rate���ٶȣ�
VE : Velocity�ٶȣ�
DE : Deceleration���ٶȣ�
MR : Microstep Resolution ΢�����ֱ��ʣ�
DI : Distance / Position ���룬λ�ã�
FL : Feed to Length ��һ�������ṩ������
DC : Change Distance �ı���룻
FY3L : (FY)Feed to Sensor with Safety Distance��ȫ�������ṩ������������
DL1 : Define Limits �������ƣ��������������볣�����ƿ���һ������
AL : Alarm Code �������룻
AR : Alarm Reset(Immediate) ������λ��ֱ�ӣ�
*/

////////////////////////////////���캯��////////////////////////////////////
SampleComm::SampleComm()
{
	//ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�
	_port = ini->value("BRDFSystem-Configuration/stepperMotorPortSelection").toInt();
	_velocity = ini->value("BRDFSystem-Configuration/stepperMotorSpeed").toFloat();
	_accelerate = ini->value("BRDFSystem-Configuration/stepperMotorAcceleration").toInt();
	_decelerate = ini->value("BRDFSystem-Configuration/stepperMotorDeceleration").toInt();
	_resolution = ini->value("BRDFSystem-Configuration/stepperMotorResolution").toInt();
	_homeadj = ini->value("BRDFSystem-Configuration/stepperMotorToHome").toInt();
	_step = 0;
	}
SampleComm::~SampleComm()
{
    Fini();
}
///////////////////////////////////���г�Ա����/////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// ������Init
// �����������ʼ��
// ����: port:   ���������Ҫ���ӵĴ���
//       step:   ��������һλ�õ����Ҫת���Ĳ���
//       velocity:     ����ٶ�
//       accelerate:   ������ٶ�
//       decelerate:   ������ٶ�
//       resolution��  ����ֱ���
//       wheel_homeadj:  ��λ��Ҫ���ڵĲ���
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::Init(int port)
{
	bool ret = false;
	char cmd[STEP_STRINGLEN];
	char rdstr[STEP_STRINGLEN];

	_port = port;
	//_accelerate = STEP_ACCELERATE;
	//_velocity = STEP_VELOCITY;
	//_decelerate = STEP_DECELERATE;
	//_resolution = STEP_RESOLUTION;
	//_homeadj = STEP_TOHOME;
	//_step = 0;

	/*
	//������������PC����
	ret = InitA();
	if (ret)
	{
		//���õ������
		ClearInputBuffer();
		sprintf(cmd, "CHR\rRE\rSS%s\r", STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}
	*/
	
	//���õ��Ϊ���������
	ret = InitA();
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//�ÿ��˷��Ұ�����磬��Ҫ����Ϊ���������
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	//���õ������
	ret = InitA();
	if (ret)
	{
		ClearInputBuffer();
		sprintf_s(cmd, "CHR\rAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", _accelerate, _velocity, _decelerate, _resolution, STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	if (ret)
	{
		Fini();
		//ret = Reset();//һֱתԭ������������⣬�Ҳ��������λ����
	}

	return ret;	
}
////////////////////////////////////////////////////////////////////////////
// ������Init
// �����������ʼ��
// ����: port:   ���������Ҫ���ӵĴ���
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::InitA()
{
	bool ret = false;
	char cmd[STEP_STRINGLEN];

	if (IsOpen())
	{
		Close();
	}
	if (!Open(_port))
	{
		ret = false;
	}
	else
	{
		ret = true;
	}
	
	
	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������GotoNextPos
// ������ת����һλ��
// ����: 
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::GotoNextPos(int step)
{
	char cmd[STEP_STRINGLEN];
	bool ret = false;

	// ��ʼ���豸
	ret = InitA();
	_step = step;

	ClearInputBuffer();
	sprintf_s(cmd, "DI%d\rFL\rSS%s\r", step, STEP_FEEDBACK);	//д��ת������
	Write(cmd);
	if (IsFinished(STEP_TIMEOUT))
	{
		Fini();
		ret = true;
	}
	else 
		ret = false;

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Reset
// ��������Ʒƽ̨��λ
// ����: 
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::Reset()
{
	char cmd[STEP_STRINGLEN];
	char rdstr[STEP_STRINGLEN];
	bool ret = false;

	ret = InitA();
	ClearInputBuffer();	
	sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rSS%s\r", _homeadj, STEP_SAFESTEP, STEP_FEEDBACK);
	//sprintf_s(cmd, "DI%d\rFL%d\rFY3L\rSS%s\r", _homeadj, STEP_SAFESTEP, STEP_FEEDBACK);

	Write(cmd);
	
	if (IsFinished(STEP_TIMEOUT))
	{
		//������λ��ʹ�ܵ��
		ClearInputBuffer();
		sprintf(cmd, "AL\r");
		Write(cmd);

		Wait(5000);//�ȴ���Ʒ̨��λ
		Read(rdstr, 256, 20);
		//if (strstr(rdstr, "AL=0004"))
		if (strstr(rdstr, "AL=0001"))
		{
			ClearInputBuffer();
			sprintf(cmd, "DI%d\rFL\rSS%s\r", -500, STEP_FEEDBACK);
			Write(cmd);
			if (IsFinished(STEP_TIMEOUT))
			{

				ClearInputBuffer();
				sprintf(cmd, "AR\r");
				Write(cmd);

				Wait(20);
				ClearInputBuffer();
				sprintf(cmd, "AL\r");
				Write(cmd);

				Wait(20);
				Read(rdstr, 256, 20);
				if (strstr(rdstr, "AL=0000"))
				{
					ret = true;
					Fini();
				}
			}
		}
		else
			ret = false;
	}
	
	
	return ret;	
}
////////////////////////////////////////////////////////////////////////////
// ������Fini
// ��������ֹ����
// ����: 
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void SampleComm::Fini()
{
	if(IsOpen())
	{
		Close();
	}
}
///////////////////////////////////˽�г�Ա����/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������IsFinished
// ��������ѯ����Ƿ��Ѿ���ɵ�ǰָ��
// ����: ��Ҫ�ȴ���ʱ��
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::IsFinished(int timeout)
{
	#define READ_WAIT_TIME   20
	#define WAIT_DEAY        200
	bool ret = false;
	char rdstr[STEP_STRINGLEN];
	int  time_start = GetTickCount();
	//MSG  msg;
	while(1)
	{	
		Wait(WAIT_DEAY);//���⴮�ڷ�æ,�ȴ�200ms
		Read(rdstr, STEP_STRINGLEN, READ_WAIT_TIME);
		if (strstr(rdstr, STEP_FEEDBACK))
		{
			ret = true;
			break;
		}
		if ((int)(GetTickCount() - time_start) > timeout)
		{
			ret = false;           
			break;
		} 	
		//while(PeekMessage(&msg,NULL,NULL,NULL,P_REMOVE))
		//{
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}	
    }
	return ret;
	#undef READ_WAIT_TIME
	#undef WAIT_DEAY
}
////////////////////////////////////////////////////////////////////////////
// ������Wait
// ������ʱ��ȴ�����
// ����: millisec:  �ȴ�������
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void SampleComm::Wait(int millisec)
{
	MSG msg;
	int time_start = GetTickCount();
	while(1)
	{	
		if ((int)(GetTickCount() - time_start) > millisec)
		{
			break;
		}
		//while(PeekMessage(&msg,NULL,NULL,NULL,P_REMOVE))
		//{
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}
	}
}

