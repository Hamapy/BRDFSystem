#include "imageProcess.h"

int main0()
{
	//ImageProcess s;
	Mat test = imread("..//imgs_test//test.bmp");
	resize(test, test, Size(test.cols*0.6, test.rows*0.6));

	//Mat kernel = Mat::ones(3, 3, CV_32F) / 3 / 3;
	//Mat dst;
	//filter2D(test, dst, -1, kernel);
	//imshow("原图", test);
	//imshow("模糊", dst);
	
	
	Mat gray;
	cvtColor(test, gray, CV_BGR2GRAY);
	//threshold(gray, gray, 80, 255, CV_THRESH_OTSU);
	//medianBlur(gray, gray, 3);
	//imshow("灰度图", gray);
	
	Mat edge;
	Canny(gray, edge, 400, 800);
	imshow("canny边缘", edge);

	vector<Vec3f> pcircles;
	HoughCircles(gray, pcircles, CV_HOUGH_GRADIENT, 1, 800, 600, 10, 0, 30);//中心累加器参数设置比较重要
	Mat dst;
	test.copyTo(dst);
	for (size_t i = 0; i < pcircles.size(); i++)
	{
		Vec3f circles = pcircles[i];
		circle(test, Point(circles[0], circles[1]), circles[2], CV_RGB(255, 0, 0), 2);
	}
	imshow("output", test);
	waitKey();
	

	/*
	vector<Point2f> pointBuf;
	bool ret = findCirclesGrid(test, Size(1,1), pointBuf);
	circle(test, pointBuf[0], 2, Scalar(0, 0, 255));
	*/

	/*
	Mat edge;
	Canny(test, edge, 500, 70);
	imshow("canny边缘", edge);
	waitKey();
	vector<vector<Point> > contours;
	findContours(edge, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	imshow("标记点检测", edge);
	waitKey();
	Mat dst;
	for (size_t i = 0; i < contours.size(); i++)
	{
		//拟合的点至少为6
		size_t count = contours[i].size();
		if (count < 6)
			continue;
		//椭圆拟合
		RotatedRect box = fitEllipse(contours[i]);
		//如果长宽比大于30，则排除，不做拟合
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;
		//画出追踪出的轮廓
		drawContours(dst, contours, (int)i, Scalar::all(255), 1, 8);
		//画出拟合的椭圆
		ellipse(dst, box, Scalar(0, 0, 255), 1, CV_AA);
	}
	imshow("拟合结果", dst);
	waitKey();
	*/

	/*
	ImageProcess::ComputeWhiteTrans(vector<Mat> mats);
	Mat dst = ImageProcess::WhiteBalance(test);
	imshow("白平衡测试", dst);
	waitKey();
	*/

	return 1;
}