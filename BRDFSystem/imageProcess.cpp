#include "imageProcess.h"

ImageProcess::ImageProcess()
{

}

ImageProcess::~ImageProcess()
{

}

//////////////////////////////////公有函数/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：RotateImageXoY
// 描述：采集图像XoY角度旋转
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void ImageProcess::RotateImageXoY(Mat& img, int sampleID, Point2f center)
{
	float angle = 10 * sampleID;
	float radian = (float)(angle / 180.0 * CV_PI);

	int maxBorder = (int)(max(img.cols, img.rows)* sqrt(2)); //填充到45°图像边界最大的情况
	int dx = (maxBorder - img.cols) / 2;
	int dy = (maxBorder - img.rows) / 2;
	copyMakeBorder(img, img, dy, dy, dx, dx, BORDER_CONSTANT);

	//旋转
	//Point2f center((float)(img.cols / 2), (float)(img.rows / 2));//暂定整张图像中心为旋转中心
	Mat affMatrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
	warpAffine(img, img, affMatrix, img.size());

	/*
	//计算图像旋转之后包含图像的最大的矩形
	float sinVal = abs(sin(radian));
	float cosVal = abs(cos(radian));
	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//剪掉多余边框
	int x = (dst.cols - targetSize.width) / 2;
	int y = (dst.rows - targetSize.height) / 2;
	Rect rect(x, y, targetSize.width, targetSize.height);
	dst = Mat(dst, rect);
	*/
	return;
}
////////////////////////////////////////////////////////////////////////////
// 函数：AngelCalibration
// 描述：根据相机外参及样品旋转角度将拍摄视图对齐到正视角
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::ComputeAffineTrans(Point2f* pSrc, Point2f* pDst)
{
	Mat affMatrix = getAffineTransform(pSrc, pDst);

	return affMatrix;
}
////////////////////////////////////////////////////////////////////////////
// 函数：AngelCalibration
// 描述：将拍摄视图对齐到正视角
// 输入：
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void ImageProcess::AngelCalibration(Mat& img, Mat affMatrix)
{
	warpAffine(img, img, affMatrix, img.size());
}
////////////////////////////////////////////////////////////////////////////
// 函数：IsProperExposure
// 描述：判断相机是否合适
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
bool ImageProcess::IsProperExposure(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	int sum = 0;
	const int size = gray.cols * gray.rows;
	//计算灰度值在80-180范围内的像素点比例
	for (int i = 0; i < gray.rows; i++)
	{
		uchar* data = gray.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (100 < data[j] && data[j] < 180)//设定阈值
			{
				sum++;
			}
		}
	}

	if (float(sum) / float(size) > 0.7)//设定阈值
		return true;
	else
		return false;
}
////////////////////////////////////////////////////////////////////////////
// 函数：ComputeExposureTime
// 描述：计算相机在一定角度光源下的合适曝光时间
// 输入：Null
// 输出：曝光时间
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float ImageProcess::ComputeExposureTime(Mat oriMat, float oriExposureTime)
{
	Mat gray;
	cvtColor(oriMat, gray, CV_RGB2GRAY);
	Scalar scalar = mean(gray);
	float ave = scalar.val[0];
	float k = oriExposureTime / ave;//50ms下拍摄图像
	float t = k * 255 * 0.60;//设定阈值

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
//// 函数：ComputeMask
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
void ImageProcess::ComputeMask(vector<Mat, string> srcs)
{
	int i = 0;
	//vector<Mat> dsts;
	Mat dst;
	vector<Mat>::iterator iter;
	
	for (iter = srcs.begin(); iter != srcs.end(); iter++)
	{
		string path = "..//imgs_mask//";
		i++;
		path = path + to_string(i) + ".bmp";
		cvtColor(*iter, dst, CV_BGR2GRAY);
		//int th = ComputeThreshold(*iter);
		threshold(dst, dst, 25, 255, CV_THRESH_BINARY);
		imwrite(path, dst);
		//dsts.push_back(dst);
	}

	return;
}
//
////////////////////////////////////////////////////////////////////////////////
////// 函数：ComputeChessTrans
////// 描述：函数计算的是相机校正后的内外参数、旋转矩阵、旋转向量、平移向量
////// 输入：mats为输入图片的集合，boardSize为棋盘格上角点的信息、squareSize是棋盘格上每个方格的边长信息
////// 输出：
////// 返回：包含相机信息的结构体
////// 备注：务必注意boardSize要与实际的角点信息匹配，否则程序将报错
////// Modified by 
////////////////////////////////////////////////////////////////////////////////
//CameraCalibrationParamaters ImageProcess::ComputeChessTrans(const vector<Mat>& mats, const Size boardSize, const Size squareSize)
//{
//	//CameraCalibrationParamaters camCalParas;
//	int imageCount = mats.size();
//	Size imageSize;
//	Mat imageInput;
//	//读取每一幅图像，从中提取出角点，然后对角点进行亚像素精确化
//	vector<Point2f> imagePointsBuf;/* 缓存每幅图像上检测到的角点 */
//	vector<vector<Point2f>> imagePointsSeq;/* 缓存每幅图像上检测到的角点 */
//	for (int i = 0; i < imageCount; i++)
//	{
//		imageInput = mats[i];
//		if (i == 0)
//		{
//			imageSize.width = imageInput.cols;
//			imageSize.height = imageInput.rows;
//		}
//		//注意这里的boardSize必须和用于检测的棋盘格图片匹配，否则程序将退出
//		if (0 == findChessboardCorners(imageInput, boardSize, imagePointsBuf))
//		{
//			//cout << "can not find chessboard corneres!\n";
//			exit(1);
//		}
//		else
//		{
//			Mat viewGray;
//			cvtColor(imageInput, viewGray, CV_RGB2GRAY);
//			/* 亚像素精确化 */
//			find4QuadCornerSubpix(viewGray, imagePointsBuf, Size(5, 5)); //对粗提取的角点进行精确化
//			imagePointsSeq.push_back(imagePointsBuf);  //保存亚像素角点
//			/* 在图像上显示角点位置 */
//			//drawChessboardCorners(viewGray, boardSize, imagePointsBuf, true);
//			//waitKey(500);
//		}
//	}
//	vector<vector<Point3f>> objectPoints;
//	/* 摄像机内参数矩阵 */
//	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
//	vector<int> pointCounts;
//	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0)); /* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */
//	/* 初始化标定板上角点的三维坐标 */
//	int i, j, t;
//	for (t = 0; t<imageCount; t++)
//	{
//		vector<Point3f> tempPointSet;
//		for (i = 0; i<boardSize.height; i++)
//		{
//			for (j = 0; j<boardSize.width; j++)
//			{
//				Point3f realPoint;
//				/* 假设标定板放在世界坐标系中z=0的平面上 */
//				realPoint.x = i*squareSize.width;
//				realPoint.y = j*squareSize.height;
//				realPoint.z = 0;
//				tempPointSet.push_back(realPoint);
//			}
//		}
//		objectPoints.push_back(tempPointSet);
//	}
//	/* 初始化每幅图像中的角点数量，假定每幅图像中都可以看到完整的标定板 */
//	for (i = 0; i<imageCount; i++)
//	{
//		pointCounts.push_back(boardSize.width*boardSize.height);
//	}
//	/* 开始标定 */
//	calibrateCamera(objectPoints, imagePointsSeq, imageSize, cameraMatrix, distCoeffs, camCalParas.rvecsMats, camCalParas.tvecsMats, 0);
//	//对标定结果进行评价
//	double total_err = 0.0; /* 所有图像的平均误差的总和 */
//	double err = 0.0; /* 每幅图像的平均误差 */
//	vector<Point2f> image_points2; /* 保存重新计算得到的投影点 */
//	vector<Mat> rotationMatrixs(imageCount);
//	for (i = 0; i<imageCount; i++)
//	{
//		vector<Point3f> tempPointSet = objectPoints[i];
//		/* 通过得到的摄像机内外参数，对空间的三维点进行重新投影计算，得到新的投影点 */
//		projectPoints(tempPointSet, rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, image_points2);
//		/* 计算新的投影点和旧的投影点之间的误差*/
//		vector<Point2f> tempImagePoint = imagePointsSeq[i];
//		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
//		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
//		for (int j = 0; j < tempImagePoint.size(); j++)
//		{
//			image_points2Mat.at<Vec2f>(0, j) = Vec2f(image_points2[j].x, image_points2[j].y);
//			tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
//		}
//		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
//		total_err += err /= pointCounts[i];
//		Rodrigues(camCalParas.rvecsMats[i], rotationMatrixs[i]);
//	}
//	camCalParas.Count = imageCount;
//	camCalParas.cameraMatrix = cameraMatrix;
//	camCalParas.distCoeffs = distCoeffs;
//	camCalParas.error = total_err / imageCount;
//	camCalParas.rotationMatrixs = rotationMatrixs;
//	return camCalParas;
//}
////////////////////////////////////////////////////////////////////////////////
////// 函数：Calibration
////// 描述：根据相机的畸变系数、内参来校正拍摄的图片
////// 输入：imageSource为待校正的图片，cameraMatrix为相机的内参数，distCoeffs为畸变系数向量
////// 输出：
////// 返回：畸变校正后的图片
////// 备注：
////// Modified by 
////////////////////////////////////////////////////////////////////////////////
//Mat ImageProcess::Calibration(Mat imageSource, Mat cameraMatrix, Mat distCoeffs)
//{
//	Size imageSize;
//	imageSize.width = imageSource.cols;
//	imageSize.height = imageSource.rows;
//	Mat R = Mat::eye(3, 3, CV_32F);
//	Mat mapx = Mat(imageSize, CV_32FC1);
//	Mat mapy = Mat(imageSize, CV_32FC1);
//	Mat newImage = imageSource.clone();
//	initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
//	remap(imageSource, newImage, mapx, mapy, INTER_LINEAR);
//	return newImage;
//}

//////////////////////////////////////////////////////////////////////////////
//// 函数：ComputeWhiteTrans
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
float* ImageProcess::ComputeWhiteTrans(Mat src)
{
	/*
	Mat src = AverageImage(mats);
#define WHITE_NUM 500
	//暂时取全图亮度最高的一定数量像素点均值作为白点
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
	*/

	const int height = src.rows;
	const int width = src.cols;

	vector<Mat> channels;
	split(src, channels);
	Mat srcB = channels.at(0);
	Mat srcG = channels.at(1);
	Mat srcR = channels.at(2);

	//Mat B = srcB.clone();
	//Mat G = srcG.clone();
	//Mat R = srcR.clone();
	
	//计算白色区域三通道的平均值
	int sum[3] = {};//B G R
	float v[3] = {};
	float ave[3] = {};
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				//if (data[j] == 0)
				//	break;
				v[k] += data[j];				
				sum[k]++;
			}
		}
		ave[k] = v[k] / float(sum[k]);
	}
	//计算三通道平均值中的最大值，其他两通道变换一致
	float temp = 0.00;
	int maxCh = 0;
	for (int i = 0; i < 3; i++)
	{
		if (ave[i] > temp)
		{
			maxCh = i;
			temp = ave[i];
		}
	}

	static float trans[3];
	for (int i = 0; i < 3; i++)
	{
		if (i == maxCh)
			trans[i] = 1;
		else
			trans[i] = temp / ave[i];
	}

	return trans;
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

	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	//Mat srcB = channels.at(0);
	//Mat srcG = channels.at(1);
	//Mat srcR = channels.at(2);

	//float transB = trans[0];
	//float transG = trans[1];
	//float transR = trans[2];

	//map<int,map<int,float>> selectPoints;
	Mat selectPoints = Mat::zeros(height, width, CV_32FC3);//标记溢出255的点，用Mat搜索快很多

	float tempVal = 0;
	float tempTrans;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < channels.at(k).rows; i++)
		{
			uchar* data = channels.at(k).ptr<uchar>(i);
			uchar* data_s = selectPoints.ptr<uchar>(i);
			for (int j = 0; j < channels.at(k).cols; j++)
			{
				tempVal = float(data[j]) * trans[k];
				if (!IsSelected(gray, i, j))
				{
					if (tempVal > 255)
					{
						data[j] = 255;
						tempTrans = 255.00 / float(data[j]);
						//selectPoints[i][j] = tempTrans;
						data_s[j] = tempTrans;
						Select(gray, i, j);
					}
					else
						data[j] = tempVal;
				}	
				else
				{
					//tempTrans = selectPoints[i][j];
					data_s[j] = tempTrans;
					data[j] = tempTrans * data[j];
				}				
			}
		}
	}
	Mat	dst(src.rows, src.cols, CV_8UC3, Scalar(0));
	merge(channels, dst);
	return dst;


	/*
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
	*/
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：GrayStrech
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::GrayStrech(Mat src)
{
	Mat gray;
	cvtColor(src, gray, CV_RGB2GRAY);
	double minVal = 0, maxVal = 0;
	minMaxLoc(gray, &minVal, &maxVal);

	float tran = 255.00 / maxVal;
	float trans[3] = { tran };
	Mat dst = WhiteBalance(src, trans);//鸡贼地借用了白平衡里的循环，懒得再写了

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
vector<Mat, string> ImageProcess::ReadImages(cv::String path)
{
	vector<Mat, string> mats;
	String imgPattern = "//*.bmp";
	vector<String> imgFiles;
	imgPattern = path + imgPattern;
	glob(imgPattern, imgFiles);
	for (size_t i = 0; i<imgFiles.size(); i++)
	{
		Mat mat = imread(imgFiles[i]);
		mats.push_back(mat);
	}

	return mats;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：ComputeWhiteArea
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
	float count = 0.0;
	float mean = 0.0;
	float variance = 0.0;
	cvtColor(mask, mask, CV_BGR2GRAY);
	//非零个数n
	int n = countNonZero(mask);
	//元素总和sumAll
	total = sum(mask);
	sumAll = total[0];

	//求非零元素均值mean
	mean = sumAll*1.0 / n;

	//求非零元素方差variance
	for (i = 0; i < maskRows; i++)
	{
		uchar* data2 = mask.ptr<uchar>(i);
		for (j = 0; j < maskCols; j++)
		{
			if (data2[j] != 0)
			{
				count = count + pow(data2[j] - mean, 2);
			}

		}
	}

	variance = count*1.0 / n;

	if (mean<220.0&&variance>5.0)
	{
		Mat whiteArea = Mat::zeros(maskRows, maskCols, CV_8UC3);
		return whiteArea;
	}
	else
	{
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
		return whiteArea;
	}
}

//////////////////////////////////////////////////////////////////////////////
//// 函数：ComputeKeyPoints
//// 描述：求取连通域的几何中心
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Point2f* ImageProcess::ComputeKeyPoints(Mat src)
{
	GaussianBlur(src, src, Size(5, 5), 0);
	vector<vector<Point> > contours;//contours的类型，双重的vector
	vector<Vec4i> hierarchy;//Vec4i是指每一个vector元素中有四个int型数据。
	//阈值
	threshold(src, src, 60, 255, THRESH_BINARY);
	findContours(src.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// 计算矩
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}
	///  计算中心矩:
	static Point2f pts[3];
	for (int i = 0; i < contours.size(); i++)
	{
		pts[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}
	/// 绘制轮廓
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255);
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, pts[i], 4, color, -1, 8, 0);
	}
	imshow("outImage", drawing);
	waitKey();

	return pts;
}

//////////////////////////////////////////////////////////////////////////////
//// 函数：ComputeSampleArea
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//// 函数：ComputeThreshold
//// 描述：
//// 输入：
//// 输出：
//// 返回：
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
int ImageProcess::ComputeThreshold(Mat srcImage)
{
	srcImage = srcImage.reshape(0, 1);
	Size imageSize = srcImage.size();
	cv::sort(srcImage, srcImage, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
	uchar* data = srcImage.ptr<uchar>(0);
	int index = (int)imageSize.width*imageSize.height*0.05;
	int total = 0;
	for (int i = 0; i < index; i++)
	{
		total += data[i];
	}
	return total / index;
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
Mat ImageProcess::ReadMask(int cameraID, int sampleID)
{
	string filename, path;
	string tmp;
	path = "..//imgs_mask//";
	stringstream ss;
	ss << cameraID;
	string s1 = ss.str();
	tmp = "00" + s1;
	filename = path + tmp + ".bmp";
	Mat mask;
	mask = imread(filename);
	return mask;
}