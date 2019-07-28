#include "imageProcess.h"

ImageProcess::ImageProcess()
{

}

ImageProcess::~ImageProcess()
{

}

//////////////////////////////////公有函数/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：IsOverExposure
// 描述：判断相机是否过曝
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsOverExposure(Mat src)
{
	//通过计算直方图判断采集图像是否过曝或过暗

	return 0;

}
////////////////////////////////////////////////////////////////////////////
// 函数：GetExposureTime
// 描述：计算相机在一定角度光源下的合适曝光时间
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float ImageProcess::ComputeExposureTime(Mat mat)
{
	Mat gray;
	cvtColor(mat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	float k = 50.00 / ave;//50ms下拍摄图像
	float t = k * 255 * 0.60;

	return t;
}

////////////////////////////////////////////////////////////////////////////
// 函数：ContributeHDR
// 描述：合成高动态范围图像
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ContributeHDR(map<double, Mat> imgs, bool crfFlag)
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
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<int*> ImageProcess::DeadPixelDetect(Mat src, int maxNum)
{
#define LAMDA 5.0  //亮度值 > mean + LAMDA * sigma 被视为坏点
#define MASK_SEL 1 
#define MASK_UNSEL 0 //是否已选像素

	vector<int*> deadPixelPos;

	//对均匀图像可暂时不用滑动窗口
	/*
	int count = 0;  //记录滑动窗口的数目
	int x_step = cvCeil(x_percent*wndSize.width);
	int y_step = cvCeil(y_percent*wndSize.height);
	//利用窗口对图像进行遍历
	for (int i = 0; i < src.cols - wndSize.width; i += y_step)
	{
	for (int j = 0; j < src.rows - wndSize.height; j += x_step)
	{
	Rect roi(Point(j, i), wndSize);
	Mat ROI = src(roi);
	wnd.push_back(ROI);
	count++;
	}
	}
	*/

	//坏点不受通道影响，直接看灰度图就好
	/*
	//求出图像三通道均值和方差
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float meanB = mean.val[0];
	float stddevB = stddev.val[0];
	float meanG = mean.val[1];
	float stddevG = stddev.val[1];
	float meanR = mean.val[2];
	float stddevR = stddev.val[2];

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);
	int ch = src.channels;
	*/

	PIXEL pSel;
	Mat gray;
	Mat zeroMat = Mat::zeros(gray.size(), CV_8UC1);
	cvtColor(src, gray, CV_BGR2GRAY);
	Scalar mean;
	Scalar stddev;
	meanStdDev(src, mean, stddev);
	float m = mean.val[0];
	float sigma = stddev.val[0];

	//通过循环找最大值的方法，确定坏点
	for (int k = 0; k < maxNum; k++)
	{
		int maxVal = 0;
		for (int i = 0; i < src.rows; i++)
		{
			uchar* data = gray.ptr<uchar>(i);
			for (int j = 0; j < src.cols; j++)
			{
				if (IsSelected(zeroMat, i, j) == MASK_UNSEL)
				{
					if (data[j] > maxVal)
					{
						maxVal = data[j];
						pSel.i = i;
						pSel.j = j;
					}
				}
			}
		}

		if (maxVal > m + LAMDA * sigma)  // 若该点满足坏点条件
		{
			int deadPos[2] = { pSel.i, pSel.j };
			deadPixelPos.push_back(deadPos);
			Select(zeroMat, pSel.i, pSel.j);
		}
		else
		{
			break;
		}
	}

	return deadPixelPos;
#undef LAMDA
#undef MASK_SEL
#undef MASK_UNSEL
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
float* ImageProcess::ComputeWhiteTrans(vector<Mat> mats)
{
	Mat src = AverageImage(mats);

#define WHITE_NUM 500
	//暂时取全图亮度最高的一定数量像素点均值作为白点
	//...
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	Mat B = srcB.clone();
	Mat G = srcG.clone();
	Mat R = srcR.clone();

	//把Mat矩阵拉成一维数组
	unsigned char *arrayB = new unsigned char[srcB.rows*srcB.cols];
	if (srcB.isContinuous())
		arrayB = B.data;//防止修改原图像素顺序
	unsigned char *arrayG = new unsigned char[srcG.rows*srcG.cols];
	if (srcG.isContinuous())
		arrayG = G.data;
	unsigned char *arrayR = new unsigned char[srcR.rows*srcR.cols];
	if (srcR.isContinuous())
		arrayR = R.data;


	//计算亮度值最大像素点集合平均亮度
	sort(arrayB, arrayB + srcB.rows*srcB.cols);
	reverse(arrayB, arrayB + srcB.rows*srcB.cols);
	//sort(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	//reverse(arrayB[0], arrayB[srcB.rows*srcB.cols - 1]);
	int sumB = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumB += (int)arrayB[i];
	}
	uchar aveB = (float)sumB / (float)WHITE_NUM;

	sort(arrayG, arrayG + srcG.rows*srcG.cols);
	reverse(arrayG, arrayG + srcG.rows*srcG.cols);
	int sumG = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumG += (int)arrayG[i];
	}
	uchar aveG = (float)sumG / (float)WHITE_NUM;

	sort(arrayR, arrayR + srcR.rows*srcR.cols);
	reverse(arrayR, arrayR + srcR.rows*srcR.cols);
	int sumR = 0;
	for (int i = 0; i < WHITE_NUM; i++)
	{
		sumR += (int)arrayR[i];
	}
	uchar aveR = (float)sumR / (float)WHITE_NUM;

	float transB = 255 / (float)aveB;
	float transG = 255 / (float)aveG;
	float transR = 255 / (float)aveR;

	static float trans[3];
	trans[0] = transB;
	trans[1] = transG;
	trans[2] = transR;

	return trans;

#undef WHITE_NUM
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::WhiteBalance(Mat src, float* trans)
{
	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	float transB = trans[0];
	float transG = trans[1];
	float transR = trans[2];

	//对每个点将像素拉到[0,255]之间
	//针对超出1的像素，重新归一化
	vector<vector<float>> fBGR(height, vector<float>(width, 0));
	float max = 0;
	//float s = 255 / (float)aveB;
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			fBGR[i][j] = srcB.at<uchar>(i, j) / 255.00 * transB;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcB.rows; i++)
	{
		for (int j = 0; j < srcB.cols; j++)
		{
			srcB.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveG;
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			fBGR[i][j] = srcG.at<uchar>(i, j) / 255.00 * transG;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcG.rows; i++)
	{
		for (int j = 0; j < srcG.cols; j++)
		{
			srcG.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	max = 0;
	//s = 255 / (float)aveR;
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			fBGR[i][j] = srcR.at<uchar>(i, j) / 255.00 * transR;
			//srcB.at<uchar>(i, j) = srcB.at<uchar>(i, j) / 255 * s;
			if (fBGR[i][j] > max)
				max = fBGR[i][j];
		}
	}
	for (int i = 0; i < srcR.rows; i++)
	{
		for (int j = 0; j < srcR.cols; j++)
		{
			srcR.at<uchar>(i, j) = fBGR[i][j] / max * 255;
		}
	}

	Mat dst(src.rows, src.cols, CV_8UC3, Scalar::all(0));
	channels.push_back(srcB);
	channels.push_back(srcG);
	channels.push_back(srcR);
	//merge({ srcB, srcG, srcR }, dst);
	merge(channels, dst);

	return dst;
}

/////////////////////////////////私有函数/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsSelected(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	if (data[j] == 1)
		return true;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void ImageProcess::Select(Mat src, int i, int j)
{
	uchar* data = src.ptr<uchar>(i);
	data[j] = 1;

	return;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：多幅图像求平均
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
double ImageProcess::ComputeAverage(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	
	return scalar.val[0];
}

//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：多幅图像求平均
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::AverageImage(vector<Mat> mats)
{
	Mat aver = Mat::zeros(mats[0].size(), CV_32FC3);
	for (int i = 0; i < mats.size(); i++)
	{
		accumulate(mats[i], aver);
	}
	aver /= mats.size();
	aver.convertTo(aver, CV_8UC3);

	return aver;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
vector<Mat> ImageProcess::ReadImages(string path)
{
	vector<Mat> mats;
	string imgPattern = "//*.jpg";
	vector<cv::String> imgFiles;
	imgPattern = path + imgPattern;
	glob(imgPattern, imgFiles);
	for (int i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		mats.push_back(mat);
	}

	return mats;

}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ComputeWhiteArea(Mat mask, Mat src)
{
	int i, j;
	int maskCols, maskRows;
	maskCols = mask.cols;
	maskRows = mask.rows;
	Scalar total;
	int sumAll;
	int n;
	int Max = 0;
	float count = 0.0;
	float mean, variance;


	vector<Mat> channels;
	split(src, channels);

	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);


	for (i = 0; i < maskRows; i++)
	{
		uchar* data1 = mask.ptr<uchar>(i);

		for (j = 0; j < maskCols; j++)
		{

			data1[j] = data1[j] / 255;
		}
	}

	channels.at(0) = srcB.mul(mask);
	channels.at(1) = srcG.mul(mask);
	channels.at(2) = srcR.mul(mask);


	Mat	whiteArea(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, whiteArea);
	

	//创建与whiteArea同类型和同大小的矩阵
	Mat grayImage;
	grayImage.create(whiteArea.size(), whiteArea.type());
	//将原图转换为灰度图像
	cvtColor(whiteArea, grayImage, CV_BGR2GRAY);

	//非零个数n
	n = countNonZero(mask);
	//元素总和sumAll
	for (i = 0; i < maskRows; i++)
	{
		uchar* data3 = grayImage.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data3[j] > Max)
			{
				Max = data3[j];
			}

		}
	}
	//元素总和sumAll
	total = sum(grayImage);
	sumAll = total[0];

	//求非零元素均值mean
	mean = sumAll*1.0 / n;

	//求非零元素方差variance
	for (i = 0; i < maskRows; i++)
	{
		uchar* data2 = grayImage.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data2[j] != 0)
			{
				count = count + pow(data2[j] - mean, 2);
			}

		}
	}

	variance = count*1.0 / n;

	if (mean<=(Max*0.6)&&variance>1.0)
	{
		whiteArea = Mat::zeros(maskRows, maskCols, CV_8UC3);
		return whiteArea;
	}
	
	return whiteArea;

}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
//Mat ImageProcess::ComputeSampleArea(Mat mask, Mat src)
//{
//	int i, j;
//	int maskCols, maskRows;
//	maskCols = mask.cols;
//	maskRows = mask.rows;
//
//	int xMin, xMax, yMin, yMax;
//	int countRows;
//	int countCols;
//
//	xMin = maskRows;
//	yMin = maskCols;
//	xMax = 0;
//	yMax = 0;
//
//
//	
//	//寻找非零点
//	for (i = 0; i < maskRows; i++)
//	{
//		uchar* data1 = mask.ptr<uchar>(i);
//		for (j = 0; j < maskCols; j++)
//		{
//			if (data1[j] == 255)
//			{
//				
//				//记录xMin
//				if (i < xMin)
//				{
//					xMin = i;
//				}
//	
//
//				//记录xMax
//				if (i > xMax)
//				{
//					xMax = i;
//				}
//
//
//				//记录yMin
//				if (j < yMin)
//				{
//					yMin = j;
//				}
//
//
//				//记录yMax
//				if (j > yMax)
//				{
//					yMax = j;
//				}
//
//			}
//
//		}
//	}
//
//	countRows = (xMax - xMin + 1) * 130 / 200;
//	countCols = (yMax - yMin + 1) * 130 / 200;
//
//	Mat temp = Mat::zeros(maskRows, maskCols, CV_8UC1);
//	for (i = xMin + (xMax - xMin + 1) * 35 / 200; i < xMin + (xMax - xMin + 1) * 35 / 200 + countRows; i++)
//	{
//		uchar* data2 = temp.ptr<uchar>(i);
//		for (j = yMin + (yMax - yMin + 1) * 35 / 200; j < yMin + (yMax - yMin + 1) * 35 / 200 + countCols; j++)
//		{
//			data2[j] = 1;
//		}
//	}
//
//
//
//
//	vector<Mat> channels;
//	split(src, channels);
//
//	Mat srcB = channels.at(0);
//	Mat srcG = channels.at(1);
//	Mat srcR = channels.at(2);
//
//
//	channels.at(0) = srcB.mul(temp);
//	channels.at(1) = srcG.mul(temp);
//	channels.at(2) = srcR.mul(temp);
//
//
//
//	Mat	sampleArea(src.rows, src.cols, CV_8UC3, Scalar(0));
//	merge(channels, sampleArea);
//	return sampleArea;
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ComputeSampleArea(vector<Point> point, Mat src)
{
	int i, j;
	int srcCols, srcRows;
	srcCols = src.cols;
	srcRows = src.rows;

	int xMin, xMax, yMin, yMax;
	int countRows;
	int countCols;

	xMin = srcRows;
	yMin = srcCols;
	xMax = 0;
	yMax = 0;

	for (i = 0; i < point.size(); i++)
	{
		if (point[i].x < xMin)
			xMin = point[i].x;
		if (point[i].y < yMin)
			yMin = point[i].y;
		if (point[i].x > xMax)
			xMax = point[i].x;
		if (point[i].y > yMax)
			yMax = point[i].y;
	}

	////寻找非零点
	//for (i = 0; i < maskRows; i++)
	//{
	//	uchar* data1 = mask.ptr<uchar>(i);
	//	for (j = 0; j < maskCols; j++)
	//	{
	//		if (data1[j] == 255)
	//		{

	//			//记录xMin
	//			if (i < xMin)
	//			{
	//				xMin = i;
	//			}


	//			//记录xMax
	//			if (i > xMax)
	//			{
	//				xMax = i;
	//			}


	//			//记录yMin
	//			if (j < yMin)
	//			{
	//				yMin = j;
	//			}


	//			//记录yMax
	//			if (j > yMax)
	//			{
	//				yMax = j;
	//			}

	//		}

	//	}
	//}

	countRows = (xMax - xMin + 1) * 120 / 200;
	countCols = (yMax - yMin + 1) * 120 / 200;

	Mat temp = Mat::zeros(srcRows, srcCols, CV_8UC1);
	for (i = xMin + (xMax - xMin + 1) * 40 / 200; i < xMin + (xMax - xMin + 1) * 40 / 200 + countRows; i++)
	{
		uchar* data2 = temp.ptr<uchar>(i);
		for (j = yMin + (yMax - yMin + 1) * 40 / 200; j < yMin + (yMax - yMin + 1) * 40 / 200 + countCols; j++)
		{
			data2[j] = 1;
		}
	}




	vector<Mat> channels;
	split(src, channels);

	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);


	channels.at(0) = srcB.mul(temp);
	channels.at(1) = srcG.mul(temp);
	channels.at(2) = srcR.mul(temp);



	Mat	sampleArea(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, sampleArea);
	return sampleArea;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ReadMaskWithWorkerID(int workerID)
{
	string filename, path;
	string tmp;
	path = "..//imgs_mask//";
	stringstream ss;
	ss << workerID;
	string s1 = ss.str();
	tmp = "00" + s1;
	filename = path + tmp + ".bmp";
	Mat mask;
	mask = imread(filename);
	return mask;
}