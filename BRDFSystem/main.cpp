#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	//CCriticalSection critical_section;
	QApplication a(argc, argv);
	MainWindow *w = new MainWindow();
	w->show();
	return a.exec();
}


