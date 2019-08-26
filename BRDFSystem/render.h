/*
#pragma once
#ifndef RENDER_H
#define RENDER_H


#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <atlimage.h>
#include <math.h>
#include <GL/gl.h>//这个头文件有冲突？
#include <GL/glu.h>
#include "glut.h"
#include "render/function.h"
#include "render/glut_ply.h"
#include "render/glut_obj.h"
#include "render/rotate.h"
#include "render/gltools.h"
#include "render/math3d.h"
#include "time.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define FLAG    0
#define M_PI	3.1415926535897932384626433832795

string materialName;
CString _materialName;

///////////////rendering////////////////
static float dwcof = -20.0;
// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat xScene = 0.0f;
static GLfloat yScene = 0.0f;
static GLuint toTextures;

Model_PLY model;
//Model_OBJ model;
Scene s;
Color* coeffs;
Color* light;
double *scale;
int material_count;
int n = 100;
int bands = 128;
int ccount = 0;

CString savename, env_name, model_name, file_fold, save_fold, _materialName, onlyMaterialName;
string materialName;


class Render
{
public:
	Render();
	virtual ~Render();

	void rendering(int argc, char* argv[]);

private:
	//生成渲染中间参数
	void RenderParameters();
	//
	//void grab();
	//
	//void dw_skybox();
	//
	//void init();
	//
	//void display(void);
	//
	//void reshape(int w, int h);
	//
	//void specialkey(int key, int x, int y);
	//
	//void keyboard(unsigned char key, int x, int y);


};
#endif
*/