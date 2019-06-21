///////////////////////////////////����ͨ�����/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//COM port Base Library(WIN32) free version 1.2
//C/ompiled by�� BC++ 5(free tool); C++ BUILDER 4, 5, 6, X; VC++ 5, 6, 7, 8;  GCC;
//Writer: llbird 
//Mail  : wushaojian@21cn.com  
//Blog  : http://blog.csdn.net/wujian53
//Copyright(c) 2004.5 - 2006.1 
////////////////////////////////////////////////////////////////////////////////
#ifndef _CN_COMM_H_
#define _CN_COMM_H_

#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>

//�͵����ڵ���Ϣ  WPARAM �˿ں�
#define ON_COM_RECEIVE WM_USER + 618    
#define ON_COM_CTS     WM_USER + 619 //LPARAM 1 valid 
#define ON_COM_DSR     WM_USER + 621 //LPARAM 1 valid
#define ON_COM_RING    WM_USER + 623
#define ON_COM_RLSD    WM_USER + 624
#define ON_COM_BREAK   WM_USER + 625
#define ON_COM_TXEMPTY WM_USER + 626
#define ON_COM_ERROR   WM_USER + 627 //LPARAM save Error ID
#define DEFAULT_COM_MASK_EVENT  EV_RXCHAR | EV_ERR | EV_CTS | EV_DSR | EV_BREAK | EV_TXEMPTY | EV_RING | EV_RLSD

class CRs232Comm    
{
public:
	
	//------------------------------Construction-----------------------------------
	//��1������Ϊ�Ƿ��ڴ򿪴���ʱ���������̣߳� ��2������ΪIO��ʽ ������ʽ��0��/ �첽�ص���ʽ��Ĭ�ϣ�
	CRs232Comm(bool fAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED)
		: _dwIOMode(dwIOMode), _fAutoBeginThread(fAutoBeginThread)
	{
		Init(); 
	}
	
	virtual ~CRs232Comm()//����������
	{
		Close(); 
		Destroy();
	}
	
	//----------------------------------Attributes----------------------------------
	//�жϴ����ǻ��
	inline bool IsOpen()
	{
		return _hCommHandle != INVALID_HANDLE_VALUE;
	}
	//�жϴ����ǻ��
	operator bool ()                     //����ת������
	{
		return _hCommHandle != INVALID_HANDLE_VALUE;
	}
	//��ô��ھ��
	HANDLE GetHandle()
	{
		return _hCommHandle;
	}
	//��ô��ھ��
	operator HANDLE()
	{
		return _hCommHandle;
	}
	//��ô������
	const int GetPortID()
	{
		return _dwPort;
	}
	//��ô���ȫ��
	const char *GetPortName()
	{
		return _szCommStr;
	}
	//��ô��ڲ��� DCB
	DCB *GetState()
	{
		return IsOpen() && ::GetCommState(_hCommHandle, &_DCB) == TRUE ? &_DCB : NULL;
	}
	//���ô��ڲ��� DCB
	bool SetState(DCB *pdcb = NULL)
	{
		return IsOpen() ? ::SetCommState(_hCommHandle, pdcb == NULL ? &_DCB : pdcb) == TRUE : false;
	}
	//���ô��ڲ����������ʣ�ֹͣλ���� ֧�������ַ��� "9600, n, 8, 1"
	bool SetState(char *szSetStr) 
	{
		///////Unicode�ַ�������/////////
		WCHAR wszSetStr[256];
		memset(wszSetStr, 0, sizeof(wszSetStr));
		MultiByteToWideChar(CP_ACP, 0, szSetStr, strlen(szSetStr) + 1, wszSetStr,
			sizeof(wszSetStr) / sizeof(wszSetStr[0]));


		if(IsOpen())
		{
			if(::GetCommState(_hCommHandle, &_DCB) != TRUE)
				return false;
			if(::BuildCommDCB(wszSetStr, &_DCB) != TRUE)
				return false;
			return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
		}
		return false;
	}
	//���ô��ڲ����������ʣ�ֹͣλ����
	bool SetState(DWORD dwBaudRate,  DWORD dwParity = NOPARITY,DWORD dwByteSize = 8, DWORD dwStopBits = ONESTOPBIT)
	{
		if(IsOpen())
		{
			if(::GetCommState(_hCommHandle, &_DCB) != TRUE)
				return false;
			_DCB.BaudRate = dwBaudRate;
			_DCB.ByteSize = (unsigned char)dwByteSize;
			_DCB.Parity   = (unsigned char)dwParity;
			_DCB.StopBits = (unsigned char)dwStopBits;
			return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
		}
		return false;
	}
	//��ó�ʱ�ṹ
	LPCOMMTIMEOUTS GetTimeouts(void)
	{
		return IsOpen() && ::GetCommTimeouts(_hCommHandle, &_CO) == TRUE  ? &_CO : NULL;
	}
	//���ó�ʱ
	bool SetTimeouts(LPCOMMTIMEOUTS lpCO)
	{
		return IsOpen() ? ::SetCommTimeouts(_hCommHandle, lpCO) == TRUE : false;
	}
	//���ô��ڵ�I/O��������С
	bool SetBufferSize(DWORD dwInputSize, DWORD dwOutputSize)
	{
		return IsOpen() ? ::SetupComm(_hCommHandle, dwInputSize, dwOutputSize) == TRUE : false; 
	}
	//������ܻ�����
	void ClearInputBuffer()
	{
		if(IsOpen())
			::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR );
	}
	//������ͻ�����
	void ClearOutputBuffer()
	{
		if(IsOpen())
			::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR );
	}
	//������Ϣ�Ĵ��ھ��
	inline void SetWnd(HWND hWnd)
	{
		assert(::IsWindow(hWnd));
		_hNotifyWnd = hWnd;
	}
	//�趨����֪ͨ, �����ַ���Сֵ
	inline void SetNotifyNum(DWORD dwNum)
	{
		_dwNotifyNum = dwNum;
	}
	//�߳��Ƿ�����
	inline bool IsThreadRunning()
	{
		return _hThreadHandle != NULL;
	}
	//����߳̾��
	inline HANDLE GetThread()
	{
		return _hThreadHandle;
	}
	//����Ҫ���ӵ��¼��� ��ǰ������Ч
	void SetMaskEvent(DWORD dwEvent = DEFAULT_COM_MASK_EVENT)
	{
		_dwMaskEvent = dwEvent;
	}
	//��ö����������ַ���
	int GetInputSize()
	{
		COMSTAT Stat;
		DWORD dwError;
		
		return ::ClearCommError(_hCommHandle, &dwError, &Stat) == TRUE  ? Stat.cbInQue : (DWORD)-1L;
	}
	
	//----------------------------------Operations----------------------------------
	//�򿪴��� ȱʡ 9600, n, 8, 1
	bool Open(DWORD dwPort)
	{
		return Open(dwPort, 9600);
	}
	//�򿪴��� ȱʡ baud_rate, n, 8, 1
	bool Open(DWORD dwPort, DWORD dwBaudRate)
	{
		if(dwPort < 1 || dwPort > 1024)
			return false;
		
		BindCommPort(dwPort);
		
		if(!OpenCommPort())
			return false;
		
		if(!SetupPort())
			return false;
		
		return SetState(dwBaudRate);
	}
	//�򿪴���, ʹ������"9600, n, 8, 1"�������ַ������ô���
	bool Open(DWORD dwPort, char *szSetStr)
	{
		if(dwPort < 1 || dwPort > 1024)
			return false;
		
		BindCommPort(dwPort);
		
		if(!OpenCommPort())
			return false;
		
		if(!SetupPort())
			return false;
		
		return SetState(szSetStr);
	}
	//��ȡ���� dwBufferLength���ַ��� Buffer ����ʵ�ʶ������ַ���  �ɶ���������
	DWORD Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime = 20)
	{
		if(!IsOpen())
			return 0;
		
		COMSTAT  Stat;
		DWORD dwError;
		
		if(::ClearCommError(_hCommHandle, &dwError, &Stat) && dwError > 0)
		{
			::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR);
			return 0;
		}
		
		if(!Stat.cbInQue) // ������������
			return 0;
		
		unsigned long uReadLength = 0;
		
		dwBufferLength = dwBufferLength > Stat.cbInQue ? Stat.cbInQue : dwBufferLength;
		
		if(!::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength, &_ReadOverlapped)) 
		{
			if(::GetLastError() == ERROR_IO_PENDING) 
			{
				WaitForSingleObject(_ReadOverlapped.hEvent, dwWaitTime); // �����첽I/O
				if(!::GetOverlappedResult(_hCommHandle, &_ReadOverlapped, &uReadLength, false))
				{
					if(::GetLastError() != ERROR_IO_INCOMPLETE)
						uReadLength = 0;
				}
			}
			else
				uReadLength = 0;
		}
		
		return uReadLength;
	}
	//��ȡ���� dwBufferLength - 1 ���ַ��� szBuffer ����ANSI C ģʽ�ַ���ָ�� �ʺ�һ���ַ�ͨѶ
	char * ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 20)
	{
		unsigned long uReadLength = Read(szBuffer, dwBufferLength - 1, dwWaitTime);
		szBuffer[uReadLength] = '\0';
		return szBuffer;
	}
	//д���� ��д�������� "abcd" or "\x0\x1\x2"
	DWORD Write(LPVOID Buffer, DWORD dwBufferLength)
	{
		if(!IsOpen())
			return 0;
		
		DWORD dwError;
		
		if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0) 
			::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 
		
		unsigned long uWriteLength = 0;
		
		if(!::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength, &_WriteOverlapped))
			if(::GetLastError() != ERROR_IO_PENDING)
				uWriteLength = 0;
			
			return uWriteLength;
	}
	//д���� дANSI C ģʽ�ַ���ָ�� 
	DWORD Write(const char *szBuffer)
	{
		assert(szBuffer);
		
		return Write((void *)szBuffer, strlen(szBuffer));
	}
	//������	ͬ��Ӧ��
	DWORD ReadSync(LPVOID Buffer, DWORD dwBufferLength)
	{
		if(!IsOpen())
			return 0;
		
		DWORD dwError;
		if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0)
		{
			::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR);
			return 0;
		}

		DWORD uReadLength = 0;
		::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength, NULL);
		return uReadLength;
	}
	//д���� ͬ��Ӧ��
	DWORD WriteSync(LPVOID Buffer, DWORD dwBufferLength)
	{
		if(!IsOpen())
			return 0;
		
		DWORD dwError;
		if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0) 
			::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 
		
		unsigned long uWriteLength = 0;
		::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength, NULL);
		return uWriteLength;
	}
	//д���� szBuffer ���������ʽ�ַ��� ��������������
	DWORD Write(char *szBuffer, DWORD dwBufferLength, char * szFormat, ...)
	{
		if(!IsOpen())
			return 0;
		
		va_list va;
		va_start(va, szFormat);
		_vsnprintf(szBuffer, dwBufferLength, szFormat, va);
		va_end(va);
		
		return Write(szBuffer);
	}
	//д���� szBuffer ���������ʽ�ַ��� ����黺�������� С�����
	DWORD Write(char *szBuffer, char * szFormat, ...)
	{
		if(!IsOpen())
			return 0;
		
		va_list va;
		va_start(va, szFormat);
		vsprintf(szBuffer, szFormat, va);
		va_end(va);
		
		return Write(szBuffer);
	}
	//�رմ��� ͬʱҲ�رչ����߳�
	virtual void Close()
	{
		if(IsOpen())  
		{
			PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 
			
			EndThread();
			::CloseHandle(_hCommHandle);
			
			_hCommHandle = INVALID_HANDLE_VALUE;
		}
	}

	struct InnerLock
	{
		CRs232Comm* ptr;

		InnerLock(CRs232Comm* p) : ptr(p)
		{
			ptr->Lock();
		}
		~InnerLock()
		{
			ptr->Unlock();
		}
	};

	//������Դ
	void Lock()
	{
		::EnterCriticalSection(&_Mutex);
	}
	void Unlock()
	{
		::LeaveCriticalSection(&_Mutex);
	}
	//DTR ��ƽ����
	bool SetDTR(bool OnOrOff)
	{
		return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETDTR : CLRDTR) != 0 : false;
	}
	//RTS ��ƽ����
	bool SetRTS(bool OnOrOff)
	{
		return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETRTS : CLRRTS) != 0 : false;
	}
	//
	bool SetBreak(bool OnOrOff)
	{
		return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETBREAK : CLRBREAK) != 0 : false;
	}
	//�����߳̿��� �������߳�
	bool BeginThread() 
	{
		if(!IsThreadRunning()) 
		{
			_fRunFlag = true;
			_hThreadHandle = NULL;
			
			#ifdef _MT
			unsigned int id;
			_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, CommThreadProc, this, 0, &id);
			#else
			DWORD id;
			_hThreadHandle = ::CreateThread(NULL, 0, CommThreadProc, this, 0, &id); 
			#endif
			
			return (_hThreadHandle != NULL); 
		}
		return false;
	}
	//��ͣ�����߳�
	inline bool SuspendThread()
	{
		return IsThreadRunning() ? ::SuspendThread(_hThreadHandle) != 0xFFFFFFFF : false;
	}
	//�ָ������߳�
	inline bool ResumeThread()
	{
		return IsThreadRunning() ? ::ResumeThread(_hThreadHandle) != 0xFFFFFFFF : false;
	}
	//��ֹ�߳�
	bool EndThread(DWORD dwWaitTime = 100)
	{
		if(IsThreadRunning()) 
		{
			_fRunFlag = false;
			::SetCommMask(_hCommHandle, 0);
			::SetEvent(_WaitOverlapped.hEvent);
			if(::WaitForSingleObject(_hThreadHandle, dwWaitTime) != WAIT_OBJECT_0)
				if(!::TerminateThread(_hThreadHandle, 0))
					return false;
				
				::CloseHandle(_hThreadHandle);
				::ResetEvent(_WaitOverlapped.hEvent);
				
				_hThreadHandle = NULL;
				
				return true;
		}
		return false;
	}
	
protected:

	volatile DWORD _dwPort;   //���ں�
	volatile HANDLE _hCommHandle;  //���ھ��
	char _szCommStr[20];  //����COM1���Ƶ��ַ���
	
	DCB _DCB;  //�����ʣ�ֹͣλ����   
	COMMTIMEOUTS _CO;  //��ʱ�ṹ   
	
	DWORD _dwIOMode;  // 0 ͬ��  Ĭ�� FILE_FLAG_OVERLAPPED �ص�I/O �첽
	OVERLAPPED _ReadOverlapped, _WriteOverlapped; // �ص�I/O
	
	volatile HANDLE _hThreadHandle; //�����߳�
	volatile HWND _hNotifyWnd;  // ֪ͨ����
	volatile DWORD _dwNotifyNum; //���ܶ����ֽ�(>=_dwNotifyNum)����֪ͨ��Ϣ
	volatile DWORD _dwMaskEvent; //���ӵ��¼�
	volatile bool _fRunFlag;  //�߳�����ѭ����־
	bool _fAutoBeginThread;  //Open() �Զ� BeginThread();
	OVERLAPPED _WaitOverlapped;  //WaitCommEvent use
	
	//��ʼ��
	virtual void Init() 
	{
		memset(_szCommStr, 0, 20);
		memset(&_DCB, 0, sizeof(_DCB));
		_DCB.DCBlength = sizeof(_DCB);
		_hCommHandle = INVALID_HANDLE_VALUE;
		
		memset(&_ReadOverlapped, 0, sizeof(_ReadOverlapped));
		memset(&_WriteOverlapped, 0, sizeof(_WriteOverlapped));
		
		_ReadOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
		assert(_ReadOverlapped.hEvent != NULL); 
		
		_WriteOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
		assert(_WriteOverlapped.hEvent != NULL);
		
		_hNotifyWnd = NULL;
		_dwNotifyNum = 0;
		_dwMaskEvent = DEFAULT_COM_MASK_EVENT;
		_hThreadHandle = NULL;
		
		memset(&_WaitOverlapped, 0, sizeof(_WaitOverlapped));
		_WaitOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
		assert(_WaitOverlapped.hEvent != NULL); 

		::InitializeCriticalSection(&_Mutex);
	} 
	//����
	virtual void Destroy()
	{
		if(_ReadOverlapped.hEvent != NULL)
			CloseHandle(_ReadOverlapped.hEvent);
		
		if(_WriteOverlapped.hEvent != NULL)
			CloseHandle(_WriteOverlapped.hEvent);
		
		if(_WaitOverlapped.hEvent != NULL)
			CloseHandle(_WaitOverlapped.hEvent);

		::DeleteCriticalSection(&_Mutex);
	}
	//�󶨴���
	void BindCommPort(DWORD dwPort)
	{
		assert(dwPort >= 1 && dwPort <= 1024);
		
		char p[5];
		
		_dwPort = dwPort;
		strcpy_s(_szCommStr, "\\\\.\\COM"); 
		_ltoa_s(_dwPort, p, 10);
		strcat_s(_szCommStr, p);
	}
	//�򿪴���
	virtual bool OpenCommPort()
	{
		if(IsOpen())
			Close();
		
		///////Unicode�ַ�������/////////
		WCHAR wszCommStr[256];
		memset(wszCommStr, 0, sizeof(wszCommStr));
		MultiByteToWideChar(CP_ACP, 0, _szCommStr, strlen(_szCommStr) + 1, wszCommStr,
			sizeof(wszCommStr) / sizeof(wszCommStr[0]));

		_hCommHandle = ::CreateFile(
			wszCommStr,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | _dwIOMode, 
			NULL
			);
		
		if(_fAutoBeginThread)
		{
			if(IsOpen() && BeginThread())
				return true;
			else
			{
				Close();  //�����߳�ʧ��
				return false;
			}
		}
		return IsOpen();
	}
	//���ô���
	virtual bool SetupPort()
	{
		if(!IsOpen())
			return false;
		
		
		if(!::SetupComm(_hCommHandle, 4096, 4096))
			return false; 
		
		
		if(!::GetCommTimeouts(_hCommHandle, &_CO))
			return false;
		_CO.ReadIntervalTimeout = 0;
		_CO.ReadTotalTimeoutMultiplier = 1;
		_CO.ReadTotalTimeoutConstant = 1000;
		_CO.WriteTotalTimeoutMultiplier = 1;
		_CO.WriteTotalTimeoutConstant = 1000;
		if(!::SetCommTimeouts(_hCommHandle, &_CO))
			return false; 
		
		
		if(!::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
			return false; 
		
		return true;
	} 
	
	//---------------------------------------threads callback-----------------------------------------------------
	//�߳��յ���Ϣ�Զ�����, �細�ھ����Ч, �ͳ���Ϣ, �������ڱ�ţ� ��Ϊ�麯�������ڻ���������չ       
	virtual void OnReceive()//EV_RXCHAR
	{
		if(::IsWindow(_hNotifyWnd))
			::PostMessage(_hNotifyWnd, ON_COM_RECEIVE, WPARAM(_dwPort), LPARAM(0));
	}
	
	virtual void OnDSR()
	{
		if(::IsWindow(_hNotifyWnd))
		{
			DWORD Status;
			if(GetCommModemStatus(_hCommHandle, &Status))
				::PostMessage(_hNotifyWnd, ON_COM_DSR, WPARAM(_dwPort), LPARAM( (Status & MS_DSR_ON) ? 1 : 0));
		}
	}
	
	virtual void OnCTS()
	{
		if(::IsWindow(_hNotifyWnd))
		{
			DWORD Status;
			if(GetCommModemStatus(_hCommHandle, &Status))
				::PostMessage(_hNotifyWnd, ON_COM_CTS, WPARAM(_dwPort), LPARAM( (Status & MS_CTS_ON) ? 1 : 0));
		}
	}
	
	virtual void OnBreak()
	{
		if(::IsWindow(_hNotifyWnd))
		{
			::PostMessage(_hNotifyWnd, ON_COM_BREAK , WPARAM(_dwPort), LPARAM(0));
		}
	}
	
	virtual void OnTXEmpty()
	{
		if(::IsWindow(_hNotifyWnd))
			::PostMessage(_hNotifyWnd, ON_COM_TXEMPTY, WPARAM(_dwPort), LPARAM(0));
	}
	
	virtual void OnError()
	{
		DWORD dwError;
		::ClearCommError(_hCommHandle, &dwError, NULL);
		if(::IsWindow(_hNotifyWnd))
			::PostMessage(_hNotifyWnd, ON_COM_ERROR, WPARAM(_dwPort), LPARAM(dwError));
	}
	
	virtual void OnRing()
	{
		if(::IsWindow(_hNotifyWnd))
			::PostMessage(_hNotifyWnd, ON_COM_RING, WPARAM(_dwPort), LPARAM(0));
	}
	
	virtual void OnRLSD()
	{
		if(::IsWindow(_hNotifyWnd))
			::PostMessage(_hNotifyWnd, ON_COM_RLSD, WPARAM(_dwPort), LPARAM(0));
	}
	
	virtual DWORD ThreadFunc()
	{
		if(!::SetCommMask(_hCommHandle, _dwMaskEvent))
		{
			char szBuffer[256];
			_snprintf_s(szBuffer, 255, "%s(%d) : COM%d Call WINAPI SetCommMask(%x, %x) Fail, thread work invalid! GetLastError() = %d;",
				__FILE__, __LINE__, _dwPort, _hCommHandle, _dwMaskEvent, GetLastError());
			//MessageBox(NULL, szBuffer, "Class CRs232Comm", MB_OK);
			return 1;
		}
		
		COMSTAT Stat;
		DWORD dwError;
		
		for(DWORD dwLength, dwMask = 0; _fRunFlag && IsOpen(); dwMask = 0)
		{
			if(!::WaitCommEvent(_hCommHandle, &dwMask, &_WaitOverlapped))
			{
				if(::GetLastError() == ERROR_IO_PENDING)// asynchronous
					::GetOverlappedResult(_hCommHandle, &_WaitOverlapped, &dwLength, TRUE);
				else
					continue;
			}
			
			if(dwMask == 0)
				continue;
			
			switch(dwMask)
			{
			case EV_RXCHAR :
				::ClearCommError(_hCommHandle, &dwError, &Stat);
				if(Stat.cbInQue >= _dwNotifyNum)
					OnReceive();
				break;
				
			case EV_TXEMPTY :
				OnTXEmpty();
				break;
				
			case EV_CTS :
				OnCTS();
				break;
				
			case EV_DSR :
				OnDSR();
				break;
				
			case EV_RING :
				OnRing();
				break;
				
			case EV_RLSD :
				OnRLSD();
				break;
				
			case EV_BREAK:
				OnBreak();
				break;
				
			case EV_ERR :
				OnError();
				break;
				
			}//case
        }//for
		return 0;
	}
	
private:  //the function protected 
	
	CRs232Comm(const CRs232Comm&);
	CRs232Comm &operator = (const CRs232Comm&);
	CRITICAL_SECTION _Mutex;
	
	//base function for thread
	#ifdef _MT
	static UINT APIENTRY CommThreadProc(LPVOID lpPara)
	#else
	static DWORD WINAPI CommThreadProc(LPVOID lpPara)
	#endif
	{
		return ( (CRs232Comm *)lpPara )->ThreadFunc();
	}
};

#endif //_CN_COMM_H_

