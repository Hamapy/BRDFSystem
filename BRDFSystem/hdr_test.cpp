/*
#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

#include "ccd.h"

using namespace cv;
using namespace std;

void loadExposureSeq(String, vector<Mat>&, vector<float>&);

int main9(int, char**argv)
{
	VimbaSystem& system = VimbaSystem::GetInstance();
	AVTCamera* cam = new AVTCamera(system);
	cam->IniCamera();
	cam->CameraSettings(50000);
	VmbUchar_t* pImageFrame = cam->CaptureAnImage();
	Mat mat = Mat(1234, 1624, CV_8UC3, pImageFrame);
	cvtColor(mat, mat, CV_RGB2BGR);
	CaptureByDifTimes(5000, mat);


	vector<Mat> images;
	vector<float> times;
	loadExposureSeq(argv[1], images, times);

	Mat response;
	Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();
	calibrate->process(images, response, times);

	Mat hdr;
	Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
	merge_debevec->process(images, hdr, times, response);

	Mat ldr;
	Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);
	tonemap->process(hdr, ldr);

	Mat fusion;
	Ptr<MergeMertens> merge_mertens = createMergeMertens();
	merge_mertens->process(images, fusion);

	imwrite("fusion.png", fusion * 255);
	imwrite("ldr.png", ldr * 255);
	imwrite("hdr.hdr", hdr);

	return 0;
}

map<double, Mat> CaptureByDifTimes(double originalTime, Mat originalMat)
{
	map<double, Mat> imgs;
	//imgs[originalTime] = originalMat;
	Mat gray;
	cvtColor(originalMat, gray, CV_RGB2GRAY);
	
	gray.reshape(0, 1);
	Size imageSize = gray.size();
	cv::sort(gray, gray, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	uchar* data = gray.ptr<uchar>(0);

	int index = (int)imageSize.width*imageSize.height*0.20;
	int total = 0;
	for (int i = 0; i < index; i++)
	{
		total += data[i];
	}
	return total / float(index);

	double ped = mean(gray).val[0];
	double max = 255.00 / ped * originalTime;
	double step = max / HDR_NUM;
	double exposureTime = 0.00;
	for (int i = 0; i < HDR_NUM; i++)
	{
		exposureTime += step;
		CameraSettings(exposureTime);
		//AVTCamera::GetImageSize(_width, _height);
		uchar* pImageFrame = CaptureAnImage();
		Mat mat = Mat(_height, _width, CV_8UC3, pImageFrame);
		imgs[exposureTime] = mat;

		Sleep(exposureTime);
	}

	return imgs;
}

Mat ContributeHDR(map<double, Mat> imgs, bool crfFlag)
{
	vector<Mat> srcs;
	vector<double> exposureTimes;
	map<double, Mat>::iterator it;
	for (it = imgs.begin(); it != imgs.end(); it++)
	{
	exposureTimes.push_back(it->first);
	srcs.push_back(it->second);
	}

	vector<Mat> ali;
	//MTB图像对齐
	//通过将值1分配给比中间亮度更亮的像素来计算图像的MTB，否则为0。 MTB对曝光时间不变。
	Ptr<AlignMTB> alignMTB = createAlignMTB();
	alignMTB->process(srcs, ali);

	//在采集BRDF时，采集图像ROI区域可视为单个像素，单个像素亮度可用图像整体平均亮度近似，故此前测试可认为CRF为线性。
	//而当采集svBRDF时，CRF不是线性的，需要通过不同曝光时间的图像估计CRF
	if (crfFlag == 1)
	{
	//获取相机CRF曲线(Camera Response Function)
	Mat responseDebevec;
	Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
	calibrateDebevec->process(ali, responseDebevec, exposureTimes);

	//合并生成HDR图像
	Mat hdrDebevec;
	Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
	mergeDebevec->process(ali, hdrDebevec, exposureTimes, responseDebevec);
	//imwrite("hdrDebevec.hdr", hdrDebevec);

	//Drago色调映射
	Mat ldrDrago;
	Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
	tonemapDrago->process(hdrDebevec, ldrDrago);
	ldrDrago = 3 * ldrDrago;
	imwrite("ldr-Drago.jpg", ldrDrago * 255);

	return ldrDrago;
	}
	else
	{
	//获取相机CRF曲线(Camera Response Function)
	Mat responseDebevec;
	Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
	calibrateDebevec->process(ali, responseDebevec, exposureTimes);

	//合并生成HDR图像
	Mat hdrDebevec;
	Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
	mergeDebevec->process(ali, hdrDebevec, exposureTimes);
	//imwrite("hdrDebevec.hdr", hdrDebevec);

	//Drago色调映射
	Mat ldrDrago;
	Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
	tonemapDrago->process(hdrDebevec, ldrDrago);
	ldrDrago = 3 * ldrDrago * 255;
	//imwrite("ldr-Drago.jpg", ldrDrago * 255);

	return ldrDrago;
	}
	Mat mat;
	return mat;
}

void loadExposureSeq(String path, vector<Mat>& images, vector<float>& times)
{
	path = path + std::string("/");
	ifstream list_file((path + "list.txt").c_str());
	string name;
	float val;
	while (list_file >> name >> val) {
		Mat img = imread(path + name);
		images.push_back(img);
		times.push_back(1 / val);
	}
	list_file.close();
}

*/