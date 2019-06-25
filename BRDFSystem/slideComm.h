/////////////////////////////材质台导轨伺服电机驱动控制类//////////////////////////////////////
#pragma once
#ifndef SLIDECOMM_H
#define SLIDECOMM_H

#include "stdafx.h"
#include "cnComm.h"

#define SERVO_FEEDBACK           "DONE"      // 电机反馈标识
#define SERVO_ACCELERATE		 100         // 伺服电机加速度
#define SERVO_DECELERATE		 100         // 伺服电机减速度
#define SERVO_VELOCITY			 5	         // 伺服电机速度
#define SERVO_RESOLUTION		 20000		 // 伺服电机分辨率
#define SERVO_REDUCTION			 5			 // 伺服电机减速比
#define MOVE_DISTANCE			 150		 // 材质台移动距离，单位cm
#define STRING_LEN               2048       // 普通字符串变量的长度


class SlideComm : public CRs232Comm
{
public:
	SlideComm();
	virtual ~SlideComm();

	// 开启设备
	bool Init(int port, double velocity, int accelerate, int decelerate, int resolution);
	// 每次指令通信前再打开一次串口
	bool Init(int port);
	// 顺时针转动电机回到光电开关位置
	bool  servo_move_sensor_home_cw(int com_port, bool wait_feedback = true);
	// 逆时针转动电机回到光电开关位置
	bool  servo_move_sensor_home_ccw(int com_port, bool wait_feedback = true);
	//	移动滑台一定距离
	//bool  servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback = true);
	//关闭设备
	void Fini();
	
	// 顺时针转动电机,降
	//bool  servo_move_cw(int com_port = 3, bool wait_feedback = true);
	// 逆时针转动电机,升
	//bool  servo_move_ccw(int com_port = 3, bool wait_feedback = true);	 
	// 停止电机顺时针转动，并更新位置
	//bool  servo_stop_and_update_distance_cw(double distance_per_rev, int com_port, bool wait_feedback = true);
	// 停止电机逆时针转动，并更新位置
	//bool  servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port, bool wait_feedback = true);	
	//	移动滑台一定距离
	//bool  servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback = true);	 
	// 获取上次关机器时的滑台位置
	//void  servo_get_last_position(double last_distance);	 
						



protected:
	virtual int get_feedback_timeout();

private:
	// 电机执行指令后的反馈（若反馈超时，表明设备连接出状况）
	bool IsFinished(int timeout);  
	// 等待（feedback_success中调用，为避免太过频繁访问串口而引起拥塞）
	static void  Wait(int millisec);
	// 返回滑台当前位置
	double	servo_return_current_position();

	int     m_port;
	int     m_accelerate;       //电机加速度
	int     m_decelerate;       //电机减速度
	double  m_velocity;         //电机速度
	int		m_resolution;		//电机分辨率
	int		m_timestart;		//记录电机开始转动时间
	int		m_timestop;			//记录电机停止转动时间
	double	m_distance;			//滑台当前至起点的距离，单位cm
};
#endif
