#include "Illuminant.h"
/** 线程退出标志 */
bool Illuminant::_bExit = false;
atomic<bool> Illuminant::_flag = 1;
atomic<int>  Illuminant::_num = 0;
int Illuminant::_steadyTime = 10;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:毫秒 */
const UINT SLEEP_TIME_INTERVAL = 50;
Illuminant::Illuminant()
{
	//ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件
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


///////////////////////////////////光源控制类公有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：OpenCOM
// 描述：打开串口
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::OpenCOM(UINT portNo)
{
	_portNo = portNo;
	EnterCriticalSection(&_csCommunicationSync);
	char szPort[50];
	sprintf_s(szPort, "COM%d", _portNo);

	///////Unicode字符集问题/////////
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
		//fprintf(stderr, "打开串口失败！\n");
		return false;
	}
	//cout << "打开串口成功！\n" << endl;
	LeaveCriticalSection(&_csCommunicationSync);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// 函数：InitCOM
// 描述：初始化端口
// 输入：Null
// 输出：
// 返回：是否成功完成操作
// 备注：
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
		//cout << "初始化串口失败1" << endl;
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
		//cout << "初始化串口失败2" << endl;
		return false;
	}
	//cout << "初始化串口成功" << endl;
	PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	LeaveCriticalSection(&_csCommunicationSync);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// 函数：OpenListenThread
// 描述：开启监听线程
// 输入：
// 输出：
// 返回：当线程已经处于开启状态时，返回false;
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::OpenListenThread()
{
	/** 检测线程是否已经开启了 */
	if (_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 线程已经开启 */
		return false;
	}
	_bExit = false;
	/** 线程ID */
	UINT threadId;
	/** 开启串口数据监听线程 */
	_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!_hListenThread)
	{
		return false;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////
// 函数：SetSteadyTime
// 描述：设置延时
// 输入：steadyTime  从界面（测试时）/配置文件（采集时）读取的延时时间
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::SetSteadyTime(int steadyTime)
{
	_steadyTime = steadyTime;
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//机位号
	temp[2] = 0xFe;
	temp[3] = (char)_steadyTime;//光源亮灯时间
	temp[4] = Xor(temp + 1, 3);//光源的校验值
	if (!WriteData(temp, 5))
	{
		return false;
	}
	delete[]temp;
	Sleep(1 * 100);
	return true;//
}
////////////////////////////////////////////////////////////////////////////
// 函数：LightenById
// 描述：按ID亮灯
// 输入：order 输入的亮灯序列
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////	
bool Illuminant::LightenById(UINT id)
{
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//机位号
	//temp[2] = 0xFb;
	temp[2] = 0x2A;
	temp[3] = (char)id;//光源的id
	temp[4] = Xor(temp + 1, 3);//光源的校验值
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
// 函数：LightenNext
// 描述：用定时器来点亮灯顺序
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：在这个函数里面定义了灯的顺序
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Illuminant::LightNext(int num)
{
	UINT IdOrder[] = {1,2,3,4,5,6,7,8,9};//这个数组定义的是灯点亮的顺序,按照需要点亮顺序来
	EnterCriticalSection(&_csCommunicationSync);
	LightenById(IdOrder[_num]);
	Start();
	LeaveCriticalSection(&_csCommunicationSync);
}
////////////////////////////////////////////////////////////////////////////
// 函数：LightenByOrder
// 描述：按顺序亮灯
// 输入：num 输入的顺序亮灯数量
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::LightenByOrder(int num)
{
	unsigned char *temp = new unsigned char[5];
	temp[0] = 0x05;
	temp[1] = 0x01;//机位号
	temp[2] = 0xBb;
	temp[3] = (unsigned char)num;//光源的数量
	temp[4] = Xor(temp + 1, 3);//光源字节的校验值
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
// 函数：LightenByCustomOrder
// 描述：按自定义的顺序亮灯
// 输入：自定义的亮灯顺序和亮灯的数量
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::LightenByCustomOrder(unsigned char* Order, int length)
{
	unsigned char *temp = new unsigned char[length+4];
	temp[0] = (unsigned char)(length+4);
	temp[1] = 0x01;//机位号
	temp[2] = 0xFb;
	memcpy(temp + 3, Order, length);
	temp[length + 3] = Xor(temp + 1, length+2);//光源的校验值
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
// 函数：Suspend
// 描述：暂停
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::Suspend(UINT id)
{
	//unsigned char *temp1 = new unsigned char[4];
	//temp1[0] = 0x04;
	//temp1[1] = 0x01;//机位号
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
	temp[1] = 0x01;//机位号
	//temp[2] = 0xFb;
	temp[2] = 0x2B;
	temp[3] = (char)id;//光源的id
	temp[4] = Xor(temp + 1, 3);//光源的校验值
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
// 函数：Start
// 描述：启动
// 输入：
// 输出：
// 返回：是否成功完成操作
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool Illuminant::Start()
{
	unsigned char *temp = new unsigned char[4];
	temp[0] = 0x04;
	temp[1] = 0x01;//机位号
	temp[2] = 0xA0;
	temp[3] = 0xA1;//光源的数量
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
// 函数：Collection
// 描述：执行正式采集时候，光源亮灯的程序
// 输入：Oeder是亮灯顺序的数组，length为需要点亮的顺序
// 输出：
// 返回：是否成功完成操作
// 备注：
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
///////////////////////////////////光源控制类私有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：CloseCOM
// 描述：关闭串口
// 输入：
// 输出：
// 返回：操作是否成功
// 备注：
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
// 函数：OpenListenThread
// 描述：开启监听线程
// 输入：
// 输出：
// 返回：当线程已经处于开启状态时，返回false;
// 备注：
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
// 函数：GetBytesInCOM
// 描述：获取输入缓冲区的字节数
// 输入：
// 输出：
// 返回：输入缓冲区的字节数
// 备注：
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
// 函数：ReadChar
// 描述：读取串口接受缓冲区中一个字节的数据
// 输入：
// 输出：
// 返回：操作是否成功
// 备注：
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
// 函数：WriteData
// 描述：往串口设备中写数据
// 输入：
// 输出：
// 返回：操作是否成功
// 备注：
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
	//EnterCriticalSection(&_csCommunicationSync);//这句莫名跳出
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
// 函数：Xor
// 描述：实现异或运算，得到校验码
// 输入：字节数组，数组的长度
// 输出：
// 返回：异或的运算结果
// 备注：
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
// 函数：ListenThread
// 描述：实现异或运算，得到校验码
// 输入：字节数组，数组的长度
// 输出：
// 返回：异或的运算结果
// 备注：
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
		/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
		if (BytesInQue == 0)
		{
			//cout << "串口无数据" << endl;
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
					//cout << "第" << i << "个亮灯出错" << endl;
					_flag = 0;
					i = i-1;
					//pIlluminant->Suspend();
				}
			}
			++i;
			++_num;//用于判断点亮了num个灯
		}
		if (i == 196)//这个是和所需要点亮光源的数量相等
			return 0;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
// 函数：Clear
// 描述：
// 输入：清空串口输入缓冲区
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Illuminant::Clear()
{
	PurgeComm(_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
}