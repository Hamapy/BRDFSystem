////////////////////////////���ʲɼ��߳�������////////////////////////////
/*
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
	//WorkerCCD(int workerID, QObject *parent = 0);
	WorkerMeasurement(int workerID, VimbaSystem& system, QObject *parent = 0);
	virtual ~WorkerMeasurement();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//����������Ҫ�õ�����Ĳɼ�ͼ����ر���

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
	//void sendingImg(QImage);
	void startMeasurement(QImage);
};
#endif
*/