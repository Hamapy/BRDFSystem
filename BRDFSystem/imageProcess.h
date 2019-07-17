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

	//通过直方图方法判断采集图像是否过曝或过暗
	static bool IsOverExposure(Mat src);
	//计算合适曝光时间
	static float ComputeExposureTime(Mat mat);

	//坏点校正
	static vector<int*> DeadPixelDetect(Mat src, int maxNum);
	static Mat DeadPixelCorrection(Mat src);

	//暗电平处理 返回需要减去的基值
	static int ComputeBlackPedal(Mat src);
	static Mat BlackLevelCorrection(Mat src);

	//Zhang棋盘格标定 返回相机参数矩阵
	static vector<Mat> ComputeChessTrans(vector<Mat> mats);
	static Mat ChessCorrection(Mat src);

	//白平衡校正（完美全反射）&均匀度校正（事先用白板标定，载物台上标记点仅用于仿射变换及采集过程中的白平衡检验）
	static float* ComputeWhiteTrans(vector<Mat> mats);
	static Mat WhiteBalance(Mat src, float* trans);
	
	//颜色校正（多项式回归） 返回变换矩阵
	//static vector<Mat> GetColorTrans(vector<Mat> mats);
	//Mat ColorCorrection(Mat src);

	friend class MainWindow;

private:
	//vector<float>				_trans;//白平衡校正系数
	//vector<vector<float>>		_transs;
	//int							_darkResponse;//暗电流响应值
	//vector<int*>				_deadPixelPos;//坏点位置

	//多张连续图像求平均
	static Mat AverageImage(vector<Mat> mats);
	//读取一个文件夹下所有图片
	static vector<Mat> ReadImages(string path);
	//判断该像素点是否被标记
	static bool IsSelected(Mat src, int i, int j);
	//选中标记像素点
	static void Select(Mat src, int i, int j);

};
#endif