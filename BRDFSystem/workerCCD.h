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
#include "config.h"
#include "imageProcess.h"

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
	QImage                  _imgCheckMarkArea;
	Mat						_mat;
	Mat						_matWB;
	Mat                     _matCheckMarkArea;
	bool					_capture;
	bool                    _checkMarkArea;
	string					_materialName;
	int						_timerId;
	unsigned char*			_pImageFrame;
	int						_height;
	int						_width;
	//int						_saveName;
	float					_exposureTime;
	string					_imageSavingPath1 = "..\\imgs_measurement1\\";
	string					_imageSavingPath2 = "..\\imgs_measurement2\\";
	string					_imageSavingPath3 = "..\\imgs_periodcapture\\";
	string					_imageSavingPathHDR = "..\\imgs_measurementHDR\\";
	int						_measureFlag;//主界面传入的采集类型标记
	QMutex					_mutex;
	//QSettings *ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件

signals:
	//void next();
	//void sendingMat(int workerID, /*Mat*/QImage mat);
	void sendingImg(int workerID, QImage img);
	void grabDone(int workerID);
	//void startMeasurement(int measureFlag);
};
#endif

