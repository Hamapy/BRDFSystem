#include "Illuminant.h"
/** �߳��˳���־ */
bool Illuminant::_bExit = false;
atomic<bool> Illuminant::_flag = 1;
atomic<int>  Illuminant::_num = 0;
int Illuminant::_steadyTime = 10;
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:���� */
const UINT SLEEP_TIME_INTERVAL = 50;
Illuminant::Illuminant()
{
	//ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�
	_portNo = ini->value("BRDFSystem-Configuration/serialPortSelection").toInt();
	_hComm = INVALID_HANDLE_VALUE;
	_hListenThread = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&_csCommunicationSync);
}

Illuminant::~Illuminant()
{
    CloseListenTread();
	CloseCOM();
	DeleteCriticalSection(&_csCommunicationSync);
}


///////////////////////////////////��Դ�����๫�к���/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������OpenCOM
// �������򿪴���
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::OpenCOM(UINT portNo)
{
	_portNo = portNo;
	EnterCriticalSection(&_csCommunicationSync);
	char szPort[50];
	sprintf_s(szPort, "COM%d", _portNo);

	///////Unicode�ַ�������/////////
	WCHAR wszPort[256];
	memset(wszPort, 0, sizeof(wszPort));
	MultiByteToWideChar(CP_ACP, 0, szPort, strlen(szPort) + 1, wszPort,
		sizeof(wszPort) / sizeof(wszPort[0]));

	_hComm = CreateFile(wszPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (_hComm == INVALID_HANDLE_VALUE)
	{
		//fprintf(stderr, "�򿪴���ʧ�ܣ�\n");
		return false;
	}
	//cout << "�򿪴��ڳɹ���\n" << endl;
	LeaveCriticalSection(&_csCommunicationSync);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// ������InitCOM
// ��������ʼ���˿�
// ���룺Null
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::InitCOM(UINT portNo)
{
	if (!OpenCOM(portNo))
	{
		return false;
	}
	EnterCriticalSection(&_csCommunicationSync);
	if (!SetupComm(_hComm, 1024, 1024))
	{
		//cout << "��ʼ������ʧ��1" << endl;
		return false;
	}
	DCB dcb;
	GetCommState(_hComm, &dcb);
	dcb.BaudRate = 19200;
	dcb.ByteSize = 8;
	dcb.Parity = 0;
	dcb.StopBits = 1;
	if (!SetCommState(_hComm, &dcb))
	{
		//cout << "��ʼ������ʧ��2" << endl;
		return false;
	}
	//cout << "��ʼ�����ڳɹ�" << endl;
	PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	LeaveCriticalSection(&_csCommunicationSync);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// ������OpenListenThread
// ���������������߳�
// ���룺
// �����
// ���أ����߳��Ѿ����ڿ���״̬ʱ������false;
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */
	if (_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}
	_bExit = false;
	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������SetSteadyTime
// ������������ʱ
// ���룺steadyTime  �ӽ��棨����ʱ��/�����ļ����ɼ�ʱ����ȡ����ʱʱ��
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::SetSteadyTime(int steadyTime)
{
	_steadyTime = steadyTime;
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//��λ��
	temp[2] = 0xFe;
	temp[3] = (char)_steadyTime;//��Դ����ʱ��
	temp[4] = Xor(temp + 1, 3);//��Դ��У��ֵ
	if (!WriteData(temp, 5))
	{
		return false;
	}
	delete[]temp;
	Sleep(1 * 100);
	return true;//
}
////////////////////////////////////////////////////////////////////////////
// ������LightenById
// ��������ID����
// ���룺order �������������
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////	
bool Illuminant::LightenById(UINT id)
{
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//��λ��
	//temp[2] = 0xFb;
	temp[2] = 0x2A;
	temp[3] = (char)id;//��Դ��id
	temp[4] = Xor(temp + 1, 3);//��Դ��У��ֵ
	if (!WriteData(temp, 5))
	{
		//cout << "LightenById error" << endl;
		return false; 
	}
	delete[]temp;
	Sleep(2 * 100);

	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������LightenNext
// �������ö�ʱ����������˳��
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��������������涨���˵Ƶ�˳��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Illuminant::LightNext(int num)
{
	UINT IdOrder[] = {1,2,3,4,5,6,7,8,9};//������鶨����ǵƵ�����˳��,������Ҫ����˳����
	EnterCriticalSection(&_csCommunicationSync);
	LightenById(IdOrder[_num]);
	Start();
	LeaveCriticalSection(&_csCommunicationSync);
}
////////////////////////////////////////////////////////////////////////////
// ������LightenByOrder
// ��������˳������
// ���룺num �����˳����������
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::LightenByOrder(int num)
{
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//��λ��
	temp[2] = 0xBb;
	temp[3] = (unsigned char)num;//��Դ������
	temp[4] = Xor(temp + 1, 3);//��Դ�ֽڵ�У��ֵ
	if (!WriteData(temp, 5))
	{
		//cout << "error" << endl;
		return false;
	}
	delete[]temp;
	Sleep(3 * 100);
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������LightenByCustomOrder
// ���������Զ����˳������
// ���룺�Զ��������˳������Ƶ�����
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::LightenByCustomOrder(unsigned char* Order, int length)
{
	unsigned char *temp = new unsigned char[length+4];
	temp[0] = (unsigned char)(length+4);
	temp[1] = 0x01;//��λ��
	temp[2] = 0xFb;
	memcpy(temp + 3, Order, length);
	temp[length + 3] = Xor(temp + 1, length+2);//��Դ��У��ֵ
	//for (int i = 0; i < 13; i++)
	//	printf("%x  ", temp[i]);
	if (!WriteData(temp, length+4))
	{
		//cout << "LightenByCustomOrder error" << endl;
		return false;
	}
	delete[]temp;
	Sleep(100);
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������Suspend
// ��������ͣ
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::Suspend(UINT id)
{
	//unsigned char *temp1 = new unsigned char[4];
	//temp1[0] = 0x04;
	//temp1[1] = 0x01;//��λ��
	//temp1[2] = 0xA1;
	//temp1[3] = 0xA0;
	//if (!WriteData(temp1, 4))
	//{
	//	return false;
	//}
	//Sleep(2 * 100);
	//delete[]temp1;
	//return true;

	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//��λ��
	//temp[2] = 0xFb;
	temp[2] = 0x2B;
	temp[3] = (char)id;//��Դ��id
	temp[4] = Xor(temp + 1, 3);//��Դ��У��ֵ
	if (!WriteData(temp, 5))
	{
		//cout << "LightenById error" << endl;
		return false;
	}
	delete[]temp;
	Sleep(3 * 100);

	return true;
}

////////////////////////////////////////////////////////////////////////////
// ������Start
// ����������
// ���룺
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::Start()
{
	unsigned char *temp = new unsigned char[4];
	temp[0] = 0x04;
	temp[1] = 0x01;//��λ��
	temp[2] = 0xA0;
	temp[3] = 0xA1;//��Դ������
	if (!WriteData(temp, 4))
	{
		//cout << "Start  error" << endl;
		return false;
	}
	//cout << "start ok!" << endl;
	Sleep(2*100);
	delete[]temp;
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������Collection
// ������ִ����ʽ�ɼ�ʱ�򣬹�Դ���Ƶĳ���
// ���룺Oeder������˳������飬lengthΪ��Ҫ������˳��
// �����
// ���أ��Ƿ�ɹ���ɲ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Illuminant::Collection(unsigned char* Order, int length)
{
	LightenByCustomOrder(Order,length);
	Start();
	//Clear();
	//Sleep(150);
	while (_flag == 0)
	{
		LightenByCustomOrder(Order, length);
	}
}
///////////////////////////////////��Դ������˽�к���/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������CloseCOM
// �������رմ���
// ���룺
// �����
// ���أ������Ƿ�ɹ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::CloseCOM()
{
	if (_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_hComm);
		_hComm = INVALID_HANDLE_VALUE;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
// ������OpenListenThread
// ���������������߳�
// ���룺
// �����
// ���أ����߳��Ѿ����ڿ���״̬ʱ������false;
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::CloseListenTread()
{
	if (_hListenThread != INVALID_HANDLE_VALUE)
	{
		_bExit = true;
		Sleep(10);
		CloseHandle(_hListenThread);
		_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������GetBytesInCOM
// ��������ȡ���뻺�������ֽ���
// ���룺
// �����
// ���أ����뻺�������ֽ���
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
UINT Illuminant::GetBytesInCOM()
{
	DWORD dwError = 0;
	COMSTAT comstat;
	memset(&comstat, 0, sizeof(COMSTAT));
	UINT BytesInQue = 0;
	if (ClearCommError(_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue;
	}
	return BytesInQue;
}
////////////////////////////////////////////////////////////////////////////
// ������ReadChar
// ��������ȡ���ڽ��ܻ�������һ���ֽڵ�����
// ���룺
// �����
// ���أ������Ƿ�ɹ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::ReadChar(unsigned char &cRecved)
{
	BOOL bResult = TRUE;
	DWORD BytesRead = 0;
	if (_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	EnterCriticalSection(&_csCommunicationSync);
	bResult = ReadFile(_hComm, &cRecved, 1, &BytesRead, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		//PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		LeaveCriticalSection(&_csCommunicationSync);
		return false;
	}
	LeaveCriticalSection(&_csCommunicationSync);
	return (BytesRead == 1);
}
////////////////////////////////////////////////////////////////////////////
// ������WriteData
// �������������豸��д����
// ���룺
// �����
// ���أ������Ƿ�ɹ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::WriteData(unsigned char *pData, int length)
{
	//int *pData = new int;
	BOOL bResult = TRUE;
	DWORD BytesToSend = 0;
	if (_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	//EnterCriticalSection(&_csCommunicationSync);//���Ī������
	bResult = WriteFile(_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		//PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		LeaveCriticalSection(&_csCommunicationSync);
		return false;
	}
	LeaveCriticalSection(&_csCommunicationSync);
	return true;
}
////////////////////////////////////////////////////////////////////////////
// ������Xor
// ������ʵ��������㣬�õ�У����
// ���룺�ֽ����飬����ĳ���
// �����
// ���أ�����������
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
unsigned char Illuminant::Xor(unsigned char* array, int size)
{
	unsigned char dxor = 0;
	for (int i = 0; i < size; i++)
	{
		dxor ^= array[i];
	}
	return dxor;
}
////////////////////////////////////////////////////////////////////////////
// ������ListenThread
// ������ʵ��������㣬�õ�У����
// ���룺�ֽ����飬����ĳ���
// �����
// ���أ�����������
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
UINT WINAPI Illuminant::ListenThread(void* pParam)
{
	int i = 0;
	clock_t* times = new clock_t[200];
	Illuminant *pIlluminant = reinterpret_cast<Illuminant*>(pParam);
	while (!pIlluminant->_bExit)
	{
		EnterCriticalSection(&(pIlluminant->_csCommunicationSync));
		UINT BytesInQue = pIlluminant->GetBytesInCOM();
		LeaveCriticalSection(&(pIlluminant->_csCommunicationSync));
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			//cout << "����������" << endl;
			Sleep(SLEEP_TIME_INTERVAL);
			//++i;
			//continue;
		}
		else if(BytesInQue>3)
		{
			//cout << " ||" << BytesInQue << "||  " << endl;;
			times[i] = clock();
			pIlluminant->Clear();
			Sleep(200);
			if (i>0)
			{
				clock_t difference = times[i] - times[i - 1];
				double interval = (double)(difference / CLOCKS_PER_SEC);
				cout << interval << endl;
				if (!(interval <= _steadyTime * 2 && interval >= _steadyTime * 0.5))
				{
					//cout << "��" << i << "�����Ƴ���" << endl;
					_flag = 0;
					i = i-1;
					//pIlluminant->Suspend();
				}
			}
			++i;
			++_num;//�����жϵ�����num����
		}
		if (i == 196)//����Ǻ�����Ҫ������Դ���������
			return 0;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
// ������Clear
// ������
// ���룺��մ������뻺����
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Illuminant::Clear()
{
	PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
}