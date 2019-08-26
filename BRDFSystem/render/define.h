#pragma once
#define M_PI	3.1415926535897932384626433832795
struct Vector3
{
	double x;
	double y;
	double z;
};

struct Spherical
{
	float theta;
	float phi;
};

struct Sample
{
	Spherical spherical_coord;
	Vector3 cartesian_coord;
	float* sh_functions;
};

struct Sampler
{
	Sample* samples;
	int number_of_samples;
};

struct Color
{
	double r;
	double g;
	double b;
};

typedef struct {
	float left;
	float right;
	float up;
	float down;
	float front; //z minus
	float back;  //z plus
	float centerx;
	float centery;
	float centerz;
	float width;
	float hight;
}PlySize;


typedef struct {
	float left;
	float right;
	float up;
	float down;
	float front; //z minus
	float back;  //z plus
	float centerx;
	float centery;
	float centerz;
	float width;
	float hight;
}ObjSize;


struct Scene
{
	float* vertices;
	float* normals;
	float* face_normals;
	int* triangles;
	Color albedo;
	int number_of_vertices;
	int number_of_triangles;
	//PlySize *size;
	ObjSize *size;
};

struct MeshIndex
{
	int id;
	MeshIndex *next;
};

struct KDTNode
{
	struct KDTNode *left;
	struct KDTNode *right;
	int axis;
	float splitplane;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	MeshIndex index;
};

struct StackElem
{
	KDTNode *node;
	float t;
	float pb[3];
	int prev;
};
