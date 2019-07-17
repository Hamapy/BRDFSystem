#pragma once
#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <algorithm>
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

	//ͨ��ֱ��ͼ�����жϲɼ�ͼ���Ƿ���ػ����
	static bool IsOverExposure(Mat src);
	//��������ع�ʱ��
	static float ComputeExposureTime(Mat mat);

	//����У��
	static vector<int*> DeadPixelDetect(Mat src, int maxNum);
	static Mat DeadPixelCorrection(Mat src);

	//����ƽ���� ������Ҫ��ȥ�Ļ�ֵ
	static int ComputeBlackPedal(Mat src);
	static Mat BlackLevelCorrection(Mat src);

	//Zhang���̸�궨 ���������������
	static vector<Mat> ComputeChessTrans(vector<Mat> mats);
	static Mat ChessCorrection(Mat src);

	//��ƽ��У��������ȫ���䣩&���ȶ�У���������ðװ�궨������̨�ϱ�ǵ�����ڷ���任���ɼ������еİ�ƽ����飩
	static float* ComputeWhiteTrans(vector<Mat> mats);
	static Mat WhiteBalance(Mat src, float* trans);
	
	//��ɫУ��������ʽ�ع飩 ���ر任����
	//static vector<Mat> GetColorTrans(vector<Mat> mats);
	//Mat ColorCorrection(Mat src);

	friend class MainWindow;

private:
	//vector<float>				_trans;//��ƽ��У��ϵ��
	//vector<vector<float>>		_transs;
	//int							_darkResponse;//��������Ӧֵ
	//vector<int*>				_deadPixelPos;//����λ��

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