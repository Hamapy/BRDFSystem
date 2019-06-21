#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <qsettings.h>
#include <qmainwindow.h>
#include <qmessagebox.h>

#include "ui_configuration.h"


class Configuration : public QMainWindow
{
	Q_OBJECT

public:
	Configuration(QWidget *parent = 0);
	virtual ~Configuration();
	friend class MainWindow;

private:
	Ui::ConfigurationClass ui;

	//…Ë÷√≤Œ ˝
	QSettings *ini = NULL;	
	int aff = 0;
	QString aff_Str = "";
	int framerate = 0;
	QString framerate_Str = "";
	int acqFrequency = 0;
	QString acqFrequency_Str = "";
	QString savepath = "";

private slots:
	void Save();
	void Defaults();
	void IsEdited();
};

#endif