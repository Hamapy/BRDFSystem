#pragma once
#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <algorithm>
#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"  //这个hpp文件opencv2没有，需要更换版本
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

	//若采集Bayer图像，RAW转RGB去马赛克
	//Mat Demosiac(Mat src);

	//通过直方图方法判断采集图像是否过曝或过暗，并计算合适曝光时间
	static bool IsOverExposure(Mat src);
	static float ComputeExposureTime(Mat mat);

	//高动态范围HDR
	//求取一幅RGB图像平均亮度
	static double ComputeAverage(Mat src);
	static Mat ContributeHDR(map<double, Mat> imgs, bool crfFlag);

	//坏点校正
	static vector<int*> DeadPixelDetect(Mat src, int maxNum);
	static Mat DeadPixelCorrection(Mat src);

	//暗电平处理 返回需要减去的基值，MatLab测试BlackLevel = 0时像素点值范围1-2（0-255），故暂不用考虑做暗电平处理
	//static int ComputeBlackPedal(Mat src);
	//static Mat BlackLevelCorrection(Mat src);

	//Zhang棋盘格标定 返回相机参数矩阵
	static vector<Mat> ComputeChessTrans(vector<Mat> mats);
	static Mat ChessCorrection(Mat src);

	//白平衡校正（完美全反射）&均匀度校正（事先用白板标定，载物台上标记点仅用于仿射变换及采集过程中的白平衡检验）
	static float* ComputeWhiteTrans(vector<Mat> mats);
	static Mat WhiteBalance(Mat src, float* trans);
	
	//颜色校正（多项式回归），返回变换矩阵
	//static vector<Mat> GetColorTrans(vector<Mat> mats);
	//Mat ColorCorrection(Mat src);

	//根据mask得到标记点位置，并根据二值化后的均值和方差来判断位置是否准确
	static Mat ComputeWhiteArea(Mat mask, Mat src);
	//根据标记点计算样品感兴趣区域
	static Mat ComputeSampleArea(vector<Point> point, Mat src);
	//根据相机序号读取对应的mask
	static Mat ReadMaskWithWorkerID(int workerID);

	friend class MainWindow;

private:
	//定位白色标记点
	static vector<Point> findKeyPoints(Mat src);
	//多张连续图像求平均
	static Mat AverageImage(vector<Mat> mats);
	//读取一个文件夹下所有图片
	static vector<Mat> ReadImages(string path);
	//判断该像素点是否被标记
	static bool IsSelected(Mat src, int i, int j);
	//选中标记像素点
	static void Select(Mat src, int i, int j);

	////根据mask得到标记点位置，并根据二值化后的均值和方差来判断位置是否准确
	//static Mat ComputeWhiteArea(Mat mask, Mat src);
	////根据标记点计算样品感兴趣区域
	//static Mat ComputeSampleArea(vector<Point> point, Mat src);
	////根据相机序号读取对应的mask
	//static Mat ReadMaskWithWorkerID(int workerID);
};
#endif