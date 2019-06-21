// ServoRs232Comm.h: interface for the CServoRs232Comm class.
//
//////////////////////////////////////////////////////////////////////
#ifdef SERVORS232COMM_EXPORTS
#define SERVORS232COMM_API __declspec(dllexport)
#else
#define SERVORS232COMM_API __declspec(dllimport)
#endif

extern "C"
{
	SERVORS232COMM_API bool Servo_MoveSensorHome_CW(int com_port);
	SERVORS232COMM_API bool Servo_MoveSensorHome_CCW(int com_port);
	SERVORS232COMM_API bool Servo_Move_CW(int com_port);
	SERVORS232COMM_API bool Servo_Move_CCW(int com_port);
	SERVORS232COMM_API bool Servo_MoveDistance(double distance, double distance_per_rev, int com_port);
	SERVORS232COMM_API bool Servo_StopAndUpdateDistance_CW(double distance_per_rev, int com_port);
	SERVORS232COMM_API bool Servo_StopAndUpdateDistance_CCW(double distance_per_rev, int com_port);
	SERVORS232COMM_API double Servo_ReturnCurrentPosition();
	SERVORS232COMM_API void Servo_GetLastPosition(double last_distance);

}

#if !defined(AFX_SERVORS232COMM_H__E2D16A4F_8F40_44C4_A6EC_926AB466C80B__INCLUDED_)
#define AFX_SERVORS232COMM_H__E2D16A4F_8F40_44C4_A6EC_926AB466C80B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cnComm.h"

#define SERVO_ACCELERATE		 100         // 伺服电机加速度
#define SERVO_DECELERATE		 100         // 伺服电机减速度
#define SERVO_VELOCITY			 5	         // 伺服电机速度
#define SERVO_RESOLUTION		 20000		 // 伺服电机分辨率
#define SERVO_REDUCTION			 5			 // 伺服电机减速比

#define LIFT_HEIGHT				 90		 // 滑台升降高度，单位cm

#define STRING_LEN               2048       // 普通字符串变量的长度
#define FEEDBACK_SIGN           "DONE"      // 电机反馈标识

class CServoRs232Comm : public CRs232Comm
{
public:
	CServoRs232Comm();
	virtual ~CServoRs232Comm();

private:

	bool  open_port();                         // 打开端口（每次发送命令时都需调用）
	void  close_port();                        // 关闭端口

	bool  feedback_success(int timeout);       // 电机执行指令后的反馈（若反馈超时，表明设备连接出状况）

public:

	bool  servo_init(int port, double velocity, int accelerate, int decelerate, int resolution);  // 设备初始化
	void  servo_fini();

	// 电机直接控制函数
	bool  servo_move_sensor_home_cw(int com_port = 3, bool wait_feedback = true);	 // 顺时针转动电机回到光电开关位置
	bool  servo_move_sensor_home_ccw(int com_port = 3, bool wait_feedback = true);	 // 逆时针转动电机回到光电开关位置
	bool  servo_move_cw(int com_port = 3, bool wait_feedback = true);	 // 顺时针转动电机,降
	bool  servo_move_ccw(int com_port = 3, bool wait_feedback = true);	 // 逆时针转动电机,升
	bool  servo_stop_and_update_distance_cw(double distance_per_rev, int com_port = 3, bool wait_feedback = true);	// 停止电机顺时针转动，并更新位置
	bool  servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port = 3, bool wait_feedback = true);	// 停止电机逆时针转动，并更新位置
	bool  servo_move_distance(double distance, double distance_per_rev, int com_port = 3, bool wait_feedback = true);	 //	移动滑台一定距离

	void  servo_get_last_position(double last_distance);	// 获取上次关机器时的滑台位置 _
	double	servo_return_current_position();							 // 返回滑台当前位置

	static void  wait(int millisec);					// 等待（feedback_success中调用，为避免太过频繁访问串口而引起拥塞）

	bool init_devices(int com_port = 3);								// 初始化设备
	void fini_devices();								// 终止设备

protected:
	virtual int get_feedback_timeout();


private:
	int     m_port;
	int     m_accelerate;       //电机加速度
	int     m_decelerate;       //电机减速度
	double  m_velocity;         //电机速度
	int		m_resolution;		//电机分辨率

	int		m_timestart;		//记录电机开始转动时间
	int		m_timestop;			//记录电机停止转动时间

	double	m_distance;			//滑台当前的高度，单位cm

};

CServoRs232Comm m_servo;

extern SERVORS232COMM_API int nServoRs232Comm;

SERVORS232COMM_API int fnServoRs232Comm(void);	

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_MoveSensorHome_CW()
// 描述：正转电机至底部光电开关
// 输入：com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveSensorHome_CW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// 正转电机至底部光电开关
	ret = m_servo.servo_move_sensor_home_cw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_MoveSensorHome_CCW()
// 描述：反转电机至底部光电开关
// 输入：com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveSensorHome_CCW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// 反转电机至底部光电开关
	ret = m_servo.servo_move_sensor_home_ccw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_Move_CW()
// 描述：正转电机, 下降
// 输入：com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_Move_CW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// 正转电机至底部光电开关
	ret = m_servo.servo_move_cw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_Move_CCW()
// 描述：反转电机，上升
// 输入：com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_Move_CCW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// 反转电机至底部光电开关
	ret = m_servo.servo_move_ccw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_MoveDistance()
// 描述：电机带动滑台升降高度
// 输入：distance(需要达到的高度), distance_per_rev(为同步轮转一圈滑台升降的高度，义乌义正为10cm深圳久宸为12.5cm), com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveDistance(double distance, double distance_per_rev, int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// 反转电机至底部光电开关
	ret = m_servo.servo_move_distance(distance, distance_per_rev, com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_StopAndUpdateDistance_CW()
// 描述：停止电机顺时针转动并更新位置
// 输入：distance_per_rev(为同步轮转一圈滑台升降的高度，义乌义正为10cm深圳久宸为12.5cm), com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_StopAndUpdateDistance_CW(double distance_per_rev, int com_port)
{
	//CServoRs232Comm m_servo;
	bool ret = false;

	ret = m_servo.servo_stop_and_update_distance_cw(distance_per_rev, com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_StopAndUpdateDistance_CCW()
// 描述：停止电机逆时针转动并更新位置
// 输入：distance_per_rev(为同步轮转一圈滑台升降的高度，义乌义正为10cm深圳久宸为12.5cm), com_port(串口序号，默认3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_StopAndUpdateDistance_CCW(double distance_per_rev, int com_port)
{
	//CServoRs232Comm m_servo;
	bool ret = false;

	ret = m_servo.servo_stop_and_update_distance_ccw(distance_per_rev, com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_ReturnCurrentPosition()
// 描述：返回滑台当前所在的位置
// 输出：current_position(当前滑台所在的位置)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API double Servo_ReturnCurrentPosition()
{
	//CServoRs232Comm m_servo;

	// 返回当前滑台所在的位置
	double current_position = m_servo.servo_return_current_position();

	return current_position;
}

////////////////////////////////////////////////////////////////////////////
// 函数：Servo_GetLastPosition()
// 描述：从配置文件获取上次关机的滑台
// 输出：
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API void Servo_GetLastPosition(double last_distance)
{
	//CServoRs232Comm m_servo;

	m_servo.servo_get_last_position(last_distance);

}

#endif
