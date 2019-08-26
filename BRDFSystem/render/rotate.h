#include <math.h>
#include "define.h"
void RotateSHCoefficients(int cnt, int numBands, Color* light, float * unrotatedCoeffs, float * rotatedCoeffs,
							float theta, float phi, float gamma);

//Get a matrix for rotating one band of coefficients about the Z axis
void GetZRotationMatrix(int band, float * entries, float angle);

//Get a matrix for rotating one band of coefficients about the X axis, by 90 degrees
void GetX90DegreeRotationMatrix(int band, float * entries);

//Apply a matrix to a vector of coefficients
void ApplyMatrix(	int size, float * matrix, bool transpose,
					float * inVector, float * outVector);
