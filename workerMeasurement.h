////////////////////////////材质采集线程类声明////////////////////////////

#pragma once
//定义的常量
const int               thetaOutNum = 9;
const int               fiOutNum = 18;
const int               thetaInNum = 9;
const int               lightSourceNum = 196;



#ifndef WORKERMEASUREMENT_H

#define WORKERMEASUREMENT_H
#include <QObject>
#include <QImage>
#include <QTimerEvent>
#include <QThread>
#include <QMutex>
#include "ccd.h"
#include "illuminant.h"
#include "sampleComm.h"
#include "slideComm.h"
#include <sstream>

class WorkerMeasurement : public QObject
{
	Q_OBJECT
public:
	//WorkerMeasurement(QObject *parent = 0);
	WorkerMeasurement(VimbaSystem& system, QObject *parent = 0);
	virtual ~WorkerMeasurement();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//主界面类需要用到该类的采集图像相关变量
	friend class WorkerCCD;
	private slots:
	//void	NextMeasureState(int workerID, /*Mat*/QImage mat);
	//void	SaveAMat(int workerID, /*Mat*/QImage mat);
	void CheckDone(int workerID);
	void StartTimer(int measureFlag);
	void CloseWorker();
	//void ContributeBRDF();
	vector<double> AverageRGB(const Mat& inputImage);
	bool ReadBrdf(const char *filename, double* &brdf);
	void LookupBrdfVal(double* brdf, int theta_in, int fi_in,
		int theta_out, int fi_out,
		double& red_val, double& green_val, double& blue_val);
	bool WriteBRDF()
		//void GetExposureTime(int workerID, Mat mat);
private:
	VimbaSystem&			_system;
	int						_workerID;
	//AVTCamera*				cameraAVT;
	QImage					_img;
	Mat						_mat;
	unsigned char*			_pImageFrame;
	int						_height;
	int						_width;
	//int*					_saveName;
	//float					_exposureTime;
	//string					_imageSavingPath1 = "..\\imgs_measurement1\\";
	//string					_imageSavingPath2 = "..\\imgs_measurement2\\";
	//string					_imageSavingPath3 = "..\\imgs_calibration\\";
	bool					_isReady;
	bool					_captureDone;
	//byte					_seriesCam; //表示已收到的9台相机中的图像数量，byte只有8位，只能暂时用数组标记
	bool*					_seriesCAM;
	Illuminant*				illuminant;
	SampleComm*				sampleComm;
	SlideComm*				slideComm;
	UINT                    portNo;
	int						_timerId;
	UINT*					_illuminantID;
	UINT					_iID;//亮灯序号
	UINT					_sID;//样品台角度序号
	int						_measureFlag;
	bool					_sampleFlag;
	QMutex					_mutex;
	string                  path;//读取图片的路径
	char*                   savePath;//保存.binary文件的路径
	int theta_out_index(int theta_out);
	inline int  fi_out_index(int fi_out);
	inline int theta_in_index(int theta_in);
	inline int fi_in_index(int fi_in, int theta_in);
signals:
	void					done(); //通知其他线程采集结束
	void					readyForGrab();
};
#endif