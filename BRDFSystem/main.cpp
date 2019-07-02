#include "mainwindow.h"
#include <QtWidgets/QApplication>


int main1(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VimbaSystem& system = VimbaSystem::GetInstance();//相机的SDK-Vimba系统
													 //为了避免每个相机线程重复开启Vimba系统，只能暂时吧该引用提到最上层
													 //程序打开的同时打开相机SDK
	//MainWindow w;
	MainWindow *w = new MainWindow(system);
	w->show();
	return a.exec();
}


