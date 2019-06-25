///////////////////////////////////������Ʒ̨�����������������/////////////////////////////////
//���ܣ����Ʋ������������MSST5 - S, �Ӷ�����AMP�����ɲ���̨��Ʒƽ����ת

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

#pragma once
#ifndef SAMPLECOMM_H
#define SAMPLECOMM_H

#include "stdafx.h"
#include "cnComm.h"

#define STEP_FEEDBACK      "DONE"    //���������ʶ
#define STEP_ACCELERATE    10       //������ٶ�
#define STEP_DECELERATE    10       //������ٶ�
#define STEP_RESOLUTION    8        //����ֱ���
#define STEP_TIMEOUT       120000   //��ʱ,����
#define STEP_SAFESTEP	   500000	 //��λ�����������,���ת��ô�ಽ���ͣ����

class SampleComm : public CRs232Comm
{
public:
	SampleComm();
	virtual ~SampleComm();
	
	//��ʼ���豸
    bool Init(int wheel_port, int wheel_step, double velocity, int accelerate, int decelerate, int resolution, int wheel_homeadj);
	bool Init(int wheel_port);
	//��ת����һ�Ƕ�
	bool  GotoNextPos(int step);
	//�����ٶ�
	//bool SetVel(double v);
	//��λ
	bool Reset();
	//ֹͣ�豸
	void Fini();



private:
	int m_step;             //ת����һͨ�������Ҫ�ƶ��Ĳ���
	int m_homeadj;          //��λʱ�����Ҫ�ƶ��Ĳ���
    int m_port;             //������ӵĴ���	
	int m_accelerate;       //������ٶ�
	int m_decelerate;       //������ٶ�
	int m_resolution;       //����ֱ���
	double m_velocity;      //����ٶ�

	//ȷ�ϵ���Ѿ���ɵ�ǰָ��
	bool IsFinished(int wait_time);
	//ʱ��ȴ�����
	void Wait(int millisec);

};
#endif
