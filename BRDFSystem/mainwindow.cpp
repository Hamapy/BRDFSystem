#include "mainwindow.h"

////////////////////////////////主界面定义////////////////////////////
//VimbaSystem& _system = VimbaSystem::GetInstance();

MainWindow::MainWindow(VimbaSystem&	system, QWidget *parent) : 
_system(system),
QMainWindow(parent)
{
	ui.setupUi(this);
	
	//界面及软件初始化
	AVTCamera::IniVimba(_system);
	this->setAttribute(Qt::WA_DeleteOnClose, true);//关闭窗口时清空内存
	TurnToMeasurement1();
	//this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
	ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件



	////滑动条相关设置
	int nMin = 0;
	int nMax = 200;
	int nSingleStep = 10;


	QSlider *pSlider1 = this->ui.horizontalSlider_gain;
	pSlider1->setMinimum(nMin);  // 最小值
	pSlider1->setMaximum(nMax);  // 最大值
	pSlider1->setSingleStep(nSingleStep);  // 步长

	QSlider *pSlider2 = this->ui.horizontalSlider_darkLevel;
	pSlider2->setMinimum(nMin);  // 最小值
	pSlider2->setMaximum(nMax);  // 最大值
	pSlider2->setSingleStep(nSingleStep);  // 步长


	/*
	////显示配置文件的参数
	////工业相机配置参数
	gain = this->ini->value("SWIR-Configuration/gain").toInt();
	gain = QString::number(gain, 10);
	this->ui.lineEdit_gain->setText(gain);
	darkLevel = this->ini->value("SWIR-Configuration/darkLevel").toInt();
	darkLevel = QString::number(darkLevel, 10);
	this->ui.lineEdit_darkLevel->setText(darkLevel);
	imageSaveFormat = this->ini->value("SWIR-Configuration/imageSaveFormat").toString();
	int index = ui->comboBox_imageSaveFormat->findText(imageSaveFormat);
	ui->comboBox_imageSaveFormat->setCurrentIndex(index);
	imageSavePath = this->ini->value("SWIR-Configuration/imageSavePath").toString();
	this->ui.lineEdit_imageSavePath->setText(imageSavePath);


	////光源配置参数
	serialPortSelection = this->ini->value("SWIR-Configuration/serialPortSelection").toString();
	int index = ui->comboBox_serialPortSelection->findText(serialPortSelection);
	ui->comboBox_serialPortSelection->setCurrentIndex(index);
	baudRate = this->ini->value("SWIR-Configuration/baudRate").toString();
	int index = ui->comboBox_baudRate->findText(baudRate);
	ui->comboBox_baudRate->setCurrentIndex(index);
	delaySetting = this->ini->value("SWIR-Configuration/delaySetting").toInt();
	delaySetting = QString::number(delaySetting, 10);
	this->ui.lineEdit_delaySetting->setText(delaySetting);
	lightingSequence = this->ini->value("SWIR-Configuration/lightingSequence").toInt();
	lightingSequence = QString::number(lightingSequence, 10);
	this->ui.lineEdit_lightingSequence->setText(lightingSequence);


	////样品台电机驱动配置参数
	stepperMotorPortSelection = this->ini->value("SWIR-Configuration/stepperMotorPortSelection").toString();
	int index = ui->comboBox_stepperMotorPortSelection->findText(stepperMotorPortSelection);
	ui->comboBox_stepperMotorPortSelection->setCurrentIndex(index);
	stepperMotorSpeed = this->ini->value("SWIR-Configuration/stepperMotorSpeed").toString();
	int index = ui->comboBox_stepperMotorSpeed->findText(stepperMotorSpeed);
	ui->comboBox_stepperMotorSpeed->setCurrentIndex(index);
	stepperMotorAcceleration = this->ini->value("SWIR-Configuration/stepperMotorAcceleration").toString();
	int index = ui->comboBox_stepperMotorAcceleration->findText(stepperMotorAcceleration);
	ui->comboBox_stepperMotorAcceleration->setCurrentIndex(index);
	stepperMotorDeceleration = this->ini->value("SWIR-Configuration/stepperMotorDeceleration").toString();
	int index = ui->comboBox_stepperMotorDeceleration->findText(stepperMotorDeceleration);
	ui->comboBox_stepperMotorDeceleration->setCurrentIndex(index);
	stepperMotorResolution = this->ini->value("SWIR-Configuration/stepperMotorResolution").toString();
	int index = ui->comboBox_stepperMotorResolution->findText(stepperMotorResolution);
	ui->comboBox_stepperMotorResolution->setCurrentIndex(index);
	sampleRotationAngle = this->ini->value("SWIR-Configuration/sampleRotationAngle").toString();
	int index = ui->comboBox_sampleRotationAngle->findText(sampleRotationAngle);
	ui->comboBox_sampleRotationAngle->setCurrentIndex(index);
	

	////滑轨电机驱动配置参数
	servoMotorPortSelection = this->ini->value("SWIR-Configuration/servoMotorPortSelection").toString();
	int index = ui->comboBox_servoMotorPortSelection->findText(servoMotorPortSelection);
	ui->comboBox_servoMotorPortSelection->setCurrentIndex(index);
	servoMotorSpeed = this->ini->value("SWIR-Configuration/servoMotorSpeed").toString();
	int index = ui->comboBox_servoMotorSpeed->findText(servoMotorSpeed);
	ui->comboBox_servoMotorSpeed->setCurrentIndex(index);
	servoMotorAcceleration = this->ini->value("SWIR-Configuration/servoMotorAcceleration").toString();
	int index = ui->comboBox_servoMotorAcceleration->findText(servoMotorAcceleration);
	ui->comboBox_servoMotorAcceleration->setCurrentIndex(index);
	servoMotorDeceleration = this->ini->value("SWIR-Configuration/servoMotorDeceleration").toString();
	int index = ui->comboBox_servoMotorDeceleration->findText(servoMotorDeceleration);
	ui->comboBox_servoMotorDeceleration->setCurrentIndex(index);
	servoMotorResolution = this->ini->value("SWIR-Configuration/servoMotorResolution").toString();
	int index = ui->comboBox_servoMotorResolution->findText(servoMotorResolution);
	ui->comboBox_servoMotorResolution->setCurrentIndex(index);
	slideTableMovingDistance = this->ini->value("SWIR-Configuration/slideTableMovingDistance").toInt();
	slideTableMovingDistance = QString::number(slideTableMovingDistance, 10);
	this->ui.lineEdit_slideTableMovingDistance->setText(slideTableMovingDistance);

	
	////连接信号槽,保存和恢复默认设置
	connect(this->ui.pushButton_save, SIGNAL(pressed()), this, SLOT(PushButton_Save_Pressed()));
	connect(this->ui.pushButton_defaults, SIGNAL(pressed()), this, SLOT(PushButton_Defaults_Pressed()));
	////连接信号槽（相互改变）
	connect(this->ui.lineEdit_gain, SIGNAL(valueChanged(int)), this->ui.horizontalSlider_gain, SLOT(setValue(int)));
	connect(this->ui.horizontalSlider_gain, SIGNAL(valueChanged(int)), this->ui.lineEdit_gain, SLOT(setValue(int)));
	connect(this->ui.lineEdit_gain, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_gain, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	////连接信号槽（相互改变）
	connect(this->ui.lineEdit_darkLevel, SIGNAL(valueChanged(int)), this->ui.horizontalSlider_darkLevel, SLOT(setValue(int)));
	connect(this->ui.horizontalSlider_darkLevel, SIGNAL(valueChanged(int)), this->ui.lineEdit_darkLevel, SLOT(setValue(int)));
	connect(this->ui.lineEdit_darkLevel, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_darkLevel, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	connect(this->ui.comboBox_imageSaveFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.lineEdit_imageSavePath, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));

	connect(this->ui.comboBox_serialPortSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_baudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.lineEdit_delaySetting, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_delaySetting, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_lightingSequence, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_lightingSequence, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));

	connect(this->ui.comboBox_stepperMotorPortSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_stepperMotorSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_stepperMotorAcceleration, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_stepperMotorDeceleration, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_stepperMotorResolution, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_sampleRotationAngle, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));

	connect(this->ui.comboBox_servoMotorPortSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_servoMotorSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_servoMotorAcceleration, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_servoMotorDeceleration, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.comboBox_servoMotorResolution, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.lineEdit_slideTableMovingDistance, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_slideTableMovingDistance, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	*/

	////页面切换
	connect(this->ui.pushButton_measure1, SIGNAL(pressed()), this, SLOT(TurnToMeasurement1()));
	connect(this->ui.pushButton_measure2, SIGNAL(pressed()), this, SLOT(TurnToMeasurement2()));
	connect(this->ui.pushButton_measure3, SIGNAL(pressed()), this, SLOT(TurnToMeasurement3()));
	connect(this->ui.pushButton_brdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling1()));
	connect(this->ui.pushButton_svbrdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling2()));
	connect(this->ui.pushButton_settings, SIGNAL(pressed()), this, SLOT(TurnToSettings()));
	connect(this->ui.pushButton_test, SIGNAL(pressed()), this, SLOT(TurnToTest()));
	connect(this->ui.pushButton_preCamera, SIGNAL(pressed()), this, SLOT(TurnToPreCamera()));

	////采集页面
	connect(this->ui.pushButton_startMeasurement, SIGNAL(pressed()), this, SLOT(PushButton_StartMeasurement_Pressed()));
	connect(this->ui.pushButton_stopMeasurement, SIGNAL(pressed()), this, SLOT(StopMeasurement()));
	
	slideComm = new SlideComm();
	slideComm->Init(9, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);

	workerMeasurement = new WorkerMeasurement(_system);//指明每一个采集线程的父指针
	threadMeasurement = new QThread();
	workerMeasurement->moveToThread(threadMeasurement);

	////相机预处理页面
	this->ui.pushButton_captureContinuously->setEnabled(false);
	this->ui.pushButton_finiCCD->setEnabled(false);
	connect(this->ui.pushButton_iniCCD, SIGNAL(pressed()), this, SLOT(PushButton_IniCCD_Pressed()));
	connect(this->ui.pushButton_captureContinuously, SIGNAL(pressed()), this, SLOT(PushButton_CaptureContinuously_Pressed()));
	
	for (int i = 0; i < CAM_NUM; i++)
	{
		workerCCD[i] = new WorkerCCD(i, _system);
		threadCCD[i] = new QThread();
		workerCCD[i]->moveToThread(threadCCD[i]);
		connect(workerCCD[i], SIGNAL(sendingImg(int, QImage)), this, SLOT(DisplayImage(int, QImage)), Qt::UniqueConnection);//用队列方式会有延时
	}
}

MainWindow::~MainWindow()
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		delete workerCCD[i];
		workerCCD[i] = NULL;

		if (threadCCD[i]->isFinished())
			return;
		threadCCD[i]->quit();
		threadCCD[i]->wait();

		delete threadCCD[i];
		threadCCD[i] = NULL;
	}

	delete slideComm;
	delete workerMeasurement;
	if (threadMeasurement->isFinished())
		return;
	threadMeasurement->quit();
	threadMeasurement->wait();
	delete threadMeasurement;
	threadMeasurement = NULL;

	AVTCamera::FiniVimba(_system);
}

////////////////////////////////私有槽函数/////////////////////////////////////
////////////////////////////////配置页面/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：isEdited()
// 描述：文本框参数有改动时，按钮控件可点击
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::IsEdited()
{
	this->ui.pushButton_defaults->setEnabled(true);
	this->ui.pushButton_save->setEnabled(true);
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_Save_Pressed
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Save_Pressed()
{
	/*
	////工业相机配置保存
	gain = this->ui.lineEdit_gain->text().toInt();
	this->ini->setValue("/SWIR-Configuration/gain", gain);
	darkLevel = this->ui.lineEdit_darkLevel->text().toInt();
	this->ini->setValue("/SWIR-Configuration/darkLevel", darkLevel);
	imageSaveFormat = this->ui.comboBox_imageSaveFormat->currentText();
	this->ini->setValue("/SWIR-Configuration/imageSaveFormat", imageSaveFormat);
	imageSavePath = this->ui.lineEdit_imageSavePath->text().toString();
	this->ini->setValue("/SWIR-Configuration/imageSavePath", imageSavePath);

	////光源配置保存
	serialPortSelection = this->ui.comboBox_serialPortSelection->currentText();
	this->ini->setValue("/SWIR-Configuration/serialPortSelection", serialPortSelection);
	baudRate = this->ui.comboBox_baudRate->currentText();
	this->ini->setValue("/SWIR-Configuration/baudRate", baudRate);
	delaySetting = this->ui.lineEdit_delaySetting->text().toInt();
	this->ini->setValue("/SWIR-Configuration/delaySetting", delaySetting);
	lightingSequence = this->ui.lineEdit_lightingSequence->text().toInt();
	this->ini->setValue("/SWIR-Configuration/lightingSequence", lightingSequence);

	////样品台电机驱动配置保存
	stepperMotorPortSelection = this->ui.comboBox_stepperMotorPortSelection->currentText();
	this->ini->setValue("/SWIR-Configuration/stepperMotorPortSelection", stepperMotorPortSelection);
	stepperMotorSpeed = this->ui.comboBox_stepperMotorSpeed->currentText();
	this->ini->setValue("/SWIR-Configuration/stepperMotorSpeed", stepperMotorSpeed);
	stepperMotorAcceleration = this->ui.comboBox_stepperMotorAcceleration->currentText();
	this->ini->setValue("/SWIR-Configuration/stepperMotorAcceleration", stepperMotorAcceleration);
	stepperMotorDeceleration = this->ui.comboBox_stepperMotorDeceleration->currentText();
	this->ini->setValue("/SWIR-Configuration/stepperMotorDeceleration", stepperMotorDeceleration);
	stepperMotorResolution = this->ui.comboBox_stepperMotorResolution->currentText();
	this->ini->setValue("/SWIR-Configuration/stepperMotorResolution", stepperMotorResolution);
	sampleRotationAngle = this->ui.comboBox_sampleRotationAngle->currentText();
	this->ini->setValue("/SWIR-Configuration/sampleRotationAngle", sampleRotationAngle);

	////滑轨电机驱动配置保存
	servoMotorPortSelection = this->ui.comboBox_servoMotorPortSelection->currentText();
	this->ini->setValue("/SWIR-Configuration/servoMotorPortSelection", servoMotorPortSelection);
	servoMotorSpeed = this->ui.comboBox_servoMotorSpeed->currentText();
	this->ini->setValue("/SWIR-Configuration/servoMotorSpeed", servoMotorSpeed);
	servoMotorAcceleration = this->ui.comboBox_servoMotorAcceleration->currentText();
	this->ini->setValue("/SWIR-Configuration/servoMotorAcceleration", servoMotorAcceleration);
	servoMotorDeceleration = this->ui.comboBox_servoMotorDeceleration->currentText();
	this->ini->setValue("/SWIR-Configuration/servoMotorDeceleration", servoMotorDeceleration);
	servoMotorResolution = this->ui.comboBox_servoMotorResolution->currentText();
	this->ini->setValue("/SWIR-Configuration/servoMotorResolution", servoMotorResolution);
	slideTableMovingDistance = this->ui.lineEdit_slideTableMovingDistance->text().toInt();
	this->ini->setValue("/SWIR-Configuration/slideTableMovingDistance", slideTableMovingDistance);
	*/

	QMessageBox::information(NULL, "Save", "Saved Successfully.", QMessageBox::Ok, QMessageBox::Ok);
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_Save_Pressed
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Defaults_Pressed()
{
	/*
	////工业相机默认配置
	this->ui.lineEdit_gain->setText(gain);
	this->ui.lineEdit_darkLevel->setText(darkLevel);
	int index = ui->comboBox_imageSaveFormat->findText(imageSaveFormat);
	ui->comboBox_imageSaveFormat->setCurrentIndex(index);
	this->ui.lineEdit_imageSavePath->setText(imageSavePath);

	////光源默认配置
	int index = ui->comboBox_serialPortSelection->findText(serialPortSelection);
	ui->comboBox_serialPortSelection->setCurrentIndex(index);
	int index = ui->comboBox_baudRate->findText(baudRate);
	ui->comboBox_baudRate->setCurrentIndex(index);
	this->ui.lineEdit_delaySetting->setText(delaySetting);
	this->ui.lineEdit_lightingSequence->setText(lightingSequencel);

	////样品台电机驱动默认配置
	int index = ui->comboBox_stepperMotorPortSelection->findText(stepperMotorPortSelection);
	ui->comboBox_stepperMotorPortSelection->setCurrentIndex(index);
	int index = ui->comboBox_stepperMotorSpeed->findText(stepperMotorSpeed);
	ui->comboBox_stepperMotorSpeed->setCurrentIndex(index);
	int index = ui->comboBox_stepperMotorAcceleration->findText(stepperMotorAcceleration);
	ui->comboBox_stepperMotorAcceleration->setCurrentIndex(index);
	int index = ui->comboBox_stepperMotorDeceleration->findText(stepperMotorDeceleration);
	ui->comboBox_stepperMotorDeceleration->setCurrentIndex(index);
	int index = ui->comboBox_stepperMotorResolution->findText(stepperMotorResolution);
	ui->comboBox_stepperMotorResolution->setCurrentIndex(index);
	int index = ui->comboBox_sampleRotationAngle->findText(sampleRotationAngle);
	ui->comboBox_sampleRotationAngle->setCurrentIndex(index);


	////滑轨电机驱动默认配置
	int index = ui->comboBox_servoMotorPortSelection->findText(servoMotorPortSelection);
	ui->comboBox_servoMotorPortSelection->setCurrentIndex(index);
	int index = ui->comboBox_servoMotorSpeed->findText(servoMotorSpeed);
	ui->comboBox_servoMotorSpeed->setCurrentIndex(index);
	int index = ui->comboBox_servoMotorAcceleration->findText(servoMotorAcceleration);
	ui->comboBox_servoMotorAcceleration->setCurrentIndex(index);
	int index = ui->comboBox_servoMotorDeceleration->findText(servoMotorDeceleration);
	ui->comboBox_servoMotorDeceleration->setCurrentIndex(index);
	int index = ui->comboBox_servoMotorResolution->findText(servoMotorResolution);
	ui->comboBox_servoMotorResolution->setCurrentIndex(index);
	this->ui.lineEdit_slideTableMovingDistance->setText(slideTableMovingDistance);
	*/
}

////////////////////////////////切换页面/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToMeasurement1
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement1()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);	
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToMeasurement2
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToMeasurement3
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement3()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToModeling1
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToModeling1()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToModeling2
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToModeling2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToSettings
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToSettings()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Settings);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToTest
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToTest()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Test);
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToPreCamera
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToPreCamera()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.PreCamera);
}


////////////////////////////////采集页面/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：StartMeasurement
// 描述：开始采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_StartMeasurement_Pressed()
{
	_displayFlag = 1;
	_qMaterialName = this->ui.lineEdit_materialName->text();
	if (_qMaterialName == "")
	{
		QMessageBox::critical(NULL, QStringLiteral("警告"), QStringLiteral("请输入材质名称"), QMessageBox::Ok);
		return;
	}

	this->ui.lineEdit_materialName->setEnabled(false);
	this->ui.pushButton_startMeasurement->setEnabled(false);
	if (_measureFlag == 1)
	{
		CreateFolds(1, "..\\imgs_measurement1\\", _qMaterialName.toStdString());
		CreateFolds(2, "..\\imgs_measurement1\\" + _qMaterialName.toStdString());
	}	
	if (_measureFlag == 2)
	{
		CreateFolds(1, "..\\imgs_measurement2\\", _qMaterialName.toStdString());
		CreateFolds(2, "..\\imgs_measurement2\\" + _qMaterialName.toStdString());
	}

	this->ui.lineEdit_materialName->setEnabled(false);
	this->ui.pushButton_stopMeasurement->setEnabled(true);
	this->ui.toolBox->setEnabled(false);
	
	//滑轨就位
	//slideComm->Init(9, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	slideComm->MoveToX2();
	Sleep(15000);//等待滑轨就位

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
			connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
			emit startTimer();

			//用槽机制传递Mat保存下来的是空图？ 注册类型之后还是不行
			////向槽机制注册一下Mat类型
			//qRegisterMetaType<Mat>("Mat");
			//connect(this->workerCCD[i], SIGNAL(sendingMat(int, Mat)), this->workerMeasurement, SLOT(NextMeasureState(int, Mat)), Qt::UniqueConnection);//唯一连接，等我把槽函数执行完，别催
			//connect(this->workerCCD[i], SIGNAL(sendingMat(int, QImage)), this->workerMeasurement, SLOT(NextMeasureState(int, QImage)), Qt::UniqueConnection);//唯一连接，等我把槽函数执行完，别催

			//connect(this->workerCCD[i], SIGNAL(next()), this->workerMeasurement, SLOT(NextMeasureState()));
			connect(this->workerMeasurement, SIGNAL(readyForCapture()), this->workerCCD[i], SLOT(SetExposureTime()));

			connect(this->workerCCD[i], SIGNAL(sendingMat(int, QImage)), this, SLOT(SendingMat(int, QImage)), Qt::UniqueConnection);//唯一连接

		}
		//if (!threadMeasurement[i]->isRunning())
		//{
		//	threadMeasurement[i]->start();
		//	connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
		//	emit startTimer();
		//	this->workerCCD[i]->_measurement = 1;
		//}
	}
	//需要主线程中转一下，不然还是九个相机线程在抢光源和样品台串口
	//这样其实还是并行采集串行保存
	connect(this, SIGNAL(sendingMat(int, QImage)), this->workerMeasurement, SLOT(NextMeasureState(int, QImage)), Qt::UniqueConnection);//唯一连接，等我把槽函数执行完，别催

	if (!threadMeasurement->isRunning())
	{
		_mutex.lock();
		this->workerMeasurement->_measureFlag = _measureFlag;
		_mutex.unlock();

		threadMeasurement->start();
		//connect(this, SIGNAL(startMeasurement(int)), workerMeasurement, SLOT(NextMeasureState(int, Mat)));
		//emit startMeasure(_measureFlag);
		connect(this, SIGNAL(sendingMaterialName(QString)), this->workerMeasurement, SLOT(GetMaterialName(QString)));
		emit sendingMaterialName(_qMaterialName);
	}
	Sleep(500);//等待相机初始化

}
////////////////////////////////////////////////////////////////////////////
// 函数：StopMeasurement
// 描述：停止采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::StopMeasurement()
{

}
////////////////////////////////////////////////////////////////////////////
// 函数：StopMeasurement
// 描述：停止采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::SendingMat(int workerID, QImage mat)
{
	emit sendingMat(workerID, mat);
}
////////////////////////////////////////////////////////////////////////////
// 函数：ChangeWindows
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::ConnectRGB()
{
	//workerCCD[0]->cameraRGB_->Init();
	//workerCCD[0]->cameraRGB_->OpenCamera(0);
	//workerCCD[0]->cameraRGB_->SetCameraSettings(0, 40000, 0.00, 0.00);//图像格式老问题，5.9又踩坑
	//if (!threadRGB[0]->isRunning())
	//{
	//	threadRGB[0]->start();
	//}
}

/////////////////////////////相机预处理页面/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_iniCCD_pressed
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_IniCCD_Pressed()
{
	ui.pushButton_captureContinuously->setEnabled(true);
	ui.pushButton_finiCCD->setEnabled(true);

	_displayFlag = 0;
	CreateFolds(2, "..\\imgs_calibration");
	
	for (int i = 0; i < CAM_NUM; i++)
	{
		//workerCCD[i]->cameraRGB_->SetCameraSettings(0, 40000, 0.00, 0.00);//图像格式老问题，5.9又踩坑
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
			connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
			emit startTimer();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_captureContinuously_pressed
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_CaptureContinuously_Pressed()
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		this->workerCCD[i]->_capture = 1;
	}
}

/////////////////////////////槽函数的公用函数/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage0
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage(int workerID, QImage img)
{
	if (_displayFlag == 0)
	{
		if (workerID == 0)
			ShowImgOnQLabel(this->ui.label_precamera0, img);
		if (workerID == 1)
			ShowImgOnQLabel(this->ui.label_precamera1, img);
		if (workerID == 2)
			ShowImgOnQLabel(this->ui.label_precamera2, img);
		if (workerID == 3)
			ShowImgOnQLabel(this->ui.label_precamera3, img);
		if (workerID == 4)
			ShowImgOnQLabel(this->ui.label_precamera4, img);
		if (workerID == 5)
			ShowImgOnQLabel(this->ui.label_precamera5, img);
		if (workerID == 6)
			ShowImgOnQLabel(this->ui.label_precamera6, img);
		if (workerID == 7)
			ShowImgOnQLabel(this->ui.label_precamera7, img);
		if (workerID == 8)
			ShowImgOnQLabel(this->ui.label_precamera8, img);
	}
	else if (_displayFlag == 1)
	{
		if (workerID == 0)
			ShowImgOnQLabel(this->ui.label_camera0, img);
		if (workerID == 1)
			ShowImgOnQLabel(this->ui.label_camera1, img);
		if (workerID == 2)
			ShowImgOnQLabel(this->ui.label_camera2, img);
		if (workerID == 3)
			ShowImgOnQLabel(this->ui.label_camera3, img);
		if (workerID == 4)
			ShowImgOnQLabel(this->ui.label_camera4, img);
		if (workerID == 5)
			ShowImgOnQLabel(this->ui.label_camera5, img);
		if (workerID == 6)
			ShowImgOnQLabel(this->ui.label_camera6, img);
		if (workerID == 7)
			ShowImgOnQLabel(this->ui.label_camera7, img);
		if (workerID == 8)
			ShowImgOnQLabel(this->ui.label_camera8, img);
	}
}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage1
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage1(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera1->width(), ui.label_precamera1->height());
//			this->ui.label_precamera1->setPixmap(pic);
//			this->ui.label_precamera1->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera1->width(), ui.label_camera1->height());
//			this->ui.label_camera1->setPixmap(pic);
//			this->ui.label_camera1->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage2
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage2(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera2->width(), ui.label_precamera2->height());
//			this->ui.label_precamera2->setPixmap(pic);
//			this->ui.label_precamera2->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera2->width(), ui.label_camera2->height());
//			this->ui.label_camera2->setPixmap(pic);
//			this->ui.label_camera2->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage3
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage3(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera3->width(), ui.label_precamera3->height());
//			this->ui.label_precamera3->setPixmap(pic);
//			this->ui.label_precamera3->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera3->width(), ui.label_camera3->height());
//			this->ui.label_camera3->setPixmap(pic);
//			this->ui.label_camera3->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage4
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage4(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera4->width(), ui.label_precamera4->height());
//			this->ui.label_precamera4->setPixmap(pic);
//			this->ui.label_precamera4->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera4->width(), ui.label_camera4->height());
//			this->ui.label_camera4->setPixmap(pic);
//			this->ui.label_camera4->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage5
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage5(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera5->width(), ui.label_precamera5->height());
//			this->ui.label_precamera5->setPixmap(pic);
//			this->ui.label_precamera5->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera5->width(), ui.label_camera5->height());
//			this->ui.label_camera5->setPixmap(pic);
//			this->ui.label_camera5->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage6
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage6(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera6->width(), ui.label_precamera6->height());
//			this->ui.label_precamera6->setPixmap(pic);
//			this->ui.label_precamera6->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera6->width(), ui.label_camera6->height());
//			this->ui.label_camera6->setPixmap(pic);
//			this->ui.label_camera6->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage7
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage7(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera7->width(), ui.label_precamera7->height());
//			this->ui.label_precamera7->setPixmap(pic);
//			this->ui.label_precamera7->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera7->width(), ui.label_camera7->height());
//			this->ui.label_camera7->setPixmap(pic);
//			this->ui.label_camera7->setScaledContents(true);
//		}
//	}
//}
//////////////////////////////////////////////////////////////////////////////
//// 函数：DisplayImage8
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::DisplayImage8(QImage img)
//{
//	if (_displayFlag == 0)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_precamera8->width(), ui.label_precamera8->height());
//			this->ui.label_precamera8->setPixmap(pic);
//			this->ui.label_precamera8->setScaledContents(true);
//		}
//	}
//	else if (_displayFlag == 1)
//	{
//		if (!img.isNull())
//		{
//			QPixmap pic = QPixmap::fromImage(img);
//			pic = pic.scaled(ui.label_camera8->width(), ui.label_camera8->height());
//			this->ui.label_camera8->setPixmap(pic);
//			this->ui.label_camera8->setScaledContents(true);
//		}
//	}
//}

////////////////////////////////////私有函数////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：CreateFolds
// 描述：在指定目录下创建相机对应文件夹
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateFolds(int flag, string root, string fileName)
{
	if (flag == 1)
	{
		///////Unicode字符集问题/////////
		WCHAR wszStr[256];
		memset(wszStr, 0, sizeof(wszStr));
		MultiByteToWideChar(CP_ACP, 0, root.c_str(), strlen(root.c_str()) + 1, wszStr,
			sizeof(wszStr) / sizeof(wszStr[0]));

		if (GetFileAttributes(wszStr) & FILE_ATTRIBUTE_DIRECTORY) //判断路径是文件还是目录
		{
			string newFolderPath = root + fileName;
			///////Unicode字符集问题/////////
			WCHAR wszNewStr[256];
			memset(wszNewStr, 0, sizeof(wszNewStr));
			MultiByteToWideChar(CP_ACP, 0, newFolderPath.c_str(), strlen(newFolderPath.c_str()) + 1, wszNewStr,
				sizeof(wszNewStr) / sizeof(wszNewStr[0]));

			if (!CreateDirectory(wszNewStr, NULL))
			{
				cout << "文件夹已存在！" << endl;
			}
		}
	}
	if (flag == 2)
	{
		for (int i = 0; i < CAM_NUM; i++)
		{
			///////Unicode字符集问题/////////
			WCHAR wszStr[256];
			memset(wszStr, 0, sizeof(wszStr));
			MultiByteToWideChar(CP_ACP, 0, root.c_str(), strlen(root.c_str()) + 1, wszStr,
				sizeof(wszStr) / sizeof(wszStr[0]));

			if (GetFileAttributes(wszStr) & FILE_ATTRIBUTE_DIRECTORY) //判断路径是文件还是目录
			{
				string newFolderPath = root + "\\camera" + to_string(i);
				///////Unicode字符集问题/////////
				WCHAR wszNewStr[256];
				memset(wszNewStr, 0, sizeof(wszNewStr));
				MultiByteToWideChar(CP_ACP, 0, newFolderPath.c_str(), strlen(newFolderPath.c_str()) + 1, wszNewStr,
					sizeof(wszNewStr) / sizeof(wszNewStr[0]));

				if (!CreateDirectory(wszNewStr, NULL))
				{
					cout << "文件夹已存在！" << endl;
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：ShowImgOnQLabel
// 描述：在指定Qlabel上显示采集图像
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::ShowImgOnQLabel(QLabel* qlabel, QImage img)
{
	_mutex.lock();
	if (!img.isNull())
	{
		QPixmap pic = QPixmap::fromImage(img);
		pic = pic.scaled(qlabel->width(), qlabel->height());
		qlabel->setPixmap(pic);
		qlabel->setScaledContents(true);
	}
	_mutex.unlock();
}