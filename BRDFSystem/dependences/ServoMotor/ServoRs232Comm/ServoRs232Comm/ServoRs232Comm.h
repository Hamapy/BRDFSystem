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

#define SERVO_ACCELERATE		 100         // �ŷ�������ٶ�
#define SERVO_DECELERATE		 100         // �ŷ�������ٶ�
#define SERVO_VELOCITY			 5	         // �ŷ�����ٶ�
#define SERVO_RESOLUTION		 20000		 // �ŷ�����ֱ���
#define SERVO_REDUCTION			 5			 // �ŷ�������ٱ�

#define LIFT_HEIGHT				 90		 // ��̨�����߶ȣ���λcm

#define STRING_LEN               2048       // ��ͨ�ַ��������ĳ���
#define FEEDBACK_SIGN           "DONE"      // ���������ʶ

class CServoRs232Comm : public CRs232Comm
{
public:
	CServoRs232Comm();
	virtual ~CServoRs232Comm();

private:

	bool  open_port();                         // �򿪶˿ڣ�ÿ�η�������ʱ������ã�
	void  close_port();                        // �رն˿�

	bool  feedback_success(int timeout);       // ���ִ��ָ���ķ�������������ʱ�������豸���ӳ�״����

public:

	bool  servo_init(int port, double velocity, int accelerate, int decelerate, int resolution);  // �豸��ʼ��
	void  servo_fini();

	// ���ֱ�ӿ��ƺ���
	bool  servo_move_sensor_home_cw(int com_port = 3, bool wait_feedback = true);	 // ˳ʱ��ת������ص���翪��λ��
	bool  servo_move_sensor_home_ccw(int com_port = 3, bool wait_feedback = true);	 // ��ʱ��ת������ص���翪��λ��
	bool  servo_move_cw(int com_port = 3, bool wait_feedback = true);	 // ˳ʱ��ת�����,��
	bool  servo_move_ccw(int com_port = 3, bool wait_feedback = true);	 // ��ʱ��ת�����,��
	bool  servo_stop_and_update_distance_cw(double distance_per_rev, int com_port = 3, bool wait_feedback = true);	// ֹͣ���˳ʱ��ת����������λ��
	bool  servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port = 3, bool wait_feedback = true);	// ֹͣ�����ʱ��ת����������λ��
	bool  servo_move_distance(double distance, double distance_per_rev, int com_port = 3, bool wait_feedback = true);	 //	�ƶ���̨һ������

	void  servo_get_last_position(double last_distance);	// ��ȡ�ϴιػ���ʱ�Ļ�̨λ�� _
	double	servo_return_current_position();							 // ���ػ�̨��ǰλ��

	static void  wait(int millisec);					// �ȴ���feedback_success�е��ã�Ϊ����̫��Ƶ�����ʴ��ڶ�����ӵ����

	bool init_devices(int com_port = 3);								// ��ʼ���豸
	void fini_devices();								// ��ֹ�豸

protected:
	virtual int get_feedback_timeout();


private:
	int     m_port;
	int     m_accelerate;       //������ٶ�
	int     m_decelerate;       //������ٶ�
	double  m_velocity;         //����ٶ�
	int		m_resolution;		//����ֱ���

	int		m_timestart;		//��¼�����ʼת��ʱ��
	int		m_timestop;			//��¼���ֹͣת��ʱ��

	double	m_distance;			//��̨��ǰ�ĸ߶ȣ���λcm

};

CServoRs232Comm m_servo;

extern SERVORS232COMM_API int nServoRs232Comm;

SERVORS232COMM_API int fnServoRs232Comm(void);	

////////////////////////////////////////////////////////////////////////////
// ������Servo_MoveSensorHome_CW()
// ��������ת������ײ���翪��
// ���룺com_port(������ţ�Ĭ��3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveSensorHome_CW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// ��ת������ײ���翪��
	ret = m_servo.servo_move_sensor_home_cw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_MoveSensorHome_CCW()
// ��������ת������ײ���翪��
// ���룺com_port(������ţ�Ĭ��3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveSensorHome_CCW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// ��ת������ײ���翪��
	ret = m_servo.servo_move_sensor_home_ccw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_Move_CW()
// ��������ת���, �½�
// ���룺com_port(������ţ�Ĭ��3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_Move_CW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// ��ת������ײ���翪��
	ret = m_servo.servo_move_cw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_Move_CCW()
// ��������ת���������
// ���룺com_port(������ţ�Ĭ��3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_Move_CCW(int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// ��ת������ײ���翪��
	ret = m_servo.servo_move_ccw(com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_MoveDistance()
// ���������������̨�����߶�
// ���룺distance(��Ҫ�ﵽ�ĸ߶�), distance_per_rev(Ϊͬ����תһȦ��̨�����ĸ߶ȣ���������Ϊ10cm���ھ��Ϊ12.5cm), com_port(������ţ�Ĭ��3)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API bool Servo_MoveDistance(double distance, double distance_per_rev, int com_port)
{
	bool ret = false;
	//CServoRs232Comm m_servo;

	// ��ת������ײ���翪��
	ret = m_servo.servo_move_distance(distance, distance_per_rev, com_port);

	return ret;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_StopAndUpdateDistance_CW()
// ������ֹͣ���˳ʱ��ת��������λ��
// ���룺distance_per_rev(Ϊͬ����תһȦ��̨�����ĸ߶ȣ���������Ϊ10cm���ھ��Ϊ12.5cm), com_port(������ţ�Ĭ��3)
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
// ������Servo_StopAndUpdateDistance_CCW()
// ������ֹͣ�����ʱ��ת��������λ��
// ���룺distance_per_rev(Ϊͬ����תһȦ��̨�����ĸ߶ȣ���������Ϊ10cm���ھ��Ϊ12.5cm), com_port(������ţ�Ĭ��3)
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
// ������Servo_ReturnCurrentPosition()
// ���������ػ�̨��ǰ���ڵ�λ��
// �����current_position(��ǰ��̨���ڵ�λ��)
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API double Servo_ReturnCurrentPosition()
{
	//CServoRs232Comm m_servo;

	// ���ص�ǰ��̨���ڵ�λ��
	double current_position = m_servo.servo_return_current_position();

	return current_position;
}

////////////////////////////////////////////////////////////////////////////
// ������Servo_GetLastPosition()
// �������������ļ���ȡ�ϴιػ��Ļ�̨
// �����
// Modified by 
////////////////////////////////////////////////////////////////////////////
extern "C" SERVORS232COMM_API void Servo_GetLastPosition(double last_distance)
{
	//CServoRs232Comm m_servo;

	m_servo.servo_get_last_position(last_distance);

}

#endif
