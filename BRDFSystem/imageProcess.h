#pragma once
#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <algorithm>
#include <opencv2/photo.hpp>
//#include "opencv2/imgcodecs.hpp"  //���hpp�ļ�opencv2û�У���Ҫ�����汾
//#include "opencv2/imgproc.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/ml.hpp"
//#include "opencv2/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include "stdafx.h"

using namespace std;
using namespace cv;

typedef struct _tagPIXEL
{
	int i;   // i axis (row)
	int j;   // j axis (column)
}PIXEL;

class ImageProcess
{
public:
	ImageProcess();
	virtual ~ImageProcess();

	//����궨����
	//static CameraCalibrationParamaters ComputeChessTrans(const vector<Mat>& mats, const Size boardSize, const Size squareSize);
	//����������У��
	//static Mat ImageProcess::Calibration(Mat imageSource, Mat cameraMatrix, Mat distCoeffs);
	//�������ͼУ��
	static void RotateImageXoY(Mat& img, int sampleID, Point2f center);
	static Mat ComputeAffineTrans(vector<Point2f> pSrc, vector<Point2f> pDst);
	static void AngelCalibration(Mat& img, Mat trans);

	//���ɼ�Bayerͼ��RAWתRGBȥ������
	//Mat Demosiac(Mat src);

	//ͨ��ֱ��ͼ�����жϲɼ�ͼ���Ƿ���ػ����������������ع�ʱ��
	static bool IsProperExposure(Mat src);
	static float ComputeExposureTime(Mat oriMat, float oriExposureTime);

	//�߶�̬��ΧHDR
	//��ȡһ��RGBͼ��ƽ������
	static double ComputeAverage(Mat src);
	static Mat ContributeHDR(map<double, Mat> imgs, bool crfFlag);

	//����У��
	static vector<int*> DeadPixelDetect(Mat src, int maxNum);
	static Mat DeadPixelCorrection(Mat src);

	//����ƽ���� ������Ҫ��ȥ�Ļ�ֵ��MatLab����BlackLevel = 0ʱ���ص�ֵ��Χ1-2��0-255�������ݲ��ÿ���������ƽ����
	//static int ComputeBlackPedal(Mat src);
	//static Mat BlackLevelCorrection(Mat src);

	//��ֵ������Maskͼ��
	static void ComputeMask(map<string, Mat> src);

	//��ƽ��У��������ȫ���䣩&���ȶ�У��
	static float* ComputeWhiteTrans(Mat src);
	static Mat WhiteBalance(Mat src, float* trans);
	//static Mat GrayStrech(Mat src);//ֱ��д�ڰ�ƽ������
	
	//��ɫУ��������ʽ�ع飩�����ر任����
	//static vector<Mat> GetColorTrans(vector<Mat> mats);
	//Mat ColorCorrection(Mat src);



	friend class MainWindow;

private:
	//��λ��ɫ��ǵ�
	static vector<Point> findKeyPoints(Mat src);
	//��������ͼ����ƽ��
	static Mat AverageImage(vector<Mat> mats);
	//��ȡһ���ļ���������ͼƬ
	static map<string, Mat> ReadImages(cv::String path);
	//�жϸ����ص��Ƿ񱻱��
	static bool IsSelected(Mat src, int i, int j);
	//ѡ�б�����ص�
	static void Select(Mat src, int i, int j);

public:
	//��ȡ�������ɵ�MaskͼƬ
	static Mat ReadMask(int cameraID, int sampleID);
	//����Mask�õ���ǵ����򣬲����ݶ�ֵ����ľ�ֵ�ͷ������ж�λ���Ƿ�׼ȷ
	static Mat ComputeWhiteArea(Mat mask, Mat src);
	//���ݱ��������ȡ���������ڷ���任
	static vector<Point2f> ComputeKeyPoints(Mat src);
	//���ݱ�ǵ������Ʒ����Ȥ����
	static Mat ComputeSampleArea(vector<Point2f> points, Mat src);
	//����Ҷ�ͼ���������ؼ���ƽ��ֵ��Ϊ��ֵ����ֵ
	static int ComputeThreshold(Mat src);
	//��������������꣨������ת
	static Point2f ComputeCenterPoint(vector<Point2f> pts);
	//�ü���������
	static void CutSampleArea(Mat& src);

};
#endif