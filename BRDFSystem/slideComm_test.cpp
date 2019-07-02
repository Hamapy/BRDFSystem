#include "slideComm.h"


void main4()
{
	SlideComm t;
	t.Init(13, SERVO_VELOCITY, SERVO_ACCELERATE, SERVO_DECELERATE, SERVO_RESOLUTION);
	t.MoveToX2();
	//Sleep(30000);
	//t.servo_move_sensor_home_cw(13);//น้ฮป
}



