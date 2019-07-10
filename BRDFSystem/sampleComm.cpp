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
	m_accelerate = 0;
	m_decelerate = 0;
	m_resolution = 0;
	m_velocity   = 0;
	m_homeadj    = 0;
	m_step       = 0;
	m_port       = 0;
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
bool SampleComm::Init(int port, int step, double velocity, int accelerate, int decelerate, int resolution, int homeadj)
{
	ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�

	bool ret = false;
	char cmd[STEP_STRINGLEN];
	m_port = port;

	if (IsOpen())
	{
		Close();
	}
	if (!Open(m_port))
	{
		ret = false;
	}
	else
	{
		ret = true;
	}

	//������������PC����
	if (ret)
	{
		//���õ������
		ClearInputBuffer();
		sprintf(cmd, "CHR\rRE\rSS%s\r", STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}
	//Fini();
	
	ret = Init(m_port);


	if (ret)
	{
		//���õ��Ϊ���������
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//�ÿ��˷��Ұ�����磬��Ҫ����Ϊ���������
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	m_velocity = this->ini->value("SWIR-Configuration/stepperMotorSpeed").toInt();
	m_accelerate = this->ini->value("SWIR-Configuration/stepperMotorAcceleration").toInt();
	m_decelerate = this->ini->value("SWIR-Configuration/stepperMotorDeceleration").toInt();
	m_resolution = this->ini->value("SWIR-Configuration/stepperMotorResolution").toInt();
	//m_accelerate = accelerate;
	//m_velocity = velocity;
	//m_decelerate = decelerate;
	//m_resolution = resolution;
	m_homeadj = homeadj;
	m_step = step;
	if (ret)
	{
		//���õ������
		ClearInputBuffer();
		//sprintf_s(cmd, "CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		sprintf_s(cmd, "AC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	if (ret)
	{
		//Fini();
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
bool SampleComm::Init(int port)
{
	bool ret = false;
	char cmd[STEP_STRINGLEN];

	if (IsOpen())
	{
		Close();
	}
	if (!Open(m_port))
	{
		ret = false;
	}
	else
	{
		ret = true;
	}
	
	/*
	if (ret)
	{
		//���õ��Ϊ���������
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//�ÿ��˷��Ұ�����磬��Ҫ����Ϊ���������
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	if (ret)
	{
		//���õ������
		ClearInputBuffer();
		//sprintf_s(cmd, "CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		sprintf_s(cmd, "AC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}
	*/
	
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
	ret = Init(m_port);
	m_step = step;

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
//////////////////////////////////////////////////////////////////////////////
//// ������SetVel
//// �����������ٶ�
//// ����: v: ����ٶ�
//// �����
//// ���أ��Ƿ�ɹ���ɲ���
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
//bool SampleComm::SetVel(double v)
//{
//	char cmd[256];
//	bool ret = false;
//
//	ClearInputBuffer();
//	sprintf_s(cmd, "VE%.2f\rSS%s\r", v, STEP_FEEDBACK);
//	Write(cmd);
//
//	if (IsFinished(STEP_TIMEOUT))
//		ret = true;
//	else 
//		ret = false;
//	return ret;
//}
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

	ret = Init(m_port);
	ClearInputBuffer();	
	//sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rSS%s\r", m_homeadj, STEP_SAFESTEP, STEP_FEEDBACK);
	sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rSS%s\r", m_homeadj, STEP_SAFESTEP, STEP_FEEDBACK);

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
		//while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
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
		while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

