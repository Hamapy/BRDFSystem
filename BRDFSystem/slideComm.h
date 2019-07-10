/////////////////////////////材质台导轨伺服电机驱动控制类//////////////////////////////////////
#pragma once
#ifndef SLIDECOMM_H
#define SLIDECOMM_H

#include "stdafx.h"
#include "cnComm.h"
#include <QSettings>

#define SERVO_FEEDBACK           "DONE"      // 电机反馈标识
#define SERVO_ACCELERATE		 100         // 伺服电机加速度
#define SERVO_DECELERATE		 100         // 伺服电机减速度
#define SERVO_VELOCITY			 20	         // 伺服电机速度
#define SERVO_RESOLUTION		 20000		 // 伺服电机分辨率
#define SERVO_REDUCTION			 5			 // 伺服电机减速比
#define MOVE_DISTANCE			 170		 // 材质台移动最大距离，单位cm
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
	// 顺时针(CW)转动电机X1光电开关位置处（材质台归位）
	bool  MoveToX1(bool wait_feedback = true);
	// 顺时针(CCW)转动电机X2光电开关位置处（材质台就位）
	bool  MoveToX2(bool wait_feedback = true);
	//	移动滑台一定距离
	bool  MoveTest(double distance, bool wait_feedback = true);
	//关闭设备
	void Fini();
	//声明主界面类是本类友元
	//friend class MainWindow;

private:
	// 电机执行指令后的反馈（若反馈超时，表明设备连接出状况）
	bool IsFinished(int timeout);  
	// 等待（IsFinished中调用，为避免太过频繁访问串口而引起拥塞）
	static void  Wait(int millisec);

	QSettings	*ini;
	int     _port;
	int     _accelerate;       //电机加速度
	int     _decelerate;       //电机减速度
	double  _velocity;         //电机速度
	int		_resolution;		//电机分辨率
	//int		_timestart;			//记录电机开始转动时间
	//int		_timestop;			//记录电机停止转动时间
	double	_distance;			//滑台当前至起点的距离，单位cm
};
#endif
