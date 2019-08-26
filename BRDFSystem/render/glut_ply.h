#pragma once
#include "define.h"

class Model_PLY 
{
public:
	Model_PLY();
	int Model_PLY::Load(char *filename);
	void Model_PLY::Draw();
	float* Model_PLY::calculateNormal( float *coord1, float *coord2, float *coord3 );	
	float* Model_PLY::normalize(float v1, float v2, float v3);
 
    float* Faces_Triangles;
    float* Faces_Quads;
	float* Vertex_Buffer;
	float* Vertex_Normals;
	float* Faces_Normals;
	int*   Triangles;
 
	int TotalConnectedTriangles;	
	int TotalConnectedQuads;	
	int TotalConnectedPoints;
	int TotalFaces;

	PlySize size;
 
 
};