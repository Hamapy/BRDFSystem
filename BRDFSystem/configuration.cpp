#include "configuration.h"

Configuration::Configuration(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//�����ʽ����
	this->setFixedSize(580, 355);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
	//this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setWindowModality(Qt::WindowModal);
	this->ui.pushButton_defaults->setEnabled(false);
	this->ui.pushButton_save->setEnabled(false);

	//�����ļ�
	ini = new QSettings("./SWIR.ini", QSettings::IniFormat);
	
	//��ʾ�����ļ��Ĳ���
	//����任ϵ��
	aff = this->ini->value("SWIR-Configuration/aff").toInt();
	aff_Str = QString::number(aff, 10);
	this->ui.lineEdit_affine->setText(aff_Str);

	//��Ƶ֡��
	framerate = this->ini->value("SWIR-Configuration/framerate").toInt();
	framerate_Str = QString::number(framerate, 10);
	this->ui.lineEdit_framerate->setText(framerate_Str);

	//�ɼ�Ƶ��
	acqFrequency = this->ini->value("SWIR-Configuration/acqFrequency").toInt();
	acqFrequency_Str = QString::number(acqFrequency, 10);
	this->ui.lineEdit_acqf->setText(acqFrequency_Str);

	//�ɼ�ͼ�񱣴�·��
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
// ������isEdited()
// �������ı�������иĶ�ʱ����ť�ؼ��ɵ��
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::IsEdited()
{
	this->ui.pushButton_defaults->setEnabled(true);
	this->ui.pushButton_save->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////
// ������save()
// ����������
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::Save()
{
	aff = this->ui.lineEdit_affine->text().toInt();
	this->ini->setValue("/SWIR-Configuration/aff", aff);

	framerate = this->ui.lineEdit_framerate->text().toInt();
	this->ini->setValue("/SWIR-Configuration/framerate", aff);
	
	//������������...

	QMessageBox::information(NULL, "Save", "Saved Successfully.", QMessageBox::Ok, QMessageBox::Ok);
}

////////////////////////////////////////////////////////////////////////////
// ������defaults()
// �������ָ�Ĭ��ֵ
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Configuration::Defaults()
{
	this->ui.lineEdit_affine->setText(aff_Str);
	this->ui.lineEdit_framerate->setText(framerate_Str);

	//������������...
}

