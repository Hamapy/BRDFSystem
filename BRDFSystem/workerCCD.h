////////////////////////////Camera����߳�������////////////////////////////
#pragma once
#ifndef WORKERCCD_H
#define WORKERCCD_H

#include "workerMeasurement.h"
#include <QObject>

class WorkerCCD : public QObject
{
	Q_OBJECT

public:
	//WorkerCCD(int workerID, QObject *parent = 0);
	WorkerCCD(int workerID, VimbaSystem& system, QObject *parent = 0);
	virtual ~WorkerCCD();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//����������Ҫ�õ�����Ĳɼ�ͼ����ر���

private slots:
	void StartTimer();
	void SetExposureTime();
	//void SaveAMat();

private:
	//WorkerMeasurement*		workerMeasurement;
	//QThread*				threadMeasurement;

	VimbaSystem&			_system;
	int						_workerID;
	AVTCamera				*cameraAVT;
	QImage					_img;
	Mat						_mat;
	bool					_capture;
	//int framerate = 0;
	int						_timerId;
	unsigned char*			_pImageFrame;
	int						_height;
	int						_width;
	//int						_saveName;
	float					_exposureTime;
	//string					_imageSavingPath1 = "..\\imgs_measurement1";
	//string					_imageSavingPath2 = "..\\imgs_measurement2";
	//string					_imageSavingPath3 = "..\\imgs_calibration";
	int						_measureFlag;//�����洫��Ĳɼ����ͱ��
	QMutex					_mutex;

signals:
	//void next();
	void sendingMat(int workerID, /*Mat*/QImage mat);
	void sendingImg(int workerID, QImage img);
	//void startMeasurement(int measureFlag);
};
#endif

