// ServoRs232Comm.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ServoRs232Comm.h"
#include "resource.h"		// main symbols



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServoRs232Comm::CServoRs232Comm()
{
	m_accelerate = 0;
	m_decelerate = 0;
	m_resolution = 0;
	m_velocity = 0;
	m_distance = 0;
	m_port = 0;
	m_timestart = 0;
	m_timestop = 0;
}

CServoRs232Comm::~CServoRs232Comm()
{
	fini_devices();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    init
// 描述:    电机初始化
// 输入:    com_port:     电机需要连接的串口
//          velocity:     电机速度
//          accelerate:   电机加速度
//          decelerate:   电机减速度
//          resolution：  电机分辨率  
// 备注:   
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::servo_init(int port, double velocity, int accelerate, int decelerate, int resolution)
{
	bool ret = false;

	m_accelerate = accelerate;
	m_velocity = velocity;
	m_decelerate = decelerate;
	m_resolution = resolution;
	m_port = port;

	ret = open_port();  // 打开串口
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", FEEDBACK_SIGN);
		Write(cmd);  // 设置电机为计算机控制
		ret = feedback_success(get_feedback_timeout());
	}

	if (ret)
	{
		//设置伺服电机参数
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, FEEDBACK_SIGN);
		Write(cmd);

		ret = feedback_success(get_feedback_timeout());
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    fini
// 描述:    电机终止化  
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::servo_fini()
{
	// 关闭串口
	close_port();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    open_port
// 描述:    打开串口 
// 备注:    每次与控制器通信前都执行此函数（为在串口线脱落后再插上情况下，能即时工作而无需重启软件）
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::open_port()
{
	if (IsOpen())
	{
		Close();
	}

	if (!Open(m_port))
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    close_port
// 描述:    关闭串口  
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::close_port()
{
	if (IsOpen())
	{
		Close();
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    feedback_success()
// 描述:    查询电机是否已经完成当前指令
// 输入:    需要等待的时间   
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::feedback_success(int timeout)
{
#define READ_WAITTIME    20
#define WAITDELAY        200

	bool ret = false;
	char rdstr[STRING_LEN];
	int  time_start = GetTickCount();


	while (1)
	{
		wait(WAITDELAY);//避免串口繁忙,等待200ms

		Read(rdstr, STRING_LEN, READ_WAITTIME);

		if (strstr(rdstr, FEEDBACK_SIGN))
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
// 函数:    wait()
// 描述:    时间等待函数
// 输入:    millisec:  等待毫秒数   
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::wait(int millisec)
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
bool CServoRs232Comm::servo_move_sensor_home_cw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 初始化设备
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS1L\rSS%s\r", 1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	// 返回至光电开关位置
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	// 记录位置
	if (ret)
	{
		m_distance = 0;
	}	
	
	// 终止化设备
	fini_devices();

	return ret;
}


//**********************************************************************//
// 函数:    servo_move_sensor_home_ccw();
// 描述:    逆时针转动电机回到光电开关位置
//**********************************************************************//
bool CServoRs232Comm::servo_move_sensor_home_ccw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 设备初始化
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	// 返回至光电开关位置
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	if (ret)
	{
		// 记录位置
		m_distance = LIFT_HEIGHT;
	}

	// 终止化设备
	fini_devices();

	return ret;
}

//**********************************************************************//
// 函数:    servo_move_cw();
// 描述:    顺时针转动电机，降
//**********************************************************************//
bool CServoRs232Comm::servo_move_cw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 初始化设备
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS1L\r", 1000);

		m_timestart = GetTickCount();   //记录开始转动时间

		Write(cmd);

		// 在发下一个指令前稍微等一下
		wait(200);
	}

	// 终止化设备
	fini_devices();

	return ret;
}


//**********************************************************************//
// 函数:    servo_move_ccw();
// 描述:    逆时针转动电机，升
//**********************************************************************//
bool CServoRs232Comm::servo_move_ccw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// 设备初始化
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, FEEDBACK_SIGN);

		m_timestart = GetTickCount(); //记录开始转动时间

		Write(cmd);

		// 在发下一个指令前稍微等一下
		wait(200);
	}

	// 终止化设备
	fini_devices();

	return ret;
}


//**********************************************************************//
// 函数:   servo_stop_and_update_distance_cw();
// 描述:   停止电机顺时针转动，并更新位置
//**********************************************************************//
bool CServoRs232Comm::servo_stop_and_update_distance_cw(double distance_per_rev, int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	int move_time = 0;
	double move_distance;

	m_port = com_port;
	ret = open_port();

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rSS%s\r", FEEDBACK_SIGN);
		Write(cmd);

		m_timestop = GetTickCount();  //记录停止时间

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	//更新位置
	move_time = m_timestop - m_timestart;
	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
	m_distance = m_distance - move_distance;

	// 判断是否已超过光电开关位置
	if (m_distance < 0)
	{
		// 返回至光电开关位置
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}

		if (ret)
		{
			// 位置置为0
			m_distance = 0;
		}
	}

	// 终止化设备
	fini_devices();

	return ret;
}

//**********************************************************************//
// 函数:   servo_stop_and_update_distance_ccw();
// 描述:   停止电机逆时针转动，并更新位置
//**********************************************************************//
bool CServoRs232Comm::servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	int move_time = 0;
	double move_distance;

	m_port = com_port;
	ret = open_port();

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rSS%s\r", FEEDBACK_SIGN);
		Write(cmd);

		m_timestop = GetTickCount();  //记录停止时间

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}
	}

	//更新位置
	move_time = m_timestop - m_timestart;
	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
	m_distance = m_distance + move_distance;

	// 判断是否已超过光电开关位置
	if (m_distance > LIFT_HEIGHT)
	{
		// 返回至光电开关位置
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// 在发下一个指令前稍微等一下
			wait(200);
		}

		if (ret)
		{
			// 位置置为最高
			m_distance = LIFT_HEIGHT;
		}
	}

	// 终止化设备
	fini_devices();

	return ret;
}

//**********************************************************************//
// 函数:    servo_move_distance();
// 输入:		distance_per_rev，为同步轮转一圈滑台升降的高度，义乌义正为10cm深圳久宸为12.5cm
// 描述:    移动滑台一定距离
//**********************************************************************//
bool CServoRs232Comm::servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	double move_distance; //需要移动的高度
	int move_pulses; //需要转动的脉冲步数SI

	// 初始化设备
	m_port = com_port;
	ret = init_devices(m_port);

	int rlr = m_resolution / 2; //“R”寄存器匹配伺服驱动器中的EG设置

	if (distance < 0 || distance > LIFT_HEIGHT)
	{
		//MessageBox(NULL, "Please set the lift height between 0 to 120 cm!", "IAS", MB_ICONWARNING);
		return ret;
	}
	else
	{
		move_distance = m_distance - distance;
		move_pulses = static_cast<int> (move_distance * rlr * SERVO_REDUCTION / distance_per_rev);

		if (ret)
		{
			ClearInputBuffer();
			sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", move_pulses, FEEDBACK_SIGN);
			Write(cmd);

			if (wait_feedback)
			{
				ret = feedback_success(get_feedback_timeout());
			}
			else
			{
				// 在发下一个指令前稍微等一下
				wait(200);
			}
		}
		if (ret)
		{
			// 更新位置信息
			m_distance = distance;
		}

		// 终止化设备
		fini_devices();
	}

	return ret;
}

//**********************************************************************//
// 函数:    servo_return_current_position();						 
// 描述:    返回当前滑台位置
//**********************************************************************//
double CServoRs232Comm::servo_return_current_position()
{
	return m_distance;
}

//**********************************************************************//
// 函数:    servo_get_last_position();						 
// 描述:    获取上次关机器时滑台所在的位置
//**********************************************************************//
void CServoRs232Comm::servo_get_last_position(double last_distance)
{
	m_distance = last_distance;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:    get_feedback_timeout
// 描述:    获得色轮连接的超时阈值
////////////////////////////////////////////////////////////////////////////////////////////////
int CServoRs232Comm::get_feedback_timeout()
{
	const int	servo_timeout = 10000;

	return servo_timeout;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:   init_devices()
// 描述:   初始化设备函数
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::init_devices(int com_port)
{
	bool ret = false;

	// 伺服电机初始化
	ret = servo_init(com_port, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);

	if (!ret)
	{
		//MessageBox(NULL, "Fail to connect the focus motor, please check.", "IAS", MB_ICONWARNING);
		ret = false;
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数:   fini_devices()
// 描述:   终止设备函数
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::fini_devices()
{
	servo_fini();
}

