#include "slideComm.h"

/////////////////////////////////////���캯��//////////////////////////////////////////
SlideComm::SlideComm()
{
	m_accelerate = 0;
	m_decelerate = 0;
	m_resolution = 0;
	m_velocity = 0;
	m_distance = 0;
	m_port = 0;
}

SlideComm::~SlideComm()
{
	Fini();
}

///////////////////////////////////���г�Ա����/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������Init
// �����������ʼ��
// ����:    port:     �����Ҫ���ӵĴ���
//          velocity:     ����ٶ�
//          accelerate:   ������ٶ�
//          decelerate:   ������ٶ�
//          resolution��  ����ֱ���  
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::Init(int port, double velocity, int accelerate, int decelerate, int resolution)
{
	bool ret = true;

	m_accelerate = accelerate;
	m_velocity = velocity;
	m_decelerate = decelerate;
	m_resolution = resolution;
	m_port = port;

	// �򿪴���
	if (IsOpen())
	{
		Close();
	}
	if (!Open(m_port))
	{
		return false;
	}
	
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode��Q/SCLģʽ�ϵ�
		Write(cmd);  // ���õ��Ϊ���������
		//ret = IsFinished(get_feedback_timeout());
	}

	if (ret)
	{
		//�����ŷ��������
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(get_feedback_timeout());
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������Init
// �����������ʼ��
// ����:    port:     �����Ҫ���ӵĴ���
//          velocity:     ����ٶ�
//          accelerate:   ������ٶ�
//          decelerate:   ������ٶ�
//          resolution��  ����ֱ���  
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::Init(int port)
{
	//bool ret = true;

	// �򿪴���
	if (IsOpen())
		Close();
	if (Open(m_port))
		return true;
	else
		return false;

	/*
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // ���õ��Ϊ���������
		ret = IsFinished(get_feedback_timeout());
	}

	if (ret)
	{
		//�����ŷ��������
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", m_accelerate, m_velocity, m_decelerate, m_resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(get_feedback_timeout());
	}
	*/
	//return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������Fini
// �����������ֹ����
// ����:    port:     �����Ҫ���ӵĴ���
//          velocity:     ����ٶ�
//          accelerate:   ������ٶ�
//          decelerate:   ������ٶ�
//          resolution��  ����ֱ���  
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void SlideComm::Fini()
{
	// �رմ���
	if (IsOpen())
	{
		Close();
	}
}
///////////////////////////////////˽�г�Ա����/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������IsFinished()
// ��������ѯ����Ƿ��Ѿ���ɵ�ǰָ��
// ����: timeout ��Ҫ�ȴ���ʱ��
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SlideComm::IsFinished(int timeout)
{
#define READ_WAITTIME    20
#define WAITDELAY        200

	bool ret = false;
	char rdstr[STRING_LEN];
	int  time_start = GetTickCount();


	while (1)
	{
		Wait(WAITDELAY);//���⴮�ڷ�æ,�ȴ�200ms

		Read(rdstr, STRING_LEN, READ_WAITTIME);

		if (strstr(rdstr, SERVO_FEEDBACK))
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
// ����:    Wait()
// ����:    ʱ��ȴ�����
// ����:    millisec:  �ȴ�������   
////////////////////////////////////////////////////////////////////////////////////////////////
void SlideComm::Wait(int millisec)
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
bool SlideComm::servo_move_sensor_home_cw(int com_port, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// ��ʼ���豸
	m_port = com_port;
	ret = Init(m_port);

	if (ret)
	{
		ClearInputBuffer();
		/*
		STD ����ֹͣ
		DI ����λ��
		FS1L ������1�Ĵ����ź�ΪLOWʱ������ֹͣ
		SS �����ַ���
		*/
		sprintf(cmd, "STD\rDI%d\rFS1L\rSS%s\r", 1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	// ��������翪��λ��
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	// ��¼λ��
	if (ret)
	{
		m_distance = 0;
	}

	// ��ֹ���豸
	Fini();

	return ret;
}


//**********************************************************************//
// ����:    servo_move_sensor_home_ccw();
// ����:    ��ʱ��ת������ص���翪��λ��
//**********************************************************************//
bool SlideComm::servo_move_sensor_home_ccw(int com_port, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// �豸��ʼ��
	m_port = com_port;
	ret = Init(m_port);

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	// ��������翪��λ��
	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(get_feedback_timeout());
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	if (ret)
	{
		// ��¼λ��
		m_distance = MOVE_DISTANCE;
	}

	// ��ֹ���豸
	Fini();

	return ret;
}


////// ����:    servo_move_cw();
////// ����:    ˳ʱ��ת���������
////
////bool SlideComm::servo_move_cw(int com_port, bool Wait_feedback/* = true*/)
////{
////	char cmd[STRING_LEN];
////	bool ret = false;
////
////	// ��ʼ���豸
////	m_port = com_port;
////	ret = init_devices(m_port);
////
////	if (ret)
////	{
////		ClearInputBuffer();
////		sprintf(cmd, "STD\rDI%d\rFS1L\r", 1000);
////
////		m_timestart = GetTickCount();   //��¼��ʼת��ʱ��
////
////		Write(cmd);
////
////		// �ڷ���һ��ָ��ǰ��΢��һ��
////		Wait(200);
////	}
////
////	// ��ֹ���豸
////	Fini();
////
////	return ret;
////}
////
////
////
////// ����:    servo_move_ccw();
////// ����:    ��ʱ��ת���������
////bool SlideComm::servo_move_ccw(int com_port, bool Wait_feedback/* = true*/)
////{
////	char cmd[STRING_LEN];
////	bool ret = false;
////
////	// �豸��ʼ��
////	m_port = com_port;
////	ret = init_devices(m_port);
////
////	if (ret)
////	{
////		ClearInputBuffer();
////		sprintf(cmd, "STD\rDI%d\rFS2L\rSS%s\r", -1000, SERVO_FEEDBACK);
////
////		m_timestart = GetTickCount(); //��¼��ʼת��ʱ��
////
////		Write(cmd);
////
////		// �ڷ���һ��ָ��ǰ��΢��һ��
////		Wait(200);
////	}
////
////	// ��ֹ���豸
////	Fini();
////
////	return ret;
////}
//
//
////**********************************************************************//
//// ����:   servo_stop_and_update_distance_cw();
//// ����:   ֹͣ���˳ʱ��ת����������λ��
////**********************************************************************//
//bool SlideComm::servo_stop_and_update_distance_cw(double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	int move_time = 0;
//	double move_distance;
//
//	m_port = com_port;
//	//ret = open_port();
//
//	if (ret)
//	{
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rSS%s\r", SERVO_FEEDBACK);
//		Write(cmd);
//
//		m_timestop = GetTickCount();  //��¼ֹͣʱ��
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// �ڷ���һ��ָ��ǰ��΢��һ��
//			Wait(200);
//		}
//	}
//
//	//����λ��
//	move_time = m_timestop - m_timestart;
//	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
//	m_distance = m_distance - move_distance;
//
//	// �ж��Ƿ��ѳ�����翪��λ��
//	if (m_distance < 0)
//	{
//		// ��������翪��λ��
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", -1000, SERVO_FEEDBACK);
//		Write(cmd);
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// �ڷ���һ��ָ��ǰ��΢��һ��
//			Wait(200);
//		}
//
//		if (ret)
//		{
//			// λ����Ϊ0
//			m_distance = 0;
//		}
//	}
//
//	// ��ֹ���豸
//	Fini();
//
//	return ret;
//}
//
////**********************************************************************//
//// ����:   servo_stop_and_update_distance_ccw();
//// ����:   ֹͣ�����ʱ��ת����������λ��
////**********************************************************************//
//bool SlideComm::servo_stop_and_update_distance_ccw(double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	int move_time = 0;
//	double move_distance;
//
//	m_port = com_port;
//	//ret = open_port();
//
//	if (ret)
//	{
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rSS%s\r", SERVO_FEEDBACK);
//		Write(cmd);
//
//		m_timestop = GetTickCount();  //��¼ֹͣʱ��
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// �ڷ���һ��ָ��ǰ��΢��һ��
//			Wait(200);
//		}
//	}
//
//	//����λ��
//	move_time = m_timestop - m_timestart;
//	move_distance = (double)(move_time * distance_per_rev * SERVO_VELOCITY / SERVO_REDUCTION / 1000);
//	m_distance = m_distance + move_distance;
//
//	// �ж��Ƿ��ѳ�����翪��λ��
//	if (m_distance > MOVE_DISTANCE)
//	{
//		// ��������翪��λ��
//		ClearInputBuffer();
//		sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", 1000, SERVO_FEEDBACK);
//		Write(cmd);
//
//		if (Wait_feedback)
//		{
//			ret = IsFinished(get_feedback_timeout());
//		}
//		else
//		{
//			// �ڷ���һ��ָ��ǰ��΢��һ��
//			Wait(200);
//		}
//
//		if (ret)
//		{
//			// λ����Ϊ���
//			m_distance = MOVE_DISTANCE;
//		}
//	}
//
//	// ��ֹ���豸
//	Fini();
//
//	return ret;
//}
////**********************************************************************//
//// ����:    servo_move_distance();
//// ����:		distance_per_rev��Ϊͬ����תһȦ��̨�����ĸ߶ȣ���������Ϊ10cm���ھ��Ϊ12.5cm
//// ����:    �ƶ���̨һ������
////**********************************************************************//
//bool SlideComm::servo_move_distance(double distance, double distance_per_rev, int com_port, bool Wait_feedback/* = true*/)
//{
//	char cmd[STRING_LEN];
//	bool ret = false;
//	double move_distance; //��Ҫ�ƶ��ĸ߶�
//	int move_pulses; //��Ҫת�������岽��SI
//	int rlr = m_resolution / 2; //��R���Ĵ���ƥ���ŷ��������е�EG����
//
//	//double d = MOVE_DISTANCE;
//	if (distance < 0 || distance > MOVE_DISTANCE)
//	{
//		//MessageBox(NULL, "Please set the lift height between 0 to 120 cm!", "IAS", MB_ICONWARNING);
//		return ret;
//	}
//	else
//	{
//		move_distance = m_distance - distance;
//		move_pulses = static_cast<int> (move_distance * rlr * SERVO_REDUCTION / distance_per_rev);  
//
//		// ��ʼ���豸
//		ret = Init(com_port);
//
//		if (ret)
//		{
//			ClearInputBuffer();
//			sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", move_pulses, SERVO_FEEDBACK);
//			Write(cmd);
//
//			if (Wait_feedback)
//			{
//				ret = IsFinished(get_feedback_timeout());
//			}
//			else
//			{
//				// �ڷ���һ��ָ��ǰ��΢��һ��
//				Wait(200);
//			}
//		}
//		if (ret)
//		{
//			// ����λ����Ϣ
//			m_distance = distance;
//		}
//
//		// ��ֹ���豸
//		Fini();
//	}
//
//	return ret;
//}

//**********************************************************************//
// ����:    servo_return_current_position();						 
// ����:    ���ص�ǰ��̨λ��
//**********************************************************************//
double SlideComm::servo_return_current_position()
{
	return m_distance;
}

////**********************************************************************//
//// ����:    servo_get_last_position();						 
//// ����:    ��ȡ�ϴιػ���ʱ��̨���ڵ�λ��
////**********************************************************************//
//void SlideComm::servo_get_last_position(double last_distance)
//{
//	m_distance = last_distance;
//}

////////////////////////////////////////////////////////////////////////////////////////////////
// ����:    get_feedback_timeout
// ����:    ���ɫ�����ӵĳ�ʱ��ֵ
////////////////////////////////////////////////////////////////////////////////////////////////
int SlideComm::get_feedback_timeout()
{
	const int	servo_timeout = 10000;

	return servo_timeout;
}


