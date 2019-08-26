////////////////////////////Camera����߳�������////////////////////////////
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

using namespace std;
using namespace cv;

class WorkerCCD : public QObject
{
	Q_OBJECT

public:
	WorkerCCD(int workerID, QObject *parent = 0);
	virtual ~WorkerCCD();
	virtual void timerEvent(QTimerEvent *event);
	//virtual void run();
	friend class MainWindow;//����������Ҫ�õ�����Ĳɼ�ͼ����ر���

private slots:
	void StartTimer(int measureFlag);
	//void SetExposureTime();
	void CloseWorker();
	void Grab(int sID, int iID);
	void GetMaterialName(QString materialName);

private:
	//VimbaSystem&			_system;
	int						_workerID;
	int						_sampleID;
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
	string					_imageSavingPath3 = "..\\imgs_measuremasks\\";
	string					_imageSavingPathHDR = "..\\imgs_measurementHDR\\";
	int						_measureFlag;//�����洫��Ĳɼ����ͱ��
	//QMutex					_mutex;
	bool					_isMasked;
	bool					_hasWhiteTrans;
	bool					_hasAffineTrans;
	QImage					CvMat2QImage(const Mat& mat);
	//QSettings *ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�
	//ImageProcess*           imageProcess;
	Mat						_gray;
	bool					_isHdr;

	//�궨��ӱ��������ļ���ȡ�Ĳ���
	vector<vector<int>>		_deadPos;
	Mat						_cameraMatrix;
	Mat						_distCoeffs;
	Mat						_affineTrans;
	float*					_whiteTrans;
	Point2f					pCenter;

signals:
	//void next();
	//void sendingMat(int workerID, /*Mat*/QImage mat);
	void sendingImg(int workerID, QImage img);
	void grabDone(int workerID);
	//void startMeasurement(int measureFlag);
};
#endif

