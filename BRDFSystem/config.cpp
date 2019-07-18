#include "config.h"

QSettings* ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件

CameraParameters* cameraParameters = new CameraParameters{
	/*acquisitionFrameCount =*/ 1,
	/*acquisitionFrameRate =*/ 5.000000,
	/*bool autofunctionAOIEnable =*/ false,
	/*int autofunctionAOIHeight =*/ 1234,
	/*int autofunctionAOIOffsetX =*/ 0,
	/*int autofunctionAOIOffsetY =*/ 0,
	/*bool autofunctionAOIShowArea =*/ false,
	/*int autofunctionAOIWidth =*/ 1624,
	/*int autofunctionTargetIntensity =*/ 50,
	/*float blackLevel =*/ 0.000000,
	/*bool colorTransformationEnable =*/ false,
	/*float exposureTime =*/ 60000.000000,
	/*float gain =*/ 0.000000,
	/*float gamma =*/ 1.000000,
	/*int height =*/ 1234,
	/*int width =*/ 1624,
	/*float hue =*/ 0.000000,
	/*float saturation =*/ 1.000000,
	/*string pixelFormat =*/ "RGB8Packed",//直接用彩色图像格式 一般RGB
	/*string exposureAuto =*/ "Off",
	//string acquisitionMode = "Continuous";
	/*string gainAuto =*/ "Off"
};
