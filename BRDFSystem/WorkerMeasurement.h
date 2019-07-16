////////////////////////////���ʲɼ��߳�������////////////////////////////

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
	friend class MainWindow;//����������Ҫ�õ�����Ĳɼ�ͼ����ر���
	friend class WorkerCCD;
	private slots:
	//void	NextMeasureState(int workerID, /*Mat*/QImage mat);
	//void	SaveAMat(int workerID, /*Mat*/QImage mat);
	void CheckDone(int workerID);
	void StartTimer(int measureFlag);
	void CloseWorker();
	void ContributeBRDF();
	void AverageRGB(const Mat& inputImage);
	bool read_brdf(const char *filename, double* &brdf);
	void lookup_brdf_val(double* brdf, double theta_in, double fi_in,
		double theta_out, double fi_out,
		double& red_val, double& green_val, double& blue_val);
	bool WriteBRDF(string path, string savePath)
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
	//byte					_seriesCam; //��ʾ���յ���9̨����е�ͼ��������byteֻ��8λ��ֻ����ʱ��������
	bool*					_seriesCAM;
	Illuminant*				illuminant;
	SampleComm*				sampleComm;
	SlideComm*				slideComm;
	UINT                    portNo;
	int						_timerId;
	UINT*					_illuminantID;
	UINT					_iID;//�������
	UINT					_sID;//��Ʒ̨�Ƕ����
	int						_measureFlag;
	bool					_sampleFlag;
	QMutex					_mutex;
	const int               thetaOutNum = 9;
	const int               fiOutNum = 12;
	const int               thetaInNum = 9;
	const int               lightSourceNum = 196;
	int theta_out_index(int theta_out);
	inline int  fi_out_index(int fi_out);
	inline int theta_in_index(int theta_in);
	inline int fi_in_index(int fi_in, int theta_in);
signals:
	void					done(); //֪ͨ�����̲߳ɼ�����
	void					readyForGrab();
};
#endif