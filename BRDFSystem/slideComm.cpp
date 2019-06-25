#include "slideComm.h"

/////////////////////////////////////构造函数//////////////////////////////////////////
SlideComm::SlideComm()
{
	m_accelerate = 0;
	m_decelerate = 0;
	m_resolution = 0;
	m_velocity = 0;
	m_distance = 0;
	m_port = 0;
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

	m_accelerate = accelerate;
	m_velocity = velocity;
	m_decelerate = decelerate;
	m_resolution = resolution;
	m_port = port;

	// 打开串口
	if (IsOpen())
	{
		Close();
	}
	if (!Open(m_port))
	{
		return false;
	}
	
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode以Q/SCL模式上电
		Write(cmd);  // 设置电机为计算机控制
		//ret = IsFinished(get_feedback_timeout());
	}

	if (ret)
	{
		//设置伺服电机参数
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(get_feedback_timeout());
	}

	return ret;
}
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
bool SlideComm::Init(int port)
{
	//bool ret = true;

	// 打开串口
	if (IsOpen())
		Close();
	if (Open(m_port))
		return true;
	else
		return false;

	/*
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // 设置电机为计算机控制
		ret = IsFinished(get_feedback_timeout());
	}

	if (ret)
	{
		//设置伺服电机参数
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(get_feedback_timeout());
	}
	*/
	//return ret;
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

//**********************************************************************//
// 函数:    servo_move_sensor_home_cw();
// 描述:    顺时针转动电机回到光电开关位置
//**********************************************************************//
bool SlideComm::servo_move_sensor_home_cw(int com_port, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 初始化设备
	m_port = com_port;
	ret = Init(m_port);

	if (ret)
	{
		ClearInputBuffer();
		/*
		STD 立刻停止
		DI 距离位置
		FS1L 当输入1的传感信号为LOW时，减速停止
		SS 发送字符串
		*/
		sprintf(cmd, "STD\rDI%d\rFS1L\rSS%s\r", 1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	// 返回至光电开关位置
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	// 记录位置
	if (ret)
	{
		m_distance = 0;
	}

	// 终止化设备
	Fini();

	return ret;
}


//**********************************************************************//
// 函数:    servo_move_sensor_home_ccw();
// 描述:    逆时针转动电机回到光电开关位置
//**********************************************************************//
bool SlideComm::servo_move_sensor_home_ccw(int com_port, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 设备初始化
	m_port = com_port;
	ret = Init(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	// 返回至光电开关位置
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			Wait(200);
		}
	}

	if (ret)
	{
		// 记录位置
		m_distance = MOVE_DISTANCE;
	}

	// 终止化设备
	Fini();

	return ret;
}


////// 函数:    servo_move_cw();
////// 描述:    顺时针转动电机，降
////
////bool SlideComm::servo_move_cw(int com_port, bool Wait_feedback/* = true*/)
////{
////	char cmd[STRING_LEN];
////	bool ret = false;
////
////	// 初始化设备
////	m_port = com_port;
////	ret = init_devices(m_port);
////
////	if (ret)
////	{
////		ClearInputBuffer();
////		sprintf(cmd, "STD\rDI%d\rFS1L\r", 1000);
////
////		m_timestart = GetTickCount();   //记录开始转动时间
////
////		Write(cmd);
////
////		// 在发下一个指令前稍微等一下
////		Wait(200);
////	}
////
////	// 终止化设备
////	Fini();
////
////	return ret;
////}
////
////
////
////// 函数:    servo_move_ccw();
////// 描述:    逆时针转动电机，升
////bool SlideComm::servo_move_ccw(int com_port, bool Wait_feedback/* = true*/)
////{
////	char cmd[STRING_LEN];
////	bool ret = false;
////
////	// 设备初始化
////	m_port = com_port;
////	ret = init_devices(m_port);
////
////	if (ret)
////	{
////		ClearInputBuffer();
////		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, SERVO_FEEDBACK);
////
////		m_timestart = GetTickCount(); //记录开始转动时间
////
////		Write(cmd);
////
////		// 在发下一个指令前稍微等一下
////		Wait(200);
////	}
////
////	// 终止化设备
////	Fini();
////
////	return ret;
////}
//
//
////**********************************************************************//
//// 函数:   servo_stop_and_update_distance_cw();
//// 描述:   停止电机顺时针转动，并更新位置
////**********************************************************************//
//bool SlideComm::servo_stop_and_update_distance_cw(double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	int move_time = 0;
//	double move_distance;
//
//	m_port = com_port;
//	//ret = open_port();
//
//	if (ret)
//	{
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rSS%s\r", SERVO_FEEDBACK);
//		Write(cmd);
//
//		m_timestop = GetTickCount();  //记录停止时间
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// 在发下一个指令前稍微等一下
//			Wait(200);
//		}
//	}
//
//	//更新位置
//	move_time = m_timestop - m_timestart;
//	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
//	m_distance = m_distance - move_distance;
//
//	// 判断是否已超过光电开关位置
//	if (m_distance < 0)
//	{
//		// 返回至光电开关位置
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, SERVO_FEEDBACK);
//		Write(cmd);
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// 在发下一个指令前稍微等一下
//			Wait(200);
//		}
//
//		if (ret)
//		{
//			// 位置置为0
//			m_distance = 0;
//		}
//	}
//
//	// 终止化设备
//	Fini();
//
//	return ret;
//}
//
////**********************************************************************//
//// 函数:   servo_stop_and_update_distance_ccw();
//// 描述:   停止电机逆时针转动，并更新位置
////**********************************************************************//
//bool SlideComm::servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	int move_time = 0;
//	double move_distance;
//
//	m_port = com_port;
//	//ret = open_port();
//
//	if (ret)
//	{
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rSS%s\r", SERVO_FEEDBACK);
//		Write(cmd);
//
//		m_timestop = GetTickCount();  //记录停止时间
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// 在发下一个指令前稍微等一下
//			Wait(200);
//		}
//	}
//
//	//更新位置
//	move_time = m_timestop - m_timestart;
//	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
//	m_distance = m_distance + move_distance;
//
//	// 判断是否已超过光电开关位置
//	if (m_distance > MOVE_DISTANCE)
//	{
//		// 返回至光电开关位置
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, SERVO_FEEDBACK);
//		Write(cmd);
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// 在发下一个指令前稍微等一下
//			Wait(200);
//		}
//
//		if (ret)
//		{
//			// 位置置为最高
//			m_distance = MOVE_DISTANCE;
//		}
//	}
//
//	// 终止化设备
//	Fini();
//
//	return ret;
//}
////**********************************************************************//
//// 函数:    servo_move_distance();
//// 输入:		distance_per_rev，为同步轮转一圈滑台升降的高度，义乌义正为10cm深圳久宸为12.5cm
//// 描述:    移动滑台一定距离
////**********************************************************************//
//bool SlideComm::servo_move_distance(double distance, double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	double move_distance; //需要移动的高度
//	int move_pulses; //需要转动的脉冲步数SI
//	int rlr = m_resolution / 2; //“R”寄存器匹配伺服驱动器中的EG设置
//
//	//double d = MOVE_DISTANCE;
//	if (distance < 0 || distance > MOVE_DISTANCE)
//	{
//		//MessageBox(NULL, "Please set the lift height between 0 to 120 cm!", "IAS", MB_ICONWARNING);
//		return ret;
//	}
//	else
//	{
//		move_distance = m_distance - distance;
//		move_pulses = static_cast<int> (move_distance * rlr * SERVO_REDUCTION / distance_per_rev);  
//
//		// 初始化设备
//		ret = Init(com_port);
//
//		if (ret)
//		{
//			ClearInputBuffer();
//			sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", move_pulses, SERVO_FEEDBACK);
//			Write(cmd);
//
//			if (Wait_feedback)
//			{
//				ret = IsFinished(get_feedback_timeout());
//			}
//			else
//			{
//				// 在发下一个指令前稍微等一下
//				Wait(200);
//			}
//		}
//		if (ret)
//		{
//			// 更新位置信息
//			m_distance = distance;
//		}
//
//		// 终止化设备
//		Fini();
//	}
//
//	return ret;
//}

//**********************************************************************//
// 函数:    servo_return_current_position();						 
// 描述:    返回当前滑台位置
//**********************************************************************//
double SlideComm::servo_return_current_position()
{
	return m_distance;
}

////**********************************************************************//
//// 函数:    servo_get_last_position();						 
//// 描述:    获取上次关机器时滑台所在的位置
////**********************************************************************//
//void SlideComm::servo_get_last_position(double last_distance)
//{
//	m_distance = last_distance;
//}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    get_feedback_timeout
// 描述:    获得色轮连接的超时阈值
////////////////////////////////////////////////////////////////////////////////////////////////
int SlideComm::get_feedback_timeout()
{
	const int	servo_timeout = 10000;

	return servo_timeout;
}


