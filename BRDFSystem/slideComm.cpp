#include "slideComm.h"

/////////////////////////////////////���캯��//////////////////////////////////////////
SlideComm::SlideComm()
{
	//Init(13, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	_accelerate = 0;
	_decelerate = 0;
	_resolution = 0;
	_velocity = 0;
	_distance = 0;
	_port = 0;
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

	_accelerate = accelerate;
	_velocity = velocity;
	_decelerate = decelerate;
	_resolution = resolution;
	_port = port;

	// �򿪴���
	if (IsOpen())
	{
		Close();
	}
	if (!Open(_port))
	{
		return false;
	}
	
	char cmd[STRING_LEN];

	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		//sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode��Q/SCLģʽ�ϵ�
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // ���õ��Ϊ���������
		ret = IsFinished(10000);
	}

	if (ret)
	{
		//�����ŷ��������
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", _accelerate, _velocity, _decelerate, _resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(10000);
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������Init
// �����������ʼ��
// ����: port:     �����Ҫ���ӵĴ���
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::Init(int port)
{
	bool ret = false;
	char cmd[STRING_LEN];
	_port = port;

	// �򿪴���
	if (IsOpen())
	{
		Close();
	}
	if (!Open(_port))
	{
		ret = false;
	}
	else
		ret = true;

	//����ע�ͼ��˾�������
	/*
	if (ret)
	{
		ClearInputBuffer();
		//sprintf(cmd, "PM2\rSI3\rCM21\rSS%s\r", SERVO_FEEDBACK);
		//sprintf_s(cmd, "PM%d\r", 2);//Power-up Mode��Q/SCLģʽ�ϵ�
		sprintf(cmd, "CHR\rPM2\rSA\rSS%s\r", SERVO_FEEDBACK);
		Write(cmd);  // ���õ��Ϊ���������
		ret = IsFinished(10000);
	}

	if (ret)
	{
		//�����ŷ��������
		ClearInputBuffer();
		sprintf(cmd, "AC%d\rVE%.2f\rDE%d\rEG%d\rSS%s\r", _accelerate, _velocity, _decelerate, _resolution, SERVO_FEEDBACK);
		Write(cmd);

		ret = IsFinished(10000);
	}
	*/

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������MoveToX1
// ������
// ����: 
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveToX1(bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// ��ʼ���豸
	ret = Init(_port);

	if (ret)
	{
		ClearInputBuffer();
		/*
		STD DI �ƶ�һ�������ֹͣ
		FS1L ������1���ź�ΪLOWʱֹͣ
		FS1R ������1��λ�źű仯ʱֹͣ
		SS �����ַ���
		*/
		sprintf(cmd, "STD\rDI%d\rFS1R\rSS%s\r", 10000, SERVO_FEEDBACK);
		//��λǰ�ȹ�λʱ����λ����
		//sprintf(cmd, "STD\rDI%d\rFS2R\rSS%s\r", 10000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(10000);
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	// ��ֹ�豸
	Fini();

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������MoveToX2
// ������
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveToX2(bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;

	// �豸��ʼ��
	ret = Init(_port);

	//����ϵͳ����ǰ���촦������λ�ã���λǰ��ʹ�����λ
	//ret = MoveToX1();

	if (ret)
	{
		ClearInputBuffer();
		sprintf(cmd, "STD\rDI%d\rFS2R\rSS%s\r", -10000, SERVO_FEEDBACK);
		Write(cmd);

		if (Wait_feedback)
		{
			ret = IsFinished(10000);
		}
		else
		{
			// �ڷ���һ��ָ��ǰ��΢��һ��
			Wait(200);
		}
	}

	// ��ֹ�豸
	Fini();

	return ret;
}
////////////////////////////////////////////////////////////////////////////
// ������MoveTest
// ������
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool SlideComm::MoveTest(double distance, bool Wait_feedback/* = true*/)
{
	char cmd[STRING_LEN];
	bool ret = false;
	double mDistance; //��Ҫ�ƶ�����
	int mPulses; //��Ҫת�������岽��SI
	int rlr = _resolution / 2; //��R���Ĵ���ƥ���ŷ��������е�EG����
	double distancePerRev = 10;//��ֵ�д�����

	if (distance < 0 || distance > MOVE_DISTANCE)
		return ret;
	else
	{
		mDistance = _distance - distance;
		mPulses = static_cast<int> (mDistance * rlr * SERVO_REDUCTION / distancePerRev);

		// ��ʼ���豸
		ret = Init(_port);

		if (ret)
		{
			ClearInputBuffer();
			sprintf(cmd, "STD\rDI%d\rFL\rSS%s\r", mPulses, SERVO_FEEDBACK);
			Write(cmd);

			if (Wait_feedback)
			{
				ret = IsFinished(10000);
			}
			else
			{
				Wait(200);
			}
		}
		if (ret)
		{
			// ����λ����Ϣ
			_distance = distance;
		}
		// ��ֹ�豸
		Fini();
	}

	return ret;
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
///////////////////////////////////////////////////////////////////////////////
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






