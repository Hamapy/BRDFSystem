#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QDebug>
//#include <QTimerEvent>
#include <QFileDialog>
#include <QSettings>
#include <time.h>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "slideComm.h"
#include "sampleComm.h"
#include "ccd.h"
#include "workerCCD.h"
//#include "configuration.h"

#define CAM_NUM 2  //�������

////////////////////////////�������߳�������////////////////////////////
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(VimbaSystem&	system, QWidget *parent = 0);
	virtual ~MainWindow();

private slots:
	//ҳ���л���
	void TurnToMeasurement1();
	void TurnToMeasurement2();
	void TurnToMeasurement3();
	void TurnToModeling1();
	void TurnToModeling2();
	void TurnToSettings();
	void TurnToTest();
	void TurnToPreCamera();
	
	//�����ļ�
	void PushButton_Save_Pressed();
	void PushButton_Defaults_Pressed();

	//���ʲɼ�ҳ��
	void PushButton_StartMeasurement_Pressed();
	void StopMeasurement();
	void ConnectRGB();

	//���Ԥ����ҳ��
	void PushButton_IniCCD_Pressed();
	void PushButton_CaptureContinuously_Pressed();


	//�ۺ����Ĺ��ú���
	void IsEdited();
	void DisplayImage(int workerID, QImage img);
	//void DisplayImage1(QImage img);
	//void DisplayImage2(QImage img);
	//void DisplayImage3(QImage img);
	//void DisplayImage4(QImage img);
	//void DisplayImage5(QImage img);
	//void DisplayImage6(QImage img);
	//void DisplayImage7(QImage img);
	//void DisplayImage8(QImage img);

private:
	Ui::MainWindowClass ui;

	SlideComm*					slideComm;
	SampleComm*					sampleComm;
	WorkerCCD*					workerCCD[9];
	QThread*					threadCCD[9];
	//WorkerMeasurement*			workerMeasurement[9];
	//QThread*					threadMeasurement[9];
	VimbaSystem&				_system;
	QSettings					*ini;
	//VimbaSystem&				_system = VimbaSystem::GetInstance();//�����SDK-Vimbaϵͳ
	//Ϊ�˱���ÿ������߳��ظ�����Vimbaϵͳ��ֻ����ʱ�ɸ������ᵽ���ϲ�
	QString _qMaterialName;
	bool _displayFlag;

	void CreateFolds(string root);
	void ShowImgOnQLabel(QLabel* qlabel, QImage img);

signals:
	void startTimer();
	//void startMeasureMent();
};
#endif
