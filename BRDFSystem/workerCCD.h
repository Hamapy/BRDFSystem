////////////////////////////Camera相机线程类声明////////////////////////////
#pragma once
#ifndef WORKERCCD_H
#define WORKERCCD_H

#include <QObject>
#include <QImage>
#include <QTimerEvent>
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
	void StartTimer();

private:
	VimbaSystem&			_system;
	int						_workerID;
	AVTCamera				*_cameraAVT;
	QImage					_img;
	Mat						_mat;
	bool					_capture;
	bool					_measurement;
	//int framerate = 0;
	int						_timerId;
	unsigned char*			_pImageFrame;
	int						_height;
	int						_width;

signals:
	//void sendingImg(int, QImage);
	void sendingImg(QImage);
	//void startMeasurement(QImage);
};
#endif