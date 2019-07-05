#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
//#include <QThread>
#include <QDebug>
//#include <QTimerEvent>
#include <QFileDialog>
#include <QSettings>
#include <time.h>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "slideComm.h"
//#include "sampleComm.h"
//#include "ccd.h"

#include "workerCCD.h"
//#include "configuration.h"

#define CAM_NUM 9  //相机数量

////////////////////////////界面主线程类声明////////////////////////////
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(VimbaSystem&	system, QWidget *parent = 0);
	virtual ~MainWindow();

private slots:
	//页面切换栏
	void TurnToMeasurement1();
	void TurnToMeasurement2();
	void TurnToMeasurement3();
	void TurnToModeling1();
	void TurnToModeling2();
	void TurnToSettings();
	void TurnToTest();
	void TurnToPreCamera();
	
	//配置文件
	void PushButton_Save_Pressed();
	void PushButton_Defaults_Pressed();

	//材质采集页面
	void PushButton_StartMeasurement_Pressed();
	void StopMeasurement();
	void SendingMat(int workerID, QImage mat);
	void ConnectRGB();


	//相机预处理页面
	void PushButton_IniCCD_Pressed();
	void PushButton_CaptureContinuously_Pressed();


	//槽函数的公用函数
	void IsEdited();
	void DisplayImage(int workerID, QImage img);

private:
	Ui::MainWindowClass ui;

	WorkerMeasurement*			workerMeasurement;
	QThread*					threadMeasurement;
	SlideComm*					slideComm;
	SampleComm*					sampleComm;
	WorkerCCD*					workerCCD[9];
	QThread*					threadCCD[9];
	//WorkerMeasurement*			workerMeasurement[9];
	//QThread*					threadMeasurement[9];
	VimbaSystem&				_system;
	QSettings					*ini;
	//VimbaSystem&				_system = VimbaSystem::GetInstance();//相机的SDK-Vimba系统
	//为了避免每个相机线程重复开启Vimba系统，只能暂时吧该引用提到最上层
	QString						_qMaterialName;
	bool						_displayFlag;
	int							_measureFlag;
	QMutex						_mutex;

	void CreateFolds(int flag, string root, string fileName = "");
	void ShowImgOnQLabel(QLabel* qlabel, QImage img);

	/*
	//配置文件参数
	////工业相机配置保存
	int gain;
	int darkLevel;
	QString imageSaveFormat;
	QString imageSavePath;

	////光源配置保存
	QString serialPortSelection;
	QString baudRate;
	int delaySetting;
	int lightingSequence;

	////样品台电机驱动配置保存
	QString stepperMotorPortSelection;
	QString stepperMotorSpeed;
	QString stepperMotorAcceleration;
	QString stepperMotorDeceleration;
	QString stepperMotorResolution;
	QString sampleRotationAngle;

	////滑轨电机驱动配置保存
	QString servoMotorPortSelection;
	QString servoMotorSpeed;
	QString servoMotorAcceleration;
	QString servoMotorDeceleration;
	QString servoMotorResolution;
	QString slideTableMovingDistance;
	*/

signals:
	void startTimer(/*int dispalyFlag*/);
	void startMeasure(int measureFlag);
	void sendingMaterialName(QString materialName);
	void sendingMat(int workerID, QImage mat);//主界面转手一下

};
#endif
