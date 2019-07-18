////////////////////////////BRDF数据建模类声明////////////////////////////
//Author:LJH
//Review:ZJX\ZJB

#pragma once
#include "stdafx.h"
#include <atlimage.h>
#include "stdlib.h"
#include <stdio.h>
#include <tchar.h> // 影响字符集问题
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
#define SIZE 7875      // 根据采样角度得到的数据量,SIZE=thetaInNumber * thetaInNumber *phiInNumber



class BRDFFitting
{
public:

	// 根据BRDF文件名序号读取相应文件,开始拟合,传入参数还包括采样角度信息，入射角度和观测角度中的theta采用一样的信息，theta和phi采用一样的采样间隔Gap，
	// thetaInLowerLimit为theta的采样角度下限，thetaInNumber为theta的采样数量，phiInLowerLimit为入射角度phi的采样角度下限，phiInNumber为入射角度phi的采样数量
	void StartFitting(int BRDFSerialNumber, int thetaInLowerLimit, int thetaInNumber,
		int phiInLowerLimit, int phiInNumber, int Gap);

	// 根据BRDF文件名序号以及采样信息进行采样和保存，并读取BRDF数据
	Mat  BRDFSample(int BRDFSerialNumber, int thetaInLowerLimit, int thetaInNumber,
		int phiInLowerLimit, int phiInNumber, int Gap);




private:

	const double _flatAngle = 180.0;
	const double _redScale = 1.0;
	const double _greenScale = 1.15;
	const double _blueScale = 1.66;

	static double		_T[SIZE * 3];   // 存储模型相关变量_l、_G、_delta
	static double       _p[7];          // 7个参数值
	double				_alpha;
	Mat					_lth0;          // 对应_thetaIn
	Mat					_lph0;          // 对应_phiIn
	Mat					_vth0;          // 对应_thetaOut
	Mat					_l, _G, _delta;   // 模型计算用到的一些参数
	Mat                 _brdfandAngle, _brdf;  // _brdfandAngle存储角度信息和brdf数据，_brdf存储brdf数据 
	Mat                 _Ad, _As;         // 漫反射、镜面反射系数初值
	CString				_file_fold = "..\\..\\..\\Param\\";     // BRDF原始数据文件路径
	CString				_save_fold = "..\\..\\..\\Param\\";     // 拟合结果存储路径
	CString				_s_tmp, _filename, _savename;


	// 获得brdf_simple数据的采样角度
	void ComputeSamplingAngle(int thetaInNumber, int phiInNumber, int Gap);
	// 最小二乘法求初值
	void GetOriginValue();
	// LM求最优值
	void GetFinalValue();
	// WardDuer模型函数
	static void WardDuerfun(double *p, double *hx, int m, int n, void *adata);


	//////////////////////////////////BRDF原始数据进行采样预处理的相关函数/////////////////////////////////
	// 两个相量的交叉乘积
	void cross_product(double* v1, double* v2, double* out);
	// 归一化矢量
	void normalize(double* v);
	// 沿一个轴旋转矢量
	void rotate_vector(double* vector, double* axis, double angle, double* out);
	// 将标准坐标转换为半矢量/差矢量坐标
	void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
		double& theta_half, double& fi_half, double& theta_diff, double& fi_diff);
	// 查找theta_half索引
	// 这是一个非线性映射！
	// In:  [0 .. pi/2]
	// Out: [0 .. 89]
	int theta_half_index(double theta_half);
	// 查找theta_diff索引
	// In:  [0 .. pi/2]
	// Out: [0 .. 89]
	int theta_diff_index(double theta_diff);
	// 查找phi_diff索引
	int phi_diff_index(double phi_diff);
	// 给定一对传入/传出角度，查找BRDF。
	void lookup_brdf_val(double* brdf, double theta_in, double fi_in,
		double theta_out, double fi_out,
		double& red_val, double& green_val, double& blue_val);
	// 读取BRDF数据
	bool read_brdf(const char *filename, double* &brdf);


};