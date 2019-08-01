#include "mainwindow.h"

////////////////////////////////主界面定义////////////////////////////
MainWindow::MainWindow(VimbaSystem&	system, QWidget *parent) : 
_system(system),
QMainWindow(parent)
{
	ui.setupUi(this);
	
	//界面及软件初始化
	AVTCamera::IniVimba(_system);
	//extern QSettings *ini;
	this->setAttribute(Qt::WA_DeleteOnClose, true);//关闭窗口时清空内存
	TurnToMeasurement1();
	

//////////////////////////////////////////////配置页面/////////////////////////////////////////////////////
	//ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件
	////滑动条相关设置
	int nMin = 0;
	int nMax = 50;
	int nSingleStep = 10;

	QSlider *pSlider1 = this->ui.horizontalSlider_gain;
	pSlider1->setMinimum(nMin);  // 最小值
	pSlider1->setMaximum(nMax);  // 最大值
	pSlider1->setSingleStep(nSingleStep);  // 步长
	pSlider1->setTickPosition(QSlider::TicksAbove);  //刻度在上方

	QSlider *pSlider2 = this->ui.horizontalSlider_darkLevel;
	pSlider2->setMinimum(nMin);  // 最小值
	pSlider2->setMaximum(nMax);  // 最大值
	pSlider2->setSingleStep(nSingleStep);  // 步长
	pSlider2->setTickPosition(QSlider::TicksAbove);  //刻度在上方

	////显示配置文件的参数
	////工业相机配置参数
	gain = mainWindowParameters->gain;
	gain_Float = gain;
	this->ui.horizontalSlider_gain->setValue(gain);
	this->ui.spinBox_gain->setValue(gain);
	blackLevel = mainWindowParameters->blackLevel;
	blackLevel_Float = blackLevel;
	this->ui.horizontalSlider_darkLevel->setValue(blackLevel);
	this->ui.spinBox_darkLevel->setValue(blackLevel);
	//imageSaveFormat = ini->value("BRDFSystem-Configuration/imageSaveFormat").toString();
	imageSaveFormat = mainWindowParameters->imageSaveFormat;
	imageSaveFormat_Str = imageSaveFormat;
	int index0 = ui.comboBox_imageSaveFormat->findText(imageSaveFormat);
	ui.comboBox_imageSaveFormat->setCurrentIndex(index0);
	imageSavePath = mainWindowParameters->imageSavePath;
	imageSavePath_Str = imageSavePath;
	this->ui.lineEdit_imageSavePath->setText(imageSavePath);


	////光源配置参数
	serialPortSelection = mainWindowParameters->serialPortSelection;
	serialPortSelection_Str = serialPortSelection;
	int index1 = ui.comboBox_serialPortSelection->findText(serialPortSelection);
	ui.comboBox_serialPortSelection->setCurrentIndex(index1);
	baudRate = mainWindowParameters->baudRate;
	baudRate_Str = baudRate;
	int index2 = ui.comboBox_baudRate->findText(baudRate);
	ui.comboBox_baudRate->setCurrentIndex(index2);
	delaySetting = mainWindowParameters->delaySetting;
	delaySetting_Str = QString::number(delaySetting, 10);
	this->ui.lineEdit_delaySetting->setText(delaySetting_Str);
	lightingSequence = ini->value("BRDFSystem-Configuration/lightingSequence").toInt();
	lightingSequence_Str = QString::number(lightingSequence, 10);
	this->ui.lineEdit_lightingSequence->setText(lightingSequence_Str);

	////样品台电机驱动配置参数
	stepperMotorPortSelection = mainWindowParameters->stepperMotorPortSelection;
	stepperMotorPortSelection_Str = stepperMotorPortSelection;
	int index3 = ui.comboBox_stepperMotorPortSelection->findText(stepperMotorPortSelection);
	ui.comboBox_stepperMotorPortSelection->setCurrentIndex(index3);
	stepperMotorSpeed = mainWindowParameters->stepperMotorSpeed;
	stepperMotorSpeed_Str = stepperMotorSpeed;
	int index4 = ui.comboBox_stepperMotorSpeed->findText(stepperMotorSpeed);
	ui.comboBox_stepperMotorSpeed->setCurrentIndex(index4);
	stepperMotorAcceleration = mainWindowParameters->stepperMotorAcceleration;
	stepperMotorAcceleration_Str = stepperMotorAcceleration;
	int index5 = ui.comboBox_stepperMotorAcceleration->findText(stepperMotorAcceleration);
	ui.comboBox_stepperMotorAcceleration->setCurrentIndex(index5);
	stepperMotorDeceleration = mainWindowParameters->stepperMotorDeceleration;
	stepperMotorDeceleration_Str = stepperMotorDeceleration;
	int index6 = ui.comboBox_stepperMotorDeceleration->findText(stepperMotorDeceleration);
	ui.comboBox_stepperMotorDeceleration->setCurrentIndex(index6);
	stepperMotorResolution = mainWindowParameters->stepperMotorResolution;
	stepperMotorResolution_Str = stepperMotorResolution;
	int index7 = ui.comboBox_stepperMotorResolution->findText(stepperMotorResolution);
	ui.comboBox_stepperMotorResolution->setCurrentIndex(index7);
	sampleRotationAngle = mainWindowParameters->sampleRotationAngle;
	sampleRotationAngle_Str = sampleRotationAngle;
	int index8 = ui.comboBox_sampleRotationAngle->findText(sampleRotationAngle);
	ui.comboBox_sampleRotationAngle->setCurrentIndex(index8);

	////滑轨电机驱动配置参数
	servoMotorPortSelection = mainWindowParameters->servoMotorPortSelection;
	servoMotorPortSelection_Str = servoMotorPortSelection;
	int index9 = ui.comboBox_servoMotorPortSelection->findText(servoMotorPortSelection);
	ui.comboBox_servoMotorPortSelection->setCurrentIndex(index9);
	servoMotorSpeed = mainWindowParameters->servoMotorSpeed;
	servoMotorSpeed_Str = servoMotorSpeed;
	int index10 = ui.comboBox_servoMotorSpeed->findText(servoMotorSpeed);
	ui.comboBox_servoMotorSpeed->setCurrentIndex(index10);
	servoMotorAcceleration = mainWindowParameters->servoMotorAcceleration;
	servoMotorAcceleration_Str = servoMotorAcceleration;
	int index11 = ui.comboBox_servoMotorAcceleration->findText(servoMotorAcceleration);
	ui.comboBox_servoMotorAcceleration->setCurrentIndex(index11);
	servoMotorDeceleration = mainWindowParameters->servoMotorDeceleration;
	servoMotorDeceleration_Str = servoMotorDeceleration;
	int index12 = ui.comboBox_servoMotorDeceleration->findText(servoMotorDeceleration);
	ui.comboBox_servoMotorDeceleration->setCurrentIndex(index12);
	servoMotorResolution = mainWindowParameters->servoMotorResolution;
	servoMotorResolution_Str = servoMotorResolution;
	int index13 = ui.comboBox_servoMotorResolution->findText(servoMotorResolution);
	ui.comboBox_servoMotorResolution->setCurrentIndex(index13);
	slideTableMovingDistance = mainWindowParameters->slideTableMovingDistance;
	slideTableMovingDistance_Str = QString::number(slideTableMovingDistance, 10);
	this->ui.lineEdit_slideTableMovingDistance->setText(slideTableMovingDistance_Str);

	////连接信号槽,保存和恢复默认设置
	connect(this->ui.pushButton_save, SIGNAL(pressed()), this, SLOT(PushButton_Save_Pressed()));
	connect(this->ui.pushButton_defaults, SIGNAL(pressed()), this, SLOT(PushButton_Defaults_Pressed()));
	////连接信号槽（相互改变）
	connect(this->ui.spinBox_gain, SIGNAL(valueChanged(int)), pSlider1, SLOT(setValue(int)));
	connect(pSlider1, SIGNAL(valueChanged(int)), this->ui.spinBox_gain, SLOT(setValue(int)));
	////连接信号槽（相互改变）
	connect(this->ui.spinBox_darkLevel, SIGNAL(valueChanged(int)), pSlider2, SLOT(setValue(int)));
	connect(pSlider2, SIGNAL(valueChanged(int)), this->ui.spinBox_darkLevel, SLOT(setValue(int)));
	connect(this->ui.comboBox_imageSaveFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(deal(int)));
	connect(this->ui.lineEdit_imageSavePath, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
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

	
//////////////////////////////////////////////切换页面/////////////////////////////////////////////////////
	connect(this->ui.pushButton_measure1, SIGNAL(pressed()), this, SLOT(TurnToMeasurement1()));
	connect(this->ui.pushButton_measure2, SIGNAL(pressed()), this, SLOT(TurnToMeasurement2()));
	connect(this->ui.pushButton_measure3, SIGNAL(pressed()), this, SLOT(TurnToMeasurement3()));
	connect(this->ui.pushButton_brdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling1()));
	connect(this->ui.pushButton_svbrdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling2()));
	connect(this->ui.pushButton_settings, SIGNAL(pressed()), this, SLOT(TurnToSettings()));
	connect(this->ui.pushButton_test, SIGNAL(pressed()), this, SLOT(TurnToTest()));
	connect(this->ui.pushButton_preCamera, SIGNAL(pressed()), this, SLOT(TurnToPreCamera()));

//////////////////////////////////////////////建模页面/////////////////////////////////////////////////////
	brdfFitting = new BRDFFitting();
	connect(this->ui.pushButton_startFitting, SIGNAL(pressed()), this, SLOT(PushButton_StartFitting_Pressed()));

//////////////////////////////////////////////采集页面/////////////////////////////////////////////////////
	workerMeasurement = new WorkerMeasurement();
	threadMeasurement = new QThread();
	workerMeasurement->moveToThread(threadMeasurement);
	for (int i = 0; i < CAM_NUM; i++)
	{
		workerCCD[i] = new WorkerCCD(i, _system);
		threadCCD[i] = new QThread();
		workerCCD[i]->moveToThread(threadCCD[i]);

		//用槽机制传递Mat保存下来的是空图？
		//qRegisterMetaType<Mat>("Mat");
		connect(this, SIGNAL(startTimer(int)), this->workerCCD[i], SLOT(StartTimer(int)));
		connect(this->workerCCD[i], SIGNAL(sendingImg(int, QImage)), this, SLOT(DisplayImage(int, QImage)), Qt::UniqueConnection);//防止重复连接。如果当前信号和槽已经连接过了，就不再连接了;UniqueConnection 模式：严格说不算连接方式，只是一个附加的参数。
		connect(this->workerCCD[i], SIGNAL(grabDone(int)), this->workerMeasurement, SLOT(CheckDone(int)), Qt::QueuedConnection);
		connect(this, SIGNAL(sendingMaterialName(QString)), this->workerCCD[i], SLOT(GetMaterialName(QString)));
		connect(this->workerMeasurement, SIGNAL(readyForGrab(int,int)), this->workerCCD[i], SLOT(Grab(int,int)), Qt::QueuedConnection);
		connect(this->workerMeasurement, SIGNAL(done()), this->workerCCD[i], SLOT(WokerClose()));
		connect(this->workerCCD[i]->cameraAVT, SIGNAL(sendingMeasureState(int, int, int)), this, SLOT(DisplayMeasureState(int, int, int)));

	}
	connect(this, SIGNAL(startMeasurement(int)), this->workerMeasurement, SLOT(StartTimer(int)));
	connect(this->ui.pushButton_startMeasurement, SIGNAL(pressed()), this, SLOT(PushButton_StartMeasurement_Pressed()));
	connect(this->ui.pushButton_stopMeasurement, SIGNAL(pressed()), this, SLOT(StopMeasurement()));
	connect(this->ui.pushButton_sampleReset, SIGNAL(pressed()), this, SLOT(PushButton_SampleReset_Pressed()));
	connect(this, SIGNAL(sendingMaterialName(QString)), this->workerMeasurement, SLOT(GetMaterialName(QString)));

///////////////////////////////////////////////相机预处理页面///////////////////////////////////////////////
	this->ui.pushButton_captureContinuously->setEnabled(false);
	this->ui.pushButton_chess->setEnabled(false);
	this->ui.pushButton_deadPixels->setEnabled(false);
	this->ui.pushButton_blackLevel->setEnabled(false);
	this->ui.pushButton_finiCCD->setEnabled(false);
	
	connect(this->ui.pushButton_iniCCD, SIGNAL(pressed()), this, SLOT(PushButton_IniCCD_Pressed()));
	connect(this->ui.pushButton_captureContinuously, SIGNAL(pressed()), this, SLOT(PushButton_CaptureContinuously_Pressed()));
	connect(this->ui.pushButton_chess, SIGNAL(pressed()), this, SLOT(PushButton_Chess_Pressed()));
	connect(this->ui.pushButton_whiteBalance, SIGNAL(pressed()), this, SLOT(PushButton_WhiteBalance_Pressed()));
	connect(this->ui.pushButton_deadPixels, SIGNAL(pressed()), this, SLOT(PushButton_DeadPixel_Pressed()));
	connect(this->ui.pushButton_blackLevel, SIGNAL(pressed()), this, SLOT(PushButton_BlackLevel_Pressed()));
	connect(this->ui.pushButton_finiCCD, SIGNAL(pressed()), this, SLOT(PushButton_FiniCCD_Pressed()));
	
	//辐射度标定
	this->ui.pushButton_captureOfPeriod->setEnabled(false);
	this->ui.pushButton_computeMask->setEnabled(false);
	this->ui.pushButton_masked->setEnabled(false);
	this->ui.pushButton_whiteBalance->setEnabled(false);
	connect(this->ui.pushButton_captureOfPeriod, SIGNAL(pressed()), this, SLOT(PushButton_CaptureOfPeriod_Pressed()));
	connect(this->ui.pushButton_computeMask, SIGNAL(pressed()), this, SLOT(PushButton_ComputeMask_Pressed()));
	connect(this->ui.pushButton_masked, SIGNAL(pressed()), this, SLOT(PushButton_Masked_Pressed()));
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
	////工业相机配置保存
	gain = this->ui.spinBox_gain->text().toFloat();
	ini->setValue("/BRDFSystem-Configuration/gain", gain);
	blackLevel = this->ui.spinBox_darkLevel->text().toFloat();
	ini->setValue("/BRDFSystem-Configuration/blackLevel", blackLevel);
	imageSaveFormat = this->ui.comboBox_imageSaveFormat->currentText();
	ini->setValue("/BRDFSystem-Configuration/imageSaveFormat", imageSaveFormat);
	imageSavePath = this->ui.lineEdit_imageSavePath->text();
	ini->setValue("/BRDFSystem-Configuration/imageSavePath", imageSavePath);

	////光源配置保存
	serialPortSelection = this->ui.comboBox_serialPortSelection->currentText();
	ini->setValue("/BRDFSystem-Configuration/serialPortSelection", serialPortSelection);
	baudRate = this->ui.comboBox_baudRate->currentText();
	ini->setValue("/BRDFSystem-Configuration/baudRate", baudRate);
	delaySetting = this->ui.lineEdit_delaySetting->text().toInt();
	ini->setValue("/BRDFSystem-Configuration/delaySetting", delaySetting);
	lightingSequence = this->ui.lineEdit_lightingSequence->text().toInt();
	ini->setValue("/BRDFSystem-Configuration/lightingSequence", lightingSequence);

	////样品台电机驱动配置保存
	stepperMotorPortSelection = this->ui.comboBox_stepperMotorPortSelection->currentText();
	ini->setValue("/BRDFSystem-Configuration/stepperMotorPortSelection", stepperMotorPortSelection);
	stepperMotorSpeed = this->ui.comboBox_stepperMotorSpeed->currentText();
	ini->setValue("/BRDFSystem-Configuration/stepperMotorSpeed", stepperMotorSpeed);
	stepperMotorAcceleration = this->ui.comboBox_stepperMotorAcceleration->currentText();
	ini->setValue("/BRDFSystem-Configuration/stepperMotorAcceleration", stepperMotorAcceleration);
	stepperMotorDeceleration = this->ui.comboBox_stepperMotorDeceleration->currentText();
	ini->setValue("/BRDFSystem-Configuration/stepperMotorDeceleration", stepperMotorDeceleration);
	stepperMotorResolution = this->ui.comboBox_stepperMotorResolution->currentText();
	ini->setValue("/BRDFSystem-Configuration/stepperMotorResolution", stepperMotorResolution);
	sampleRotationAngle = this->ui.comboBox_sampleRotationAngle->currentText();
	ini->setValue("/BRDFSystem-Configuration/sampleRotationAngle", sampleRotationAngle);

	////滑轨电机驱动配置保存
	servoMotorPortSelection = this->ui.comboBox_servoMotorPortSelection->currentText();
	ini->setValue("/BRDFSystem-Configuration/servoMotorPortSelection", servoMotorPortSelection);
	servoMotorSpeed = this->ui.comboBox_servoMotorSpeed->currentText();
	ini->setValue("/BRDFSystem-Configuration/servoMotorSpeed", servoMotorSpeed);
	servoMotorAcceleration = this->ui.comboBox_servoMotorAcceleration->currentText();
	ini->setValue("/BRDFSystem-Configuration/servoMotorAcceleration", servoMotorAcceleration);
	servoMotorDeceleration = this->ui.comboBox_servoMotorDeceleration->currentText();
	ini->setValue("/BRDFSystem-Configuration/servoMotorDeceleration", servoMotorDeceleration);
	servoMotorResolution = this->ui.comboBox_servoMotorResolution->currentText();
	ini->setValue("/BRDFSystem-Configuration/servoMotorResolution", servoMotorResolution);
	slideTableMovingDistance = this->ui.lineEdit_slideTableMovingDistance->text().toInt();
	ini->setValue("/BRDFSystem-Configuration/slideTableMovingDistance", slideTableMovingDistance);

	QMessageBox::information(NULL, "Save", "Saved Successfully.", QMessageBox::Ok, QMessageBox::Ok);
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_Defaults_Pressed
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Defaults_Pressed()
{
	////工业相机默认配置
	this->ui.horizontalSlider_gain->setValue(gain_Float);
	this->ui.spinBox_gain->setValue(gain_Float);
	this->ui.horizontalSlider_darkLevel->setValue(blackLevel_Float);
	this->ui.spinBox_darkLevel->setValue(blackLevel_Float); int index14 = ui.comboBox_imageSaveFormat->findText(imageSaveFormat_Str);
	ui.comboBox_imageSaveFormat->setCurrentIndex(index14);
	this->ui.lineEdit_imageSavePath->setText(imageSavePath_Str);

	////光源默认配置
	int index15 = ui.comboBox_serialPortSelection->findText(serialPortSelection_Str);
	ui.comboBox_serialPortSelection->setCurrentIndex(index15);
	int index16 = ui.comboBox_baudRate->findText(baudRate_Str);
	ui.comboBox_baudRate->setCurrentIndex(index16);
	this->ui.lineEdit_delaySetting->setText(delaySetting_Str);
	this->ui.lineEdit_lightingSequence->setText(lightingSequence_Str);

	////样品台电机驱动默认配置
	int index17 = ui.comboBox_stepperMotorPortSelection->findText(stepperMotorPortSelection_Str);
	ui.comboBox_stepperMotorPortSelection->setCurrentIndex(index17);
	int index18 = ui.comboBox_stepperMotorSpeed->findText(stepperMotorSpeed_Str);
	ui.comboBox_stepperMotorSpeed->setCurrentIndex(index18);
	int index19 = ui.comboBox_stepperMotorAcceleration->findText(stepperMotorAcceleration_Str);
	ui.comboBox_stepperMotorAcceleration->setCurrentIndex(index19);
	int index20 = ui.comboBox_stepperMotorDeceleration->findText(stepperMotorDeceleration_Str);
	ui.comboBox_stepperMotorDeceleration->setCurrentIndex(index20);
	int index21 = ui.comboBox_stepperMotorResolution->findText(stepperMotorResolution_Str);
	ui.comboBox_stepperMotorResolution->setCurrentIndex(index21);
	int index22 = ui.comboBox_sampleRotationAngle->findText(sampleRotationAngle_Str);
	ui.comboBox_sampleRotationAngle->setCurrentIndex(index22);


	////滑轨电机驱动默认配置
	int index23 = ui.comboBox_servoMotorPortSelection->findText(servoMotorPortSelection_Str);
	ui.comboBox_servoMotorPortSelection->setCurrentIndex(index23);
	int index24 = ui.comboBox_servoMotorSpeed->findText(servoMotorSpeed_Str);
	ui.comboBox_servoMotorSpeed->setCurrentIndex(index24);
	int index25 = ui.comboBox_servoMotorAcceleration->findText(servoMotorAcceleration_Str);
	ui.comboBox_servoMotorAcceleration->setCurrentIndex(index25);
	int index26 = ui.comboBox_servoMotorDeceleration->findText(servoMotorDeceleration_Str);
	ui.comboBox_servoMotorDeceleration->setCurrentIndex(index26);
	int index27 = ui.comboBox_servoMotorResolution->findText(servoMotorResolution_Str);
	ui.comboBox_servoMotorResolution->setCurrentIndex(index27);
	this->ui.lineEdit_slideTableMovingDistance->setText(slideTableMovingDistance_Str);
}
////////////////////////////////建模页面/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_StartFitting_Pressed
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_StartFitting_Pressed()
{
	string fileName;
	if (ui.treeWidget->isAnimated())
	{
		fileName = ui.treeWidget->currentItem()->text(0).toStdString();
	}
	else
	{
		QMessageBox::critical(NULL, QStringLiteral("警告"), QStringLiteral("请选择材质"), QMessageBox::Ok);
		return;
	}
	//brdfFitting->StartFitting(fileName, 10, 15, 5, 35, 5);
	//在界面显示拟合结果
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
	_measureFlag = 1;
	_displayFlag = 1;
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);	
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToMeasurement2
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement2()
{
	_measureFlag = 2;
	_displayFlag = 1;
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
	
	QDir dir("../sampledata");
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Time);

	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QTreeWidgetItem *newItem = new QTreeWidgetItem();
		newItem->setText(0, fileInfo.fileName());
		newItem->setText(1, QString::number(fileInfo.size()));
		newItem->setText(2, fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
		newItem->setText(3, fileInfo.absoluteFilePath());
		ui.treeWidget->addTopLevelItem(newItem);
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：TurnToModeling2
// 描述：根据菜单栏选项切换主窗口
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToModeling2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);

	QDir dir("../sampledata");
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Time);

	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QTreeWidgetItem *newItem = new QTreeWidgetItem();
		newItem->setText(0, fileInfo.fileName());
		newItem->setText(1, QString::number(fileInfo.size()));
		newItem->setText(2, fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
		newItem->setText(3, fileInfo.absoluteFilePath());
		ui.treeWidget->addTopLevelItem(newItem);
	}
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
	_displayFlag = 0;
	this->ui.stackedWidget->setCurrentWidget(this->ui.PreCamera);
}


////////////////////////////////采集页面/////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_StartMeasurement_Pressed()
// 描述：开始采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_StartMeasurement_Pressed()
{
	_qMaterialName = this->ui.lineEdit_materialName->text();
	if (_qMaterialName == "")
	{
		QMessageBox::critical(NULL, QStringLiteral("警告"), QStringLiteral("请输入材质名称"), QMessageBox::Ok);
		return;
	}

	this->ui.lineEdit_materialName->setEnabled(false);
	this->ui.pushButton_startMeasurement->setEnabled(false);
	this->ui.lineEdit_materialName->setEnabled(false);
	this->ui.pushButton_stopMeasurement->setEnabled(true);
	this->ui.toolBox->setEnabled(false);

	//string save_measurement1 = ini->value("BRDFSystem-Configuration/save_brdfiso").toString().toStdString();
	//string save_measurement2 = ini->value("BRDFSystem-Configuration/save_brdfiniso").toString().toStdString();
	string save_measurement1 = acquisitionParameters->save_brdfiso;//ini->value("BRDFSystem-Configuration/save_brdfiso").toString().toStdString();
	string save_measurement2 = acquisitionParameters->save_brdfiniso;// ini->value("BRDFSystem-Configuration/save_brdfiniso").toString().toStdString();

	//创建以材质名称命名的文件夹
	if (_measureFlag == 1)
	{
		CreateFolds(save_measurement1, _qMaterialName.toStdString());
	}	
	if (_measureFlag == 2)
	{
		CreateFolds(save_measurement2, _qMaterialName.toStdString());
	}
	emit sendingMaterialName(_qMaterialName);

	//开启光源与样品线程
	if (!threadMeasurement->isRunning())
	{
		threadMeasurement->start();	
	}
	emit startMeasurement(_measureFlag);

	//开启相机线程
	for (int i = 0; i < CAM_NUM; i++)
	{
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();		
		}
	}
	emit startTimer(_measureFlag);

	//Sleep(500);//等待相机初始化
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_StopMeasurement_Pressed()
// 描述：停止采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_StopMeasurement_Pressed()
{

}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：停止采集
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_SampleReset_Pressed()
{
	this->workerMeasurement->sampleComm->Reset();
	this->workerMeasurement->slideComm->MoveToX1();
}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
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
// 函数：DisplayMeasureState
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayMeasureState(int cameraID, int sampleID, int illuminantID)
{
	char str[1024];
	sprintf(str, "第(%d / 36)个样品角度，第(%d / 196)个光源", sampleID, illuminantID);
	//QString str_ch = QString::fromUtf8(str);//qlabel显示中文
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	QString str_ch = codec->toUnicode(str);
	this->ui.label_measureState->setText(str_ch);
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
	_displayFlag = 0;
	
	for (int i = 0; i < CAM_NUM; i++)
	{
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
		}
	}
	emit startTimer(_measureFlag);

	this->ui.pushButton_captureContinuously->setEnabled(true);
	this->ui.pushButton_chess->setEnabled(true);
	this->ui.pushButton_deadPixels->setEnabled(true);
	this->ui.pushButton_blackLevel->setEnabled(true);
	this->ui.pushButton_finiCCD->setEnabled(true);
	this->ui.pushButton_captureOfPeriod->setEnabled(true);
	this->ui.pushButton_computeMask->setEnabled(true);
	this->ui.pushButton_masked->setEnabled(true);
	this->ui.pushButton_whiteBalance->setEnabled(true);
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
		//string capturePath = ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();
		string capturePath = acquisitionParameters->save_calibration; //ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();

		char cameraPath[16];
		sprintf(cameraPath, "camera%d", i);
		CreateFolds(capturePath, cameraPath);
		this->workerCCD[i]->_capture = 1;
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
void MainWindow::PushButton_Masked_Pressed()
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		//string capturePath = acquisitionParameters->save_calibration; //ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString();
		//char cameraPath[16];
		//sprintf(cameraPath, "camera%d", i);
		//CreateFolds(capturePath, cameraPath);
		this->workerCCD[i]->_isMasked = 1;
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：PushButton_ComputeMask_Pressed
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_ComputeMask_Pressed()
{
	map<string, Mat> imgs = ImageProcess::ReadImages("..//imgs_measuremasks");
	ImageProcess::ComputeMask(imgs);
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
void MainWindow::PushButton_CaptureOfPeriod_Pressed()
{
	_measureFlag = 3;

	//开启相机线程
	for (int i = 0; i < CAM_NUM; i++)
	{
		if (threadCCD[i]->isRunning())
		{
			//threadCCD[i]->start();
			workerCCD[i]->_measureFlag = _measureFlag;
		}
	}
	//emit startTimer(_measureFlag);

	//开启光源与样品线程
	if (!threadMeasurement->isRunning())
	{
		threadMeasurement->start();
	}
	emit startMeasurement(_measureFlag);

}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Chess_Pressed()
{

}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_WhiteBalance_Pressed()
{
	vector<Mat> mats;
	Mat mat;
	for (int i = 0; i < CAM_NUM; i++)
	{
		//string path = _capturePath + "camera" + to_string(i);
		//mats = AVTCamera::ReadImages(path);
		//mat = AVTCamera::AverageImage(mats);
		//_trans = ImageProcess::ComputeWhiteTrans(mats);
	}
	_transs.push_back(_trans);
	//需要写在配置文件里
	//...
}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_DeadPixel_Pressed()
{

}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_BlackLevel_Pressed()
{

}
////////////////////////////////////////////////////////////////////////////
// 函数：
// 描述：
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_FiniCCD_Pressed()
{

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
	//这里这么多if可以有化成map
	if (_displayFlag == 0)
	{
		if (workerID == 0)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera0, img);
			//_mutex.unlock();
		}
		if (workerID == 1)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera1, img);
			//_mutex.unlock();
		}
		if (workerID == 2)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera2, img);
			//_mutex.unlock();
		}
		if (workerID == 3)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera3, img);
			//_mutex.unlock();
		}
		if (workerID == 4)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera4, img);
			//_mutex.unlock();
		}
		if (workerID == 5)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera5, img);
			//_mutex.unlock();
		}
		if (workerID == 6)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera6, img);
			//_mutex.unlock();
		}
		if (workerID == 7)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera7, img);
			//_mutex.unlock();
		}
		if (workerID == 8)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_precamera8, img);
			//_mutex.unlock();
		}
	}
	else if (_displayFlag == 1)
	{
		if (workerID == 0)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera0, img);
			//_mutex.unlock();
		}
		if (workerID == 1)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera1, img);
			//_mutex.unlock();
		}
		if (workerID == 2)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera2, img);
			//_mutex.unlock();
		}
		if (workerID == 3)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera3, img);
			//_mutex.unlock();
		}
		if (workerID == 4)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera4, img);
			//_mutex.unlock();
		}
		if (workerID == 5)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera5, img);
			//_mutex.unlock();
		}
		if (workerID == 6)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera6, img);
			//_mutex.unlock();
		}
		if (workerID == 7)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera7, img);
			//_mutex.unlock();
		}
		if (workerID == 8)
		{
			//_mutex.lock();
			ShowImgOnQLabel(this->ui.label_camera8, img);
			//_mutex.unlock();
		}
	}
	connect(this->workerCCD[workerID], SIGNAL(sendingImg(int, QImage)), this, SLOT(DisplayImage(int, QImage)), Qt::UniqueConnection);
}

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
void MainWindow::CreateFolds(string root, string fileName)
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
	if (!img.isNull())
	{
		QPixmap pic = QPixmap::fromImage(img);
		pic = pic.scaled(qlabel->width(), qlabel->height());
		qlabel->setPixmap(pic);
		qlabel->setScaledContents(true);
	}
}