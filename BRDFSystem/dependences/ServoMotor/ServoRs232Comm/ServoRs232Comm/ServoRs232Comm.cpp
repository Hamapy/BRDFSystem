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
// ����:    init
// ����:    �����ʼ��
// ����:    com_port:     �����Ҫ���ӵĴ���
//          velocity:     ����ٶ�
//          accelerate:   ������ٶ�
//          decelerate:   ������ٶ�
//          resolution��  ����ֱ���  
// ��ע:   
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::servo_init(int port, double velocity, int accelerate, int decelerate, int resolution)
{
	bool ret = false;

	m_accelerate = accelerate;
	m_velocity = velocity;
	m_decelerate = decelerate;
	m_resolution = resolution;
	m_port = port;

	ret = open_port();  // �򿪴���
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", FEEDBACK_SIGN);
		Write(cmd);  // ���õ��Ϊ���������
		ret = feedback_success(get_feedback_timeout());
	}

	if (ret)
	{
		//�����ŷ��������
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, FEEDBACK_SIGN);
		Write(cmd);

		ret = feedback_success(get_feedback_timeout());
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// ����:    fini
// ����:    �����ֹ��  
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::servo_fini()
{
	// �رմ���
	close_port();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ����:    open_port
// ����:    �򿪴��� 
// ��ע:    ÿ���������ͨ��ǰ��ִ�д˺�����Ϊ�ڴ�����������ٲ�������£��ܼ�ʱ�������������������
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
// ����:    close_port
// ����:    �رմ���  
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::close_port()
{
	if (IsOpen())
	{
		Close();
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////
// ����:    feedback_success()
// ����:    ��ѯ����Ƿ��Ѿ���ɵ�ǰָ��
// ����:    ��Ҫ�ȴ���ʱ��   
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
		wait(WAITDELAY);//���⴮�ڷ�æ,�ȴ�200ms

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
// ����:    wait()
// ����:    ʱ��ȴ�����
// ����:    millisec:  �ȴ�������   
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
// ����:    servo_move_sensor_home_cw();
// ����:    ˳ʱ��ת������ص���翪��λ��
//**********************************************************************//
bool CServoRs232Comm::servo_move_sensor_home_cw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// ��ʼ���豸
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
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	// ��������翪��λ��
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
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	// ��¼λ��
	if (ret)
	{
		m_distance = 0;
	}	
	
	// ��ֹ���豸
	fini_devices();

	return ret;
}


//**********************************************************************//
// ����:    servo_move_sensor_home_ccw();
// ����:    ��ʱ��ת������ص���翪��λ��
//**********************************************************************//
bool CServoRs232Comm::servo_move_sensor_home_ccw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// �豸��ʼ��
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
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	// ��������翪��λ��
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
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	if (ret)
	{
		// ��¼λ��
		m_distance = LIFT_HEIGHT;
	}

	// ��ֹ���豸
	fini_devices();

	return ret;
}

//**********************************************************************//
// ����:    servo_move_cw();
// ����:    ˳ʱ��ת���������
//**********************************************************************//
bool CServoRs232Comm::servo_move_cw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// ��ʼ���豸
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS1L\r", 1000);

		m_timestart = GetTickCount();   //��¼��ʼת��ʱ��

		Write(cmd);

		// �ڷ���һ��ָ��ǰ��΢��һ��
		wait(200);
	}

	// ��ֹ���豸
	fini_devices();

	return ret;
}


//**********************************************************************//
// ����:    servo_move_ccw();
// ����:    ��ʱ��ת���������
//**********************************************************************//
bool CServoRs232Comm::servo_move_ccw(int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// �豸��ʼ��
	m_port = com_port;
	ret = init_devices(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, FEEDBACK_SIGN);

		m_timestart = GetTickCount(); //��¼��ʼת��ʱ��

		Write(cmd);

		// �ڷ���һ��ָ��ǰ��΢��һ��
		wait(200);
	}

	// ��ֹ���豸
	fini_devices();

	return ret;
}


//**********************************************************************//
// ����:   servo_stop_and_update_distance_cw();
// ����:   ֹͣ���˳ʱ��ת����������λ��
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

		m_timestop = GetTickCount();  //��¼ֹͣʱ��

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	//����λ��
	move_time = m_timestop - m_timestart;
	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
	m_distance = m_distance - move_distance;

	// �ж��Ƿ��ѳ�����翪��λ��
	if (m_distance < 0)
	{
		// ��������翪��λ��
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}

		if (ret)
		{
			// λ����Ϊ0
			m_distance = 0;
		}
	}

	// ��ֹ���豸
	fini_devices();

	return ret;
}

//**********************************************************************//
// ����:   servo_stop_and_update_distance_ccw();
// ����:   ֹͣ�����ʱ��ת����������λ��
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

		m_timestop = GetTickCount();  //��¼ֹͣʱ��

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}
	}

	//����λ��
	move_time = m_timestop - m_timestart;
	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
	m_distance = m_distance + move_distance;

	// �ж��Ƿ��ѳ�����翪��λ��
	if (m_distance > LIFT_HEIGHT)
	{
		// ��������翪��λ��
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, FEEDBACK_SIGN);
		Write(cmd);

		if (wait_feedback)
		{
			ret = feedback_success(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			wait(200);
		}

		if (ret)
		{
			// λ����Ϊ���
			m_distance = LIFT_HEIGHT;
		}
	}

	// ��ֹ���豸
	fini_devices();

	return ret;
}

//**********************************************************************//
// ����:    servo_move_distance();
// ����:		distance_per_rev��Ϊͬ����תһȦ��̨�����ĸ߶ȣ���������Ϊ10cm���ھ��Ϊ12.5cm
// ����:    �ƶ���̨һ������
//**********************************************************************//
bool CServoRs232Comm::servo_move_distance(double distance, double distance_per_rev, int com_port, bool wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	double move_distance; //��Ҫ�ƶ��ĸ߶�
	int move_pulses; //��Ҫת�������岽��SI

	// ��ʼ���豸
	m_port = com_port;
	ret = init_devices(m_port);

	int rlr = m_resolution / 2; //��R���Ĵ���ƥ���ŷ��������е�EG����

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
				// �ڷ���һ��ָ��ǰ��΢��һ��
				wait(200);
			}
		}
		if (ret)
		{
			// ����λ����Ϣ
			m_distance = distance;
		}

		// ��ֹ���豸
		fini_devices();
	}

	return ret;
}

//**********************************************************************//
// ����:    servo_return_current_position();						 
// ����:    ���ص�ǰ��̨λ��
//**********************************************************************//
double CServoRs232Comm::servo_return_current_position()
{
	return m_distance;
}

//**********************************************************************//
// ����:    servo_get_last_position();						 
// ����:    ��ȡ�ϴιػ���ʱ��̨���ڵ�λ��
//**********************************************************************//
void CServoRs232Comm::servo_get_last_position(double last_distance)
{
	m_distance = last_distance;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ����:    get_feedback_timeout
// ����:    ���ɫ�����ӵĳ�ʱ��ֵ
////////////////////////////////////////////////////////////////////////////////////////////////
int CServoRs232Comm::get_feedback_timeout()
{
	const int	servo_timeout = 10000;

	return servo_timeout;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// ����:   init_devices()
// ����:   ��ʼ���豸����
////////////////////////////////////////////////////////////////////////////////////////////////
bool CServoRs232Comm::init_devices(int com_port)
{
	bool ret = false;

	// �ŷ������ʼ��
	ret = servo_init(com_port, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);

	if (!ret)
	{
		//MessageBox(NULL, "Fail to connect the focus motor, please check.", "IAS", MB_ICONWARNING);
		ret = false;
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// ����:   fini_devices()
// ����:   ��ֹ�豸����
////////////////////////////////////////////////////////////////////////////////////////////////
void CServoRs232Comm::fini_devices()
{
	servo_fini();
}

