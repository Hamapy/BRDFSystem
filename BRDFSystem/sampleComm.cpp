#include "sampleComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
//       wheel_homeadj:  �����λ��Ҫ���ڵĲ���
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::Init(int port, int step, double velocity, int accelerate, int decelerate, int resolution, int homeadj)
{
	bool ret = false;
	m_accelerate = accelerate;
	m_velocity   = velocity;
	m_decelerate = decelerate;
	m_resolution = resolution;
	m_homeadj    = homeadj;
	m_step       = step;
	m_port       = port;

    char cmd[256];

	if(IsOpen())
	{
		Close();
	}
	if(!Open(m_port))
	{
		return false;
	}

	//���õ��Ϊ���������
	ClearInputBuffer();
	sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode��Q/SCLģʽ�ϵ�
	Write(cmd);

	//���õ������
	ClearInputBuffer();
	//sprintf_s(cmd,"CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r",m_accelerate,m_velocity,m_decelerate,m_resolution,FEEDBACK_SIGN);
	sprintf_s(cmd, "AC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
	Write(cmd);

	if (IsFinished(STEP_TIMEOUT))
		ret = true;
	else 
		ret = false;
	if (ret)
		Reset();

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
	m_port = port;

	char cmd[256];

	if (IsOpen())
	{
		Close();
	}
	if (!Open(m_port))
	{
		return false;
	}

	//���õ��Ϊ���������
	ClearInputBuffer();
	sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode��Q/SCLģʽ�ϵ�
	Write(cmd);

	//���õ������
	ClearInputBuffer();
	//sprintf_s(cmd,"CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r",m_accelerate,m_velocity,m_decelerate,m_resolution,FEEDBACK_SIGN);
	sprintf_s(cmd, "CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
	Write(cmd);

	if (IsFinished(STEP_TIMEOUT))
		ret = true;
	else
		ret = false;
	if (ret)
		Reset();

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
	m_step = step;
	char cmd[256];
	sprintf_s(cmd, "DI%d\rFL\rSS%s\r", step, STEP_FEEDBACK);	//д��ת������
	ClearInputBuffer();
	Write(cmd);
	if (IsFinished(STEP_TIMEOUT))
		return true;
	else 
		return false;
}
////////////////////////////////////////////////////////////////////////////
// ������SetVel
// �����������ٶ�
// ����: v: ����ٶ�
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::SetVel(double v)
{
	char cmd[256];
	bool ret = false;

	ClearInputBuffer();
	sprintf_s(cmd, "VE%.2f\rSS%s\r", v, STEP_FEEDBACK);
	Write(cmd);

	if (IsFinished(STEP_TIMEOUT))
		ret = true;
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
	char cmd[256];
	bool ret = false;
	ClearInputBuffer();	
	//��δ�ҵ���Ƭ�����תWHEEL_SAFE_STEP=500000��2000תһȦ��Ҳ��ͣ��
	sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rDI%d\rFL\rSS%s\r", 0 - m_homeadj, STEP_SAFESTEP, 5700, STEP_FEEDBACK);
	Write(cmd);
	if (IsFinished(STEP_TIMEOUT))
		ret = true;
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
// ������Fini
// ��������ѯ����Ƿ��Ѿ���ɵ�ǰָ��
// ����: ��Ҫ�ȴ���ʱ��
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::IsFinished(int wait_time)
{
	#define READ_WAIT_TIME   20
	#define WAIT_DEAY        50
	bool success = false;
	char rdstr[256];
	int  time_start = GetTickCount();
	MSG  msg;
	while(1)
	{	
		Wait(WAIT_DEAY);//���⴮�ڷ�æ,�ȴ�50ms
		Read(rdstr, 256, READ_WAIT_TIME);
		if (strstr(rdstr, STEP_FEEDBACK))
		{
			success = true;
			break;
		}
		if ((int)(GetTickCount() - time_start) > wait_time)
		{
			success = false;           
			break;
		} 	
		while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}	
    }
	return success;
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

