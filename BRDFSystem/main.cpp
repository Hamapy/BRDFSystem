#include "mainwindow.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
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


//#include "opencv2/imgproc.hpp"
//#include "opencv2/highgui.hpp"
////#include "opencv2/ml.hpp"
////#include "opencv2/objdetect.hpp"
//
//int main(int argc, char *argv[])
//{
//
//
//	std::vector<cv::String> filenames; // notice here that we are using the Opencv's embedded "String" class
//	cv::String folder = "F:/test/*.bmp"; //"/home/rui"; // again we are using the Opencv's embedded "String" class
//
//	cv::glob(folder, filenames); // new function that does the job ;-)
//
//	for (size_t i = 0; i < filenames.size(); ++i)
//	{
//		//std::cout << filenames[i] << std::endl;
//		cv::Mat src = cv::imread(filenames[i]);
//
//		//if (!src.data)
//		//std::cerr << "Problem loading image!!!" << std::endl;
//
//		cv::imshow("temp", src);
//		cv::waitKey(0);
//		/* do whatever you want with your images here */
//	}
//
//
//}

