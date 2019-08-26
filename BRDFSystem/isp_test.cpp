#include "imageProcess.h"

int main4()
{	
	Mat mat = imread("..//imgs_test//test1.bmp");

	int sID = 3;
	//��ȡ�궨�ķ���任����
	Mat _affineTrans = Mat_<double>(3,3);
	FileStorage cvfs("affineTrans.xml", FileStorage::READ);
	cvfs["cam" + to_string(1)] >> _affineTrans;
	//��ȡ���ӽ��½ǵ�
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

	//1.2��HDR�ɼ���opencv2�汾�´����ƣ�
	/*
	if (_isHdr)
	{
	map<double, Mat> imgs = cameraAVT->CaptureByDifTimes(_exposureTime, mat);
	Mat hdr = ImageProcess::ContributeHDR(imgs, 0);//����Ϊ��ɫ��������CRF����Ϊ����
	}
	*/

	////2������У���������ԣ���һ��������demosiacʱӦ���������ݿɲ��ã�
	//ImageProcess::DeadPixelCorrection(mat, _deadPos);

	////3������ڲλ��������ͬ�ϣ��ݿɲ��ã�
	//ImageProcess::DistortionCorrection(mat, _cameraMatrix, _distCoeffs);

	////4���ɼ�ͼ�����ӽ�У��
	//_affineTrans.convertTo(_affineTrans, CV_32FC1);
	//�߶ȽǷ���任
	//ImageProcess::AngelCalibration(mat, _affineTrans);
	//��λ�ǽǶ���ת
	//ImageProcess::RotateImageXoY(mat, sID, pCenter);
	/*
	//7.26��ǰ���ǵĶ��㷽��������γ���������Ұ�ǳ�С�����Լ���mask
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
	//��һ��������ڰ�װλ�����ƣ���Ҫ������ת45��
	if (_workerID == 0)
	ImageProcess::RotateImageXoY(mat, 4.5, center);
	*/

	////5�����ݱ궨Mask����׵������򣬽��а�ƽ��У��	
	Mat mask = ImageProcess::ReadMask(0, 34);//ͳһת�������ӽ��º�����mask������ƽ���ã�������mask�Ĵ�ȡ����
	Mat dst = ImageProcess::ComputeWhiteArea(mask, mat);//��ȡ��ɫ�������
	imshow("test", dst);
	waitKey();
	//�ɼ�ʱ���ݵ�һ֡��ȡ��ƽ����󣬲����Ƚ��б궨
	float* _whiteTrans = ImageProcess::ComputeWhiteTrans(dst);
	ImageProcess::WhiteBalance(mat, _whiteTrans);

	////6����ȡ�������ĸ�����������һ���Ӳɼ���ʽ��������Ʒ��ǶȲɼ���Ҫ��ȡ������������
	//ImageProcess::ComputeSampleArea(mat, pCorners);//��ȡ������Ʒ����svbrdfҪ�����ؾ��ȣ�
	//ImageProcess::CutSampleArea(mat);//��ȡ��������������Чֵ
	imshow("test", mat);
	waitKey();

	return 0;
}