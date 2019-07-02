////////////////////////////材质采集线程类声明////////////////////////////
#pragma once
#ifndef WORKERMEASUREMENT_H
#define WORKERMEASUREMENT_H

#include <QObject>
#include <QImage>
#include <QTimerEvent>
#include "ccd.h"
#include "illuminant.h"
#include "sampleComm.h"
#include "ccd.h"

class WorkerMeasurement : public QObject
{
	Q_OBJECT

public:
	WorkerMeasurement(QObject *parent = 0);
	virtual ~WorkerMeasurement();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//主界面类需要用到该类的采集图像相关变量
	friend class WorkerCCD;

private slots:
	void StartTimer(int measureFlag);
	void SaveAMat(int workerID, Mat mat);

private:
	Illuminant*				illuminant;
	SampleComm*				sampleComm;
	UINT                    portNo;
	int						_timerId;
	int						_saveName;
	string					_imageSavingPath1 = "..\\imgs_measurement1";
	string					_imageSavingPath2 = "..\\imgs_measurement2";
	UINT*					_illuminantID;
	UINT					_ID;
	int						_measureFlag;
	float					_exposureTime;
};
#endif
