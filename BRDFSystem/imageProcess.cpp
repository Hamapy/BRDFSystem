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
	merge({ srcB, srcG, srcR }, dst);

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
//// 函数：ComputeChessTrans
//// 描述：函数计算的是相机校正后的内外参数、旋转矩阵、旋转向量、平移向量
//// 输入：mats为输入图片的集合，boardSize为棋盘格上角点的信息、squareSize是棋盘格上每个方格的边长信息
//// 输出：
//// 返回：包含相机信息的结构体
//// 备注：务必注意boardSize要与实际的角点信息匹配，否则程序将报错
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
cameraCalibrationParamaters ImageProcess::ComputeChessTrans(const vector<Mat>& mats, const Size boardSize, const Size squareSize)
{
	cameraCalibrationParamaters camCalParas;
	int imageCount = mats.size();
	Size imageSize;
	Mat imageInput;
	//读取每一幅图像，从中提取出角点，然后对角点进行亚像素精确化
	vector<Point2f> imagePointsBuf;/* 缓存每幅图像上检测到的角点 */
	vector<vector<Point2f>> imagePointsSeq;/* 缓存每幅图像上检测到的角点 */
	for (int i = 0; i < imageCount; i++)
	{
		imageInput = mats[i];
		if (i == 0)
		{
			imageSize.width = imageInput.cols;
			imageSize.height = imageInput.rows;
		}
		//注意这里的boardSize必须和用于检测的棋盘格图片匹配，否则程序将退出
		if (0 == findChessboardCorners(imageInput, boardSize, imagePointsBuf))
		{
			//cout << "can not find chessboard corneres!\n";
			exit(1);
		}
		else
		{
			Mat viewGray;
			cvtColor(imageInput, viewGray, CV_RGB2GRAY);
			/* 亚像素精确化 */
			find4QuadCornerSubpix(viewGray, imagePointsBuf, Size(5, 5)); //对粗提取的角点进行精确化
			imagePointsSeq.push_back(imagePointsBuf);  //保存亚像素角点
			/* 在图像上显示角点位置 */
			//drawChessboardCorners(viewGray, boardSize, imagePointsBuf, true);
			//waitKey(500);
		}
	}
	vector<vector<Point3f>> objectPoints;
	/* 摄像机内参数矩阵 */
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	vector<int> pointCounts;
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0)); /* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */
	/* 初始化标定板上角点的三维坐标 */
	int i, j, t;
	for (t = 0; t<imageCount; t++)
	{
		vector<Point3f> tempPointSet;
		for (i = 0; i<boardSize.height; i++)
		{
			for (j = 0; j<boardSize.width; j++)
			{
				Point3f realPoint;
				/* 假设标定板放在世界坐标系中z=0的平面上 */
				realPoint.x = i*squareSize.width;
				realPoint.y = j*squareSize.height;
				realPoint.z = 0;
				tempPointSet.push_back(realPoint);
			}
		}
		objectPoints.push_back(tempPointSet);
	}
	/* 初始化每幅图像中的角点数量，假定每幅图像中都可以看到完整的标定板 */
	for (i = 0; i<imageCount; i++)
	{
		pointCounts.push_back(boardSize.width*boardSize.height);
	}
	/* 开始标定 */
	calibrateCamera(objectPoints, imagePointsSeq, imageSize, cameraMatrix, distCoeffs, camCalParas.rvecsMats, camCalParas.tvecsMats, 0);
	//对标定结果进行评价
	double total_err = 0.0; /* 所有图像的平均误差的总和 */
	double err = 0.0; /* 每幅图像的平均误差 */
	vector<Point2f> image_points2; /* 保存重新计算得到的投影点 */
	vector<Mat> rotationMatrixs(imageCount);
	for (i = 0; i<imageCount; i++)
	{
		vector<Point3f> tempPointSet = objectPoints[i];
		/* 通过得到的摄像机内外参数，对空间的三维点进行重新投影计算，得到新的投影点 */
		projectPoints(tempPointSet, rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, image_points2);
		/* 计算新的投影点和旧的投影点之间的误差*/
		vector<Point2f> tempImagePoint = imagePointsSeq[i];
		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
		for (int j = 0; j < tempImagePoint.size(); j++)
		{
			image_points2Mat.at<Vec2f>(0, j) = Vec2f(image_points2[j].x, image_points2[j].y);
			tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
		}
		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
		total_err += err /= pointCounts[i];
		Rodrigues(camCalParas.rvecsMats[i], rotationMatrixs[i]);
	}
	camCalParas.Count = imageCount;
	camCalParas.cameraMatrix = cameraMatrix;
	camCalParas.distCoeffs = distCoeffs;
	camCalParas.error = total_err / imageCount;
	camCalParas.rotationMatrixs = rotationMatrixs;
	return camCalParas;
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：Calibration
//// 描述：根据相机的畸变系数、内参来校正拍摄的图片
//// 输入：imageSource为待校正的图片，cameraMatrix为相机的内参数，distCoeffs为畸变系数向量
//// 输出：
//// 返回：畸变校正后的图片
//// 备注：
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
Mat ImageProcess::Calibration(Mat imageSource, Mat cameraMatrix, Mat distCoeffs)
{
	Size imageSize;
	imageSize.width = imageSource.cols;
	imageSize.height = imageSource.rows;
	Mat R = Mat::eye(3, 3, CV_32F);
	Mat mapx = Mat(imageSize, CV_32FC1);
	Mat mapy = Mat(imageSize, CV_32FC1);
	Mat newImage = imageSource.clone();
	initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
	remap(imageSource, newImage, mapx, mapy, INTER_LINEAR);
	return newImage;
}