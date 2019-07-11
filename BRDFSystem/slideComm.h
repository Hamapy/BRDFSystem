/////////////////////////////����̨�����ŷ��������������//////////////////////////////////////
#pragma once
#ifndef SLIDECOMM_H
#define SLIDECOMM_H

#include "stdafx.h"
#include "cnComm.h"
#include <QSettings>

#define SERVO_FEEDBACK           "DONE"      // ���������ʶ
#define SERVO_ACCELERATE		 100         // �ŷ�������ٶ�
#define SERVO_DECELERATE		 100         // �ŷ�������ٶ�
#define SERVO_VELOCITY			 20	         // �ŷ�����ٶ�
#define SERVO_RESOLUTION		 20000		 // �ŷ�����ֱ���
#define SERVO_REDUCTION			 5			 // �ŷ�������ٱ�
#define MOVE_DISTANCE			 170		 // ����̨�ƶ������룬��λcm
#define STRING_LEN               2048       // ��ͨ�ַ��������ĳ���


class SlideComm : public CRs232Comm
{
public:
	SlideComm();
	virtual ~SlideComm();

	// �����豸
	bool Init(int port, double velocity, int accelerate, int decelerate, int resolution);
	// ÿ��ָ��ͨ��ǰ�ٴ�һ�δ���
	bool Init(int port);
	// ˳ʱ��(CW)ת�����X1��翪��λ�ô�������̨��λ��
	bool  MoveToX1(bool wait_feedback = true);
	// ˳ʱ��(CCW)ת�����X2��翪��λ�ô�������̨��λ��
	bool  MoveToX2(bool wait_feedback = true);
	//	�ƶ���̨һ������
	bool  MoveTest(double distance, bool wait_feedback = true);
	//�ر��豸
	void Fini();
	//�������������Ǳ�����Ԫ
	//friend class MainWindow;

private:
	// ���ִ��ָ���ķ�������������ʱ�������豸���ӳ�״����
	bool IsFinished(int timeout);  
	// �ȴ���IsFinished�е��ã�Ϊ����̫��Ƶ�����ʴ��ڶ�����ӵ����
	static void  Wait(int millisec);

	QSettings	*ini;
	int     _port;
	int     _accelerate;       //������ٶ�
	int     _decelerate;       //������ٶ�
	double  _velocity;         //����ٶ�
	int		_resolution;		//����ֱ���
	//int		_timestart;			//��¼�����ʼת��ʱ��
	//int		_timestop;			//��¼���ֹͣת��ʱ��
	double	_distance;			//��̨��ǰ�����ľ��룬��λcm
};
#endif
