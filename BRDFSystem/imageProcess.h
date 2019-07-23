#pragma once
#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <algorithm>
#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"  //���hpp�ļ�opencv2û�У���Ҫ�����汾
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

	//���ɼ�Bayerͼ��RAWתRGBȥ������
	//Mat Demosiac(Mat src);

	//ͨ��ֱ��ͼ�����жϲɼ�ͼ���Ƿ���ػ����������������ع�ʱ��
	static bool IsOverExposure(Mat src);
	static float ComputeExposureTime(Mat mat);

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

	//Zhang���̸�궨 ���������������
	static vector<Mat> ComputeChessTrans(vector<Mat> mats);
	static Mat ChessCorrection(Mat src);

	//��ֵ������Maskͼ��
	static vector<Mat> ComputeMask(vector<Mat> src);

	//��ƽ��У��������ȫ���䣩&���ȶ�У���������ðװ�궨������̨�ϱ�ǵ�����ڷ���任���ɼ������еİ�ƽ����飩
	static float* ComputeWhiteTrans(vector<Mat> mats);
	static Mat WhiteBalance(Mat src, float* trans);
	
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
	static vector<Mat> ReadImages(string path);
	//�жϸ����ص��Ƿ񱻱��
	static bool IsSelected(Mat src, int i, int j);
	//ѡ�б�����ص�
	static void Select(Mat src, int i, int j);
};
#endif