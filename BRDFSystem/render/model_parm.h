#pragma once

#include "stdafx.h"
#include "define.h"
#include "math.h"
#include "HDR.h"
#include <atlimage.h>

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

int As_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int Bp_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int Ct_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int He_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int Lf_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int Wd_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
int Wddu_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID);
double dot_vector(double* v1, double* v2);
double cross_length(double* v1, double* v2);
double erfc(double x);
double dich(double Ki, double Kr, double p1);
double Lam(double theta, double p0, double p1);
double Factor_D(double p0, double lambda, double g, double vxy);