#include "mainwindow.h"
#include <QtWidgets/QApplication>


int main1(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VimbaSystem& system = VimbaSystem::GetInstance();//�����SDK-Vimbaϵͳ
													 //Ϊ�˱���ÿ������߳��ظ�����Vimbaϵͳ��ֻ����ʱ�ɸ������ᵽ���ϲ�
													 //����򿪵�ͬʱ�����SDK
	//MainWindow w;
	MainWindow *w = new MainWindow(system);
	w->show();
	return a.exec();
}


