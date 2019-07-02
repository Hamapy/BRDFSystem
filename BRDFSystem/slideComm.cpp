#include "slideComm.h"

/////////////////////////////////////构造函数//////////////////////////////////////////
SlideComm::SlideComm()
{
	//Init(13, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	_accelerate = 0;
	_decelerate = 0;
	_resolution = 0;
	_velocity = 0;
	_distance = 0;
	_port = 0;
}

SlideComm::~SlideComm()
{
	Fini();
}

///////////////////////////////////公有成员函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：Init
// 描述：电机初始化
// 输入:    port:     电机需要连接的串口
//          velocity:     电机速度
//          accelerate:   电机加速度
//          decelerate:   电机减速度
//          resolution：  电机分辨率  
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::Init(int port, double velocity, int accelerate, int decelerate, int resolution)
{
	bool ret = true;

	_accelerate = accelerate;
	_velocity = velocity;
	_decelerate = decelerate;
	_resolution = resolution;
	_port = port;

	// 打开串口
	if (IsOpen())
	{
		Close();
	}
	if (!Open(_port))
	{
		return false;
	}
	
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		//sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode以Q/SCL模式上电
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // 设置电机为计算机控制
		ret = IsFinished(10000);
	}

	if (ret)
	{
		//设置伺服电机参数
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", _accelerate, _velocity, _decelerate, _resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(10000);
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Init
// 描述：电机初始化
// 输入: port:     电机需要连接的串口
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::Init(int port)
{
	bool ret = false;
	char cmd[STRING_LEN];
	_port = port;

	// 打开串口
	if (IsOpen())
	{
		Close();
	}
	if (!Open(_port))
	{
		ret = false;
	}
	else
		ret = true;

	//以下注释加了就有问题
	/*
	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		//sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode以Q/SCL模式上电
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // 设置电机为计算机控制
		ret = IsFinished(10000);
	}

	if (ret)
	{
		//设置伺服电机参数
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", _accelerate, _velocity, _decelerate, _resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(10000);
	}
	*/

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// 函数：MoveToX1
// 描述：
// 输入: 
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveToX1(bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 初始化设备
	ret = Init(_port);

	if (ret)
	{
		ClearInputBuffer();
		/*
		STD DI 移动一定距离后停止
		FS1L 当输入1的信号为LOW时停止
		FS1R 当输入1电位信号变化时停止
		SS 发送字符串
		*/
		sprintf(cmd, "STD\rDI%d\rFS1R\rSS%s\r", 10000, SERVO_FEEDBACK);
		//就位前先归位时的限位条件
		//sprintf(cmd, "STD\rDI%d\rFS2R\rSS%s\r", 10000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(10000);
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	// 终止设备
	Fini();

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// 函数：MoveToX2
// 描述：
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveToX2(bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 设备初始化
	ret = Init(_port);

	//避免系统启动前滑轨处于其他位置，就位前先使滑轨归位
	//ret = MoveToX1();

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2R\rSS%s\r", -10000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(10000);
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	// 终止设备
	Fini();

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// 函数：MoveTest
// 描述：
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveTest(double distance, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	double mDistance; //需要移动距离
	int mPulses; //需要转动的脉冲步数SI
	int rlr = _resolution / 2; //“R”寄存器匹配伺服驱动器中的EG设置
	double distancePerRev = 10;//该值有待修正

	if (distance < 0 || distance > MOVE_DISTANCE)
		return ret;
	else
	{
		mDistance = _distance - distance;
		mPulses = static_cast<int> (mDistance * rlr * SERVO_REDUCTION / distancePerRev);

		// 初始化设备
		ret = Init(_port);

		if (ret)
		{
			ClearInputBuffer();
			sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", mPulses, SERVO_FEEDBACK);
			Write(cmd);

			if (Wait_feedback)
			{
				ret = IsFinished(10000);
			}
			else
			{
				Wait(200);
			}
		}
		if (ret)
		{
			// 更新位置信息
			_distance = distance;
		}
		// 终止设备
		Fini();
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Fini
// 描述：电机终止运行
// 输入:    port:     电机需要连接的串口
//          velocity:     电机速度
//          accelerate:   电机加速度
//          decelerate:   电机减速度
//          resolution：  电机分辨率  
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void SlideComm::Fini()
{
	// 关闭串口
	if (IsOpen())
	{
		Close();
	}
}
///////////////////////////////////私有成员函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：IsFinished()
// 描述：查询电机是否已经完成当前指令
// 输入: timeout 需要等待的时间
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
///////////////////////////////////////////////////////////////////////////////
bool SlideComm::IsFinished(int timeout)
{
	#define READ_WAITTIME    20
	#define WAITDELAY        200

	bool ret = false;
	char rdstr[STRING_LEN];
	int  time_start = GetTickCount();


	while (1)
	{
		Wait(WAITDELAY);//避免串口繁忙,等待200ms
		Read(rdstr, STRING_LEN, READ_WAITTIME);
		if (strstr(rdstr, SERVO_FEEDBACK))
		{
			ret = true;
			break;
		}

		if ((int)(GetTickCount() - time_start) > timeout)
		{
			ret = false;
			break;
		}
	}

	return ret;

	#undef READ_WAITTIME
	#undef WAITDELAY
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    Wait()
// 描述:    时间等待函数
// 输入:    millisec:  等待毫秒数   
////////////////////////////////////////////////////////////////////////////////////////////////
void SlideComm::Wait(int millisec)
{
	int time_start = GetTickCount();

	while (1)
	{
		if ((int)(GetTickCount() - time_start) > millisec)
		{
			break;
		}
	}
}






