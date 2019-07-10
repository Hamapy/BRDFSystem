#include "sampleComm.h"

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
//       wheel_homeadj:  归位需要调节的步数
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SampleComm::Init(int port, int step, double velocity, int accelerate, int decelerate, int resolution, int homeadj)
{
	ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件

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

	//先重置再设置PC控制
	if (ret)
	{
		//重置电机驱动
		ClearInputBuffer();
		sprintf(cmd, "CHR\rRE\rSS%s\r", STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}
	//Fini();
	
	ret = Init(m_port);


	if (ret)
	{
		//设置电机为计算机控制
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//该坑浪费我半个下午，先要设置为计算机控制
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
		//设置电机参数
		ClearInputBuffer();
		//sprintf_s(cmd, "CHRAC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		sprintf_s(cmd, "AC%d\rVE%.2f\rDE%d\rMR%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	if (ret)
	{
		//Fini();
		//ret = Reset();//一直转原来是这里的问题，找不到光电限位开关
	}

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
		//设置电机为计算机控制
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//该坑浪费我半个下午，先要设置为计算机控制
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	if (ret)
	{
		//设置电机参数
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
	char cmd[STEP_STRINGLEN];
	bool ret = false;

	// 初始化设备
	ret = Init(m_port);
	m_step = step;

	ClearInputBuffer();
	sprintf_s(cmd, "DI%d\rFL\rSS%s\r", step, STEP_FEEDBACK);	//写入转动步数
	
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
//// 函数：SetVel
//// 描述：设置速度
//// 输入: v: 电机速度
//// 输出：
//// 返回：是否成功完成操作
//// 备注：
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
		//报警复位，使能电机
		ClearInputBuffer();
		sprintf(cmd, "AL\r");
		Write(cmd);

		Wait(5000);//等待样品台归位
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
// 函数：IsFinished
// 描述：查询电机是否已经完成当前指令
// 输入: 需要等待的时间
// 输出：
// 返回：
// 备注：
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
		Wait(WAIT_DEAY);//避免串口繁忙,等待200ms
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

