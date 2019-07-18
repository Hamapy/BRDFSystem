#include "workerMeasurement.h"

////////////////////////////////AVT相机线程类定义////////////////////////////
WorkerMeasurement::WorkerMeasurement(VimbaSystem&	system, QObject *parent) :
_system(system),
QObject(parent)
{
	_mutex.lock();
	//cameraAVT = new AVTCamera(_workerID, _system);
	connect(this, SIGNAL(OnClose()), this, SLOT(CloseWorker()));
	connect(this, SIGNAL(done()), this, SLOT(ContributeBRDF()));

	sampleComm = new SampleComm();
	sampleComm->Init(2);
	slideComm = new SlideComm();
	slideComm->Init(9, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	illuminant = new Illuminant(5);
	illuminant->InitCOM();
	//illuminant->SetSteadyTime(30);
	//光源排布顺序
	_illuminantID = new UINT[196] { 0, 1, 2, 3, 25, 26, 27, 28, 29, 50, 51, 52, 53, 75, 76, 77, 78, 79, 100, 101, 102, 103, 125, 126, 127, 128, 129, 150, 151, 152, 153, 175, 176, 177, 178, 179, \
		4, 5, 6, 7, 8, 30, 31, 32, 33, 54, 55, 56, 57, 58, 80, 81, 82, 83, 104, 105, 106, 107, 108, 130, 131, 132, 133, 154, 155, 156, 157, 158, 180, 181, 182, 183, \
		9, 10, 11, 12, 34, 35, 36, 37, 59, 60, 61, 62, 84, 85, 86, 87, 109, 110, 111, 112, 134, 135, 136, 137, 159, 160, 161, 162, 184, 185, 186, 187, \
		13, 14, 15, 38, 39, 40, 41, 63, 64, 65, 88, 89, 90, 91, 113, 114, 115, 138, 139, 140, 141, 163, 164, 165, 188, 189, 190, 191, \
		16, 17, 18, 42, 43, 44, 66, 67, 68, 92, 93, 94, 116, 117, 118, 142, 143, 144, 166, 167, 168, 192, 193, 194, \
		19, 20, 21, 45, 46, 69, 70, 95, 96, 119, 120, 121, 145, 146, 169, 170, 195, 196, \
		22, 23, 47, 71, 72, 97, 122, 123, 147, 171, 172, 197, \
		24, 48, 73, 124, 148, 173, \
		49, 98, 149, 198 };
	_iID = 0;
	_sID = 0;
	_isReady = 0;
	_captureDone = 0;
	_measureFlag = 0;
	_sampleFlag = 0;
	//_exposureTime = 50;
	//_saveName = 0;
	//_saveName = new int[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	_seriesCAM = new bool[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};
	/*
	//_measureFlag = measureFlag;
	illuminant->InitCOM();
	if (_measureFlag == 1)
	illuminant->SetSteadyTime(10);
	if (_measureFlag == 2)
	illuminant->SetSteadyTime(360);
	_timerId = this->startTimer(10000);//设置定时器触发子线程capture
	*/
}
WorkerMeasurement::~WorkerMeasurement()
{
	delete illuminant;
	delete _illuminantID;
	delete sampleComm;
	delete slideComm;
	//delete cameraAVT;

}

void WorkerMeasurement::StartTimer(int measureFlag)
{
	//slideComm->MoveToX2();//滑轨就位
	_measureFlag = measureFlag;
	//if (_measureFlag == 1)
	//	illuminant->SetSteadyTime(30);
	//if (_measureFlag == 2)
	//	illuminant->SetSteadyTime(200);
	_timerId = this->startTimer(8000);
}

void WorkerMeasurement::timerEvent(QTimerEvent *event)
{
	if (!_isReady)
	{
		if (_measureFlag == 1)
		{
			if (_iID != 196)
			{
				illuminant->Suspend();
				//Sleep(500);
				illuminant->SetSteadyTime(30);
				illuminant->LightenById(_illuminantID[_iID]);
				illuminant->Start();
				Sleep(500);//避免接收到灯亮前的图像
				_iID++;
				_isReady = 1;
				emit readyForGrab();//通过主线程告诉相机咱切换到下一个灯了，你可以试试调整一下你的曝光时间
			}
			else if (_iID == 196)
			{
				emit done();
				_isReady = 1;
			}
		}

		if (_measureFlag == 2)
		{
			if (_iID != 196)
			{
				if (_sampleFlag == 0)
				{
					illuminant->Suspend();
					illuminant->SetSteadyTime(200);
					illuminant->LightenById(_illuminantID[_iID]);
					illuminant->Start();
					Sleep(500);
				}

				if (_sID != 9)//36个角度耗时太长
				{
					sampleComm->GotoNextPos(175);
					Sleep(3000);//等待样品台旋转，设置2秒时指令污染
					emit readyForGrab();
					_sID++;
					_isReady = 1;
					_sampleFlag = 1;
				}
				else
				{
					_sampleFlag = 0;
					_sID = 0;//开始转下一圈
					_iID++;
					_isReady = 1;
				}
			}
			else if (_iID == 196)
			{
				emit done();
				_isReady = 1;
			}
		}
	}
	//else //光源样品就位，保存相机线程发来的采集图像
	//{
	//SaveSeriesMat(workerID, mat);
	//emit readyForGrab();
	//}
}

void WorkerMeasurement::CheckDone(int workerID)
{
#define CAM_NUM 9

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (workerID == i)
			_seriesCAM[i] = 1;
	}

	int s = 0;
	for (int i = 0; i < CAM_NUM; i++)
	{
		s += _seriesCAM[i];
	}
	if (s == CAM_NUM)
	{
		_isReady = 0;
		for (int i = 0; i < CAM_NUM; i++)
		{
			_seriesCAM[i] = 0;
		}
	}
	else
		_isReady = 1;
#undef CAM_NUM
}
////////////////////////////////////////////////////////////////////////////
// 函数：ContributeBRDF()
// 描述：采集原始图像完成后，生成材质数据
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void WorkerMeasurement::CloseWorker()
{
	this->killTimer(_timerId);
}
////////////////////////////////////////////////////////////////////////////
// 函数：AverageRGB()
// 描述：对采集得到的图像进行RGB三通道取平均得到RGB三通道的BRDF
// 输入：采集得到的图片
// 输出：Null
// 返回：返回RGB三通道BRDF值的向量
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
vector<double> WorkerMeasurement::AverageRGB(const Mat& inputImage)
{
	vector<double> temp = { 0, 0, 0 };
	int rowNumber = inputImage.rows;
	int colNumber = inputImage.cols;
	//对RGB三通道进行取平均值操作
	for (int i = 0; i<rowNumber; i++)
	{
		for (int j = 0; j<colNumber; j++)
		{
			temp[0] += inputImage.at<Vec3b>(i, j)[0];
			temp[1] += inputImage.at<Vec3b>(i, j)[1];
			temp[2] += inputImage.at<Vec3b>(i, j)[2];
		}
	}
	temp[0] = temp[0] / (rowNumber*colNumber);
	temp[1] = temp[1] / (rowNumber*colNumber);
	temp[2] = temp[2] / (rowNumber*colNumber);
	return temp;
}
////////////////////////////////////////////////////////////////////////////
// 函数：WriteBRDF()
// 描述：将一种材质在各个(光源-相机)角度下的BRDF值写进.binary文件
// 输入：
// 输出：Null
// 返回：是否读写成功
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool WorkerMeasurement::WriteBRDF()
{
	Mat Image;
	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	FILE *f = fopen(savePath, "wa+");
	if (f == 0)
	{
		return false;
	}
	string filename;
	double data[3] = { 0 };
	vector<double> tmp;
	int theta_out, fi_out, theta_in, fi_in;
	for (int i = 0; i<thetaOutNum; i++)
	{
		if (i == 0)
			theta_out = 0;
		else
			theta_out = i * 10 + 5;
		for (int j = 0; j<fiOutNum; j++)
		{
			fi_out = j*(360 / fiOutNum);
			for (int k = 0; k<thetaInNum; k++)
			{
				theta_in = k * 10;
				for (int m = 0; m<num[k]; m++)
				{
					fi_in = (360 / num[k])*m;
					filename = path + "out" + to_string(theta_out) + "_" + to_string(fi_out) +
						"in" + to_string(theta_in) + "_" + to_string(fi_in) + ".tiff";
					cout << filename << endl;
					Image = imread(filename);
					tmp = AverageRGB(Image);
					for (int n = 0; n < 3; n++)
						data[n] = tmp[n];
					fwrite(data, sizeof(double), 3, f);
				}
			}
		}
	}
	fclose(f);
	return true;
}
////////////////////////////////////////////////////////////////////////////
// 函数：ReadBrdf()
// 描述：从.binary文件读取出材质的BRDF数值
// 输入：
// 输出：Null
// 返回：是否读写成功
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool WorkerMeasurement::ReadBrdf(const char *filename, double* &brdf)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return false;
	int n = thetaOutNum*fiOutNum*lightSourceNum;
	brdf = (double*)malloc(sizeof(double) * 3 * n);
	fread(brdf, sizeof(double), 3 * n, f);
	fclose(f);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// 函数：theta_out_index()
// 描述：计算根据文件夹的命名中相机的theta角，得到当前角度的索引
// 输入：
// 输出：Null
// 返回：返回索引
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
inline int WorkerMeasurement::theta_out_index(int theta_out)
{
	int index = theta_out / 10;
	return index;
}
////////////////////////////////////////////////////////////////////////////
// 函数：fi_out_index()
// 描述：计算根据文件夹的命名中相机的fi角，得到当前角度的索引
// 输入：
// 输出：Null
// 返回：返回索引
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
inline int WorkerMeasurement::fi_out_index(int fi_out)
{
	int index = 0;
	index = fi_out / (360 / fiOutNum);
	return index;
}
////////////////////////////////////////////////////////////////////////////
// 函数：theta_in_index()
// 描述：计算根据文件夹的命名中光源的theta角，得到当前角度的索引
// 输入：
// 输出：Null
// 返回：返回索引
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
inline int WorkerMeasurement::theta_in_index(int theta_in)
{
	int index = theta_in / 10;
	return index;
}
////////////////////////////////////////////////////////////////////////////
// 函数：fi_in_index()
// 描述：计算根据文件夹的命名中相机的fi角，得到当前角度的索引
// 输入：
// 输出：Null
// 返回：返回索引
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
inline int WorkerMeasurement::fi_in_index(int fi_in, int theta_in)
{
	int num[9] = { 4, 6, 12, 18, 24, 28, 32, 36, 36 };
	int index = 0;
	int thetaInIndex = theta_in_index(theta_in);
	index = fi_in / (360 / num[thetaInIndex]);
	return index;
}
////////////////////////////////////////////////////////////////////////////
// 函数：LookupBrdfVal()
// 描述：根据给出的光源和相机的角度，计算出当前角度下的BRDF值
// 输入：光源和相机的角度以及所得到的BRDF数组。
// 输出：Null
// 返回：返回索引
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void WorkerMeasurement::LookupBrdfVal(double* brdf, int theta_in, int fi_in,
	int theta_out, int fi_out,
	double& red_val, double& green_val, double& blue_val)
{
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
	red_val = brdf[3 * ind];
	green_val = brdf[3 * ind + 1];
	blue_val = brdf[3 * ind + 2];
}








