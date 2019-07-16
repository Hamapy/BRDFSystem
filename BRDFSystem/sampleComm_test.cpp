#include "sampleComm.h"


void mainA()
{
	SampleComm t;
	t.Init(2);
	Sleep(1000);//等10s让样品台归位
	//for (int i = 0; i < 36; i++)
	//{
	//	t.GotoNextPos(175);
	//	Sleep(2000);
	//}
	for (int i = 0; i < 18; i++)
	{
		t.GotoNextPos(3500);
		Sleep(1000);
	}
	
}


