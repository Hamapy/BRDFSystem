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

	////相机预处理页面
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
	connect(workerCCD[0], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage0(QImage)), Qt::QueuedConnection);
	connect(workerCCD[1], SIGNAL(sendingImg(QImage)), this, SLOT(DisplayImage1(QImage)), Qt::QueuedConnection);
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
	//delete[] workerCCD; //这样定义不好确定workerID
	//delete[] threadCCD;

	AVTCamera::FiniVimba(_system);
}

////////////////////////////////切换页面/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 函数：ChangeWindows
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
// 函数：ChangeWindows
// 描述：根据菜单栏选项切换主窗口
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::TurnToMeasurement2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
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
void MainWindow::TurnToMeasurement3()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Measurement);
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
void MainWindow::TurnToModeling1()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
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
void MainWindow::TurnToModeling2()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Modeling);
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
void MainWindow::TurnToSettings()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Settings);
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
void MainWindow::TurnToTest()
{
	this->ui.stackedWidget->setCurrentWidget(this->ui.Test);
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
	
	//slideComm->SlideMoveIn();
	//Sleep(1000);//等待滑轨就位

	for (int i = 0; i < CAM_NUM; i++)
	{
		if (!threadCCD[i]->isRunning())
		{
			threadCCD[i]->start();
			connect(this, SIGNAL(startTimer()), workerCCD[i], SLOT(StartTimer()));
			emit startTimer();
			this->workerCCD[i]->_measurement = 1;
		}
	}
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
	for (int i = 0; i < CAM_NUM; i++)
	{
		string defaultPath = "..\\imgs_calibration";
		///////Unicode字符集问题/////////
		WCHAR wszStr[256];
		memset(wszStr, 0, sizeof(wszStr));
		MultiByteToWideChar(CP_ACP, 0, defaultPath.c_str(), strlen(defaultPath.c_str()) + 1, wszStr,
			sizeof(wszStr) / sizeof(wszStr[0]));

		if (GetFileAttributes(wszStr) & FILE_ATTRIBUTE_DIRECTORY) //判断路径是文件还是目录
		{
			string newFolderPath = defaultPath + "\\camera" + to_string(i);
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
void MainWindow::DisplayImage0(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			//将图像由QImage格式转换为QPixmap
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera0->width(), ui.label_precamera0->height());
			this->ui.label_precamera0->setPixmap(pic);
			this->ui.label_precamera0->setScaledContents(true);//限定qlabel大小
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			//将图像由QImage格式转换为QPixmap
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera0->width(), ui.label_camera0->height());
			this->ui.label_camera0->setPixmap(pic);
			this->ui.label_camera0->setScaledContents(true);//限定qlabel大小
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage1
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage1(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera1->width(), ui.label_precamera1->height());
			this->ui.label_precamera1->setPixmap(pic);
			this->ui.label_precamera1->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera1->width(), ui.label_camera1->height());
			this->ui.label_camera1->setPixmap(pic);
			this->ui.label_camera1->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage2
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage2(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera2->width(), ui.label_precamera2->height());
			this->ui.label_precamera2->setPixmap(pic);
			this->ui.label_precamera2->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera2->width(), ui.label_camera2->height());
			this->ui.label_camera2->setPixmap(pic);
			this->ui.label_camera2->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage3
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage3(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera3->width(), ui.label_precamera3->height());
			this->ui.label_precamera3->setPixmap(pic);
			this->ui.label_precamera3->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera3->width(), ui.label_camera3->height());
			this->ui.label_camera3->setPixmap(pic);
			this->ui.label_camera3->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage4
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage4(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera4->width(), ui.label_precamera4->height());
			this->ui.label_precamera4->setPixmap(pic);
			this->ui.label_precamera4->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera4->width(), ui.label_camera4->height());
			this->ui.label_camera4->setPixmap(pic);
			this->ui.label_camera4->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage5
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage5(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera5->width(), ui.label_precamera5->height());
			this->ui.label_precamera5->setPixmap(pic);
			this->ui.label_precamera5->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera5->width(), ui.label_camera5->height());
			this->ui.label_camera5->setPixmap(pic);
			this->ui.label_camera5->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage6
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage6(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera6->width(), ui.label_precamera6->height());
			this->ui.label_precamera6->setPixmap(pic);
			this->ui.label_precamera6->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera6->width(), ui.label_camera6->height());
			this->ui.label_camera6->setPixmap(pic);
			this->ui.label_camera6->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage7
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage7(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera7->width(), ui.label_precamera7->height());
			this->ui.label_precamera7->setPixmap(pic);
			this->ui.label_precamera7->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera7->width(), ui.label_camera7->height());
			this->ui.label_camera7->setPixmap(pic);
			this->ui.label_camera7->setScaledContents(true);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// 函数：DisplayImage8
////////////////////////////////////////////////////////////////////////////
void MainWindow::DisplayImage8(QImage img)
{
	if (_displayFlag == 0)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_precamera8->width(), ui.label_precamera8->height());
			this->ui.label_precamera8->setPixmap(pic);
			this->ui.label_precamera8->setScaledContents(true);
		}
	}
	else if (_displayFlag == 1)
	{
		if (!img.isNull())
		{
			QPixmap pic = QPixmap::fromImage(img);
			pic = pic.scaled(ui.label_camera8->width(), ui.label_camera8->height());
			this->ui.label_camera8->setPixmap(pic);
			this->ui.label_camera8->setScaledContents(true);
		}
	}
}