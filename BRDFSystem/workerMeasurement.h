////////////////////////////材质采集线程类声明////////////////////////////
#pragma once
#ifndef WORKERMEASUREMENT_H
#define WORKERMEASUREMENT_H

#include <QObject>
#include <QImage>
#include <QTimerEvent>
#include "ccd.h"
#include "illuminant.h"

class WorkerMeasurement : public QObject
{
	Q_OBJECT

public:
	WorkerMeasurement(QObject *parent = 0);
	virtual ~WorkerMeasurement();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//主界面类需要用到该类的采集图像相关变量

private slots:
	void StartTimer();
	void SaveAMat(int workerID, Mat mat);

private:
	Illuminant*				illuminant;
	UINT                    portNo;
	int						_timerId;
	int						_saveName;
	string					_imageSavingPath = "..\\imgs_measurement";
	//Mat						_mat;
	//VimbaSystem&			_system;
	//int						_workerID;
	//AVTCamera				*_cameraAVT;
	//QImage					_img;
	//Mat						_mat;
	//bool					_capture;
	//bool					_measurement;
	//int framerate = 0;
	
	//unsigned char*			_pImageFrame;
	//int						_height;
	//int						_width;

};
#endif
