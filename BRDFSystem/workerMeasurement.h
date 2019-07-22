////////////////////////////���ʲɼ��߳�������////////////////////////////
#pragma once
#ifndef WORKERMEASUREMENT_H
#define WORKERMEASUREMENT_H

#include <QObject>
#include <QImage>
#include <QTimerEvent>
#include <QThread>
#include <QMutex>
#include "config.h"
#include "ccd.h"
#include "illuminant.h"
#include "sampleComm.h"
#include "slideComm.h"

#define SAMPLE_NUM 36
#define ILLUMINANT_NUM 196

class WorkerMeasurement : public QObject
{
	Q_OBJECT

public:
	WorkerMeasurement(QObject *parent = 0);
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
	bool ReadBrdf(const char *filename, double* &brdf);
	void LookupBrdfVal(double* brdf, int theta_in, int fi_in, int theta_out, int fi_out, double& red_val, double& green_val, double& blue_val);
	bool WriteBRDF();

	//void GetExposureTime(int workerID, Mat mat);

private:
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
	//bool					_captureDone;
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
	//bool					_sampleFlag;
	bool					_illuminantFlag;
	//QSettings *ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�
	QMutex					_mutex;

	const int               thetaOutNum = 9;
	const int               fiOutNum = 12;
	const int               thetaInNum = 9;
	const int               lightSourceNum = 196;
	
	string                  path;//��ȡͼƬ��·��
	char*                   savePath;//����.binary�ļ���·��
	vector<double> AverageRGB(const Mat& inputImage);
	int theta_out_index(int theta_out);
	inline int  fi_out_index(int fi_out);
	inline int theta_in_index(int theta_in);
	inline int fi_in_index(int fi_in, int theta_in);

signals:
	void					done(); //֪ͨ�����̲߳ɼ�����
	void					readyForGrab(int sID, int iID);
};
#endif
