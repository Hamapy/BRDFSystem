////////////////////////////材质采集线程类声明////////////////////////////
#pragma once
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


class WorkerMeasurement : public QObject
{
	Q_OBJECT

public:
	//WorkerMeasurement(QObject *parent = 0);
	WorkerMeasurement(VimbaSystem& system, QObject *parent = 0);
	virtual ~WorkerMeasurement();
	//virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//主界面类需要用到该类的采集图像相关变量
	friend class WorkerCCD;

private slots:
void	NextMeasureState(int workerID, /*Mat*/QImage mat);
inline void	SaveAMat(int workerID, /*Mat*/QImage mat);
inline void	SaveSeriesMat(int workerID, /*Mat*/QImage mat);
void	GetMaterialName(QString materialName);
	//void StartMeasure(int measureFlag);
	//void StartTimer(int measureFlag);

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
	int*					_saveName;
	//float					_exposureTime;
	string					_imageSavingPath1 = "..\\imgs_measurement1\\";
	string					_imageSavingPath2 = "..\\imgs_measurement2\\";
	string					_imageSavingPath3 = "..\\imgs_calibration\\";
	bool					_isReady;
	bool					_captureDone;
	//byte					_seriesCam; //表示已收到的9台相机中的图像数量，byte只有8位，只能暂时用数组标记
	bool*					_seriesCAM;
	Illuminant*				illuminant;
	SampleComm*				sampleComm;
	UINT                    portNo;
	int						_timerId;
	string					_materialName;

	UINT*					_illuminantID;
	UINT					_iID;//亮灯序号
	UINT					_sID;//样品台角度序号
	int						_measureFlag;
	bool					_sampleFlag;

	QMutex					_mutex;

signals:
	//void					done();
	void					readyForCapture();
};
#endif
