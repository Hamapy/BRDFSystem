#include "imageProcess.h"

ImageProcess::ImageProcess()
{

}

ImageProcess::~ImageProcess()
{

}

//////////////////////////////////���к���/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������RotateImageXoY
// �������ɼ�ͼ��XoY�Ƕ���ת
// ���룺
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void ImageProcess::RotateImageXoY(Mat& img, int sampleID, Point2f center)
{
	float angle = 10 * sampleID;
	float radian = (float)(angle / 180.0 * CV_PI);

	int maxBorder = (int)(max(img.cols, img.rows)* sqrt(2)); //��䵽45��ͼ��߽��������
	int dx = (maxBorder - img.cols) / 2;
	int dy = (maxBorder - img.rows) / 2;
	copyMakeBorder(img, img, dy, dy, dx, dx, BORDER_CONSTANT);

	//��ת
	//Point2f center((float)(img.cols / 2), (float)(img.rows / 2));//�ݶ�����ͼ������Ϊ��ת����
	Mat affMatrix = getRotationMatrix2D(center, angle, 1.0);//�����ת����
	warpAffine(img, img, affMatrix, img.size());

	/*
	//����ͼ����ת֮�����ͼ������ľ���
	float sinVal = abs(sin(radian));
	float cosVal = abs(cos(radian));
	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//��������߿�
	int x = (dst.cols - targetSize.width) / 2;
	int y = (dst.rows - targetSize.height) / 2;
	Rect rect(x, y, targetSize.width, targetSize.height);
	dst = Mat(dst, rect);
	*/
	return;
}
////////////////////////////////////////////////////////////////////////////
// ������AngelCalibration
// ���������������μ���Ʒ��ת�ǶȽ�������ͼ���뵽���ӽ�
// ���룺
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ComputeAffineTrans(vector<Point2f> pSrc, vector<Point2f> pDst)
{

	Mat affMatrix = getAffineTransform(pSrc, pDst);

	return affMatrix;
}
////////////////////////////////////////////////////////////////////////////
// ������AngelCalibration
// ��������������ͼ���뵽���ӽ�
// ���룺
// �����
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void ImageProcess::AngelCalibration(Mat& img, Mat affMatrix)
{
	warpAffine(img, img, affMatrix, img.size());
}
////////////////////////////////////////////////////////////////////////////
// ������IsProperExposure
// �������ж�����Ƿ����
// ���룺Null
// ������ع�ʱ��
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsProperExposure(Mat gray)
{
	//Mat gray;
	//cvtColor(src, gray, CV_BGR2GRAY);
	int sum = 0;
	const int size = gray.cols * gray.rows;
	//����Ҷ�ֵ��80-180��Χ�ڵ����ص����
	for (int i = 0; i < gray.rows; i++)
	{
		uchar* data = gray.ptr<uchar>(i);
		for (int j = 0; j < gray.cols; j++)
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
float ImageProcess::ComputeExposureTime(Mat gray, float oriExposureTime)
{
	//Mat gray;
	//cvtColor(oriMat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	if (ave < 10)
		return 50000;

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
	/*
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
	*/
	Mat mat;
	return mat;
}
//////////////////////////////////////////////////////////////////////////////
//// ������DeadPixelDetect
//// ���������CCD����λ��(δ����)
//// ����
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<vector<int>> ImageProcess::DeadPixelDetect(Mat src, int maxNum)
{
#define LAMDA 5.0  //����ֵ > mean + LAMDA * sigma ����Ϊ����
#define MASK_SEL 1 
#define MASK_UNSEL 0 //�Ƿ���ѡ����

	vector<vector<int>> deadPixelPos;

	//�Ծ��ȣ�ȫ�ֽӽ�0��ͼ��ɲ��û�������

	PIXEL pSel;
	Mat gray;
	Mat zeroMat = Mat::zeros(gray.size(), CV_8UC1);
	cvtColor(src, gray, CV_BGR2GRAY);
	Scalar mean;
	Scalar stddev;
	meanStdDev(gray, mean, stddev);
	float m = mean.val[0];
	float sigma = stddev.val[0];

	//ͨ��ѭ�������ֵ�ķ�����ȷ������
	for (int k = 0; k < maxNum; k++)
	{
		int maxVal = 0;
		for (int i = 0; i < gray.rows; i++)
		{
			uchar* data = gray.ptr<uchar>(i);
			for (int j = 0; j < gray.cols; j++)
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
			vector<int> deadPos = { pSel.i, pSel.j };
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
//// ������DeadPixelCorrection
//// ����������У��
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void ImageProcess::DeadPixelCorrection(Mat& src, vector<vector<int>> deadPos)
{
	vector<Mat> channels;
	split(src, channels);
	//vector<vector<int>>::iterator iter = deadPos.begin();

	for (int k = 0; k < 3; k++)
	{
		for (int i = 1; i < src.rows-1; i++)//��ʱ����͵�����Ա߽��ϵĵ㣨�Բ��ʲ��ֲ�����Ӱ�죩
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			for (int j = 1; j < src.cols-1; j++)
			{
				//for (; iter != deadPos.end(); iter++)
				for (int m = 0; m < deadPos.size(); m++)
				{
					if (deadPos[m][0] == i && deadPos[m][1] == j)
					{
						//ȡ8����ƽ��
						data[j] = (channels.at(k).ptr<uchar>(i - 1)[j - 1] + \
							channels.at(k).ptr<uchar>(i - 1)[j + 1] + \
							channels.at(k).ptr<uchar>(i)[j - 1] + \
							channels.at(k).ptr<uchar>(i)[j] + \
							channels.at(k).ptr<uchar>(i)[j + 1] + \
							channels.at(k).ptr<uchar>(i + 1)[j - 1] + \
							channels.at(k).ptr<uchar>(i + 1)[j] + \
							channels.at(k).ptr<uchar>(i + 1)[j + 1]) / 8;
					}
				}
			}
		}
	}

	return;
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
void ImageProcess::ComputeMask(map<string, Mat> srcs)
{
	int i = 0;
	//vector<Mat> dsts;
	Mat dst;
	map<string, Mat>::iterator iter;
	
	for (iter = srcs.begin(); iter != srcs.end(); iter++)
	{
		string path = "..//imgs_mask//";
		path = path + iter->first + ".bmp";
		cvtColor(iter->second, dst, CV_BGR2GRAY);
		//int th = ComputeThreshold(iter->second);
		threshold(dst, dst, 180, 255, CV_THRESH_BINARY);
		imwrite(path, dst);
		//dsts.push_back(dst);
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////
////// ������ComputeChessTrans
////// ��������������������У����������������ת������ת������ƽ������
////// ���룺matsΪ����ͼƬ�ļ��ϣ�boardSizeΪ���̸��Ͻǵ����Ϣ��squareSize�����̸���ÿ������ı߳���Ϣ
////// �����
////// ���أ����������Ϣ�Ľṹ��
////// ��ע�����ע��boardSizeҪ��ʵ�ʵĽǵ���Ϣƥ�䣬������򽫱���
////// Modified by 
////////////////////////////////////////////////////////////////////////////////
//CameraCalibrationParamaters ImageProcess::ComputeChessTrans(const vector<Mat>& mats, const Size boardSize, const Size squareSize)
//{
//	//CameraCalibrationParamaters camCalParas;
//	int imageCount = mats.size();
//	Size imageSize;
//	Mat imageInput;
//	//��ȡÿһ��ͼ�񣬴�����ȡ���ǵ㣬Ȼ��Խǵ���������ؾ�ȷ��
//	vector<Point2f> imagePointsBuf;/* ����ÿ��ͼ���ϼ�⵽�Ľǵ� */
//	vector<vector<Point2f>> imagePointsSeq;/* ����ÿ��ͼ���ϼ�⵽�Ľǵ� */
//	for (int i = 0; i < imageCount; i++)
//	{
//		imageInput = mats[i];
//		if (i == 0)
//		{
//			imageSize.width = imageInput.cols;
//			imageSize.height = imageInput.rows;
//		}
//		//ע�������boardSize��������ڼ������̸�ͼƬƥ�䣬��������˳�
//		if (0 == findChessboardCorners(imageInput, boardSize, imagePointsBuf))
//		{
//			//cout << "can not find chessboard corneres!\n";
//			exit(1);
//		}
//		else
//		{
//			Mat viewGray;
//			cvtColor(imageInput, viewGray, CV_RGB2GRAY);
//			/* �����ؾ�ȷ�� */
//			find4QuadCornerSubpix(viewGray, imagePointsBuf, Size(5, 5)); //�Դ���ȡ�Ľǵ���о�ȷ��
//			imagePointsSeq.push_back(imagePointsBuf);  //���������ؽǵ�
//			/* ��ͼ������ʾ�ǵ�λ�� */
//			//drawChessboardCorners(viewGray, boardSize, imagePointsBuf, true);
//			//waitKey(500);
//		}
//	}
//	vector<vector<Point3f>> objectPoints;
//	/* ������ڲ������� */
//	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
//	vector<int> pointCounts;
//	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0)); /* �������5������ϵ����k1,k2,p1,p2,k3 */
//	/* ��ʼ���궨���Ͻǵ����ά���� */
//	int i, j, t;
//	for (t = 0; t<imageCount; t++)
//	{
//		vector<Point3f> tempPointSet;
//		for (i = 0; i<boardSize.height; i++)
//		{
//			for (j = 0; j<boardSize.width; j++)
//			{
//				Point3f realPoint;
//				/* ����궨�������������ϵ��z=0��ƽ���� */
//				realPoint.x = i*squareSize.width;
//				realPoint.y = j*squareSize.height;
//				realPoint.z = 0;
//				tempPointSet.push_back(realPoint);
//			}
//		}
//		objectPoints.push_back(tempPointSet);
//	}
//	/* ��ʼ��ÿ��ͼ���еĽǵ��������ٶ�ÿ��ͼ���ж����Կ��������ı궨�� */
//	for (i = 0; i<imageCount; i++)
//	{
//		pointCounts.push_back(boardSize.width*boardSize.height);
//	}
//	/* ��ʼ�궨 */
//	calibrateCamera(objectPoints, imagePointsSeq, imageSize, cameraMatrix, distCoeffs, cameraCalibrationParamaters.rvecsMats, cameraCalibrationParamaters.tvecsMats, 0);
//	//�Ա궨�����������
//	double total_err = 0.0; /* ����ͼ���ƽ�������ܺ� */
//	double err = 0.0; /* ÿ��ͼ���ƽ����� */
//	vector<Point2f> image_points2; /* �������¼���õ���ͶӰ�� */
//	vector<Mat> rotationMatrixs(imageCount);
//	for (i = 0; i<imageCount; i++)
//	{
//		vector<Point3f> tempPointSet = objectPoints[i];
//		/* ͨ���õ������������������Կռ����ά���������ͶӰ���㣬�õ��µ�ͶӰ�� */
//		projectPoints(tempPointSet, rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, image_points2);
//		/* �����µ�ͶӰ��;ɵ�ͶӰ��֮������*/
//		vector<Point2f> tempImagePoint = imagePointsSeq[i];
//		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
//		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
//		for (int j = 0; j < tempImagePoint.size(); j++)
//		{
//			image_points2Mat.at<Vec2f>(0, j) = Vec2f(image_points2[j].x, image_points2[j].y);
//			tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
//		}
//		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
//		total_err += err /= pointCounts[i];
//		Rodrigues(camCalParas.rvecsMats[i], rotationMatrixs[i]);
//	}
//	camCalParas.Count = imageCount;
//	camCalParas.cameraMatrix = cameraMatrix;
//	camCalParas.distCoeffs = distCoeffs;
//	camCalParas.error = total_err / imageCount;
//	camCalParas.rotationMatrixs = rotationMatrixs;
//	return camCalParas;
//}
////////////////////////////////////////////////////////////////////////////////
////// ������Calibration
////// ��������������Ļ���ϵ�����ڲ���У�������ͼƬ
////// ���룺imageSourceΪ��У����ͼƬ��cameraMatrixΪ������ڲ�����distCoeffsΪ����ϵ������
////// �����
////// ���أ�����У�����ͼƬ
////// ��ע��
////// Modified by 
////////////////////////////////////////////////////////////////////////////////
//void ImageProcess::DistortionCorrection(Mat& imageSource, Mat cameraMatrix, Mat distCoeffs)
//{
//	Size imageSize;
//	imageSize.width = imageSource.cols;
//	imageSize.height = imageSource.rows;
//	Mat R = Mat::eye(3, 3, CV_32F);
//	Mat mapx = Mat(imageSize, CV_32FC1);
//	Mat mapy = Mat(imageSize, CV_32FC1);
//	//Mat newImage = imageSource.clone();
//	initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
//	remap(imageSource, imageSource, mapx, mapy, INTER_LINEAR);
//	return;
//}

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

	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);
	
	//�����ɫ������ͨ����ƽ��ֵ
	int sum[3] = {};//B G R
	float v[3] = {};
	float ave[3] = {};
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				//if (data[j] == 0)
				//	break;
				v[k] += data[j];				
				sum[k]++;
			}
		}
		ave[k] = v[k] / float(sum[k]);
	}

	//RGB��ͨ��ֵ��һ����ȥ������Ӱ��
	ave[2] = ave[2] / (ave[2] + ave[1] + ave[0]);
	ave[1] = ave[1] / (ave[2] + ave[1] + ave[0]);
	ave[0] = 1 - ave[2] - ave[1];
	
	//������ͨ��ƽ��ֵ�е����ֵ��������ͨ���任һ��
	////ͨ�����������Ҷ����죬��ֱ�����쵽255��ʵ��һ���ģ�
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

	static float trans[3];
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
void ImageProcess::WhiteBalance(Mat& src, float* trans)
{
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);

	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);

	//map<int,map<int,float>> selectPoints;
	Mat selectPoints = Mat::zeros(height, width, CV_32FC3);//������255�ĵ㣬��Mat������ܶ�

	float tempVal = 0;
	float tempTrans;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			uchar* data_s = selectPoints.ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				tempVal = float(data[j]) * trans[k];
				if (!IsSelected(gray, i, j))
				{
					if (tempVal > 255)
					{
						data[j] = 255;
						tempTrans = 255.00 / float(data[j]);
						//selectPoints[i][j] = tempTrans;
						data_s[j] = tempTrans;
						Select(gray, i, j);
					}
					else
						data[j] = tempVal;
				}	
				else
				{
					//tempTrans = selectPoints[i][j];
					data_s[j] = tempTrans;
					data[j] = tempTrans * data[j];
				}				
			}
		}
	}
	//Mat	dst(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, src);
	return;
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
//Mat ImageProcess::GrayStrech(Mat src)
//{
//	Mat gray;
//	cvtColor(src, gray, CV_BGR2GRAY);
//	double minVal = 0, maxVal = 0;
//	minMaxLoc(gray, &minVal, &maxVal);
//
//	float tran = 255.00 / maxVal;
//	float trans[3] = { tran };
//	Mat dst = WhiteBalance(src, trans);//�����ؽ����˰�ƽ�����ѭ����������д��
//
//	return dst;
//}
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
map<string, Mat> ImageProcess::ReadImages(cv::String path)
{
	map<string, Mat> mats;
	String imgPattern = "//*.bmp";
	vector<String> imgFiles;
	imgPattern = path + imgPattern;
	glob(imgPattern, imgFiles);
	for (size_t i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		//��ȡͼƬ�ļ���
		string maskName = imgFiles[i];
		char maskID[16];
		int cnt_index = 0;
		for (int j = 0; maskName[j] != '\0'; ++j)
		{
			if ((maskName[j] >= '0'&& maskName[j] <= '9') || maskName[j] == '-') //���������.
			{
				//cnt_int *= 10;//����10��֤�ȼ�⵽�����ִ��ڸ�λ������Ĵ��ڵ�λ
				//cnt_int += maskName[i] - '0'; //�����ַ���ascii-�ַ�'0'��ascii��͵��ڸ�����.
				maskID[cnt_index++] = maskName[j];
			}
		}
		maskID[cnt_index++] = '\0'; //�����ַ���������
		mats[maskID] = mat;
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
Mat ImageProcess::ComputeWhiteArea(Mat mask, Mat src)
{
	int i, j;
	int maskCols, maskRows;
	maskCols = mask.cols;
	maskRows = mask.rows;
	Scalar total;
	int sumAll;
	float count = 0.0;
	float mean = 0.0;
	float variance = 0.0;
	cvtColor(mask, mask, CV_BGR2GRAY);
	//�������n
	int n = countNonZero(mask);
	//Ԫ���ܺ�sumAll
	total = sum(mask);
	sumAll = total[0];

	//�����Ԫ�ؾ�ֵmean
	mean = sumAll*1.0 / n;

	//�����Ԫ�ط���variance
	for (i = 0; i < maskRows; i++)
	{
		uchar* data2 = mask.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data2[j] != 0)
			{
				count = count + pow(data2[j] - mean, 2);
			}

		}
	}

	variance = count*1.0 / n;

	if (mean<220.0&&variance>5.0)
	{
		Mat whiteArea = Mat::zeros(maskRows, maskCols, CV_8UC3);
		return whiteArea;
	}
	else
	{
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
		return whiteArea;
	}
}

//////////////////////////////////////////////////////////////////////////////
//// ������ComputeKeyPoints
//// ��������ȡ��ͨ��ļ�������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<Point2f> ImageProcess::ComputeKeyPoints(Mat src)
{
	GaussianBlur(src, src, Size(5, 5), 0);
	vector<vector<Point> > contours;//contours�����ͣ�˫�ص�vector
	vector<Vec4i> hierarchy;//Vec4i��ָÿһ��vectorԪ�������ĸ�int�����ݡ�
	//��ֵ
	//����mask��ʱ����ת�Ҷ�ͼ
	cvtColor(src, src, CV_BGR2GRAY);
	int th = ComputeThreshold(src);
	threshold(src, src, th, 255, THRESH_BINARY);
	//��ʴ
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(src, src, element);

	findContours(src.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//�ų���������������
	vector<vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if ((itc->size()) < 50 || (itc->size()) > 200)
		{
			itc = contours.erase(itc);
		}
		else ++itc;
	}
	/// �����
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}
	///  �������ľ�:
	vector<Point2f> pts;
	for (int i = 0; i < contours.size(); i++)
	{
		pts[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}
	/// ��������
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255);
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, pts[i], 4, color, -1, 8, 0);
	}
	imshow("outImage", drawing);
	waitKey();

	return pts;
}

//////////////////////////////////////////////////////////////////////////////
//// ������ComputeSampleArea
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//// ������ComputeThreshold
//// ������
//// ���룺
//// �����
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
int ImageProcess::ComputeThreshold(Mat src)
{
	Mat srcImage = src.clone();
	//cvtColor(srcImage, srcImage, CV_BGR2GRAY);
	srcImage.reshape(0, 1);
	Size imageSize = srcImage.size();
	cv::sort(srcImage, srcImage, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	uchar* data = srcImage.ptr<uchar>(0);
	int index = (int)imageSize.width*imageSize.height*0.20;
	int total = 0;
	for (int i = 0; i < index; i++)
	{
		total += data[i];
	}
	return total / float(index);
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
void ImageProcess::ComputeSampleArea(Mat& src, vector<Point2f> points)
{
	//7.31���� RotatedRect
	/*
	int i, j;
	const int srcCols = src.cols; 
	const int srcRows = src.rows;

	int xMin, xMax, yMin, yMax;
	int countRows;
	int countCols;

	xMin = srcRows;
	yMin = srcCols;
	xMax = 0;
	yMax = 0;

	for (i = 0; i < points.size(); i++)
	{
		if (points[i].x < xMin)
			xMin = points[i].x;
		if (points[i].y < yMin)
			yMin = points[i].y;
		if (points[i].x > xMax)
			xMax = points[i].x;
		if (points[i].y > yMax)
			yMax = points[i].y;
	}

	countRows = (xMax - xMin + 1) * 130 / 145.00;
	countCols = (yMax - yMin + 1) * 130 / 145.00;

	Mat temp = Mat::zeros(srcRows, srcCols, CV_8UC1);
	for (i = xMin + (xMax - xMin + 1) * 7.5 / 145.00; i < xMin + (xMax - xMin + 1) * 7.5 / 145.00 + countRows; i++)
	{
		uchar* data2 = temp.ptr<uchar>(i);
		for (j = yMin + (yMax - yMin + 1) * 7.5 / 145.00; j < yMin + (yMax - yMin + 1) * 7.5 / 145.00 + countCols; j++)
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

	//Mat	sampleArea(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, src);
	*/


	//��Ӿ���
	RotatedRect rect = minAreaRect(points);
	//��Ӿ��ε�����
	Point2f center = rect.center;
	Mat rot_mat = getRotationMatrix2D(center, rect.angle, 1.0);//����ת����
	Mat rot_image;//src��ת��ˮƽ���ͼ��
	Size dst_sz(src.size());
	warpAffine(src, rot_image, rot_mat, dst_sz);//ԭͼ����ת����Ӿ���ˮƽ

	//��������ȡ��Ӿ����е�һ������
	Rect rect1(center.x - (rect.size.width / 2) + rect.size.width * 7.5 / 145, center.y - (rect.size.height / 2) + rect.size.height * 7.5 / 145, rect.size.width * 130 / 145, rect.size.height * 130 / 145);
	rot_image(rect1).copyTo(src);

	return;
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
Mat ImageProcess::ReadMask(int cameraID, int sampleID)
{
	string path = "..//imgs_mask//";
	stringstream ss;
	ss << cameraID;
	ss << "-";
	ss << sampleID;
	string ID = ss.str();
	string filename = path + ID + ".bmp";
	Mat mask = imread(filename);
	return mask;
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
Point2f ImageProcess::ComputeCenterPoint(vector<Point2f> points)
{
	float xMin = 0.0;
	float xMax = 0.0;
	float yMin = 0.0;
	float yMax = 0.0;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].x < xMin)
			xMin = points[i].x;
		if (points[i].y < yMin)
			yMin = points[i].y;
		if (points[i].x > xMax)
			xMax = points[i].x;
		if (points[i].y > yMax)
			yMax = points[i].y;
	}
	static Point2f center;
	center.x = (xMin + xMax) / 2;
	center.y = (yMin + yMax) / 2;

	return center;
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
void ImageProcess::CutSampleArea(Mat& src, Point2f pCenter)
{
	//src = src(Rect(pCenter.x-15, pCenter.y-15, 30, 30));//�Լ�������Ч����
	src = src(Rect(src.rows / 2 - 10, src.cols / 2 - 10, 20, 20));//�Լ�������Ч����

	/*
	int rowNumber = imageROI.rows;
	int colNumer = imageROI.cols;
	vector<double> average = { 0, 0, 0 };
	for (int i = 0; i < rowNumber; i++)
	{
		for (int j = 0; j < colNumer; j++)
		{
			average[0] += imageROI.at<Vec3b>(i, j)[0];
			average[1] += imageROI.at<Vec3b>(i, j)[1];
			average[2] += imageROI.at<Vec3b>(i, j)[2];
		}
	}
	for (int i = 0; i < 3; i++)
	{
		average[i] /= (rowNumber*colNumer);
	}
	return average;
	*/
}