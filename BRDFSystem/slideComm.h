/////////////////////////////����̨�����ŷ��������������//////////////////////////////////////
#pragma once
#ifndef SLIDECOMM_H
#define SLIDECOMM_H

#include "stdafx.h"
#include "cnComm.h"

#define SERVO_FEEDBACK           "DONE"      // ���������ʶ
#define SERVO_ACCELERATE		 100         // �ŷ�������ٶ�
#define SERVO_DECELERATE		 100         // �ŷ�������ٶ�
#define SERVO_VELOCITY			 5	         // �ŷ�����ٶ�
#define SERVO_RESOLUTION		 20000		 // �ŷ�����ֱ���
#define SERVO_REDUCTION			 5			 // �ŷ�������ٱ�
#define MOVE_DISTANCE			 150		 // ����̨�ƶ����룬��λcm
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
	// ˳ʱ��ת������ص���翪��λ��
	bool  servo_move_sensor_home_cw(int com_port, bool wait_feedback = true);
	// ��ʱ��ת������ص���翪��λ��
	bool  servo_move_sensor_home_ccw(int com_port, bool wait_feedback = true);
	//	�ƶ���̨һ������
	//bool  servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback = true);
	//�ر��豸
	void Fini();
	
	// ˳ʱ��ת�����,��
	//bool  servo_move_cw(int com_port = 3, bool wait_feedback = true);
	// ��ʱ��ת�����,��
	//bool  servo_move_ccw(int com_port = 3, bool wait_feedback = true);	 
	// ֹͣ���˳ʱ��ת����������λ��
	//bool  servo_stop_and_update_distance_cw(double distance_per_rev, int com_port, bool wait_feedback = true);
	// ֹͣ�����ʱ��ת����������λ��
	//bool  servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port, bool wait_feedback = true);	
	//	�ƶ���̨һ������
	//bool  servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback = true);	 
	// ��ȡ�ϴιػ���ʱ�Ļ�̨λ��
	//void  servo_get_last_position(double last_distance);	 
						



protected:
	virtual int get_feedback_timeout();

private:
	// ���ִ��ָ���ķ�������������ʱ�������豸���ӳ�״����
	bool IsFinished(int timeout);  
	// �ȴ���feedback_success�е��ã�Ϊ����̫��Ƶ�����ʴ��ڶ�����ӵ����
	static void  Wait(int millisec);
	// ���ػ�̨��ǰλ��
	double	servo_return_current_position();

	int     m_port;
	int     m_accelerate;       //������ٶ�
	int     m_decelerate;       //������ٶ�
	double  m_velocity;         //����ٶ�
	int		m_resolution;		//����ֱ���
	int		m_timestart;		//��¼�����ʼת��ʱ��
	int		m_timestop;			//��¼���ֹͣת��ʱ��
	double	m_distance;			//��̨��ǰ�����ľ��룬��λcm
};
#endif
