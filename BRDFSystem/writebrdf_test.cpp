#include "workerMeasurement.h"

int mainq()
{
	WorkerMeasurement t;
	string brdfPath = "..\\imgs_measurement1\\greenpaint\\";
	t.WriteBRDF(brdfPath, "..\\sampledata\\greenpaint.binary");

	return 0;
}