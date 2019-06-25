#include "sampleComm.h"


void main()
{
	SampleComm t;
	t.Init(2, 0, 0.2, STEP_ACCELERATE, STEP_DECELERATE, STEP_RESOLUTION, 20000);

	for (int i = 0; i < 18; i++)
	{
		t.GotoNextPos(100);//20000²½ÊÇÒ»È¦£¿
		Sleep(10000);
	}
	
}

