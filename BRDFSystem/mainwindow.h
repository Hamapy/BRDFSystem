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

	//设置参数
	int gain = 0;
	int gain_Int = 0;
	QString gain_Str = "";
	int darkLevel = 0;
	int darkLevel_Int = 0;
	QString darkLevel_Str = "";
	QString imageSaveFormat = "";
	QString imageSaveFormat_Str = "";
	QString imageSavePath = "";
	QString imageSavePath_Str = "";
	QString serialPortSelection = "";
	QString serialPortSelection_Str = "";
	QString baudRate = "";
	QString baudRate_Str = "";
	int delaySetting = 0;
	QString delaySetting_Str = "";
	int lightingSequence = 0;
	QString lightingSequence_Str = "";
	QString stepperMotorPortSelection = "";
	QString stepperMotorPortSelection_Str = "";
	int stepperMotorSpeed_Int = 0;
	QString stepperMotorSpeed = "";
	QString stepperMotorSpeed_Str = "";
	int stepperMotorAcceleration_Int = 0;
	QString stepperMotorAcceleration = "";
	QString stepperMotorAcceleration_Str = "";
	int stepperMotorDeceleration_Int = 0;
	QString stepperMotorDeceleration = "";
	QString stepperMotorDeceleration_Str = "";
	int stepperMotorResolution_Int = 0;
	QString stepperMotorResolution = "";
	QString stepperMotorResolution_Str = "";
	QString sampleRotationAngle = "";
	QString sampleRotationAngle_Str = "";
	QString servoMotorPortSelection = "";
	QString servoMotorPortSelection_Str = "";
	QString servoMotorSpeed = "";
	QString servoMotorSpeed_Str = "";
	QString servoMotorAcceleration = "";
	QString servoMotorAcceleration_Str = "";
	QString servoMotorDeceleration = "";
	QString servoMotorDeceleration_Str = "";
	QString servoMotorResolution = "";
	QString servoMotorResolution_Str = "";
	int slideTableMovingDistance = 0;
	QString slideTableMovingDistance_Str = "";

signals:
	void startTimer(/*int dispalyFlag*/);
	void startMeasure(int measureFlag);
	void sendingMaterialName(QString materialName);
	void sendingMat(int workerID, QImage mat);//主界面转手一下

};
#endif
