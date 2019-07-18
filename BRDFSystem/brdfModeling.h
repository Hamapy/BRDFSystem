////////////////////////////BRDF���ݽ�ģ������////////////////////////////
//Author:LJH
//Review:ZJX\ZJB

#pragma once
#include "stdafx.h"
#include <atlimage.h>
#include "stdlib.h"
#include <stdio.h>
#include <tchar.h> // Ӱ���ַ�������
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp> 
#include <opencv2/core/mat.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <levmar.h>
#include <string>



using namespace std;
using namespace cv;

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)
#define PI	3.1415926535897932384626433832795
#define SIZE 7875      // ���ݲ����Ƕȵõ���������,SIZE=thetaInNumber * thetaInNumber *phiInNumber



class BRDFFitting
{
public:

	// ����BRDF�ļ�����Ŷ�ȡ��Ӧ�ļ�,��ʼ���,������������������Ƕ���Ϣ������ǶȺ͹۲�Ƕ��е�theta����һ������Ϣ��theta��phi����һ���Ĳ������Gap��
	// thetaInLowerLimitΪtheta�Ĳ����Ƕ����ޣ�thetaInNumberΪtheta�Ĳ���������phiInLowerLimitΪ����Ƕ�phi�Ĳ����Ƕ����ޣ�phiInNumberΪ����Ƕ�phi�Ĳ�������
	void StartFitting(int BRDFSerialNumber, int thetaInLowerLimit, int thetaInNumber,
		int phiInLowerLimit, int phiInNumber, int Gap);

	// ����BRDF�ļ�������Լ�������Ϣ���в����ͱ��棬����ȡBRDF����
	Mat  BRDFSample(int BRDFSerialNumber, int thetaInLowerLimit, int thetaInNumber,
		int phiInLowerLimit, int phiInNumber, int Gap);




private:

	const double _flatAngle = 180.0;
	const double _redScale = 1.0;
	const double _greenScale = 1.15;
	const double _blueScale = 1.66;

	static double		_T[SIZE * 3];   // �洢ģ����ر���_l��_G��_delta
	static double       _p[7];          // 7������ֵ
	double				_alpha;
	Mat					_lth0;          // ��Ӧ_thetaIn
	Mat					_lph0;          // ��Ӧ_phiIn
	Mat					_vth0;          // ��Ӧ_thetaOut
	Mat					_l, _G, _delta;   // ģ�ͼ����õ���һЩ����
	Mat                 _brdfandAngle, _brdf;  // _brdfandAngle�洢�Ƕ���Ϣ��brdf���ݣ�_brdf�洢brdf���� 
	Mat                 _Ad, _As;         // �����䡢���淴��ϵ����ֵ
	CString				_file_fold = "..\\..\\..\\Param\\";     // BRDFԭʼ�����ļ�·��
	CString				_save_fold = "..\\..\\..\\Param\\";     // ��Ͻ���洢·��
	CString				_s_tmp, _filename, _savename;


	// ���brdf_simple���ݵĲ����Ƕ�
	void ComputeSamplingAngle(int thetaInNumber, int phiInNumber, int Gap);
	// ��С���˷����ֵ
	void GetOriginValue();
	// LM������ֵ
	void GetFinalValue();
	// WardDuerģ�ͺ���
	static void WardDuerfun(double *p, double *hx, int m, int n, void *adata);


	//////////////////////////////////BRDFԭʼ���ݽ��в���Ԥ�������غ���/////////////////////////////////
	// ���������Ľ���˻�
	void cross_product(double* v1, double* v2, double* out);
	// ��һ��ʸ��
	void normalize(double* v);
	// ��һ������תʸ��
	void rotate_vector(double* vector, double* axis, double angle, double* out);
	// ����׼����ת��Ϊ��ʸ��/��ʸ������
	void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
		double& theta_half, double& fi_half, double& theta_diff, double& fi_diff);
	// ����theta_half����
	// ����һ��������ӳ�䣡
	// In:  [0 .. pi/2]
	// Out: [0 .. 89]
	int theta_half_index(double theta_half);
	// ����theta_diff����
	// In:  [0 .. pi/2]
	// Out: [0 .. 89]
	int theta_diff_index(double theta_diff);
	// ����phi_diff����
	int phi_diff_index(double phi_diff);
	// ����һ�Դ���/�����Ƕȣ�����BRDF��
	void lookup_brdf_val(double* brdf, double theta_in, double fi_in,
		double theta_out, double fi_out,
		double& red_val, double& green_val, double& blue_val);
	// ��ȡBRDF����
	bool read_brdf(const char *filename, double* &brdf);


};