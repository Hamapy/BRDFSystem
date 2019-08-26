#include "imageProcess.h"

int main4()
{	
	Mat mat = imread("..//imgs_test//test1.bmp");

	int sID = 3;
	//读取标定的仿射变换矩阵
	Mat _affineTrans = Mat_<double>(3,3);
	FileStorage cvfs("affineTrans.xml", FileStorage::READ);
	cvfs["cam" + to_string(1)] >> _affineTrans;
	//读取正视角下角点
	//Mat corners(4, 2, CV_32FC1);
	Mat corners = Mat_<double>(2,4);
	cvfs["corners-cam0"] >> corners;
	float temp = _affineTrans.at<double>(0, 0);
	
	vector<Point2f> pCorners;
	Point2f pCorner;
	pCorner.x = corners.at<double>(0, 0);
	pCorner.y = corners.at<double>(0, 1);
	pCorners.push_back(pCorner);
	Point2f pCenter;
	pCenter.x = (corners.at<double>(0, 0) + corners.at<double>(1, 0) + corners.at<double>(2, 0) + corners.at<double>(3, 0)) / 4.0;
	pCenter.y = (corners.at<double>(0, 1) + corners.at<double>(1, 1) + corners.at<double>(2, 1) + corners.at<double>(3, 1)) / 4.0;
	cvfs.release();

	//1.2、HDR采集（opencv2版本下待完善）
	/*
	if (_isHdr)
	{
	map<double, Mat> imgs = cameraAVT->CaptureByDifTimes(_exposureTime, mat);
	Mat hdr = ImageProcess::ContributeHDR(imgs, 0);//暂视为单色区域整体CRF曲线为线性
	}
	*/

	////2、坏点校正（待测试，这一步厂家在demosiac时应该做过，暂可不用）
	//ImageProcess::DeadPixelCorrection(mat, _deadPos);

	////3、相机内参畸变矫正（同上，暂可不用）
	//ImageProcess::DistortionCorrection(mat, _cameraMatrix, _distCoeffs);

	////4、采集图像正视角校正
	//_affineTrans.convertTo(_affineTrans, CV_32FC1);
	//高度角仿射变换
	//ImageProcess::AngelCalibration(mat, _affineTrans);
	//方位角角度旋转
	//ImageProcess::RotateImageXoY(mat, sID, pCenter);
	/*
	//7.26日前考虑的定点方案，但低纬度相机下视野非常小，难以计算mask
	vector<Point2f> pts = ImageProcess::ComputeKeyPoints(dst);
	if (_hasAffineTrans == 0)
	{
	_affineTrans = ImageProcess::ComputeAffineTrans(pts, referPts);
	_hasAffineTrans = 1;
	}
	ImageProcess::AngelCalibration(mat, _affineTrans);
	mat = ImageProcess::ComputeSampleArea(pts, mat);
	Point2f center = ImageProcess::ComputeCenterPoint(pts);
	ImageProcess::RotateImageXoY(mat, sID, center);
	//第一个相机由于安装位置限制，需要额外旋转45°
	if (_workerID == 0)
	ImageProcess::RotateImageXoY(mat, 4.5, center);
	*/

	////5、根据标定Mask计算白点标记区域，进行白平衡校正	
	Mat mask = ImageProcess::ReadMask(0, 34);//统一转换到正视角下后再做mask（仅白平衡用），减少mask的存取数量
	Mat dst = ImageProcess::ComputeWhiteArea(mask, mat);//获取白色标记区域
	imshow("test", dst);
	waitKey();
	//采集时根据第一帧获取白平衡矩阵，不必先进行标定
	float* _whiteTrans = ImageProcess::ComputeWhiteTrans(dst);
	ImageProcess::WhiteBalance(mat, _whiteTrans);

	////6、截取材质中心高质量区域（这一步视采集方式来定，样品多角度采集需要框取整个材质区域）
	//ImageProcess::ComputeSampleArea(mat, pCorners);//框取材质样品区域（svbrdf要求像素精度）
	//ImageProcess::CutSampleArea(mat);//框取光照中心区域有效值
	imshow("test", mat);
	waitKey();

	return 0;
}