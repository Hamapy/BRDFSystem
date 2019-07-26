#include "imageProcess.h"

int main()
{
	//ImageProcess s;
	Mat test = imread("..//imgs_test//src.bmp");
	Mat mask = imread("..//imgs_test//mask.bmp");
	//imshow("ori", test);
	//imshow("mask", mask);
	//waitKey();
	//int th = ImageProcess::ComputeThreshold(test);
	Mat dst = ImageProcess::ComputeWhiteArea(mask, test);
	imwrite("..//imgs_test//whitearea.bmp", dst);
	float* trans = ImageProcess::ComputeWhiteTrans(dst);
	dst = ImageProcess::WhiteBalance(test, trans);
	resize(dst, dst, Size(test.cols*0.6, test.rows*0.6));
	imshow("result", dst);
	imwrite("..//imgs_test//result.bmp", dst);
	waitKey();

	return 1;
}