/* Demonstrates how to load PLY files
 * Needs some refactoring.
 *
 * Model needs to be triangulated
 * Use blender
 *
 * Just the class for loading PLY files. 
 * 
 */ 
#include "stdafx.h"
#include "glut_ply.h"
 
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>

  
 
Model_PLY::Model_PLY()
{
	this->TotalConnectedTriangles = 0; 
	this->TotalConnectedQuads = 0;
	this->TotalConnectedPoints = 0;
	//init by impossibel value
	this->size.left = 10000.0f;
	this->size.right= -100000.0f;
	this->size.up   = -100000.0f;
	this->size.down = 100000.0f;
	this->size.front= -100000.0f;
	this->size.back = 100000.0f;
	this->size.centerx = this->size.centery = this->size.centerz = 0;
 
}
 
 
float* Model_PLY::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord2[0] - coord3[0];
   vb[1] = coord2[1] - coord3[1];
   vb[2] = coord2[2] - coord3[2];
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	float norm[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}

float* Model_PLY::normalize(float v1, float v2, float v3)
{
	float norm[3], val;
	
	val = sqrt(v1*v1 + v2*v2 + v3*v3);
	norm[0] = v1/val;
	norm[1] = v2/val;
	norm[2] = v3/val;

	return norm;
}

 
 
int Model_PLY::Load(char* filename)
{
    char* pch = strstr(filename,".ply");
 
    if (pch != NULL)
    {
	   FILE* file = fopen(filename,"r");
 
		fseek(file,0,SEEK_END);
		long fileSize = ftell(file);
 
		try
		{
		Vertex_Buffer = (float*) malloc (ftell(file));
		}
		catch (char* )
		{
			return -1;
		}
		if (Vertex_Buffer == NULL) return -1;
		fseek(file,0,SEEK_SET); 
 
	   Faces_Triangles = (float*) malloc(fileSize*sizeof(float));
	   Faces_Normals  = (float*) malloc(fileSize*sizeof(float));
 
       if (file)
       {
			int i = 0;   
			int temp = 0;
			int quads_index = 0;
            int triangle_index = 0;
			int normal_index = 0;
			char buffer[1000];
 
 
			fgets(buffer,300,file);			// ply
 
 
			// READ HEADER
			// -----------------
 
			// Find number of vertexes
			while (  strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
			strcpy(buffer, buffer+strlen("element vertex"));
			sscanf(buffer,"%i", &this->TotalConnectedPoints);
 
 
			// Find number of vertexes
			fseek(file,0,SEEK_SET);
			while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
			strcpy(buffer, buffer+strlen("element face"));
			sscanf(buffer,"%i", &this->TotalFaces);
 
 
			// go to end_header
			while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
 
			//----------------------
 
			Vertex_Normals = (float*) malloc (3*TotalConnectedPoints*sizeof(float));
			Triangles = (int*) malloc (3*TotalFaces*sizeof(int));
 
			// read verteces
			i = 0;
			for (int iterator = 0; iterator < this->TotalConnectedPoints; iterator++)
			{
				fgets(buffer,300,file);
 
				sscanf(buffer,"%f %f %f", &Vertex_Buffer[i], &Vertex_Buffer[i+1], &Vertex_Buffer[i+2]);
				float tx = Vertex_Buffer[i] ;
				if (tx<size.left)
					size.left = tx;
				if (tx>size.right)
					size.right = tx;
				float ty = Vertex_Buffer[i+1];
				if (ty<size.down)
					size.down = ty;
				if (ty>size.up)
					size.up = ty;
				float tz = Vertex_Buffer[i+2];
				if (tz<size.back)
					size.back = tz;
				if (tz>size.front)
					size.front = tz;
				i += 3;
			}

			size.centerx = (size.left+size.right)/2;
			size.centery = (size.up +size.down)/2;
			size.centerz = (size.front + size.back)/2;

			size.width   = abs(size.right-size.left);
			size.hight   = abs(size.up - size.down);


			//move the model center to orgin
			i = 0;
			for (int iterator = 0; iterator < this->TotalConnectedPoints; iterator++)
			{
				float* norm;
				Vertex_Buffer[i+0] -= size.centerx;
				Vertex_Buffer[i+1] -= size.centery;
				Vertex_Buffer[i+2] -= size.centerz;
				norm = normalize(Vertex_Buffer[i+0], Vertex_Buffer[i+1], Vertex_Buffer[i+2]);
				Vertex_Normals[i+0] = 0;
				Vertex_Normals[i+1] = 0;
				Vertex_Normals[i+2] = 0;
				i += 3;
			}


 
			// read faces
			i =0;
			for (int iterator = 0; iterator < this->TotalFaces; iterator++)
			{
				fgets(buffer,300,file);
 
				if (buffer[0] == '3')
				{

					int vertex1 = 0, vertex2 = 0, vertex3 = 0;
					//sscanf(buffer,"%i%i%i\n", vertex1,vertex2,vertex3 );
					buffer[0] = ' ';
					sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );
					Triangles[iterator*3+0] = vertex1;
					Triangles[iterator*3+1] = vertex2;
					Triangles[iterator*3+2] = vertex3;
					
					/*vertex1 -= 1;
					vertex2 -= 1;
					vertex3 -= 1;
*/
					//  vertex == punt van vertex lijst
					// vertex_buffer -> xyz xyz xyz xyz
					//printf("%f %f %f ", Vertex_Buffer[3*vertex1], Vertex_Buffer[3*vertex1+1], Vertex_Buffer[3*vertex1+2]);

					Faces_Triangles[triangle_index+0] = Vertex_Buffer[3*vertex1];
					Faces_Triangles[triangle_index+1] = Vertex_Buffer[3*vertex1+1];
					Faces_Triangles[triangle_index+2] = Vertex_Buffer[3*vertex1+2];
					Faces_Triangles[triangle_index+3] = Vertex_Buffer[3*vertex2];
					Faces_Triangles[triangle_index+4] = Vertex_Buffer[3*vertex2+1];
					Faces_Triangles[triangle_index+5] = Vertex_Buffer[3*vertex2+2];
					Faces_Triangles[triangle_index+6] = Vertex_Buffer[3*vertex3];
					Faces_Triangles[triangle_index+7] = Vertex_Buffer[3*vertex3+1];
					Faces_Triangles[triangle_index+8] = Vertex_Buffer[3*vertex3+2];

					float coord1[3] = {Faces_Triangles[triangle_index+0],Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
					float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
					float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
					float *norm = this->calculateNormal( coord1, coord2, coord3 );

					Faces_Normals[normal_index+0] = norm[0];
					Faces_Normals[normal_index+1] = norm[1];
					Faces_Normals[normal_index+2] = norm[2];
					Faces_Normals[normal_index+3] = norm[0];
					Faces_Normals[normal_index+4] = norm[1];
					Faces_Normals[normal_index+5] = norm[2];
					Faces_Normals[normal_index+6] = norm[0];
					Faces_Normals[normal_index+7] = norm[1];
					Faces_Normals[normal_index+8] = norm[2];

					Vertex_Normals[3*vertex1+0] += norm[0];
					Vertex_Normals[3*vertex1+1] += norm[1];
					Vertex_Normals[3*vertex1+2] += norm[2];
					Vertex_Normals[3*vertex2+0] += norm[0];
					Vertex_Normals[3*vertex2+1] += norm[1];
					Vertex_Normals[3*vertex2+2] += norm[2];
					Vertex_Normals[3*vertex3+0] += norm[0];
					Vertex_Normals[3*vertex3+1] += norm[1];
					Vertex_Normals[3*vertex3+2] += norm[2];

					normal_index += 9;

					triangle_index += 9;
					TotalConnectedTriangles += 3;
				}
				i += 3;
			}
 
			for (int i = 0; i < this->TotalConnectedPoints; i++)
			{
				float* norm;
				if(Vertex_Normals[i*3+0] == 0.0)
					continue;
				norm = normalize(Vertex_Normals[i*3+0], Vertex_Normals[i*3+1], Vertex_Normals[i*3+2]);
				Vertex_Normals[i*3+0] = norm[0];
				Vertex_Normals[i*3+1] = norm[1];
				Vertex_Normals[i*3+2] = norm[2];
			}
			fclose(file);
		}
 
      else { printf("File can't be opened\n"); }
    } 
	else 
	{
      printf("File does not have a .PLY extension. ");    
    }   

	return 0;
}
 
void Model_PLY::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);	
 	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);	
	glNormalPointer(GL_FLOAT, 0, Faces_Normals);
	glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);	
	glDisableClientState(GL_VERTEX_ARRAY);    
	glDisableClientState(GL_NORMAL_ARRAY);
}