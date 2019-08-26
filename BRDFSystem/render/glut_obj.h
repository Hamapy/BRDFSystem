#pragma once
#include "define.h" 
#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9


/*************************************************************************** 
  OBJ Loading 
 ***************************************************************************/
 
class Model_OBJ
{
  public: 
	Model_OBJ();
	int Model_OBJ::Load(char *filename);	// Loads the model
	void Model_OBJ::Draw();		
    float* Model_OBJ::calculateNormal(float* coord1,float* coord2,float* coord3 );
	float* Model_OBJ::normalize(float v1, float v2, float v3);
						// Draws the model on the screen
	void Model_OBJ::Release();				// Release the model
	void Model_OBJ::calculateSize();
 
	float* Faces_Triangles;					// Stores the triangles
	float* Vertex_Buffer;					// Stores the points which make the object
	float* Vertex_Normals;
	float* Faces_Normals;							// Stores the normals
    int* Triangles;
	
	
	int TotalConnectedPoints;				// Stores the total number of connected verteces
	int TotalConnectedTriangles;			// Stores the total number of connected triangle
	int TotalFaces;

	ObjSize size;
 
};
 
 
