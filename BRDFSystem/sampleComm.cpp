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
	//ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件
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
	//先重置再设置PC控制
	ret = InitA();
	if (ret)
	{
		//重置电机驱动
		ClearInputBuffer();
		sprintf(cmd, "CHR\rRE\rSS%s\r", STEP_FEEDBACK);
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}
	*/
	
	//设置电机为计算机控制
	ret = InitA();
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", STEP_FEEDBACK);//该坑浪费我半个下午，先要设置为计算机控制
		Write(cmd);
		ret = IsFinished(STEP_TIMEOUT);
	}

	//设置电机参数
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
	ret = InitA();
	_step = step;

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

	ret = InitA();
	ClearInputBuffer();	
	sprintf_s(cmd, "DI%d\rDC%d\rFY3L\rSS%s\r", _homeadj, STEP_SAFESTEP, STEP_FEEDBACK);
	//sprintf_s(cmd, "DI%d\rFL%d\rFY3L\rSS%s\r", _homeadj, STEP_SAFESTEP, STEP_FEEDBACK);

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
		//while(PeekMessage(&msg,NULL,NULL,NULL,P_REMOVE))
		//{
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}
	}
}

