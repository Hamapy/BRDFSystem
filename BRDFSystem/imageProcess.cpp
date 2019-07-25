#include "imageProcess.h"

ImageProcess::ImageProcess()
{

}

ImageProcess::~ImageProcess()
{

}

//////////////////////////////////���к���/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������IsProperExposure
// �������ж�����Ƿ����
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsProperExposure(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	int sum = 0;
	const int size = gray.cols * gray.rows;
	//����Ҷ�ֵ��80-180��Χ�ڵ����ص����
	for (int i = 0; i < gray.rows; i++)
	{
		uchar* data = gray.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (100 < data[j] && data[j] < 180)//�趨��ֵ
			{
				sum++;
			}
		}
	}

	if (float(sum) / float(size) > 0.7)//�趨��ֵ
		return true;
	else
		return false;
}
////////////////////////////////////////////////////////////////////////////
// ������ComputeExposureTime
// ���������������һ���Ƕȹ�Դ�µĺ����ع�ʱ��
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
float ImageProcess::ComputeExposureTime(Mat oriMat, float oriExposureTime)
{
	Mat gray;
	cvtColor(oriMat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	float k = oriExposureTime / ave;//50ms������ͼ��
	float t = k * 255 * 0.60;//�趨��ֵ

	return t;
}

////////////////////////////////////////////////////////////////////////////
// ������ContributeHDR
// �������ϳɸ߶�̬��Χͼ��
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ContributeHDR(map<double, Mat> imgs, bool crfFlag)
{
	vector<Mat> srcs;
	vector<double> exposureTimes;
	map<double, Mat>::iterator it;
	for (it = imgs.begin(); it != imgs.end(); it++)
	{
		exposureTimes.push_back(it->first);
		srcs.push_back(it->second);
	}

	vector<Mat> ali;
	//MTBͼ�����
	//ͨ����ֵ1��������м����ȸ���������������ͼ���MTB������Ϊ0�� MTB���ع�ʱ�䲻�䡣
	Ptr<AlignMTB> alignMTB = createAlignMTB();
	alignMTB->process(srcs, ali);
	
	//�ڲɼ�BRDFʱ���ɼ�ͼ��ROI�������Ϊ�������أ������������ȿ���ͼ������ƽ�����Ƚ��ƣ��ʴ�ǰ���Կ���ΪCRFΪ���ԡ�
	//�����ɼ�svBRDFʱ��CRF�������Եģ���Ҫͨ����ͬ�ع�ʱ���ͼ�����CRF
	if (crfFlag == 1)
	{
		//��ȡ���CRF����(Camera Response Function)
		Mat responseDebevec;
		Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
		calibrateDebevec->process(ali, responseDebevec, exposureTimes);

		//�ϲ�����HDRͼ��
		Mat hdrDebevec;
		Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
		mergeDebevec->process(ali, hdrDebevec, exposureTimes, responseDebevec);
		//imwrite("hdrDebevec.hdr", hdrDebevec);

		//Dragoɫ��ӳ��
		Mat ldrDrago;
		Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
		tonemapDrago->process(hdrDebevec, ldrDrago);
		ldrDrago = 3 * ldrDrago;
		imwrite("ldr-Drago.jpg", ldrDrago * 255);

		return ldrDrago;
	}
	else
	{
		//��ȡ���CRF����(Camera Response Function)
		Mat responseDebevec;
		Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
		calibrateDebevec->process(ali, responseDebevec, exposureTimes);

		//�ϲ�����HDRͼ��
		Mat hdrDebevec;
		Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
		mergeDebevec->process(ali, hdrDebevec, exposureTimes);
		//imwrite("hdrDebevec.hdr", hdrDebevec);

		//Dragoɫ��ӳ��
		Mat ldrDrago;
		Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
		tonemapDrago->process(hdrDebevec, ldrDrago);
		ldrDrago = 3 * ldrDrago * 255;
		//imwrite("ldr-Drago.jpg", ldrDrago * 255);

		return ldrDrago;
	}	
}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<int*> ImageProcess::DeadPixelDetect(Mat src, int maxNum)
{
#define LAMDA 5.0  //����ֵ > mean + LAMDA * sigma ����Ϊ����
#define MASK_SEL 1 
#define MASK_UNSEL 0 //�Ƿ���ѡ����

	vector<int*> deadPixelPos;

	//�Ծ���ͼ�����ʱ���û�������
	/*
	int count = 0;  //��¼�������ڵ���Ŀ
	int x_step = cvCeil(x_percent*wndSize.width);
	int y_step = cvCeil(y_percent*wndSize.height);
	//���ô��ڶ�ͼ����б���
	for (int i = 0; i < src.cols - wndSize.width; i += y_step)
	{
	for (int j = 0; j < src.rows - wndSize.height; j += x_step)
	{
	Rect roi(Point(j, i), wndSize);
	Mat ROI = src(roi);
	wnd.push_back(ROI);
	count++;
	}
	}
	*/

	//���㲻��ͨ��Ӱ�죬ֱ�ӿ��Ҷ�ͼ�ͺ�
	/*
	//���ͼ����ͨ����ֵ�ͷ���
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float meanB = mean.val[0];
	float stddevB = stddev.val[0];
	float meanG = mean.val[1];
	float stddevG = stddev.val[1];
	float meanR = mean.val[2];
	float stddevR = stddev.val[2];

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);
	int ch = src.channels;
	*/

	PIXEL pSel;
	Mat gray;
	Mat zeroMat = Mat::zeros(gray.size(), CV_8UC1);
	cvtColor(src, gray, CV_BGR2GRAY);
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float m = mean.val[0];
	float sigma = stddev.val[0];

	//ͨ��ѭ�������ֵ�ķ�����ȷ������
	for (int k = 0; k < maxNum; k++)
	{
		int maxVal = 0;
		for (int i = 0; i < src.rows; i++)
		{
			uchar* data = gray.ptr<uchar>(i);
			for (int j = 0; j < src.cols; j++)
			{
				if (IsSelected(zeroMat, i, j) == MASK_UNSEL)
				{
					if (data[j] > maxVal)
					{
						maxVal = data[j];
						pSel.i = i;
						pSel.j = j;
					}
				}
			}
		}

		if (maxVal > m + LAMDA * sigma)  // ���õ����㻵������
		{
			int deadPos[2] = { pSel.i, pSel.j };
			deadPixelPos.push_back(deadPos);
			Select(zeroMat, pSel.i, pSel.j);
		}
		else
		{
			break;
		}
	}

	return deadPixelPos;
#undef LAMDA
#undef MASK_SEL
#undef MASK_UNSEL
}
//////////////////////////////////////////////////////////////////////////////
//// ������ComputeMask
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<Mat> ImageProcess::ComputeMask(vector<Mat> srcs)
{
	int i = 0;
	vector<Mat> dsts;
	Mat dst;
	vector<Mat>::iterator iter;
	
	for (iter = srcs.begin(); iter != srcs.end(); iter++)
	{
		string path = "..//imgs_mask//";
		i++;
		path = path + to_string(i) + ".bmp";
		cvtColor(*iter, dst, CV_BGR2GRAY);
		threshold(dst, dst, 25, 255, CV_THRESH_BINARY);
		imwrite(path, dst);
		dsts.push_back(dst);
	}

	return dsts;
}
//////////////////////////////////////////////////////////////////////////////
//// ������ComputeWhiteTrans
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
float* ImageProcess::ComputeWhiteTrans(Mat src)
{
	/*
	Mat src = AverageImage(mats);
#define WHITE_NUM 500
	//��ʱȡȫͼ������ߵ�һ���������ص��ֵ��Ϊ�׵�
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	Mat B = srcB.clone();
	Mat G = srcG.clone();
	Mat R = srcR.clone();

	//��Mat��������һά����
	unsigned char *arrayB = new unsigned char[srcB.rows*srcB.cols];
	if (srcB.isContinuous())
		arrayB = B.data;//��ֹ�޸�ԭͼ����˳��
	unsigned char *arrayG = new unsigned char[srcG.rows*srcG.cols];
	if (srcG.isContinuous())
		arrayG = G.data;
	unsigned char *arrayR = new unsigned char[srcR.rows*srcR.cols];
	if (srcR.isContinuous())
		arrayR = R.data;


	//��������ֵ������ص㼯��ƽ������
	sort(arrayB, arrayB + srcB.rows*srcB.cols);
	reverse(arrayB, arrayB + srcB.rows*srcB.cols);
	//sort(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	//reverse(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	int sumB = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumB += (int)arrayB[i];
	}
	uchar aveB = (float)sumB / (float)WHITE_NUM;

	sort(arrayG, arrayG + srcG.rows*srcG.cols);
	reverse(arrayG, arrayG + srcG.rows*srcG.cols);
	int sumG = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumG += (int)arrayG[i];
	}
	uchar aveG = (float)sumG / (float)WHITE_NUM;

	sort(arrayR, arrayR + srcR.rows*srcR.cols);
	reverse(arrayR, arrayR + srcR.rows*srcR.cols);
	int sumR = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumR += (int)arrayR[i];
	}
	uchar aveR = (float)sumR / (float)WHITE_NUM;

	float transB = 255 / (float)aveB;
	float transG = 255 / (float)aveG;
	float transR = 255 / (float)aveR;

	static float trans[3];
	trans[0] = transB;
	trans[1] = transG;
	trans[2] = transR;

	return trans;

#undef WHITE_NUM
	*/

	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	//Mat B = srcB.clone();
	//Mat G = srcG.clone();
	//Mat R = srcR.clone();
	
	//�����ɫ������ͨ����ƽ��ֵ
	int sum[3] = { 0, 0, 0 };//B G R
	float v[3] = { 0.00, 0.00, 0.00 };
	float ave[3] = { 0.00, 0.00, 0.00 };
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				if (data[j] == 0)
					break;
				v[k] += data[j];				
				sum[k]++;
			}
		}
		ave[k] = v[k] / float(sum[k]);
	}
	//������ͨ��ƽ��ֵ�е����ֵ��������ͨ���任һ��
	float temp = 0.00;
	int maxCh = 0;
	for (int i = 0; i < 3; i++)
	{
		if (ave[i] > temp)
		{
			maxCh = i;
			temp = ave[i];
		}
	}

	static float trans[3] = { 1, 1, 1 };
	for (int i = 0; i < 3; i++)
	{
		if (i == maxCh)
			trans[i] = 1;
		else
			trans[i] = temp / ave[i];
	}

	return trans;
}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::WhiteBalance(Mat src, float* trans)
{
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);

	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	//Mat srcB = channels.at(0);
	//Mat srcG = channels.at(1);
	//Mat srcR = channels.at(2);

	//float transB = trans[0];
	//float transG = trans[1];
	//float transR = trans[2];

	map<int,map<int,float>> selectPoints;

	float tempVal = 0;
	float tempTrans;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				tempVal = float(data[j]) * trans[k];
				if (!IsSelected(gray, i, j))
				{
					if (tempVal > 255)
					{
						data[j] = 255;
						tempTrans = 255.00 / float(data[j]);
						selectPoints[i][j] = tempTrans;
						Select(gray, i, j);
					}
					else
						data[j] = tempVal;
				}	
				else
				{
					tempTrans = selectPoints[i][j];
					data[j] = tempTrans * data[j];
				}				
			}
		}
	}

	return src;


	/*
	//��ÿ���㽫��������[0,255]֮��
	//��Գ���1�����أ����¹�һ��
	vector<vector<float>> fBGR(height, vector<float>(width, 0));
	float max = 0;
	//float s = 255 / (float)aveB;
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			fBGR[i][j] = srcB.at<uchar>(i, j) / 255.00 * transB;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			srcB.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveG;
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			fBGR[i][j] = srcG.at<uchar>(i, j) / 255.00 * transG;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			srcG.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveR;
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			fBGR[i][j] = srcR.at<uchar>(i, j) / 255.00 * transR;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			srcR.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}


	Mat dst(src.rows, src.cols, CV_8UC3, Scalar::all(0));
	channels.push_back(srcB);
	channels.push_back(srcG);
	channels.push_back(srcR);
	//merge({ srcB, srcG, srcR }, dst);
	merge(channels, dst);

	return dst;
	*/
}
//////////////////////////////////////////////////////////////////////////////
//// ������GrayStrech
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::GrayStrech(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	double minVal = 0, maxVal = 0;
	minMaxLoc(gray, &minVal, &maxVal);

	float tran = 255.00 / maxVal;
	float trans[3] = { tran };
	Mat dst = WhiteBalance(src, trans);//�����ؽ����˰�ƽ�����ѭ����������д��

	return dst;
}
/////////////////////////////////˽�к���/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsSelected(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	if (data[j] == 1)
		return true;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void ImageProcess::Select(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	data[j] = 1;

	return;
}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ���������ͼ����ƽ��
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
double ImageProcess::ComputeAverage(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	
	return scalar.val[0];
}

//////////////////////////////////////////////////////////////////////////////
//// ������
//// ���������ͼ����ƽ��
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::AverageImage(vector<Mat> mats)
{
	Mat aver = Mat::zeros(mats[0].size(), CV_32FC3);
	for (int i = 0; i < mats.size(); i++)
	{
		accumulate(mats[i], aver);
	}
	aver /= mats.size();
	aver.convertTo(aver, CV_8UC3);

	return aver;
}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<Mat> ImageProcess::ReadImages(cv::String path)
{
	vector<Mat> mats;
	String imgPattern = "//*.bmp";
	vector<String> imgFiles;
	imgPattern = path + imgPattern;
	glob(imgPattern, imgFiles);
	for (size_t i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		mats.push_back(mat);
	}

	return mats;
}
//////////////////////////////////////////////////////////////////////////////
//// ������ComputeWhiteArea
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//// ������ComputeSampleArea
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////