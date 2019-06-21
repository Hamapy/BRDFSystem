#include "configuration.h"

Configuration::Configuration(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//窗体格式设置
	this->setFixedSize(580, 355);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
	//this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setWindowModality(Qt::WindowModal);
	this->ui.pushButton_defaults->setEnabled(false);
	this->ui.pushButton_save->setEnabled(false);

	//配置文件
	ini = new QSettings("./SWIR.ini", QSettings::IniFormat);
	
	//显示配置文件的参数
	//仿射变换系数
	aff = this->ini->value("SWIR-Configuration/aff").toInt();
	aff_Str = QString::number(aff, 10);
	this->ui.lineEdit_affine->setText(aff_Str);

	//视频帧率
	framerate = this->ini->value("SWIR-Configuration/framerate").toInt();
	framerate_Str = QString::number(framerate, 10);
	this->ui.lineEdit_framerate->setText(framerate_Str);

	//采集频率
	acqFrequency = this->ini->value("SWIR-Configuration/acqFrequency").toInt();
	acqFrequency_Str = QString::number(acqFrequency, 10);
	this->ui.lineEdit_acqf->setText(acqFrequency_Str);

	//采集图像保存路径
	savepath = this->ini->value("SWIR-Configuration/savepath").toString();
	this->ui.lineEdit_savepath->setText(savepath);

	connect(this->ui.pushButton_save, SIGNAL(pressed()), this, SLOT(Save()));
	connect(this->ui.pushButton_defaults, SIGNAL(pressed()), this, SLOT(Defaults()));
	connect(this->ui.lineEdit_affine, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_affine, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_framerate, SIGNAL(textChanged(QString)), this, SLOT(IsEdited()));
	connect(this->ui.lineEdit_framerate, SIGNAL(textEdited(QString)), this, SLOT(IsEdited()));
}

Configuration::~Configuration()
{
}

////////////////////////////////////////////////////////////////////////////
// 函数：isEdited()
// 描述：文本框参数有改动时，按钮控件可点击
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::IsEdited()
{
	this->ui.pushButton_defaults->setEnabled(true);
	this->ui.pushButton_save->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////
// 函数：save()
// 描述：保存
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::Save()
{
	aff = this->ui.lineEdit_affine->text().toInt();
	this->ini->setValue("/SWIR-Configuration/aff", aff);

	framerate = this->ui.lineEdit_framerate->text().toInt();
	this->ini->setValue("/SWIR-Configuration/framerate", aff);
	
	//其他参数待加...

	QMessageBox::information(NULL, "Save", "Saved Successfully.", QMessageBox::Ok, QMessageBox::Ok);
}

////////////////////////////////////////////////////////////////////////////
// 函数：defaults()
// 描述：恢复默认值
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::Defaults()
{
	this->ui.lineEdit_affine->setText(aff_Str);
	this->ui.lineEdit_framerate->setText(framerate_Str);

	//其他参数待加...
}

