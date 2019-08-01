#include "imageProcess.h"

int main0()
{
	//ImageProcess s;
	
	Mat test = imread("..//imgs_test//test.bmp");
	Mat mask = imread("..//imgs_test//0-0.bmp");
	Mat dst = ImageProcess::ComputeWhiteArea(mask, test);
	imshow("test", dst);
	waitKey();
	//Point2f* pts = ImageProcess::ComputeKeyPoints(dst);
	//cvtColor(test, test, CV_BGR2GRAY);//转灰度图函数提到外面保存下来
	//Scalar scalar = mean(test);
	//float ave = scalar.val[0];
	//Mat mask = imread("..//imgs_test//mask.bmp");
	//imshow("ori", test);
	//imshow("mask", mask);
	//waitKey();
	//int th = ImageProcess::ComputeThreshold(test);
	//Mat dst = ImageProcess::ComputeWhiteArea(mask, test);
	//imwrite("..//imgs_test//whitearea.bmp", dst);
	//float* trans = ImageProcess::ComputeWhiteTrans(dst);
	//dst = ImageProcess::WhiteBalance(test, trans);
	//dst = ImageProcess::GrayStrech(dst);
	//resize(dst, dst, Size(test.cols*0.6, test.rows*0.6));
	
	//
	//Point2f* pts = ImageProcess::ComputeKeyPoints(test);
	//Point2f p[3];
	//p[0] = {pts[0].x, pts[0].y};
	//p[1] = { pts[1].x, pts[1].y };
	//p[2] = { pts[2].x, pts[2].y };


	/*
	Point2f p2[3];
	p2[0] = { 400, 300 };
	p2[1] = { 1200, 80 };
	p2[2] = { 202, 1200 };
	Mat trans = ImageProcess::ComputeAffineTrans(p, p2);
	ImageProcess::AngelCalibration(test, trans);
	*/

	//ImageProcess::RotateImageXoY(test, 12, {500, 640});

	/*
	vector<Point2f> p;
	p.push_back({ 120, 60 });
	p.push_back({ 400, 300 });
	p.push_back({ 202, 1200 });
	p.push_back({ 20, 190 });
	p.push_back({ 10, 60 });
	test = ImageProcess::ComputeSampleArea(p, test);
	*/

	//imshow("result", dst);
	//imwrite("..//imgs_test//result.bmp", dst);
	//waitKey();

	return -1;
}