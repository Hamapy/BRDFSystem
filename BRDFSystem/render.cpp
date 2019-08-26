/*
#include "render.h"

Render::Render()
{
}

Render::~Render()
{
}

void Render::rendering(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("PRT_Rendering");

	env_name = "grace";
	materialName = "blueprint.txt";
	_materialName = materialName.c_str();
	int pos = _materialName.ReverseFind('.');
	if (pos > 0)
		onlyMaterialName = _materialName.Left(pos);
	//model_name = "blueprint.txt";
	file_fold = "..\\..\\Data\\result_tone\\";
	save_fold = "..\\..\\Data\\Picture\\Environment\\";
	savename = save_fold + "001\\" + env_name + "_" + onlyMaterialName + ".bmp";
	//savename = save_fold + "001\\" + env_name + "_" + model_name + "test.bmp";
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//	glutMouseFunc(mymouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);
	glutMainLoop();

	free(coeffs);
	free(light);
	return;
}

void RenderParameters()
{
	int nCnt = 1;    //先注释为第一个binary文件，注意修改回去
	////////////修改2/////////////
	extern string materialName;
	materialName = "blueprint.txt";
	extern CString _materialName;
	_materialName = materialName.c_str();
	CString file_fold = "..\\..\\..\\Matlab\\Param\\001\\";
	//CString file_fold = "..\\..\\..\\Matlab\\Model_Fitting\\Param\\";
	CString save_fold = "..\\..\\Data\\result_tone\\";
	///////////////////////////////////////////////////

	//读取环境光数据
	CString env_name = "grace";
	HDR_Image light;
	bool xx = light.Load("..\\..\\Data\\Environments\\grace128_cross.hdr");
	if (!xx)
	{
		printf("Open Environment File Error!\n");
		system("pause");
		return;
	}

	if (light.width / 3 != light.height / 4)
	{
		printf("The resolution of the Environment File is Wrong!\n");
		system("pause");
		return;
	}
	printf("%d\n", light.width / 3);
	//读取3D球模型
	Model_PLY model;
	if (!model.Load("..\\..\\Data\\ball_70.ply"))
	{
		printf("Read 3D model Error!\n");
		system("pause");
		return;
	}

	Scene s;
	s.vertices = model.Vertex_Buffer;
	s.normals = model.Vertex_Normals;
	s.triangles = model.Triangles;
	s.face_normals = model.Faces_Normals;
	s.number_of_vertices = model.TotalConnectedPoints;
	s.number_of_triangles = model.TotalFaces;
	printf("%d**%d**%d**%d\n", s.vertices, s.triangles, s.number_of_vertices, s.number_of_triangles);
	if (FLAG == 1)
	{
		ProjectBrdf_Origin_Batch(&s, light, file_fold, save_fold, env_name, nCnt);
	}
	else
	{
		ProjectBrdf_Param_Batch(&s, light, file_fold, save_fold, env_name, nCnt);
	}
}

void grab(char *filename)
{

	GLubyte* pPixelData;
	int ColorTableSize = 0;//24位位图没有颜色表
	int LineByte = (500 * 3 + 3) / 4 * 4;
	GLint PixelDataLength = LineByte * 500;

	FILE *fp = fopen(filename, "wb");
	if (fp == 0)
	{
		cout << "Save Image Error!" << endl;
		system("pause");
		exit(1);
	}

	//文件信息头
	BITMAPFILEHEADER FileHeader;

	FileHeader.bfType = 0x4D42; //bmp格式标志
	FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorTableSize + PixelDataLength;//整个文件长度
	FileHeader.bfReserved1 = 0;
	FileHeader.bfReserved2 = 0;
	FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorTableSize;

	//文图信息头
	BITMAPINFOHEADER InfoHeader;

	InfoHeader.biWidth = 500;
	InfoHeader.biHeight = 500;
	InfoHeader.biPlanes = 1;
	InfoHeader.biSizeImage = PixelDataLength;
	InfoHeader.biSize = 40;
	InfoHeader.biBitCount = 24;
	InfoHeader.biCompression = BI_RGB;
	InfoHeader.biXPelsPerMeter = 0;
	InfoHeader.biYPelsPerMeter = 0;
	InfoHeader.biClrUsed = 0;
	InfoHeader.biClrImportant = 0;


	//分配内存 打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);

	if (pPixelData == 0)
		exit(0);


	//从GL中读取图像读取像素
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(0, 0, 500, 500, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pPixelData, PixelDataLength, 1, fp);
	//


	//释放内存和文件
	fclose(fp);
	free(pPixelData);

}

void dw_skybox()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, toTextures);

	glPushMatrix();
	glRotatef(xScene, 0.0f, 0.0f, 1.0f);
	glRotatef(yScene, 0.0f, 1.0f, 0.0f);

	float scale = 1.5;

	glBegin(GL_QUADS);
	//正面
	glTexCoord2f(2.0 / 3.0, 0.75);
	glVertex3f(-scale, scale, scale);

	glTexCoord2f(1.0 / 3.0, 0.75);
	glVertex3f(scale, scale, scale);

	glTexCoord2f(1.0 / 3.0, 0.50);
	glVertex3f(scale, scale, scale);

	glTexCoord2f(2.0 / 3.0, 0.50);
	glVertex3f(-scale, -scale, scale);

	//背面
	glTexCoord2f(2.0 / 3.0, 0.0);
	glVertex3f(-scale, scale, -scale);

	glTexCoord2f(2.0 / 3.0, 0.25);
	glVertex3f(-scale, -scale, -scale);

	glTexCoord2f(1.0 / 3.0, 0.25);
	glVertex3f(scale, -scale, -scale);

	glTexCoord2f(1.0 / 3.0, 0.0);
	glVertex3f(scale, scale, -scale);

	//左面
	glTexCoord2f(0.0, 0.75);
	glVertex3f(-scale, scale, scale);

	glTexCoord2f(0.0, 0.5);
	glVertex3f(-scale, -scale, scale);

	glTexCoord2f(1.0 / 3.0, 0.5);
	glVertex3f(-scale, -scale, -scale);

	glTexCoord2f(1.0 / 3.0, 0.75);
	glVertex3f(-scale, scale, -scale);

	//右面
	glTexCoord2f(1.0, 0.75);
	glVertex3f(scale, scale, scale);

	glTexCoord2f(2.0 / 3.0, 0.75);
	glVertex3f(scale, scale, -scale);

	glTexCoord2f(2.0 / 3.0, 0.5);
	glVertex3f(scale, -scale, -scale);

	glTexCoord2f(1.0, 0.5);
	glVertex3f(scale, -scale, scale);

	//顶面
	glTexCoord2f(1.0 / 3.0, 1.0);
	glVertex3f(-scale, scale, scale);

	glTexCoord2f(1.0 / 3.0, 0.75);
	glVertex3f(-scale, scale, -scale);

	glTexCoord2f(2.0 / 3.0, 0.75);
	glVertex3f(scale, scale, -scale);

	glTexCoord2f(2.0 / 3.0, 1.0);
	glVertex3f(scale, scale, scale);

	//底面
	glTexCoord2f(1.0 / 3.0, 0.25);
	glVertex3f(-scale, -scale, scale);

	glTexCoord2f(2.0 / 3.0, 0.25);
	glVertex3f(scale, -scale, scale);

	glTexCoord2f(2.0 / 3.0, 0.5);
	glVertex3f(scale, -scale, -scale);

	glTexCoord2f(1.0 / 3.0, 0.5);
	glVertex3f(-scale, -scale, -scale);

	glEnd();

	glPopMatrix();


	glDisable(GL_TEXTURE_2D);
}

void init()
{
	CString filename;
	//读取ply文件
	model.Load("..\\..\\Data\\ball_70.ply");

	s.vertices = model.Vertex_Buffer;
	s.normals = model.Vertex_Normals;
	s.triangles = model.Triangles;
	s.number_of_vertices = model.TotalConnectedPoints;
	s.number_of_triangles = model.TotalFaces;
	s.albedo.r = 1.0;
	s.albedo.g = 1.0;
	s.albedo.b = 1.0;

	//读取模型系数文件
	coeffs = (Color*)malloc(s.number_of_vertices*sizeof(Color));

	////////////////修改2/////////////////
	filename = file_fold + env_name + "_" + onlyMaterialName + ".binary";
	//filename = file_fold + env_name + "_001_" + model_name + "_test2tone.binary";
	///////////////////////////////////////
	ifstream in1(filename, ios::in | ios::binary);

	if (!in1)
	{
		printf("Model coefficient file does not exist!\n");
		system("pause");
		return;
	}

	for (int index = 0; index < s.number_of_vertices; index++)
	{
		in1.read((char*)&coeffs[index], sizeof(Color));
	}
	in1.close();

	//读取缩放系数
	scale = (double*)malloc(sizeof(double) * 100);
	filename = file_fold + "ScaleFile_" + env_name + ".binary";
	ifstream in2(filename, ios::in | ios::binary);

	if (!in2)
	{
		printf("Scale coefficient file does not exist!\n");
		system("pause");
		return;
	}

	for (int index = 0; index < 100; index++)
	{
		in2.read((char*)&scale[index], sizeof(double));
	}
	in2.close();
	material_count = 0;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	//加载纹理
	GLbyte *pBytes;
	GLint iwidth, iheight, icomp;
	GLenum eformat;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	pBytes = gltLoadTGA("..\\..\\Data\\cube\\grace_cube.tga", &iwidth, &iheight, &icomp, &eformat);
	glTexImage2D(GL_TEXTURE_2D, 0, icomp, iwidth, iheight, 0, eformat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 1.5, 0, 0, -5, 0, 1, 0);
	float v0[3], v1[3], v2[3];
	//float scale = 40;
	float bias = log(1.25) / log(0.5);
	float L_max = log(100000.0) / log(10.0);
	dw_skybox();

	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	for (int l = 0; l < s.number_of_triangles; l++)
	{
		int* t = &s.triangles[l * 3];
		v0[0] = s.vertices[t[0] * 3 + 0] * 2;
		v0[1] = s.vertices[t[0] * 3 + 1] * 2;
		v0[2] = s.vertices[t[0] * 3 + 2] * 2;
		v1[0] = s.vertices[t[1] * 3 + 0] * 2;
		v1[1] = s.vertices[t[1] * 3 + 1] * 2;
		v1[2] = s.vertices[t[1] * 3 + 2] * 2;
		v2[0] = s.vertices[t[2] * 3 + 0] * 2;
		v2[1] = s.vertices[t[2] * 3 + 1] * 2;
		v2[2] = s.vertices[t[2] * 3 + 2] * 2;
		Color c0 = { 0.0f, 0.0f, 0.0f };
		Color c1 = { 0.0f, 0.0f, 0.0f };
		Color c2 = { 0.0f, 0.0f, 0.0f };


		//c0.r = float(pow(coeffs[t[0]].r, (1.0/3.0)) / 50);//scale[material_count]);
		//c0.g = float(pow(coeffs[t[0]].g, (1.0/3.0)) / 50);//scale[material_count]);
		//c0.b = float(pow(coeffs[t[0]].b, (1.0/3.0)) / 50);//scale[material_count]);

		//c1.r = float(pow(coeffs[t[1]].r, (1.0/3.0)) / 50);//scale[material_count]);
		//c1.g = float(pow(coeffs[t[1]].g, (1.0/3.0)) / 50);//scale[material_count]);
		//c1.b = float(pow(coeffs[t[1]].b, (1.0/3.0)) / 50);//scale[material_count]);

		//c2.r = float(pow(coeffs[t[2]].r, (1.0/3.0)) / 50);//scale[material_count]);
		//c2.g = float(pow(coeffs[t[2]].g, (1.0/3.0)) / 50);//scale[material_count]);
		//c2.b = float(pow(coeffs[t[2]].b, (1.0/3.0)) / 50);//scale[material_count]);


		c0.r = float(coeffs[t[0]].r / scale[material_count]);
		c0.g = float(coeffs[t[0]].g / scale[material_count]);
		c0.b = float(coeffs[t[0]].b / scale[material_count]);

		c1.r = float(coeffs[t[1]].r / scale[material_count]);
		c1.g = float(coeffs[t[1]].g / scale[material_count]);
		c1.b = float(coeffs[t[1]].b / scale[material_count]);

		c2.r = float(coeffs[t[2]].r / scale[material_count]);
		c2.g = float(coeffs[t[2]].g / scale[material_count]);
		c2.b = float(coeffs[t[2]].b / scale[material_count]);

		glColor3f(c0.r, c0.g, c0.b);
		glVertex3f(v0[0], v0[1], v0[2]);
		glColor3f(c1.r, c1.g, c1.b);
		glVertex3f(v1[0], v1[1], v1[2]);
		glColor3f(c2.r, c2.g, c2.b);
		glVertex3f(v2[0], v2[1], v2[2]);
	}
	glEnd();

	glPopMatrix();

	glColor3f(1, 1, 1);

	dw_skybox();

	//	glutSwapBuffers();	//Buffer swap

	glFlush();
}

void reshape(int w, int h)
{
	GLfloat fAspect;
	if (h == 0)h = 1;
	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;

	//Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Produce the perspective projection
	//gluPerspective(dwcof,fAspect,1.0,40.0);
	if (w <= h)
		gluPerspective(180.0 / M_PI * (atan(0.1*dwcof) + 0.5 * M_PI), (GLsizei)w / (GLsizei)h, 1.0, 40.0);
	else
		gluPerspective(180.0 / M_PI * (atan(0.1*dwcof) + 0.5 * M_PI), (GLsizei)w / (GLsizei)h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void specialkey(int key, int x, int y)
{
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == '+')
	{
		dwcof -= 1;
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(180.0 / M_PI * (atan(0.1*dwcof) + 0.5 * M_PI), 1.0, 1.0, 40.0);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	if (key == '-')
	{
		dwcof += 1;
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(180.0 / M_PI * (atan(0.1*dwcof) + 0.5 * M_PI), 1.0, 1.0, 40.0);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	if (key == 'g')
	{
		char *name;
		name = (LPSTR)(LPCTSTR)savename;
		grab(name);
	}

	if (key == 'n')
	{
		int i;
		ifstream in1;
		CString filename, s_tmp;
		char *name;
		//int count = 0;           //注意是0还是1
		for (i = 1; i <= 1; i++)
		{
			name = (LPSTR)(LPCTSTR)savename;
			grab(name);
			//读取模型系数文件
			ccount = ccount + 1;
			s_tmp.Format(_T("%d"), ccount);
			if (ccount < 10)
				s_tmp = "00" + s_tmp;
			else if (ccount < 100)
				s_tmp = "0" + s_tmp;
			else if (ccount = 100)
				s_tmp = s_tmp;

			////////////////修改3/////////////////
			filename = file_fold + env_name + "_" + onlyMaterialName + ".binary";
			//filename = file_fold + env_name + "_" + s_tmp + "_" + model_name + "_test2tone.binary";
			//////////////////////////////////////
			wcout << filename.GetString() << endl;
			material_count = material_count + 1;

			in1.open(filename, ios::in | ios::binary);

			if (!in1)
			{
				printf("Model coefficient file does not exist!\n");
				system("pause");
				return;
			}

			for (int index = 0; index < s.number_of_vertices; index++)
			{
				in1.read((char*)&coeffs[index], sizeof(Color));
			}
			in1.close();
			////////////////修改4/////////////////
			savename = save_fold + s_tmp + "\\" + env_name + "_" + onlyMaterialName + ".bmp";
			//savename = save_fold + s_tmp + "\\" + env_name + "_" + model_name + "test.bmp";
			//////////////////////////////////////
			glutPostRedisplay();
		}
	}

	//	glutPostRedisplay();

}
*/