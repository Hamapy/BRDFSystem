#include "brdfModeling.h"

//初始化局部静态变量
double	BRDFFitting::_T[SIZE * 3] = { 0 };
double  BRDFFitting::_p[7] = { 0 };

///////////////////////////////////BRDF参数拟合类公有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：StartFitting
// 描述：进行第BRDFSerialNumber个材质的参数拟合
// 输入：BRDFSerialNumber
// 输出：7个拟合参数值
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::StartFitting(int BRDFSerialNumber)
{
	
	
	BRDFSample(BRDFSerialNumber);
	GetOriginValue();
	GetFinalValue();


}


////////////////////////////////////////////////////////////////////////////
// 函数：BRDFSample
// 描述：对BRDF原始数据进行采样预处理
// 输入：BRDFSerialNumber
// 输出：
// 返回：brdf
// 备注：此处的BRDFSerialNumber只要输入BRDF文件的序号，序号值为1-100
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat BRDFFitting::BRDFSample(int BRDFSerialNumber)
{

	ComputeSamplingAngle();
	double* brdftmp, *InOut;
	int count;
	ofstream out;
	
	//_s_tmp.Format(("%d"), BRDFSerialNumber);
	_s_tmp.Format(_T("%d"), BRDFSerialNumber);
	if (BRDFSerialNumber < 10)
		_s_tmp = "00" + _s_tmp;
	else if (BRDFSerialNumber < 100)
		_s_tmp = "0" + _s_tmp;
	else if (BRDFSerialNumber = 100)
		_s_tmp = _s_tmp;
	_filename = _file_fold + _s_tmp + "\\brdf.binary";
	//char *readname = _filename.GetBuffer(sizeof(_filename));
	char *readname = (char*)_filename.GetBuffer(sizeof(_filename));
	_savename = _save_fold + _s_tmp + "\\brdf_simple.binary";
	cout << _filename << endl;

	// read brdf
	if (!read_brdf(readname, brdftmp))
	{
		fprintf(stderr, "Error reading %s\n", _filename);
		system("pause");
		exit(0);
	}

	InOut = (double*)malloc(sizeof(double) * 3 * _thetaOut * _thetaIn * _phiIn);
	int ii[15], jj[15], kk[35];
	//采样的角度
	for (int i = 0; i < _thetaOut; i++)
	{
		ii[i] = 10 + i * 5;
		jj[i] = 10 + i * 5;
	}

	for (int i = 0; i< _phiIn; i++)
	{
		kk[i] = 5 + 5 * i;
	}

	//BRDF采样
	count = 0;
	for (int i = 0; i < _thetaOut; i++)
	{
		double theta_out = double(ii[i]) * PI / _flatAngle;
		for (int j = 0; j < _thetaIn; j++)
		{
			double theta_in = double(jj[j]) * PI / _flatAngle;
			for (int k = 0; k < _phiIn; k++)
			{
				
				double phi_in = double(kk[k]) * PI / _flatAngle;
				double phi_out = 0.0;
				double red, green, blue;
				lookup_brdf_val(brdftmp, theta_in, phi_in, theta_out, phi_out, red, green, blue);
				double* data1 = _brdf.ptr<double>(count);
				InOut[count * 3 + 0] = red;
				InOut[count * 3 + 1] = green / _greenScale;
				InOut[count * 3 + 2] = blue / _blueScale;
				data1[0] = InOut[count * 3 + 0];
				data1[1] = InOut[count * 3 + 1];
				data1[2] = InOut[count * 3 + 2];
				count++;
			}
		}
	}
	
	out.open(_savename, ios::out | ios::binary);
	out.write((char*)&InOut[0], sizeof(double) * _thetaOut * _thetaIn * _phiIn * 3);
	out.close();
	free(brdftmp);
	free(InOut);
	return _brdf;

	


}




//////////////////////////////////BRDF参数拟合类私有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：ComputeSamplingAngle
// 描述：获得brdf_simple数据的采样角度
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::ComputeSamplingAngle()
{
	_Lth0 = Mat::ones(SIZE, 1, CV_64FC1);
	_Lph0 = Mat::ones(SIZE, 1, CV_64FC1);
	_Vth0 = Mat::ones(SIZE, 1, CV_64FC1);
	_brdf = Mat::ones(SIZE, 3, CV_64FC1);
	_brdfandAngle = Mat::zeros(SIZE, 6, CV_64FC1);
	_l = Mat::ones(SIZE, 1, CV_64FC1);
	_delta = Mat::ones(SIZE, 1, CV_64FC1);
	_G = Mat::ones(SIZE, 1, CV_64FC1);
	int i;
	for (i = 0; i < SIZE; i++)
	{
		double* data1 = _Lph0.ptr<double>(i);
		double* data2 = _Lth0.ptr<double>(i);
		double* data3 = _Vth0.ptr<double>(i);
		if ((i + 1) % _phiIn == 0)
		{
			data1[0] = _phiIn * 5.0 / _flatAngle * PI;
		}
		else
		{
			data1[0] = (i + 1) % _phiIn * 5.0 / _flatAngle * PI;
		}
		if ((i + 1) % (_thetaIn * _phiIn) == 0)
		{
			data2[0] = (_thetaIn * _phiIn / _phiIn + 1) * 5.0 / _flatAngle * PI;
		}
		else if ((i + 1) % _phiIn == 0)
		{
			data2[0] = ((i + 1) % (_thetaIn * _phiIn) / _phiIn + 1) * 5.0 / _flatAngle * PI;
		}
		else
		{
			data2[0] = ((i + 1) % (_thetaIn * _phiIn) / _phiIn + 2) * 5.0 / _flatAngle * PI;
		}

		data3[0] = (i / (_thetaIn * _phiIn) + 2) * 5.0 / _flatAngle * PI;


	}
	
}



////////////////////////////////////////////////////////////////////////////
// 函数：GetOriginValue
// 描述：最小二乘法求初值，计算模型相关变量
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::GetOriginValue()
{
	//角度和BRDF原始数据存储到BRDF变量中
	_Vth0.col(0).copyTo(_brdfandAngle.col(0));
	_Lth0.col(0).copyTo(_brdfandAngle.col(1));
	_Lph0.col(0).copyTo(_brdfandAngle.col(2));
	_brdf.copyTo(_brdfandAngle.colRange(3, 6));

	//给定alpha的初始值
	_alpha = 0.03;


	//计算模型所需要用的变量delta、l、v
	Mat v = Mat::ones(SIZE, 1, CV_64FC1);
	Mat L = Mat::ones(SIZE, 3, CV_64FC1);
	Mat V = Mat::ones(SIZE, 3, CV_64FC1);
	Mat H = Mat::ones(SIZE, 3, CV_64FC1);
	int i;
	for (i = 0; i < SIZE; i++)
	{
		double* data1 = v.ptr<double>(i);
		double* data2 = _l.ptr<double>(i);
		double* data3 = L.ptr<double>(i);
		double* data4 = V.ptr<double>(i);
		double* data5 = _brdfandAngle.ptr<double>(i);
		data1[0] = cos(data5[0]);
		data2[0] = cos(data5[1]);
		data3[0] = sin(data5[1])*cos(data5[2]);
		data3[1] = sin(data5[1])*sin(data5[2]);
		data3[2] = cos(data5[1]);
		data4[0] = sin(data5[0]);
		data4[1] = 0;
		data4[2] = cos(data5[0]);
	}
	H = L + V;
	Mat H1 = Mat::ones(SIZE, 3, CV_64FC1);
	Mat H2 = Mat::ones(SIZE, 1, CV_64FC1);
	Mat H3 = Mat::ones(SIZE, 1, CV_64FC1);
	pow(H, 2, H1);
	reduce(H1, H2, 1, CV_REDUCE_SUM);
	sqrt(H2, H3);
	H3.copyTo(H1.col(0));
	H3.copyTo(H1.col(1));
	H3.copyTo(H1.col(2));
	divide(H, H1, H);
	Mat vh = Mat::ones(SIZE, 1, CV_64FC1);
	Mat ps = Mat::ones(SIZE, 1, CV_64FC1);
	Mat P = Mat::ones(SIZE, 2, CV_64FC1) / PI;
	for (i = 0; i < SIZE; i++)
	{
		double* data1 = H.ptr<double>(i);
		double* data2 = _delta.ptr<double>(i);
		data2[0] = acos(data1[2]);

	}
	H1 = L.mul(V);
	reduce(H1, H2, 1, CV_REDUCE_SUM);
	sqrt((H2 + 1) / 2.0, vh);
	H2 = _l.mul(v) * 4 * PI;
	divide(1, H2, _G);
	for (i = 0; i < SIZE; i++)
	{
		double* data1 = H2.ptr<double>(i);
		double* data2 = _delta.ptr<double>(i);
		data1[0] = exp(-pow(tan(data2[0]), 2) / pow(_alpha, 2)) / pow(_alpha, 2);
	}
	ps = _G.mul(H2);
	ps.copyTo(P.col(1));


	//最小二乘求参数初值
	Mat b = Mat::ones(SIZE, 3, CV_64FC1);
	Mat R = Mat::ones(2, 3, CV_64FC1);
	Mat R1 = Mat::ones(2, 2, CV_64FC1);
	_brdf.copyTo(b);
	invert(P.t()*P, R1);
	R = R1*(P.t())*b;
	R.row(0).copyTo(_Ad);
	R.row(1).copyTo(_As);
}



////////////////////////////////////////////////////////////////////////////
// 函数：GetFinalValue
// 描述：应用LM算法求最优值并保存结果
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::GetFinalValue()
{

	//调用LM算法，通过传递初值计算最优值
	int m = 7, n = SIZE * 3;
	int i;
	double x[SIZE * 3];
	for (i = 0; i < SIZE; i++)
	{
		double* data1 = _brdf.ptr<double>(i);
		double* data2 = _l.ptr<double>(i);
		double* data3 = _G.ptr<double>(i);
		double* data4 = _delta.ptr<double>(i);
		x[i] = data1[0] * data2[0];
		x[i + SIZE] = data1[1] * data2[0];
		x[i + SIZE * 2] = data1[2] * data2[0];
		_T[i] = data2[0];
		_T[i + SIZE] = data3[0];
		_T[i + SIZE * 2] = data4[0];
	}
	double* data1 = _Ad.ptr<double>(0);
	double* data2 = _As.ptr<double>(0);

	//传入7个参数初值
	_p[0] = data1[0], _p[1] = data1[1], _p[2] = data1[2];
	_p[3] = data2[0], _p[4] = data2[1], _p[5] = data2[2], _p[6] = _alpha;
	int ret = dlevmar_dif(WardDuerfun, _p, x, m, n, 1000, NULL, NULL, NULL, NULL, NULL);
	printf("%f %f %f %f %f %f %f\n", _p[0], _p[1], _p[2], _p[3], _p[4], _p[5], _p[6]);


	//保存拟合参数结果
	_savename = _save_fold + _s_tmp + "\\WdDu_parameter.txt";
	ofstream out(_savename);
	for (int i = 0; i<6; i++)
		out << _p[i]<<" ";
	out << _p[6];
	out.close();
}



////////////////////////////////////////////////////////////////////////////
// 函数：WardDuerfun
// 描述：WardDuer模型函数
// 输入：p,m,n,adata
// 输出：hx
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::WardDuerfun(double *p, double *hx, int m, int n, void *adata)
{
	register int i;
	for (i = 0; i<(n / 3); ++i)
	{
		hx[i] = (p[0] / PI)*_T[i] + _T[i] * p[3] * _T[i + SIZE] * exp(-pow(tan(_T[i + SIZE * 2]), 2)*1.0 / pow(p[6], 2)) / pow(p[6], 2);
		hx[i + SIZE] = (p[1] / PI)*_T[i] + _T[i] * p[4] * _T[i + SIZE] * exp(-pow(tan(_T[i + SIZE * 2]), 2)*1.0 / pow(p[6], 2)) / pow(p[6], 2);
		hx[i + SIZE * 2] = (p[2] / PI)*_T[i] + _T[i] * p[5] * _T[i + SIZE] * exp(-pow(tan(_T[i + SIZE * 2]), 2)*1.0 / pow(p[6], 2)) / pow(p[6], 2);
	}

}





// 两个相量的交叉乘积
void BRDFFitting::cross_product(double* v1, double* v2, double* out)
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// 归一化矢量
void BRDFFitting::normalize(double* v)
{
	// 归一化
	double len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / len;
	v[1] = v[1] / len;
	v[2] = v[2] / len;
}

// 沿一个轴旋转矢量
void BRDFFitting::rotate_vector(double* vector, double* axis, double angle, double* out)
{
	double temp;
	double cross[3];
	double cos_ang = cos(angle);
	double sin_ang = sin(angle);

	out[0] = vector[0] * cos_ang;
	out[1] = vector[1] * cos_ang;
	out[2] = vector[2] * cos_ang;

	temp = axis[0] * vector[0] + axis[1] * vector[1] + axis[2] * vector[2];
	temp = temp*(1.0 - cos_ang);

	out[0] += axis[0] * temp;
	out[1] += axis[1] * temp;
	out[2] += axis[2] * temp;

	cross_product(axis, vector, cross);

	out[0] += cross[0] * sin_ang;
	out[1] += cross[1] * sin_ang;
	out[2] += cross[2] * sin_ang;
}


// 将标准坐标转换为半矢量/差矢量坐标
void BRDFFitting::std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
	double& theta_half, double& fi_half, double& theta_diff, double& fi_diff)
{

	// 计算入射向量
	double in_vec_z = cos(theta_in);
	double proj_in_vec = sin(theta_in);
	double in_vec_x = proj_in_vec*cos(fi_in);
	double in_vec_y = proj_in_vec*sin(fi_in);
	double in[3] = { in_vec_x, in_vec_y, in_vec_z };
	normalize(in);


	// 计算出射向量
	double out_vec_z = cos(theta_out);
	double proj_out_vec = sin(theta_out);
	double out_vec_x = proj_out_vec*cos(fi_out);
	double out_vec_y = proj_out_vec*sin(fi_out);
	double out[3] = { out_vec_x, out_vec_y, out_vec_z };
	normalize(out);


	// compute halfway vector
	double half_x = (in_vec_x + out_vec_x) / 2.0f;
	double half_y = (in_vec_y + out_vec_y) / 2.0f;
	double half_z = (in_vec_z + out_vec_z) / 2.0f;
	double half[3] = { half_x, half_y, half_z };
	normalize(half);

	// 计算theta_half, fi_half
	theta_half = acos(half[2]);
	fi_half = atan2(half[1], half[0]);


	double bi_normal[3] = { 0.0, 1.0, 0.0 };
	double normal[3] = { 0.0, 0.0, 1.0 };
	double temp[3];
	double diff[3];

	// 计算diff向量
	rotate_vector(in, normal, -fi_half, temp);
	rotate_vector(temp, bi_normal, -theta_half, diff);

	// 计算theta_diff, fi_diff	
	theta_diff = acos(diff[2]);
	fi_diff = atan2(diff[1], diff[0]);

}


// 查找theta_half索引
// 这是一个非线性映射
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int BRDFFitting::theta_half_index(double theta_half)
{
	if (theta_half <= 0.0)
		return 0;
	double theta_half_deg = ((theta_half / (PI / 2.0))*BRDF_SAMPLING_RES_THETA_H);
	double temp = theta_half_deg*BRDF_SAMPLING_RES_THETA_H;
	temp = sqrt(temp);
	int ret_val = (int)temp;
	if (ret_val < 0) ret_val = 0;
	if (ret_val >= BRDF_SAMPLING_RES_THETA_H)
		ret_val = BRDF_SAMPLING_RES_THETA_H - 1;
	return ret_val;
}


// 查找theta_diff索引
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int BRDFFitting::theta_diff_index(double theta_diff)
{
	int tmp = int(theta_diff / (PI * 0.5) * BRDF_SAMPLING_RES_THETA_D);
	if (tmp < 0)
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_THETA_D - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_THETA_D - 1;
}


// 查找phi_diff索引
inline int BRDFFitting::phi_diff_index(double phi_diff)
{
	// 由于相互性，BRDF不变
	// phi_diff -> phi_diff + M_PI
	if (phi_diff < 0.0)
		phi_diff += PI;

	// In: phi_diff in [0 .. pi]
	// Out: tmp in [0 .. 179]
	int tmp = int(phi_diff / PI * BRDF_SAMPLING_RES_PHI_D / 2);
	if (tmp < 0)
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_PHI_D / 2 - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_PHI_D / 2 - 1;
}


// 给定一对传入/传出角度，查找BRDF。
void BRDFFitting::lookup_brdf_val(double* brdf, double theta_in, double fi_in,
	double theta_out, double fi_out,
	double& red_val, double& green_val, double& blue_val)
{
	// 转换为半角/差角坐标
	double theta_half, fi_half, theta_diff, fi_diff;

	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		theta_half, fi_half, theta_diff, fi_diff);


	// 查找索引.
	// 注意到由于假定为各向同性BRDF，因此忽略phi_half
	int ind = phi_diff_index(fi_diff) +
		theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
		BRDF_SAMPLING_RES_THETA_D;

	red_val = brdf[ind] * RED_SCALE;
	green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D / 2] * GREEN_SCALE;
	blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;


	if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0)
		fprintf(stderr, "Below horizon.\n");

}



// 读取BRDF数据
bool BRDFFitting::read_brdf(const char *filename, double* &brdf)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return false;

	int dims[3];
	fread(dims, sizeof(int), 3, f);

	int n = dims[0] * dims[1] * dims[2];
	if (n != BRDF_SAMPLING_RES_THETA_H *
		BRDF_SAMPLING_RES_THETA_D *
		BRDF_SAMPLING_RES_PHI_D / 2)
	{
		fprintf(stderr, "Dimensions don't match\n");
		fclose(f);
		return false;
	}

	brdf = (double*)malloc(sizeof(double) * 3 * n);
	fread(brdf, sizeof(double), 3 * n, f);

	fclose(f);


	return true;
}

