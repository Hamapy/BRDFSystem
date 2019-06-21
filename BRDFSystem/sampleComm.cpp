#include "sampleComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////电机命令释义////////////////////////////////////
/*
// 设置电机为计算机控制
PM2:  Power - up Mode以Q / SCL模式上电；
SA : Save Parameters保存参数数据；
SS : Send String发送字符串；（SSdone : 当SS执行时发送“done”）

// 设置电机参数（加速度、速度等）
AC : Acceleration Rate加速度；
VE : Velocity速度；
DE : Deceleration减速度；
MR : Microstep Resolution 微步调分辨率；
DI : Distance / Position 距离，位置；
FL : Feed to Length 按一定长度提供参数；
DC : Change Distance 改变距离；
FY3L : (FY)Feed to Sensor with Safety Distance安全距离内提供传感器参数；
DL1 : Define Limits 定义限制，设置限制输入与常开限制开关一起工作；
AL : Alarm Code 报警代码；
AR : Alarm Reset(Immediate) 报警复位（直接）
*/

////////////////////////////////构造函数////////////////////////////////////
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
///////////////////////////////////公有成员函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：Init
// 描述：电机初始化
// 输入: port:   调焦电机需要连接的串口
//       step:   调整到下一位置电机需要转动的步数
//       velocity:     电机速度
//       accelerate:   电机加速度
//       decelerate:   电机减速度
//       resolution：  电机分辨率
//       wheel_homeadj:  相机归位需要调节的步数
// 输出：
// 返回：是否成功完成操作
// 备注：
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

	//设置电机为计算机控制
	ClearInputBuffer();
	sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode以Q/SCL模式上电
	Write(cmd);

	//设置电机参数
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
// 函数：Init
// 描述：电机初始化
// 输入: port:   调焦电机需要连接的串口
// 输出：
// 返回：是否成功完成操作
// 备注：
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

	//设置电机为计算机控制
	ClearInputBuffer();
	sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode以Q/SCL模式上电
	Write(cmd);

	//设置电机参数
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
// 函数：GotoNextPos
// 描述：转到下一位置
// 输入: 
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::GotoNextPos(int step)
{
	m_step = step;
	char cmd[256];
	sprintf_s(cmd, "DI%d\rFL\rSS%s\r", step, STEP_FEEDBACK);	//写入转动步数
	ClearInputBuffer();
	Write(cmd);
	if (IsFinished(STEP_TIMEOUT))
		return true;
	else 
		return false;
}
////////////////////////////////////////////////////////////////////////////
// 函数：SetVel
// 描述：设置速度
// 输入: v: 电机速度
// 输出：
// 返回：是否成功完成操作
// 备注：
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
// 函数：Reset
// 描述：样品平台归位
// 输入: 
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::Reset()
{
	char cmd[256];
	bool ret = false;
	ClearInputBuffer();	
	//若未找到挡片，最多转WHEEL_SAFE_STEP=500000（2000转一圈）也会停下
	sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rDI%d\rFL\rSS%s\r", 0 - m_homeadj, STEP_SAFESTEP, 5700, STEP_FEEDBACK);
	Write(cmd);
	if (IsFinished(STEP_TIMEOUT))
		ret = true;
	return ret;	
}
////////////////////////////////////////////////////////////////////////////
// 函数：Fini
// 描述：终止操作
// 输入: 
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void SampleComm::Fini()
{
	if(IsOpen())
	{
		Close();
	}
}
///////////////////////////////////私有成员函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：Fini
// 描述：查询电机是否已经完成当前指令
// 输入: 需要等待的时间
// 输出：
// 返回：
// 备注：
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
		Wait(WAIT_DEAY);//避免串口繁忙,等待50ms
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
// 函数：Wait
// 描述：时间等待函数
// 输入: millisec:  等待毫秒数
// 输出：
// 返回：
// 备注：
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

