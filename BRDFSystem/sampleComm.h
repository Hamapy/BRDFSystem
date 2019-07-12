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

#include <QSettings>
#include "stdafx.h"
#include "cnComm.h"

#define STEP_FEEDBACK      "DONE"    //���������ʶ
#define STEP_VELOCITY	   0.25		//����ٶ�
#define STEP_ACCELERATE    25       //������ٶ�
#define STEP_DECELERATE    25       //������ٶ�
#define STEP_RESOLUTION    3        //����ֱ���
#define STEP_TIMEOUT       3000   //��ʱ,����
#define STEP_SAFESTEP	   10000	 //��λ�����������,���ת��ô�ಽ���ͣ����
#define STEP_STRINGLEN	   256
#define STEP_TOHOME        63000  //��λ��Ҫ���ڵĲ��� ��Ʒ̨һȦԼ6300��

class SampleComm : public CRs232Comm
{
public:
	SampleComm();
	virtual ~SampleComm();
	
	//��ʼ���豸
    bool Init(int port = 2);
	//ÿ�η���ָ��ǰ�ٴγ�ʼ��ͨ�Ŷ˿�
	bool InitA();
	//��ת����һ�Ƕ�
	bool  GotoNextPos(int step);
	//��λ
	bool Reset();
	//ֹͣ�豸
	void Fini();

private:
	//ȷ�ϵ���Ѿ���ɵ�ǰָ��
	bool IsFinished(int waitTime);
	//ʱ��ȴ�����
	void Wait(int millisec);

	QSettings	*ini;
	int			_step;             //ת����һͨ�������Ҫ�ƶ��Ĳ���
	int			_homeadj;          //��λʱ�����Ҫ�ƶ��Ĳ���
    int			_port;             //������ӵĴ���	
	int			_accelerate;       //������ٶ�
	int			_decelerate;       //������ٶ�
	int			_resolution;       //����ֱ���
	double		_velocity;         //����ٶ�

};
#endif
