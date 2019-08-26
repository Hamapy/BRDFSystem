#include "brdfModeling.h"


// 初始化局部静态变量
double	BRDFFitting::_T[SIZE * 3] = { 0 };
double  BRDFFitting::_p[7] = { 0 };

BRDFFitting::BRDFFitting()
{

};

BRDFFitting::~BRDFFitting()
{

};

///////////////////////////////////BRDF参数拟合类公有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：StartFitting
// 描述：根据材质名称进行材质的参数拟合
// 输入：MaterialName
// 输出：7个拟合参数值
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat BRDFFitting::StartFitting(string MaterialName)
{

	Mat params = Mat::zeros(1, 7, CV_64FC1);
	BRDFRead(MaterialName);
	GetOriginValue();
	GetFinalValue();

	double* data1 = params.ptr<double>(0);
	for (int i = 0; i < 7; i++)
	{
		data1[i] = _p[i];
	}

	return params;
}


////////////////////////////////////////////////////////////////////////////
// 函数：BRDFSample
// 描述：对BRDF原始数据根据角度读取对于BRDF值，并进行归一化
// 输入：MaterialName
// 输出：
// 返回：_brdf
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat BRDFFitting::BRDFRead(string materialName)
{
	ComputeAngle();
	double* brdftmp;
	int count;
	ofstream out;

	_materialName = materialName.c_str();
	string fileName = _file_fold + materialName;
	const char* readname = fileName.c_str();
	//char *readname =(char*)_fileName.GetBuffer(sizeof(_fileName));
	//cout << _fileName << endl;

	// 读取整个BRDF
	read_brdf(readname, brdftmp);
	//if (!read_brdf(readname, brdftmp))
	//{
	//	fprintf(stderr, "Error reading %s\n", _fileName);
	//	system("pause");
	//	exit(0);
	//}


	// 根据四个角度读取对应BRDF存放到_brdf中
	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	count = 0;

	for (int i = 0; i < _thetaOutNum; i++)
	{
		double theta_out;
		if (i == 0)
			theta_out = 0.0;
		else
			theta_out = double(i * 10.0 + 5.0);

		for (int j = 0; j < _thetaIntNum; j++)
		{
			double theta_in = double(j * 10.0);
			if (theta_in == 0)
				theta_in = double(5.0);

			for (int k = 0; k < num[j]; k++)
			{
				double phi_in = (_flatAngle * 2.0 / num[j]) * k;
				double phi_out = 0.0;
				double red, green, blue;
				lookup_brdf_val(brdftmp, theta_in, phi_in, theta_out, phi_out, red, green, blue);
				double* data1 = _brdf.ptr<double>(count);
				data1[0] = red;
				data1[1] = green;
				data1[2] = blue;
				count++;
			}
		}
	}

	free(brdftmp);


	// 对读取的BRDF进行归一化
	double maxR, maxG, maxB;
	double minR, minG, minB;
	maxR = maxG = maxB = 0.0;
	minR = minG = minB = 1000.0;
	for (int i = 0; i < SIZE; i++)
	{
		double* data1 = _brdf.ptr<double>(i);
		if (data1[0]>maxR) maxR = data1[0];
		if (data1[1]>maxG) maxG = data1[1];
		if (data1[2]>maxB) maxB = data1[2];
		if (data1[0]<minR) minR = data1[0];
		if (data1[1]<minG) minG = data1[1];
		if (data1[2]<minB) minB = data1[2];
	}

	for (int i = 0; i < SIZE; i++)
	{
		double* data1 = _brdf.ptr<double>(i);
		data1[0] = (data1[0] - minR) / (maxR - minR);
		data1[1] = (data1[1] - minG) / (maxG - minG);
		data1[2] = (data1[2] - minB) / (maxB - minB);
	}
	Mat brdf = _brdf.clone();
	return brdf;
}




//////////////////////////////////BRDF参数拟合类私有函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：ComputeSampleAngle
// 描述：获得brdf数据的角度信息
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void BRDFFitting::ComputeAngle()
{
	_vth0 = Mat::zeros(SIZE, 1, CV_64FC1);
	_lth0 = Mat::zeros(SIZE, 1, CV_64FC1);
	_lph0 = Mat::zeros(SIZE, 1, CV_64FC1);
	_brdf = Mat::ones(SIZE, 3, CV_64FC1);
	_brdfandAngle = Mat::zeros(SIZE, 6, CV_64FC1);
	_l = Mat::ones(SIZE, 1, CV_64FC1);
	_delta = Mat::ones(SIZE, 1, CV_64FC1);
	_G = Mat::ones(SIZE, 1, CV_64FC1);

	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	double theta_out, theta_in, fi_in;
	int count = 0;
	for (int i = 0; i < _thetaOutNum; i++)
	{
		if (i == 0)
			theta_out = 0.0;
		else
			theta_out = (i * 10.0 + 5.0)* PI / _flatAngle;
		for (int j = 0; j < _thetaIntNum; j++)
		{
			theta_in = j * 10.0* PI / _flatAngle;
			if (theta_in == 0)
				theta_in = 5.0* PI / _flatAngle;
			for (int k = 0; k < num[j]; k++)
			{
				fi_in = (_flatAngle * 2.0 / num[j])*k;
				_vth0.at<double>(count, 0) = theta_out;
				_lth0.at<double>(count, 0) = theta_in;
				_lph0.at<double>(count, 0) = fi_in;
				count++;
			}
		}
	}

	return;
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

	// 角度和BRDF原始数据存储到BRDF变量中
	_vth0.col(0).copyTo(_brdfandAngle.col(0));
	_lth0.col(0).copyTo(_brdfandAngle.col(1));
	_lph0.col(0).copyTo(_brdfandAngle.col(2));
	_brdf.copyTo(_brdfandAngle.colRange(3, 6));

	// 给定alpha的初始值
	_alpha = 0.03;


	// 计算模型所需要用的变量delta、l、v
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


	// 最小二乘求参数初值
	Mat b = Mat::ones(SIZE, 3, CV_64FC1);
	Mat R = Mat::ones(2, 3, CV_64FC1);
	Mat R1 = Mat::ones(2, 2, CV_64FC1);
	_brdf.copyTo(b);
	invert(P.t()*P, R1);
	R = R1*(P.t())*b;
	R.row(0).copyTo(_Ad);
	R.row(1).copyTo(_As);

	return;
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

	// 调用LM算法，通过传递初值计算最优值
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

	// 传入7个参数初值
	_p[0] = data1[0], _p[1] = data1[1], _p[2] = data1[2];
	_p[3] = data2[0], _p[4] = data2[1], _p[5] = data2[2], _p[6] = _alpha;
	int ret = dlevmar_dif(WardDuerfun, _p, x, m, n, 1000, NULL, NULL, NULL, NULL, NULL);
	printf("%f %f %f %f %f %f %f\n", _p[0], _p[1], _p[2], _p[3], _p[4], _p[5], _p[6]);


	// 根据传入材质名称去除后缀，保存拟合参数结果
	int pos = _materialName.ReverseFind('.');
	CString paramName;
	if (pos > 0)
		paramName = _materialName.Left(pos);
	_saveName = _save_fold.c_str() + paramName + ".txt";
	ofstream out(_saveName);
	for (int i = 0; i<6; i++)
		out << _p[i] << " ";
	out << _p[6];
	out.close();

	return;
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

	return;
}







// Read BRDF data,n为数据库的维度,后面的为数据double
bool BRDFFitting::read_brdf(const char *filename, double* &brdf)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return false;
	int thetaOutNum = 9;
	int fiOutNum = 1;
	int lightSourceNum = 196;

	int n = thetaOutNum*fiOutNum*lightSourceNum;
	brdf = (double*)malloc(sizeof(double) * 3 * n);
	fread(brdf, sizeof(double), 3 * n, f);
	fclose(f);
	return true;
}
// first index
inline int BRDFFitting::theta_out_index(double theta_out)
{
	int index = theta_out / 10;
	return index;
}
// second index
inline int BRDFFitting::fi_out_index(double fi_out)
{
	int index = 0;
	index = fi_out / (360 / 12);
	return index;
}
// third index
inline int BRDFFitting::theta_in_index(double theta_in)
{
	int index = theta_in / 10;
	return index;
}
// fourth index
inline int BRDFFitting::fi_in_index(double fi_in, double theta_in)
{
	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	int index = 0;
	int thetaInIndex = theta_in_index(theta_in);
	index = fi_in / (360 / num[thetaInIndex]);
	return index;
}
// Given a pair of incoming/outgoing angles, look up the BRDF.
void BRDFFitting::lookup_brdf_val(double* brdf, double theta_in, double fi_in,
	double theta_out, double fi_out,
	double& red_val, double& green_val, double& blue_val)
{
	int lightSourceNum = 196;
	int fiOutNum = 1;
	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	// Find index.
	int ind = theta_out_index(theta_out) * lightSourceNum * fiOutNum +
		fi_out_index(fi_out) * lightSourceNum;
	int thetaInIndex = theta_in_index(theta_in);
	for (int i = 0; i<thetaInIndex - 1; i++)
	{
		ind += num[i];
	}
	ind += fi_in_index(fi_in, theta_in);
	red_val = brdf[3 * ind + 2];
	green_val = brdf[3 * ind + 1];
	blue_val = brdf[3 * ind];
}
