#include "imageProcess.h"

int main0()
{
	//ImageProcess s;
	Mat test = imread("..//imgs_test//test.bmp");
	imshow("Ô­Í¼", test);
	waitKey();
	//ImageProcess::ComputeWhiteTrans(vector<Mat> mats);
	//Mat dst = ImageProcess::WhiteBalance(test);
	//imshow("°×Æ½ºâ²âÊÔ", dst);
	//waitKey();

	return 1;
}