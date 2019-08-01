#pragma once
#include "stdafx.h"
#include <atlimage.h>
#include "stdlib.h"
#include <math.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp> 
//#include <opencv2/core/mat.hpp> 
//#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <levmar.h>
#include <string>

using namespace std;
using namespace cv;


#define PI	3.1415926535897932384626433832795
#define SIZE 1764     // ���ݽǶ���Ϣ�õ���������,SIZE=thetaOutNum*fiOutNum*lightSourceNum

class BRDFFitting
{
public:
	BRDFFitting();
	~BRDFFitting();

	// ����BRDF�ļ����ƶ�ȡ��Ӧ�ļ�,��ʼ���
	Mat StartFitting(string MaterialName);
	// ����BRDF�ļ��ƶ�ȡBRDF���ݣ������й�һ��
	Mat  BRDFRead(string MaterialName);




private:

	const double _flatAngle = 180.0;
	const int _thetaOutNum = 9;
	const int _thetaIntNum = 9;
	const int _fiOutNum = 1;
	const int _lightSourceNum = 196;

	static double		_T[SIZE * 3];   // �洢ģ����ر���_l��_G��_delta
	static double       _p[7];          // 7������ֵ
	Mat                 _param;         //���շ��ص�7����������
	double				_alpha;
	Mat					_vth0;          // ��Ӧ_thetaOut
	Mat					_lth0;          // ��Ӧ_thetaIn
	Mat					_lph0;          // ��Ӧ_phiIn
	Mat					_l, _G, _delta;   // ģ�ͼ����õ���һЩ����
	Mat                 _brdfandAngle, _brdf;  // _brdfandAngle�洢�Ƕ���Ϣ��brdf���ݣ�_brdf�洢brdf���� 
	Mat                 _Ad, _As;         // �����䡢���淴��ϵ����ֵ
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////���ݾ�����λ���޸�//////////////////////////////////////////////////////////////////////////////////
	CString				_file_fold = "..\\..\\..\\Param\\001\\";     // BRDFԭʼ�����ļ�·��
	CString				_save_fold = "..\\..\\..\\Param\\001\\";     // ��Ͻ���洢·��
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString				_materialName, _fileName, _saveName;


	// ���brdf_simple���ݵĲ����Ƕ�
	void ComputeAngle();
	// ��С���˷����ֵ
	void GetOriginValue();
	// LM������ֵ
	void GetFinalValue();
	// WardDuerģ�ͺ���
	static void WardDuerfun(double *p, double *hx, int m, int n, void *adata);


	////////////////////////////////////BRDFԭʼ���ݽ��в���Ԥ�������غ���/////////////////////////////////

	//����һ�Դ��� / �����Ƕȣ�����BRDF��
	void lookup_brdf_val(double* brdf, double theta_in, double fi_in,
		double theta_out, double fi_out,
		double& red_val, double& green_val, double& blue_val);
	//����fi_in����
	inline int fi_in_index(double fi_in, double theta_in);
	//����theta_in����
	inline int theta_in_index(double theta_in);
	//����fi_out����
	inline int fi_out_index(double fi_out);
	//����theta_out����
	inline int theta_out_index(double theta_out);
	// ��ȡBRDF����
	bool read_brdf(const char *filename, double* &brdf);


};

