//Ԥ���� stdafx.h : ��׼ϵͳ�����ļ������Ǿ���ʹ�õ��������ĵ��ض�����Ŀ�İ����ļ�
#pragma once
#include <iostream>
#include <windows.h>
#include <direct.h>

//#define _CRT_SECURE_NO_WARNINGS  //����CRT����
#define WIN32_LEAN_AND_MEAN   //�ų���̫���õ�API����СWin32ͷ�ļ���С���������ɹ���
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  //ĳЩ CString ���캯��������ʽ��

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  //��Windowsͷ�ļ����ų�����ʹ�õ�����
#endif

// ���º궨��Ҫ������ƽ̨��Ҫ������ƽ̨�Ǿ�������Ӧ�ó������蹦�ܵ� Windows��Internet Explorer �Ȳ�Ʒ��
// ����汾��ͨ����ָ���汾�����Ͱ汾��ƽ̨���������п��õĹ��ܣ����������������
// �������Ҫ��Ե�������ָ���汾��ƽ̨�����޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER                          // ָ��Ҫ������ƽ̨�� Windows Vista��
#define WINVER 0x0600           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0600     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE                       // ָ��Ҫ������ƽ̨�� Internet Explorer 7.0��
#define _WIN32_IE 0x0700        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

