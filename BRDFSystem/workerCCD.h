////////////////////////////Camera相机线程类声明////////////////////////////
#pragma once
#ifndef WORKERCCD_H
#define WORKERCCD_H

#include <QObject>
#include <QImage>
#include <QTimerEvent>
#include <QThread>
#include <QMutex>
#include "ccd.h"

class WorkerCCD : public QObject
{
	Q_OBJECT

public:
	//WorkerCCD(int workerID, QObject *parent = 0);
	WorkerCCD(int workerID, VimbaSystem& system, QObject *parent = 0);
	virtual ~WorkerCCD();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//主界面类需要用到该类的采集图像相关变量

private slots:
	void StartTimer(int measureFlag);
	//void SetExposureTime();
	void CloseWorker();
	void Grab(int sID, int iID);
	void GetMaterialName(QString materialName);

private:
	VimbaSystem&			_system;
	int						_workerID;
	AVTCamera*				cameraAVT;
	QImage					_img;
	Mat						_mat;
	Mat						_matWB;
	bool					_capture;
	string					_materialName;
	int						_timerId;
	unsigned char*			_pImageFrame;
	int						_height;
	int						_width;
	//int						_saveName;
	float					_exposureTime;
	string					_imageSavingPath1 = "..\\imgs_measurement1\\";
	string					_imageSavingPath2 = "..\\imgs_measurement2\\";
	string					_imageSavingPath3 = "..\\imgs_calibration\\";
	int						_measureFlag;//主界面传入的采集类型标记
	QMutex					_mutex;

signals:
	//void next();
	//void sendingMat(int workerID, /*Mat*/QImage mat);
	void sendingImg(int workerID, QImage img);
	void grabDone(int workerID);
	//void startMeasurement(int measureFlag);
};
#endif

