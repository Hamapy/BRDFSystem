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
#define SIZE 1764     // 根据角度信息得到的数据量,SIZE=thetaOutNum*fiOutNum*lightSourceNum

class BRDFFitting
{
public:
	BRDFFitting();
	~BRDFFitting();

	// 根据BRDF文件名称读取相应文件,开始拟合
	Mat StartFitting(string MaterialName);
	// 根据BRDF文件称读取BRDF数据，并进行归一化
	Mat  BRDFRead(string MaterialName);




private:

	const double _flatAngle = 180.0;
	const int _thetaOutNum = 9;
	const int _thetaIntNum = 9;
	const int _fiOutNum = 1;
	const int _lightSourceNum = 196;

	static double		_T[SIZE * 3];   // 存储模型相关变量_l、_G、_delta
	static double       _p[7];          // 7个参数值
	Mat                 _param;         //最终返回的7个参数矩阵
	double				_alpha;
	Mat					_vth0;          // 对应_thetaOut
	Mat					_lth0;          // 对应_thetaIn
	Mat					_lph0;          // 对应_phiIn
	Mat					_l, _G, _delta;   // 模型计算用到的一些参数
	Mat                 _brdfandAngle, _brdf;  // _brdfandAngle存储角度信息和brdf数据，_brdf存储brdf数据 
	Mat                 _Ad, _As;         // 漫反射、镜面反射系数初值
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////根据具体存放位置修改//////////////////////////////////////////////////////////////////////////////////
	CString				_file_fold = "..\\..\\..\\Param\\001\\";     // BRDF原始数据文件路径
	CString				_save_fold = "..\\..\\..\\Param\\001\\";     // 拟合结果存储路径
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString				_materialName, _fileName, _saveName;


	// 获得brdf_simple数据的采样角度
	void ComputeAngle();
	// 最小二乘法求初值
	void GetOriginValue();
	// LM求最优值
	void GetFinalValue();
	// WardDuer模型函数
	static void WardDuerfun(double *p, double *hx, int m, int n, void *adata);


	////////////////////////////////////BRDF原始数据进行采样预处理的相关函数/////////////////////////////////

	//给定一对传入 / 传出角度，查找BRDF。
	void lookup_brdf_val(double* brdf, double theta_in, double fi_in,
		double theta_out, double fi_out,
		double& red_val, double& green_val, double& blue_val);
	//查找fi_in索引
	inline int fi_in_index(double fi_in, double theta_in);
	//查找theta_in索引
	inline int theta_in_index(double theta_in);
	//查找fi_out索引
	inline int fi_out_index(double fi_out);
	//查找theta_out索引
	inline int theta_out_index(double theta_out);
	// 读取BRDF数据
	bool read_brdf(const char *filename, double* &brdf);


};

