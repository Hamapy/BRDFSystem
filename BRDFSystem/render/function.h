#pragma once

#include "stdafx.h"
//#include <afx.h>
#include "model_parm.h"
//#include "HDR.h"
#include "define.h"
#include <stdio.h>
#include <math.h>
#include <atlimage.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;


float dot(float* normal, Vector3* coord);
float dot_mirror(float* normal, Vector3* coord);
float dot_v(Vector3* v1, Vector3* v2);
float dot_f(float* v1, float* v2);
void  find_brdf_angle(float* n, Vector3* coord, float *out, float *in);
float Get_alpha(int i,int l, Vector3* coord, float sigma);
float Get_H(float* normal, Vector3* coord, float sigma);
float max_num(float a, float b);
void  cross(Vector3* out, Vector3* v1, Vector3* v2);
float GetH(int theta, int phi, Vector3* l);

void ProjectBrdf_Param_Batch(Scene* scene, HDR_Image light, CString file_fold, CString save_fold, CString env_name, int nCnt);
void ProjectBrdf_Origin_Batch(Scene* scene, HDR_Image light, CString file_fold, CString save_fold, CString env_name, int nCnt);