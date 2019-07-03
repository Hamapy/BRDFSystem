#include "mainwindow.h"

////////////////////////////////�����涨��////////////////////////////
//VimbaSystem& _system = VimbaSystem::GetInstance();

MainWindow::MainWindow(VimbaSystem&	system, QWidget *parent) : 
_system(system),
QMainWindow(parent)
{
	ui.setupUi(this);
	
	//���漰������ʼ��
	AVTCamera::IniVimba(_system);
	this->setAttribute(Qt::WA_DeleteOnClose, true);//�رմ���ʱ����ڴ�
	TurnToMeasurement1();
	//this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
	ini = new QSettings("./config.ini", QSettings::IniFormat);//��ȡ�����ļ�



	////�������������
	int nMin = 0;
	int nMax = 200;
	int nSingleStep = 10;


	QSlider *pSlider1 = this->ui.horizontalSlider_gain;
	pSlider1->setMinimum(nMin);  // ��Сֵ
	pSlider1->setMaximum(nMax);  // ���ֵ
	pSlider1->setSingleStep(nSingleStep);  // ����

	QSlider *pSlider2 = this->ui.horizontalSlider_darkLevel;
	pSlider2->setMinimum(nMin);  // ��Сֵ
	pSlider2->setMaximum(nMax);  // ���ֵ
	pSlider2->setSingleStep(nSingleStep);  // ����



	////��ʾ�����ļ��Ĳ���
	////��ҵ������ò���
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


	////��Դ���ò���
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


	////��Ʒ̨����������ò���
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
	

	////�������������ò���
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

	
	

	
	
	////�����źŲ�,����ͻָ�Ĭ������
	connect(this->ui.pushButton_save, SIGNAL(pressed()), this, SLOT(PushButton_Save_Pressed()));
	connect(this->ui.pushButton_defaults, SIGNAL(pressed()), this, SLOT(PushButton_Defaults_Pressed()));
	////�����źŲۣ��໥�ı䣩
	connect(this->ui.lineEdit_gain, SIGNAL(valueChanged(int)), this->ui.horizontalSlider_gain, SLOT(setValue(int)));
	connect(this->ui.horizontalSlider_gain, SIGNAL(valueChanged(int)), this->ui.lineEdit_gain, SLOT(setValue(int)));
	connect(this->ui.lineEdit_gain, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_gain, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	////�����źŲۣ��໥�ı䣩
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




	////ҳ���л�
	connect(this->ui.pushButton_measure1, SIGNAL(pressed()), this, SLOT(TurnToMeasurement1()));
	connect(this->ui.pushButton_measure2, SIGNAL(pressed()), this, SLOT(TurnToMeasurement2()));
	connect(this->ui.pushButton_measure3, SIGNAL(pressed()), this, SLOT(TurnToMeasurement3()));
	connect(this->ui.pushButton_brdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling1()));
	connect(this->ui.pushButton_svbrdfModeling, SIGNAL(pressed()), this, SLOT(TurnToModeling2()));
	connect(this->ui.pushButton_settings, SIGNAL(pressed()), this, SLOT(TurnToSettings()));
	connect(this->ui.pushButton_test, SIGNAL(pressed()), this, SLOT(TurnToTest()));
	connect(this->ui.pushButton_preCamera, SIGNAL(pressed()), this, SLOT(TurnToPreCamera()));

	////�ɼ�ҳ��
	connect(this->ui.pushButton_startMeasurement, SIGNAL(pressed()), this, SLOT(PushButton_StartMeasurement_Pressed()));
	connect(this->ui.pushButton_stopMeasurement, SIGNAL(pressed()), this, SLOT(StopMeasurement()));

	//for (int i = 0; i < CAM_NUM; i++)
	//{
	//	workerMeasurement[i] = new WorkerMeasurement(i, _system);
	//	threadMeasurement[i] = new QThread();
	//	workerMeasurement[i]->moveToThread(threadMeasurement[i]);
	//}

	////���Ԥ����ҳ��
	this->ui.pushButton_captureContinuously->setEnabled(false);
	this->ui.pushButton_finiCCD->setEnabled(false);
	connect(this->ui.pushButton_iniCCD, SIGNAL(pressed()), this, SLOT(PushButton_IniCCD_Pressed()));
	connect(this->ui.pushButton_captureContinuously, SIGNAL(pressed()), this, SLOT(PushButton_CaptureContinuously_Pressed()));
	
	//*workerCCD = new WorkerCCD(workerID, this->_system)[CAM_NUM];
	//*threadCCD = new QThread[CAM_NUM];
	for (int i = 0; i < CAM_NUM; i++)
	{
		workerCCD[i] = new WorkerCCD(i, _system);
		threadCCD[i] = new QThread();
		workerCCD[i]->moveToThread(threadCCD[i]);
		//if (threadCCD[i]->isRunning())
		//{
		//	threadCCD[i]->terminate();
		//}
	}
	connect(workerCCD[0], SIGNAL(sendingImg(int, QImage)), this, SLOT(DisplayImage(int, QImage)), Qt::QueuedConnection);
	connect(workerCCD[1], SIGNAL(sendingImg(int, QImage)), this, SLOT(DisplayImage(int, QImage)), Qt::QueuedConnection);
	//connect(workerCCD[2], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage2(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[3], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage3(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[4], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage4(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[5], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage5(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[6], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage6(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[7], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage7(QImage)), Qt::QueuedConnection);
	//connect(workerCCD[8], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage8(QImage)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		if (threadCCD[i]->isFinished())
			return;
		threadCCD[i]->quit();
		threadCCD[i]->wait();

		delete workerCCD[i]->_cameraAVT;
		workerCCD[i]->_cameraAVT = NULL;

		delete workerCCD[i];
		workerCCD[i] = NULL;

		delete threadCCD[i];
		threadCCD[i] = NULL;
	}
	//delete[] workerCCD; //�������岻��ȷ��workerID
	//delete[] threadCCD;

	AVTCamera::FiniVimba(_system);
}

////////////////////////////////˽�вۺ���/////////////////////////////////////
////////////////////////////////����ҳ��/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������isEdited()
// �������ı�������иĶ�ʱ����ť�ؼ��ɵ��
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::IsEdited()
{
	this->ui.pushButton_defaults->setEnabled(true);
	this->ui.pushButton_save->setEnabled(true);
}
////////////////////////////////////////////////////////////////////////////
// ������PushButton_Save_Pressed
// ���������ݲ˵���ѡ���л�������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Save_Pressed()
{
	////��ҵ������ñ���
	gain = this->ui.lineEdit_gain->text().toInt();
	this->ini->setValue("/SWIR-Configuration/gain", gain);
	darkLevel = this->ui.lineEdit_darkLevel->text().toInt();
	this->ini->setValue("/SWIR-Configuration/darkLevel", darkLevel);
	imageSaveFormat = this->ui.comboBox_imageSaveFormat->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/imageSaveFormat", imageSaveFormat);
	imageSavePath = this->ui.lineEdit_imageSavePath->text().toString();
	this->ini->setValue("/SWIR-Configuration/imageSavePath", imageSavePath);

	////��Դ���ñ���
	serialPortSelection = this->ui.comboBox_serialPortSelection->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/serialPortSelection", serialPortSelection);
	baudRate = this->ui.comboBox_baudRate->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/baudRate", baudRate);
	delaySetting = this->ui.lineEdit_delaySetting->text().toInt();
	this->ini->setValue("/SWIR-Configuration/delaySetting", delaySetting);
	lightingSequence = this->ui.lineEdit_lightingSequence->text().toInt();
	this->ini->setValue("/SWIR-Configuration/lightingSequence", lightingSequence);

	////��Ʒ̨����������ñ���
	stepperMotorPortSelection = this->ui.comboBox_stepperMotorPortSelection->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/stepperMotorPortSelection", stepperMotorPortSelection);
	stepperMotorSpeed = this->ui.comboBox_stepperMotorSpeed->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/stepperMotorSpeed", stepperMotorSpeed);
	stepperMotorAcceleration = this->ui.comboBox_stepperMotorAcceleration->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/stepperMotorAcceleration", stepperMotorAcceleration);
	stepperMotorDeceleration = this->ui.comboBox_stepperMotorDeceleration->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/stepperMotorDeceleration", stepperMotorDeceleration);
	stepperMotorResolution = this->ui.comboBox_stepperMotorResolution->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/stepperMotorResolution", stepperMotorResolution);
	sampleRotationAngle = this->ui.comboBox_sampleRotationAngle->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/sampleRotationAngle", sampleRotationAngle);

	////�������������ñ���
	servoMotorPortSelection = this->ui.comboBox_servoMotorPortSelection->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/servoMotorPortSelection", servoMotorPortSelection);
	servoMotorSpeed = this->ui.comboBox_servoMotorSpeed->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/servoMotorSpeed", servoMotorSpeed);
	servoMotorAcceleration = this->ui.comboBox_servoMotorAcceleration->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/servoMotorAcceleration", servoMotorAcceleration);
	servoMotorDeceleration = this->ui.comboBox_servoMotorDeceleration->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/servoMotorDeceleration", servoMotorDeceleration);
	servoMotorResolution = this->ui.comboBox_servoMotorResolution->currentText().toString();
	this->ini->setValue("/SWIR-Configuration/servoMotorResolution", servoMotorResolution);
	slideTableMovingDistance = this->ui.lineEdit_slideTableMovingDistance->text().toInt();
	this->ini->setValue("/SWIR-Configuration/slideTableMovingDistance", slideTableMovingDistance);





	QMessageBox::information(NULL, "Save", "Saved Successfully.", QMessageBox::Ok, QMessageBox::Ok);
}
////////////////////////////////////////////////////////////////////////////
// ������PushButton_Save_Pressed
// ���������ݲ˵���ѡ���л�������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_Defaults_Pressed()
{
	////��ҵ���Ĭ������
	this->ui.lineEdit_gain->setText(gain);
	this->ui.lineEdit_darkLevel->setText(darkLevel);
	int index = ui->comboBox_imageSaveFormat->findText(imageSaveFormat);
	ui->comboBox_imageSaveFormat->setCurrentIndex(index);
	this->ui.lineEdit_imageSavePath->setText(imageSavePath);

	////��ԴĬ������
	int index = ui->comboBox_serialPortSelection->findText(serialPortSelection);
	ui->comboBox_serialPortSelection->setCurrentIndex(index);
	int index = ui->comboBox_baudRate->findText(baudRate);
	ui->comboBox_baudRate->setCurrentIndex(index);
	this->ui.lineEdit_delaySetting->setText(delaySetting);
	this->ui.lineEdit_lightingSequence->setText(lightingSequencel);

	////��Ʒ̨�������Ĭ������
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


	////����������Ĭ������
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
}

////////////////////////////////�л�ҳ��/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������TurnToMeasurement1
// ���������ݲ˵���ѡ���л�������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement1()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);	
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToMeasurement2
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToMeasurement3
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement3()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToModeling1
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToModeling1()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToModeling2
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToModeling2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToSettings
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToSettings()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Settings);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToTest
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToTest()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Test);
}
////////////////////////////////////////////////////////////////////////////
// ������TurnToPreCamera
// ���������ݲ˵���ѡ���л�������
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToPreCamera()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.PreCamera);
}


////////////////////////////////�ɼ�ҳ��/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������StartMeasurement
// ��������ʼ�ɼ�
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_StartMeasurement_Pressed()
{
	_displayFlag = 1;
	_qMaterialName = this->ui.lineEdit_materialName->text();
	if (_qMaterialName == "")
	{
		QMessageBox::critical(NULL, QStringLiteral("����"), QStringLiteral("�������������"), QMessageBox::Ok);
		return;
	}
	this->ui.lineEdit_materialName->setEnabled(false);
	
	//slideComm->SlideMoveIn();
	//Sleep(10000);//�ȴ������λ
	CreateFolds("..\\imgs_measurement");

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
			connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
			emit startTimer();
			this->workerCCD[i]->_measurement = 1;
		}
		//if (!threadMeasurement[i]->isRunning())
		//{
		//	threadMeasurement[i]->start();
		//	connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
		//	emit startTimer();
		//	this->workerCCD[i]->_measurement = 1;
		//}
	}

}
////////////////////////////////////////////////////////////////////////////
// ������StopMeasurement
// ������ֹͣ�ɼ�
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::StopMeasurement()
{

}
////////////////////////////////////////////////////////////////////////////
// ������ChangeWindows
// ���������ݲ˵���ѡ���л�������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::ConnectRGB()
{
	//workerCCD[0]->cameraRGB_->Init();
	//workerCCD[0]->cameraRGB_->OpenCamera(0);
	//workerCCD[0]->cameraRGB_->SetCameraSettings(0, 40000, 0.00, 0.00);//ͼ���ʽ�����⣬5.9�ֲȿ�
	//if (!threadRGB[0]->isRunning())
	//{
	//	threadRGB[0]->start();
	//}
}

/////////////////////////////���Ԥ����ҳ��/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������PushButton_iniCCD_pressed
// ������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_IniCCD_Pressed()
{
	ui.pushButton_captureContinuously->setEnabled(true);
	ui.pushButton_finiCCD->setEnabled(true);

	_displayFlag = 0;
	CreateFolds("..\\imgs_calibration");
	
	for (int i = 0; i < CAM_NUM; i++)
	{
		//workerCCD[i]->cameraRGB_->SetCameraSettings(0, 40000, 0.00, 0.00);//ͼ���ʽ�����⣬5.9�ֲȿ�
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
			connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
			emit startTimer();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// ������PushButton_captureContinuously_pressed
// ������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::PushButton_CaptureContinuously_Pressed()
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		this->workerCCD[i]->_capture = 1;
	}
}

/////////////////////////////�ۺ����Ĺ��ú���/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������DisplayImage0
// ������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
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
//// ������DisplayImage1
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
//// ������DisplayImage2
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
//// ������DisplayImage3
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
//// ������DisplayImage4
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
//// ������DisplayImage5
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
//// ������DisplayImage6
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
//// ������DisplayImage7
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
//// ������DisplayImage8
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

////////////////////////////////////˽�к���////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������CreateFolds
// ��������ָ��Ŀ¼�´��������Ӧ�ļ���
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateFolds(string root)
{
	for (int i = 0; i < CAM_NUM; i++)
	{
		///////Unicode�ַ�������/////////
		WCHAR wszStr[256];
		memset(wszStr, 0, sizeof(wszStr));
		MultiByteToWideChar(CP_ACP, 0, root.c_str(), strlen(root.c_str()) + 1, wszStr,
			sizeof(wszStr) / sizeof(wszStr[0]));

		if (GetFileAttributes(wszStr) & FILE_ATTRIBUTE_DIRECTORY) //�ж�·�����ļ�����Ŀ¼
		{
			string newFolderPath = root + "\\camera" + to_string(i);
			///////Unicode�ַ�������/////////
			WCHAR wszNewStr[256];
			memset(wszNewStr, 0, sizeof(wszNewStr));
			MultiByteToWideChar(CP_ACP, 0, newFolderPath.c_str(), strlen(newFolderPath.c_str()) + 1, wszNewStr,
				sizeof(wszNewStr) / sizeof(wszNewStr[0]));

			if (!CreateDirectory(wszNewStr, NULL))
			{
				cout << "�ļ����Ѵ��ڣ�" << endl;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// ������ShowImgOnQLabel
// ��������ָ��Qlabel����ʾ�ɼ�ͼ��
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
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