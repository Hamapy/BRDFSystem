#include "sampleComm.h"


void mainA()
{
	SampleComm t;
	t.Init(2);
	Sleep(15000);//��10s����Ʒ̨��λ
	
	for (int i = 0; i < 36; i++)
	{
		t.GotoNextPos(1730);
		Sleep(200);
	}
	
}


