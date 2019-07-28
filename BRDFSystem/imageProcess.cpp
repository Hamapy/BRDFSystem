#include "imageProcess.h"

ImageProcess::ImageProcess()
{

}

ImageProcess::~ImageProcess()
{

}

//////////////////////////////////���к���/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������IsOverExposure
// �������ж�����Ƿ����
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsOverExposure(Mat src)
{
	//ͨ������ֱ��ͼ�жϲɼ�ͼ���Ƿ���ػ����

	return 0;

}
////////////////////////////////////////////////////////////////////////////
// ������GetExposureTime
// ���������������һ���Ƕȹ�Դ�µĺ����ع�ʱ��
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
float ImageProcess::ComputeExposureTime(Mat mat)
{
	Mat gray;
	cvtColor(mat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	float k = 50.00 / ave;//50ms������ͼ��
	float t = k * 255 * 0.60;

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
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
float* ImageProcess::ComputeWhiteTrans(vector<Mat> mats)
{
	Mat src = AverageImage(mats);

#define WHITE_NUM 500
	//��ʱȡȫͼ������ߵ�һ���������ص��ֵ��Ϊ�׵�
	//...
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
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	float transB = trans[0];
	float transG = trans[1];
	float transR = trans[2];

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
vector<Mat> ImageProcess::ReadImages(string path)
{
	vector<Mat> mats;
	string imgPattern = "//*.jpg";
	vector<cv::String> imgFiles;
	imgPattern = path + imgPattern;
	glob(imgPattern, imgFiles);
	for (int i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		mats.push_back(mat);
	}

	return mats;

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
Mat ImageProcess::ComputeWhiteArea(Mat mask, Mat src)
{
	int i, j;
	int maskCols, maskRows;
	maskCols = mask.cols;
	maskRows = mask.rows;
	Scalar total;
	int sumAll;
	int n;
	int Max = 0;
	float count = 0.0;
	float mean, variance;


	vector<Mat> channels;
	split(src, channels);

	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);


	for (i = 0; i < maskRows; i++)
	{
		uchar* data1 = mask.ptr<uchar>(i);

		for (j = 0; j < maskCols; j++)
		{

			data1[j] = data1[j] / 255;
		}
	}

	channels.at(0) = srcB.mul(mask);
	channels.at(1) = srcG.mul(mask);
	channels.at(2) = srcR.mul(mask);


	Mat	whiteArea(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, whiteArea);
	

	//������whiteAreaͬ���ͺ�ͬ��С�ľ���
	Mat grayImage;
	grayImage.create(whiteArea.size(), whiteArea.type());
	//��ԭͼת��Ϊ�Ҷ�ͼ��
	cvtColor(whiteArea, grayImage, CV_BGR2GRAY);

	//�������n
	n = countNonZero(mask);
	//Ԫ���ܺ�sumAll
	for (i = 0; i < maskRows; i++)
	{
		uchar* data3 = grayImage.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data3[j] > Max)
			{
				Max = data3[j];
			}

		}
	}
	//Ԫ���ܺ�sumAll
	total = sum(grayImage);
	sumAll = total[0];

	//�����Ԫ�ؾ�ֵmean
	mean = sumAll*1.0 / n;

	//�����Ԫ�ط���variance
	for (i = 0; i < maskRows; i++)
	{
		uchar* data2 = grayImage.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data2[j] != 0)
			{
				count = count + pow(data2[j] - mean, 2);
			}

		}
	}

	variance = count*1.0 / n;

	if (mean<=(Max*0.6)&&variance>1.0)
	{
		whiteArea = Mat::zeros(maskRows, maskCols, CV_8UC3);
		return whiteArea;
	}
	
	return whiteArea;

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
//Mat ImageProcess::ComputeSampleArea(Mat mask, Mat src)
//{
//	int i, j;
//	int maskCols, maskRows;
//	maskCols = mask.cols;
//	maskRows = mask.rows;
//
//	int xMin, xMax, yMin, yMax;
//	int countRows;
//	int countCols;
//
//	xMin = maskRows;
//	yMin = maskCols;
//	xMax = 0;
//	yMax = 0;
//
//
//	
//	//Ѱ�ҷ����
//	for (i = 0; i < maskRows; i++)
//	{
//		uchar* data1 = mask.ptr<uchar>(i);
//		for (j = 0; j < maskCols; j++)
//		{
//			if (data1[j] == 255)
//			{
//				
//				//��¼xMin
//				if (i < xMin)
//				{
//					xMin = i;
//				}
//	
//
//				//��¼xMax
//				if (i > xMax)
//				{
//					xMax = i;
//				}
//
//
//				//��¼yMin
//				if (j < yMin)
//				{
//					yMin = j;
//				}
//
//
//				//��¼yMax
//				if (j > yMax)
//				{
//					yMax = j;
//				}
//
//			}
//
//		}
//	}
//
//	countRows = (xMax - xMin + 1) * 130 / 200;
//	countCols = (yMax - yMin + 1) * 130 / 200;
//
//	Mat temp = Mat::zeros(maskRows, maskCols, CV_8UC1);
//	for (i = xMin + (xMax - xMin + 1) * 35 / 200; i < xMin + (xMax - xMin + 1) * 35 / 200 + countRows; i++)
//	{
//		uchar* data2 = temp.ptr<uchar>(i);
//		for (j = yMin + (yMax - yMin + 1) * 35 / 200; j < yMin + (yMax - yMin + 1) * 35 / 200 + countCols; j++)
//		{
//			data2[j] = 1;
//		}
//	}
//
//
//
//
//	vector<Mat> channels;
//	split(src, channels);
//
//	Mat srcB = channels.at(0);
//	Mat srcG = channels.at(1);
//	Mat srcR = channels.at(2);
//
//
//	channels.at(0) = srcB.mul(temp);
//	channels.at(1) = srcG.mul(temp);
//	channels.at(2) = srcR.mul(temp);
//
//
//
//	Mat	sampleArea(src.rows, src.cols, CV_8UC3, Scalar(0));
//	merge(channels, sampleArea);
//	return sampleArea;
//}
//////////////////////////////////////////////////////////////////////////////
//// ������
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ComputeSampleArea(vector<Point> point, Mat src)
{
	int i, j;
	int srcCols, srcRows;
	srcCols = src.cols;
	srcRows = src.rows;

	int xMin, xMax, yMin, yMax;
	int countRows;
	int countCols;

	xMin = srcRows;
	yMin = srcCols;
	xMax = 0;
	yMax = 0;

	for (i = 0; i < point.size(); i++)
	{
		if (point[i].x < xMin)
			xMin = point[i].x;
		if (point[i].y < yMin)
			yMin = point[i].y;
		if (point[i].x > xMax)
			xMax = point[i].x;
		if (point[i].y > yMax)
			yMax = point[i].y;
	}

	////Ѱ�ҷ����
	//for (i = 0; i < maskRows; i++)
	//{
	//	uchar* data1 = mask.ptr<uchar>(i);
	//	for (j = 0; j < maskCols; j++)
	//	{
	//		if (data1[j] == 255)
	//		{

	//			//��¼xMin
	//			if (i < xMin)
	//			{
	//				xMin = i;
	//			}


	//			//��¼xMax
	//			if (i > xMax)
	//			{
	//				xMax = i;
	//			}


	//			//��¼yMin
	//			if (j < yMin)
	//			{
	//				yMin = j;
	//			}


	//			//��¼yMax
	//			if (j > yMax)
	//			{
	//				yMax = j;
	//			}

	//		}

	//	}
	//}

	countRows = (xMax - xMin + 1) * 120 / 200;
	countCols = (yMax - yMin + 1) * 120 / 200;

	Mat temp = Mat::zeros(srcRows, srcCols, CV_8UC1);
	for (i = xMin + (xMax - xMin + 1) * 40 / 200; i < xMin + (xMax - xMin + 1) * 40 / 200 + countRows; i++)
	{
		uchar* data2 = temp.ptr<uchar>(i);
		for (j = yMin + (yMax - yMin + 1) * 40 / 200; j < yMin + (yMax - yMin + 1) * 40 / 200 + countCols; j++)
		{
			data2[j] = 1;
		}
	}




	vector<Mat> channels;
	split(src, channels);

	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);


	channels.at(0) = srcB.mul(temp);
	channels.at(1) = srcG.mul(temp);
	channels.at(2) = srcR.mul(temp);



	Mat	sampleArea(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, sampleArea);
	return sampleArea;
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
Mat ImageProcess::ReadMaskWithWorkerID(int workerID)
{
	string filename, path;
	string tmp;
	path = "..//imgs_mask//";
	stringstream ss;
	ss << workerID;
	string s1 = ss.str();
	tmp = "00" + s1;
	filename = path + tmp + ".bmp";
	Mat mask;
	mask = imread(filename);
	return mask;
}