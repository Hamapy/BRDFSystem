#include "stdafx.h"
#include "function.h"
#include "brdf.h"




float dot_mirror(float* normal, Vector3* coord)
{
	float cosine_term;
	Vector3 H;
	H.x = coord->x;
	H.y = coord->y;
	H.z = coord->z + 1.0;

	float len = H.x*H.x + H.y*H.y + H.z*H.z;
	H.x = H.x / sqrt(len);
	H.y = H.y / sqrt(len);
	H.z = H.z / sqrt(len);

	cosine_term = dot(normal, &H);
	return cosine_term;

}

float dot_v(Vector3* v1, Vector3* v2)
{
	float cosine_term;
	cosine_term = v1->x*v2->x + v1->y+v2->y + v1->z+v2->z;
	return cosine_term;
}

float dot_f(float* v1, float* v2)
{
	float cosine_term;
	cosine_term = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	return cosine_term;
}

float Get_alpha(int i, int l, Vector3* coord, float sigma)
{
	float cosine_term, alpha;
	l = l - 180;
	Vector3 H;
	H.x = coord->x + sin(float(i)/180.0*PI)*cos(float(l)/180.0*PI);
	H.y = coord->y + sin(float(i)/180.0*PI)*sin(float(l)/180.0*PI);
	H.z = coord->z + cos(float(i)/180.0*PI);

	float len = H.x*H.x + H.y*H.y + H.z*H.z;
	H.x = H.x / sqrt(len);
	H.y = H.y / sqrt(len);
	H.z = H.z / sqrt(len);

	cosine_term = H.z;

	alpha = exp(-(pow(acos(cosine_term)/sigma, 2))) / (4*sigma*sigma*cos(float(i)/180.0*3.1415926));

	return alpha;
}



void find_brdf_angle(float* n, Vector3* coord, float *out, float *in)
{
	out[0] = 0.0;
	out[1] = 0.0;
	out[2] = 1.0;
	in[0] = coord->x;
	in[1] = coord->y;
	in[2] = coord->z;

	float temp[3], theta_normal, fi_normal, bi_normal[3] = {1.0, 0.0, 0.0}, normal[3] = {0.0, 1.0, 0.0 };
	

	if (n[2] == 0.0)
	{
		theta_normal = asin(n[1]);
		fi_normal    = 0.5 * M_PI;
	}
	else
	{
		theta_normal = asin(n[1]);
		fi_normal    = atan2(n[0], n[2]);
	}

	rotate_vector(out, normal, -fi_normal, temp);
	rotate_vector(temp, bi_normal, theta_normal, out);

	temp[0] = 0.0;
	temp[1] = 0.0;
	temp[2] = 0.0;

	rotate_vector(in, normal, -fi_normal, temp);
	rotate_vector(temp, bi_normal, theta_normal, in);
}

void cross(Vector3* out, Vector3* v1, Vector3* v2)
{
	out->x = v1->y*v2->z - v1->z*v2->y;
	out->y = v1->z*v2->x - v1->x*v2->z;
	out->z = v1->x*v2->y - v1->y*v2->x;
}

float max_num(float a, float b)
{
	if (a < b)
		return b;
	else 
		return a;
}


float GetH(int theta, int phi, Vector3* l)
{
	Vector3 view, H;
	phi = phi - 180;
	view.z = cos(float(theta)*PI/180.0);
	view.y = sin(float(theta)*PI/180.0)*sin(float(phi)*PI/180.0);
	view.x = sin(float(theta*PI/180.0))*cos(float(phi)*PI/180.0);
	
//	light = Rotate_light(l, normal);
//  view  = Rotate_light(&view, normal);
	
	H.x = view.x + l->x;
	H.y = view.y + l->y;
	H.z = view.z + l->z;

	float scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);

	H.x /= scale;
	H.y /= scale;
	H.z /= scale;

	//float cosine_term = H.x*normal[0] + H.y*normal[1] + H.z*normal[2];
	
	//if (cosine_term < 0.0)
	//	cosine_term = 0.0;
	return H.z;
}

void ProjectBrdf_Origin_Batch(Scene* scene, HDR_Image light, CString file_fold, CString save_fold, CString env_name, int nCnt)
{
	float a ,b, c, x, y;
	double red, green, blue;
	Vector3 direction;
	int i, j, k, l, cnt, bands, ind;
	Color temp;
	double theta_in, theta_out, fi_in, fi_out;
	float out[3], in[3];
	Color* coeffs;
	double* brdf;

	CString filename, savename, s_tmp;
	ofstream output;

	//申请存储渲染结果的空间
	coeffs = (Color*)malloc(scene->number_of_vertices*sizeof(Color));

	bands = light.width / 3;  //得到每个面的宽度

	for(cnt = 1; cnt <= nCnt; cnt++)    
	{
		
		
		s_tmp.Format(_T("%d"), cnt);

		if (cnt < 10)
			s_tmp = "00" + s_tmp;
		else if (cnt < 100)
			s_tmp = "0" + s_tmp;
		else if (cnt = 100)
			s_tmp =s_tmp;
		
		filename = file_fold + s_tmp + "\\brdf.binary";
		savename = save_fold + env_name + "_" + s_tmp + "_origin.binary";
		printf("%s\n", s_tmp);

		// read brdf
		if (!read_brdf(filename, brdf)) 
		{
			fprintf(stderr, "Error reading %s\n", filename);
			system("pause");
			exit(1);
		}
		//printf("%d\n",bands);
		for (l = 0; l < scene->number_of_vertices; l++)
		{
			//printf("%d\n", l);
			coeffs[l].r = 0.0;
			coeffs[l].g = 0.0;
			coeffs[l].b = 0.0;


			for (i = 0; i < 6; i++)
			{
				for(j = 0; j < bands; j++)
				{
					for (k = 0; k < bands; k++)
					{

						//printf("%d**%d**%d**%d\n",l,i,j,k);
						y = (float(j)+0.5) / bands;
						x = (float(k)+0.5) / bands;
						
						//2*u-1和2*v-1是为了把取值范围从[0, 1]变为[-1, 1]
						a = 1.0 / sqrt(1.0 + (2*x-1)*(2*x-1) + (2*y-1)*(2*y-1));
						b = a*(2*x-1);
						c = a*(2*y-1);
						
						//根据面的朝向，来最终确定光线方向
						//具体说明参见移动硬盘中的说明文档《三维模型渲染教程》
						switch (i)
						{
						case 0:
							direction.x = -b;
							direction.y =  a;
							direction.z =  c;
							ind = j*light.width + bands + k;  //计算该点在HDR文件里面的位置
							break;
						case 1:
							direction.x =  a;
							direction.y = -c;
							direction.z =  b;
							ind = (j+bands)*light.width + k;
							break;
						case 2:
							direction.x = -b;
							direction.y = -c;
							direction.z =  a;
							ind = (j+bands)*light.width + bands + k;
							break;
						case 3:
							direction.x = -a;
							direction.y = -c;
							direction.z = -b;
							ind = (j+bands)*light.width + bands*2 + k;
							break;
						case 4:
							direction.x = -b;
							direction.y = -a;
							direction.z = -c;
							ind = (j+bands*2)*light.width + bands + k;
							break;
						case 5:
							direction.x = -b;
							direction.y =  c;
							direction.z = -a;
							ind = (j+bands*3)*light.width + bands + k;
							break;
						}
					
						float cosine_term = dot(&scene->normals[l*3+0], &direction);
						if (cosine_term >= 0.0)
						{
							find_brdf_angle(&scene->normals[l*3+0], &direction, out, in);
							theta_in = acos(in[2]);
							fi_in = atan2(in[1], in[0]);
							theta_out = acos(out[2]);
							fi_out = atan2(out[1], out[0]);
							fi_in = fabs(fi_in - fi_out);
							fi_out = 0.0;
							lookup_brdf_val(brdf, theta_in, fi_in, theta_out, fi_out, red, green, blue);

							
							temp.r = red;// * cosine_term;
							temp.g = green; //* cosine_term;
							temp.b = blue; //* cosine_term;
						}
						else
						{
							temp.r = 0.0;
							temp.g = 0.0;
							temp.b = 0.0;
						}

						coeffs[l].r += temp.r * light.image[ind*3 + 0];
						coeffs[l].g += temp.g * light.image[ind*3 + 1];
						coeffs[l].b += temp.b * light.image[ind*3 + 2];

					}
				}
			}
		}
		
		output.open(savename, ios::out | ios::binary);
		output.write((char*)&coeffs[0], scene->number_of_vertices*sizeof(Color));
		output.close();
		free(brdf);
	}

	free(coeffs);
}



void ProjectBrdf_Param_Batch(Scene* scene, HDR_Image light, CString file_fold, CString save_fold, CString sEnvName, int nCnt)
{
	Color* coeffs;
	CString filename, savename, sMaterialID, sModelID;
	ofstream output;
	int flag;

	//申请存储渲染结果的空间
	coeffs = (Color*)malloc(scene->number_of_vertices*sizeof(Color));


	//分材质、分模型渲染
	for (int material_id=1; material_id<=nCnt; material_id++)
	{
		sMaterialID.Format(_T("%d"), material_id);
		if (material_id < 10)
			sMaterialID = "00" + sMaterialID;
		else if (material_id < 100)
			sMaterialID = "0" + sMaterialID;
		else if (material_id = 100)
			sMaterialID =sMaterialID;

		for (int model_index=6; model_index<=6; model_index++)
		{	
			sModelID.Format(_T("%d"), model_index);
			////////////修改3/////////////
			extern CString _materialName;
			int pos = _materialName.ReverseFind('.');
			CString onlyMaterialName;
			if (pos > 0)
				onlyMaterialName = _materialName.Left(pos);

			wcout << sEnvName.GetString() << "_" << onlyMaterialName.GetString() << endl;
			//wcout << sEnvName.GetString() << "_" << sMaterialID.GetString() << "_" << sModelID.GetString() << endl;
			/////////////////////////////////////


			//He模型渲染比较慢，我测试的时候把它跳过了
			//如果后面需要跑He模型渲染结果，则注释掉下面的语句
			if (model_index == 3)
				continue;

			switch(model_index)
			{
			case 0:
				flag = As_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 1:
				flag = Bp_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 2:
				flag = Ct_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 3:
				flag = He_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 4:
				flag = Lf_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 5:
				flag = Wd_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			case 6:
				flag = Wddu_model(coeffs, light, scene, file_fold, sMaterialID);
				break;
			default:
				printf("Please enter a right model ID!\n");
				flag = 0;
				break;
			}

	
			//保存结果
			////////////修改4/////////////
			savename = save_fold + sEnvName + "_" + onlyMaterialName + ".binary";
			//savename = save_fold + sEnvName + "_" + sMaterialID + "_" + sModelID + "_test2tone.binary";
			/////////////////////////////////
			output.open(savename, ios::out | ios::binary);
			if (!output)
			{
				printf("Save fold does not exist!\n");
				return;
			}
			output.write((char*)&coeffs[0], sizeof(Color)*scene->number_of_vertices);
			output.close();
			output.clear();
		}
	}

}