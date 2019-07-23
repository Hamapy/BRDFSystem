#include "config.h"

QSettings* ini = new QSettings("./config.ini", QSettings::IniFormat);//读取配置文件



CameraParameters* cameraParameters = new CameraParameters{
	ini->value("BRDFSystem-Configuration/acquisitionFrameCount").toInt(),
	ini->value("BRDFSystem-Configuration/acquisitionFrameRate").toFloat(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIEnable").toBool(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIHeight").toInt(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIOffsetX").toInt(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIOffsetY").toInt(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIShowArea").toBool(),
	ini->value("BRDFSystem-Configuration/autofunctionAOIWidth").toInt(),
	ini->value("BRDFSystem-Configuration/autofunctionTargetIntensity").toInt(),
	ini->value("BRDFSystem-Configuration/blackLevel").toFloat(),   
	ini->value("BRDFSystem-Configuration/colorTransformationEnable").toBool(),
	ini->value("BRDFSystem-Configuration/exposureTime").toFloat(),
	ini->value("BRDFSystem-Configuration/gain").toFloat(),   
	ini->value("BRDFSystem-Configuration/gamma").toFloat(),
	ini->value("BRDFSystem-Configuration/height").toInt(),
	ini->value("BRDFSystem-Configuration/width").toInt(),
	ini->value("BRDFSystem-Configuration/hue").toFloat(),
	ini->value("BRDFSystem-Configuration/saturation").toFloat(),
	(char*)(ini->value("BRDFSystem-Configuration/pixelFormat").toString().toLocal8Bit()).data(),
	(char*)(ini->value("BRDFSystem-Configuration/exposureAuto").toString().toLocal8Bit()).data(),
	ini->value("BRDFSystem-Configuration/acquisitionMode").toString().toStdString(),
	(char*)(ini->value("BRDFSystem-Configuration/gainAuto").toString().toLocal8Bit()).data(),

};

//CameraParameters* cameraParameters = new CameraParameters{
	//acquisitionFrameCount = //1,
	//acquisitionFrameRate =  //5.000000,
	//bool autofunctionAOIEnable =  //false,
	//int autofunctionAOIHeight =  //1234,
	//int autofunctionAOIOffsetX =  //0,
	//int autofunctionAOIOffsetY = //0,
	//bool autofunctionAOIShowArea = //false,
	//int autofunctionAOIWidth = //1624,
	//int autofunctionTargetIntensity = //50,
	//blackLevel = ini->value("BRDFSystem-Configuration/darkLevel").toFloat() //0.000000,
	//bool colorTransformationEnable = //false,
	//float exposureTime = / //30000.000000,
	//gain = ini->value("BRDFSystem-Configuration/gain").toFloat();// 0.000000,
	//gamma = ;//1.000000,
	//int height = //1234,
	//int width = //1624,
	//float hue = //0.000000,
	//float saturation = //1.000000,
	//string pixelFormat = //"RGB8Packed",//直接用彩色图像格式 一般RGB
	//string exposureAuto = //"Off",
	//string acquisitionMode = //"Continuous";
	//string gainAuto = //"Off"
//};

LightParameters* lightParameters = new LightParameters{
	ini->value("BRDFSystem-Configuration/serialPortSelection").toInt(),
	ini->value("BRDFSystem-Configuration/baudRate").toInt(),
	ini->value("BRDFSystem-Configuration/delaySetting").toInt(),
	ini->value("BRDFSystem-Configuration/lightingSequence").toInt(),
	{ 0, 1, 2, 3,    25, 26, 27, 28, 29,    50,51,52,53,       75,76,77,78,79,   100,101,102,103,      125,126,127,128,129,    150,151,152,153,      175,176,177,178,179,
	4,5,6,7,8,     30,31,32,33,           54,55,56,57,58,    80,81,82,83,      104,105,106,107,108,  130,131,132,133,        154,155,156,157,158,  180,181,182,183,
	9,10,11,12,    34,35,36,37,           59,60,61,62,       84,85,86,87,      109,110,111,112,      134,135,136,137,        159,160,161,162,      184,185,186,187,
	13,14,15,      38,39,40,41,           63,64,65,          88,89,90,91,      113,114,115,          138,139,140,141,        163,164,165,          188,189,190,191,
	16,17,18,      42,43,44,              66,67,68,          92,93,94,         116,117,118,          142,143,144,            166,167,168,          192,193,194,
	19,20,21,      45,46,                 69,70,             95,96,            119,120,121,          145,146,                169,170,              195,196,
	22,23,         47,                    71,72,             97,               122,123,              147,                    171,172,              197,
	24,            48,                    73,                                  124,                  148,                    173,                  
		49,                                       98,                                     149,                                          198 }
};


StepperMotorParameters* stepperMotorParameters = new StepperMotorParameters{
	ini->value("BRDFSystem-Configuration/stepperMotorPortSelection").toInt(),
	ini->value("BRDFSystem-Configuration/stepperMotorSpeed").toFloat(),
	ini->value("BRDFSystem-Configuration/stepperMotorAcceleration").toInt(),
	ini->value("BRDFSystem-Configuration/stepperMotorDeceleration").toInt(),
	ini->value("BRDFSystem-Configuration/stepperMotorResolution").toInt(),
	ini->value("BRDFSystem-Configuration/stepperMotorToHome").toInt(),
};

ServoMotorParameters* servoMotorParameters = new ServoMotorParameters{
	ini->value("BRDFSystem-Configuration/servoMotorPortSelection").toInt(),
	ini->value("BRDFSystem-Configuration/sampleRotationAngle").toInt(),
	ini->value("BRDFSystem-Configuration/servoMotorSpeed").toInt(),
	ini->value("BRDFSystem-Configuration/servoMotorAcceleration").toInt(),
	ini->value("BRDFSystem-Configuration/servoMotorDeceleration").toInt(),
	ini->value("BRDFSystem-Configuration/servoMotorResolution").toInt(),
	ini->value("BRDFSystem-Configuration/slideTableMovingDistance").toInt(),
};

AcquisitionParameters* acquisitionParameters = new AcquisitionParameters{
	ini->value("BRDFSystem-Configuration/save_calibration").toString().toStdString(),
	ini->value("BRDFSystem-Configuration/save_brdfiso").toString().toStdString(),
	ini->value("BRDFSystem-Configuration/save_brdfiniso").toString().toStdString(),
	ini->value("BRDFSystem-Configuration/imageSaveFormat").toString().toStdString(),
	ini->value("BRDFSystem-Configuration/imageSavePath").toString().toStdString(),
};

MainWindowParameters* mainWindowParameters = new MainWindowParameters{
	ini->value("BRDFSystem-Configuration/gain").toFloat(),
	ini->value("BRDFSystem-Configuration/blackLevel").toFloat(),
	ini->value("BRDFSystem-Configuration/imageSaveFormat").toString(),
	ini->value("BRDFSystem-Configuration/imageSavePath").toString(),

	ini->value("BRDFSystem-Configuration/serialPortSelection").toString(),
	ini->value("BRDFSystem-Configuration/baudRate").toString(),
	ini->value("BRDFSystem-Configuration/delaySetting").toInt(),
	ini->value("BRDFSystem-Configuration/lightingSequence").toInt(),

	ini->value("BRDFSystem-Configuration/stepperMotorPortSelection").toString(),
	ini->value("BRDFSystem-Configuration/stepperMotorSpeed").toString(),
	ini->value("BRDFSystem-Configuration/stepperMotorAcceleration").toString(),
	ini->value("BRDFSystem-Configuration/stepperMotorDeceleration").toString(),
	ini->value("BRDFSystem-Configuration/stepperMotorResolution").toString(),
	ini->value("BRDFSystem-Configuration/sampleRotationAngle").toString(),
	
	ini->value("BRDFSystem-Configuration/servoMotorPortSelection").toString(),
	ini->value("BRDFSystem-Configuration/servoMotorSpeed").toString(),
	ini->value("BRDFSystem-Configuration/servoMotorAcceleration").toString(),
	ini->value("BRDFSystem-Configuration/servoMotorDeceleration").toString(),
	ini->value("BRDFSystem-Configuration/servoMotorResolution").toString(),
	ini->value("BRDFSystem-Configuration/slideTableMovingDistance").toInt(),
};
