#include "imageProcess.h"

int main()
{
	//ImageProcess s;
	Mat test = imread("..//imgs_test//test6.bmp");
	resize(test, test, Size(test.cols*0.6, test.rows*0.6));
	imshow("ԭͼ", test);
	float* trans = ImageProcess::ComputeWhiteTrans(test);
	float b = trans[0];
	float g = trans[1];
	float r = trans[2];
	Mat dst = ImageProcess::WhiteBalance(test, trans);
	dst = ImageProcess::GrayStrech(dst);
	imshow("��ƽ��", dst);
	waitKey();

	return 1;
}