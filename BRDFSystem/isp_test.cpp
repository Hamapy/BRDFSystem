#include "imageProcess.h"

int main0()
{
	//ImageProcess s;
	Mat test = imread("..//imgs_test//test.bmp");
	imshow("ԭͼ", test);
	waitKey();
	//ImageProcess::ComputeWhiteTrans(vector<Mat> mats);
	//Mat dst = ImageProcess::WhiteBalance(test);
	//imshow("��ƽ�����", dst);
	//waitKey();

	return 1;
}