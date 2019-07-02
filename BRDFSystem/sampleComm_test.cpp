#include "sampleComm.h"


void main()
{
	SampleComm t;
	t.Init(2, 0, 0.1, STEP_ACCELERATE, STEP_DECELERATE, STEP_RESOLUTION, STEP_TOHOME);
	//Sleep(1000);//等10s让样品台归位
	//for (int i = 0; i < 36; i++)
	//{
		t.GotoNextPos(175);
		//Sleep(3000);
	//}
	
}


