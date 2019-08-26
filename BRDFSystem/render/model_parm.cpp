#include "stdafx.h"
#include "model_parm.h"
#include "time.h"

int factorial(int n)
{
	if (n<=1)
		return 1;

	else
		return(n * factorial(n-1));
}

float dot(float* normal, Vector3* coord)
{
	float cosine_term;
	//scale = sqrt(coord->x*coord->x + coord->y*coord->y + coord->z*coord->z);
	cosine_term = normal[0]*coord->x + normal[1]*coord->y + normal[2]*coord->z;
	return cosine_term;
}

double dot_vector(double* v1, double* v2)
{
	double cosine_term;
	cosine_term = v1[0]*v2[0] + v1[1]*v2[1] +v1[2]*v2[2];

	return cosine_term;
}

float Get_H(float* normal, Vector3* coord, float sigma)
{
	float cosine_term, alpha;
	Vector3 H;
	H.x = coord->x;
	H.y = coord->y;
	H.z = coord->z + 1.0;

	float len = H.x*H.x + H.y*H.y + H.z*H.z;
	H.x = H.x / sqrt(len);
	H.y = H.y / sqrt(len);
	H.z = H.z / sqrt(len);

	cosine_term = H.x*normal[0] + H.y*normal[1] + H.z*normal[2];

	//alpha = (sigma+1) * pow(cosine_term, sigma) / 2;
    alpha = exp(-(pow(tan(acos(cosine_term))/sigma, 2))) / (4*sigma*sigma*normal[2]);

	return alpha;
}

void cross_product (double* v1, double* v2, double* out)
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

double cross_length(double* v1, double* v2)
{
	double out[3], scale;

	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];

	scale = sqrt(out[0]*out[0] + out[1]*out[1] + out[2]*out[2]);

	if (scale < 1e-10)
		scale = 1e-10;
	
	return scale;

}

void rotate_vector(double* vector, double* axis, double angle, double* out)
{
	double temp;
	double cross[3];
	double cos_ang = cos(angle);
	double sin_ang = sin(angle);

	out[0] = vector[0] * cos_ang;
	out[1] = vector[1] * cos_ang;
	out[2] = vector[2] * cos_ang;

	temp = axis[0]*vector[0]+axis[1]*vector[1]+axis[2]*vector[2];
	temp = temp*(1.0-cos_ang);

	out[0] += axis[0] * temp;
	out[1] += axis[1] * temp;
	out[2] += axis[2] * temp;

	cross_product (axis,vector,cross);
	
	out[0] += cross[0] * sin_ang;
	out[1] += cross[1] * sin_ang;
	out[2] += cross[2] * sin_ang;
}

void normal_vector(double* vector)
{
	double scale;
	scale = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);

	if (scale > 1e-10)
	{
		for (int i=0; i<3; i++)
			vector[i] = vector[i] / scale;
	}
}

void NormalAdjust(double* out, double* in, double* n)
{
	double temp[3], theta_normal, fi_normal, bi_normal[3] = {0.0, 1.0, 0.0}, normal1[3] = { 1.0, 0.0, 0.0 };

	temp[0] = out[0];
	temp[1] = out[1];
	temp[2] = out[2];

	theta_normal  = asin(n[0]);
	fi_normal     = atan2(n[1], n[2]);

	rotate_vector(out, normal1 , fi_normal, temp);
	rotate_vector(temp, bi_normal, -theta_normal, out);

	temp[0] = out[0];
	temp[1] = out[1];
	temp[2] = out[2];

	rotate_vector(in, normal1 , fi_normal, temp);
	rotate_vector(temp, bi_normal, -theta_normal, in);
}

double erfc(double x)
{
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);


    return (1 -sign*y);
}

double Lam(double theta, double p0, double p1)
{
	double value;
	value = 0.5 * (2/sqrt(PI)*p1/p0*tan(theta) - erfc(p0/(2*p1*tan(theta))));
	return value;
}

double dich(double Ki, double Kr, double p1)
{
	double z0, bound0, bound1, eq1, eq2;          
	bound0 = 0.0;
	bound1 = 0.001;
	z0 = bound1;
	eq1 = 4.0 * sqrt(PI / 2.0) * z0;
	eq2 = p1 * (Ki+Kr) * exp( -pow(z0,2.0) / 2.0 / pow(p1,2.0));

	if(eq1 < eq2)
	{
		while(eq1 < eq2)
		{
			bound0 = z0;
		    bound1 = bound1*5;
		    z0 = bound1;
		    eq1 = 4.0 * sqrt(PI / 2.0) * z0;
		    eq2 = p1 * (Ki+Kr) * exp( -pow(z0,2.0) / 2.0 / pow(p1,2.0));
		}

	}

	int num=0;
	while(abs(eq1-eq2)>1e-6 && num < 1000)
	{	
		if(eq1 > eq2)
		{
		    bound1 = z0;
		    z0 = (bound0 + bound1) / 2.0;
		}
		else
		{
			bound0 = z0;
		    z0 = (bound0 + bound1) / 2.0;
		 }

		 eq1 = 4 * sqrt(PI/2.0) * z0;
		 eq2 = p1 * (Ki+Kr) * exp(-pow(z0,2.0) / 2.0 / pow(p1,2.0));

		 num = num + 1;
	 }

	 if( (Ki+Kr) < 0)
	 {
		z0= -z0;
	 } 

	return z0;
}

double Factor_D(double p0, double lambda, double g, double vxy)
{
	double m, factor_m, tol, D, tmp;
	m = 0.0;
	tol = 0.0;
	factor_m = 1.0;

	do
	{
		m = m + 1.0;
		factor_m = factor_m * m;
		tmp = pow(g, m)*exp(-g)*exp(-pow(vxy*p0, 2.0)/(4*m)) / (factor_m * m);
		tol = tol + tmp;

	}while((tmp>1e-10) && (m<1000.0));
	
	D = pow(PI*p0/(2*lambda), 2) * tol;

	return D;

}

//--------------------------------------------------------------------
int As_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L, cosine_V;
    double para[9];
	double Ad[3], As[3], F0, n;
	ifstream in1;

    CString filename;
	//filename = file_fold + sMaterialID + "\\As_param.txt";
	filename = file_fold + sMaterialID + "\\AshikhminShirley_parameter.txt";
	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 8; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	F0 =  para[6];
	n = para[7];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
	for (l = 0; l < scene->number_of_vertices; l++)
	{
	//	printf("%d\n", l);
		coeffs[l].r = 0.0;
		coeffs[l].g = 0.0;
		coeffs[l].b = 0.0;

		for (i = 0; i < 6; i++)
		{
			for(j = 0; j < bands; j++)
			{
				for (k = 0; k < bands; k++)
				{
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					Vector3 H;
					H.x = direction.x ;
					H.y = direction.y ;	
					H.z = direction.z + 1;
					double scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);
					H.x = H.x / scale;
					H.y = H.y / scale;
					H.z = H.z / scale;
					
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						cosine_V = scene->normals[l*3+2];

						double cosine_alpha = dot(&scene->normals[l*3+0], &H);
						double vh = sqrt( (1 + direction.z) / 2);
						double G;
						if(cosine_L >= cosine_V)
						{
							G = 1.0 / (cosine_L * vh) / 8 / PI;
						}
						else
						{
							G = 1.0 / (cosine_V * vh) / 8 / PI;
						}


						double F = F0  + (1-F0) * pow( (1.0-vh) ,5);
						double param = (n + 1) * pow(cosine_alpha,n) * G * F;

						tempcolor.r = (Ad[0]/PI + As[0] * param) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1] * param) * cosine_L*1.15;
						tempcolor.b = (Ad[2]/PI + As[2] * param) * cosine_L*1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
						//coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
						//coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
						//coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);

					}
				}
			}
		}		
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}



	return 1;
}

int Bp_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L, cosine_V;
    double para[7];
	double Ad[3], As[3], n;
	ifstream in1;

	CString filename;
	//filename = file_fold + sMaterialID + "\\Bp_param_our.txt";
	filename = file_fold + sMaterialID + "\\BlinnPhong_parameter.txt";
	in1.open(filename, ios::in);
	
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 8; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	n = para[6];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
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
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					Vector3 H;
					H.x = direction.x ;
					H.y = direction.y ;	
					H.z = direction.z + 1;
					double scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);
					H.x = H.x / scale;
					H.y = H.y / scale;
					H.z = H.z / scale;
					
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						cosine_V = scene->normals[l*3+2];

						double cosine_a = dot(&scene->normals[l*3+0], &H);
							
						tempcolor.r = (Ad[0]/PI + As[0] * (n+2)/2/PI * pow(cosine_a,n)) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1] * (n+2)/2/PI * pow(cosine_a,n)) * cosine_L*1.15;
						tempcolor.b = (Ad[2]/PI + As[2] * (n+2)/2/PI * pow(cosine_a,n)) * cosine_L*1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}

				}
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}

	return 1;
}

int Ct_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L, cosine_V;
    double para[9];
	double Ad[3], As[3], F0, m;
	ifstream in1;

	CString filename;
    //filename = file_fold + sMaterialID + "\\Ct_param.txt";
	filename = file_fold + sMaterialID + "\\CookTorrance_parameter.txt";

	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 8; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	F0 =  para[6];
	m = para[7];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
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
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					Vector3 H;
					H.x = direction.x ;
					H.y = direction.y ;	
					H.z = direction.z + 1;
					double scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);
					H.x = H.x / scale;
					H.y = H.y / scale;
					H.z = H.z / scale;
					
					cosine_L = double(dot(&scene->normals[l*3+0], &direction));
					if ( cosine_L > 0.0 )
					{
						cosine_V = double(scene->normals[l*3+2]);
						
						double vh = H.z;//sqrt((1+direction.z)/2.0);
						double cosine_alpha = dot(&scene->normals[l*3+0], &H);
						double alpha = acos(cosine_alpha);

						if (fabs(cosine_alpha-1.0)< 1e-5)
							alpha = 0.0;

						double G1 = 2*cosine_alpha*cosine_V/vh;
						double G2 = 2*cosine_alpha*cosine_L/vh;
							  G1=(G1<1)?G1:1;
						double G=(G1<G2)?G1:G2;
					
						double param1 = exp(-pow(tan(alpha)/m,2))/(pow(m,2)*pow(cosine_alpha,4)); 
						double param2 = G *( F0 + (1-F0) * pow((1.0-vh), 5));
						
						tempcolor.r =  Ad[0]*cosine_L/PI + As[0] * param1 * param2 / cosine_V;
						tempcolor.g = (Ad[1]*cosine_L/PI + As[1] * param1 * param2 / cosine_V)*1.15;
						tempcolor.b = (Ad[2]*cosine_L/PI + As[2] * param1 * param2 / cosine_V)*1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}	
				}
				
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}

	return 1;
}

int He_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L;
    double para[9];
	double Ad[3], As[3], p0, p1, F0;
	ifstream in1;

	CString filename;

	filename = file_fold + sMaterialID + "\\He_param.txt";

	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 9; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	p0 =  para[6];
	p1 = para[7];
	F0 = para[8];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
	for (l = 0; l < scene->number_of_vertices; l++)
	{
		//printf("%d\n", l);
		coeffs[l].r = 0.0;
		coeffs[l].g = 0.0;
		coeffs[l].b = 0.0;

		double norm[3];
		norm[0] = scene->normals[l*3+0];
		norm[1] = scene->normals[l*3+1];
		norm[2] = scene->normals[l*3+2];

		for (i = 0; i < 6; i++)
		{
			for(j = 0; j < bands; j++)
			{
				for (k = 0; k < bands; k++)
				{
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					double view[3]={0.0,0.0,1.0};
					
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						double lightdir[3];
						lightdir[0]=direction.x;
						lightdir[1]=direction.y;
						lightdir[2]=direction.z;

						NormalAdjust(view, lightdir, norm);
						
						double v[3], vxy, ssi[3], ssr[3], psi[3], psr[3], G;
						double z_axis[3] = {0.0, 0.0, 1.0};
						v[0] = view[0] + lightdir[0];
						v[1] = view[1] + lightdir[1];
						v[2] = view[2] + lightdir[2];
						vxy = sqrt(v[0]*v[0] + v[1]*v[1]);
						cross_product(lightdir, z_axis, ssi);
						normal_vector(ssi);
						cross_product(ssi, lightdir, psi);
						cross_product(view, z_axis, ssr);				
						normal_vector(ssr);
						cross_product(ssr, view, psr);
						G = pow(dot_vector(v, v)/v[2], 2.0) / pow(cross_length(view, lightdir), 4.0) * 
							(pow(dot_vector(ssr, lightdir), 2.0) + pow(dot_vector(psr, lightdir), 2.0)) * 
								(pow(dot_vector(ssi, view), 2.0) + pow(dot_vector(psi, view), 2.0));


						double Si, Sr, theta_i, theta_r;
						theta_i = acos(lightdir[2]);
						if (fabs(lightdir[2]-1.0)< 1e-6)
							theta_i = 0.0;
						theta_r = acos(view[2]);
						if (fabs(view[2]-1.0)< 1e-6)
							theta_r = 0.0;
						Si = (1 - 0.5*erfc(p0/tan(theta_i)/(2*p1))) / 
							(Lam(theta_i, p0, p1) + 1);
						Sr = (1 - 0.5*erfc(p0/tan(theta_r)/(2*p1))) /
							(Lam(theta_r, p0, p1) + 1);
	
						double D, Ki, Kr, z0, sigma, g, lambda;
						lambda = 1;
						Ki = tan(theta_i) * erfc(p0/(2*p1)/tan(theta_i));
						Kr = tan(theta_r) * erfc(p0/(2*p1)/tan(theta_r));
						z0 = dich(Ki, Kr, p1);
						sigma = p1 / sqrt(1+pow(z0/p1, 2.0));
						g = pow((2*PI*sigma/lambda) * (lightdir[2]+view[2]), 2.0);
						D = Factor_D(p0, lambda, g, vxy);
					
						double F, vh;
						vh = sqrt((1+direction.z)/2.0);
						F = F0 + (1 - F0) * pow((1 - vh),5);

						tempcolor.r = (Ad[0]/PI + As[0]*F*G*Si*Sr*D/(PI*lightdir[2]*view[2])) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1]*F*G*Si*Sr*D/(PI*lightdir[2]*view[2])) * cosine_L * 1.15;
						tempcolor.b = (Ad[2]/PI + As[2]*F*G*Si*Sr*D/(PI*lightdir[2]*view[2])) * cosine_L * 1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}
				}
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}

	return 1;
}

int Lf_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L;
    double para[9];
	double Ad[3], As[3];
	long double Cxy, Cz, n;
	ifstream in1;

    CString filename;
	//filename = file_fold + sMaterialID + "\\Lf_param.txt";
	filename = file_fold + sMaterialID + "\\Lafortune_parameter.txt";

	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 9; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	Cxy =  para[6];
	Cz = para[7];
	n = para[8];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
	for (l = 0; l < scene->number_of_vertices; l++)
	{
     	//printf("%d\n", l);
		coeffs[l].r = 0.0;
		coeffs[l].g = 0.0;
		coeffs[l].b = 0.0;

		double norm[3];
		norm[0]=scene->normals[l*3+0];
		norm[1]=scene->normals[l*3+1];
		norm[2]=scene->normals[l*3+2];

		for (i = 0; i < 6; i++)
		{
			for(j = 0; j < bands; j++)
			{
				for (k = 0; k < bands; k++)
				{
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					double view[3]={0.0,0.0,1.0};
				
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						double lightdir[3];
						lightdir[0]=direction.x;
						lightdir[1]=direction.y;
						lightdir[2]=direction.z;
						
						NormalAdjust(view, lightdir, norm);

						long double lafor1, lafor2, param;
						lafor1 = Cxy*(lightdir[0]*view[0]+lightdir[1]*view[1])+Cz*lightdir[2]*view[2];
						lafor2 = (abs(Cz) > abs(Cxy))?abs(Cz):abs(Cxy);

						lafor1 = abs(lafor1);
						param = pow((lafor1/lafor2), n) * (n + 2) / 2.0 / PI;

						if (param != param)
						{
							long double tmp1, tmp2;
							tmp1 = lafor1/lafor2;
							tmp2 = pow(tmp1, n);
							printf("%lf, %lf, %lf", lafor1, lafor2, tmp2);
							printf("\n");
						}

						tempcolor.r = (Ad[0]/PI + As[0] * param) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1] * param) * cosine_L*1.15;
						tempcolor.b = (Ad[2]/PI + As[2] * param) * cosine_L*1.66;
					
						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}
				}
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}

	return 1;
}

int Wd_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L, cosine_V;
    double para[7];
	double Ad[3], As[3], n;
	ifstream in1;

    CString filename;
	//filename = file_fold + sMaterialID + "\\Ward_param.txt";
	//filename = file_fold + sMaterialID + "\\ingward_parameter.txt";
	filename = file_fold + sMaterialID + "\\Ward_parameter.txt";

	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 8; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	n = para[6];

	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
	for (l = 0; l < scene->number_of_vertices; l++)
	{
	//	printf("%d\n", l);
		coeffs[l].r = 0.0;
		coeffs[l].g = 0.0;
		coeffs[l].b = 0.0;

		for (i = 0; i < 6; i++)
		{
			for(j = 0; j < bands; j++)
			{
				for (k = 0; k < bands; k++)
				{
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					Vector3 H;
					H.x = direction.x ;
					H.y = direction.y ;	
					H.z = direction.z + 1;
					double scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);
					H.x = H.x / scale;
					H.y = H.y / scale;
					H.z = H.z / scale;
					
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						cosine_V = scene->normals[l*3+2];

						double cosine_alpha = dot(&scene->normals[l*3+0], &H);
						double alpha = acos(cosine_alpha);
						if (fabs(cosine_alpha-1.0)< 1e-5)
							alpha = 0.0;

						double G = 1.0 / sqrt(cosine_L * cosine_V) / 4 / PI;

						double param = exp(-pow(tan(alpha),2)/pow(n,2)) / pow(n,2) * G;

						tempcolor.r = (Ad[0]/PI + As[0] * param) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1] * param) * cosine_L*1.15;
						tempcolor.b = (Ad[2]/PI + As[2] * param) * cosine_L*1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}
				}
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);

	}

	return 1;
}

int Wddu_model(Color* coeffs, HDR_Image light, Scene* scene, CString file_fold, CString sMaterialID)
{
	float a ,b, c, x, y;
	Vector3 direction;
	int i, j, k, l, bands, ind;
	Color tempcolor;

	double cosine_L, cosine_V;
    double para[7];
	double Ad[3], As[3], n;
	ifstream in1;

    CString filename;
	////////////修改5/////////////
	extern CString _materialName;
	filename = file_fold + _materialName;
	//filename = file_fold + "WdDu_parameter.txt";
	//filename = file_fold + sMaterialID + "\\2.txt";
	//filename = file_fold + sMaterialID + "\\WdDu_parameter.txt";
	///////////////////////////////


	in1.open(filename, ios::in);
	if (!in1)
	{
		printf("Parameter file does not exist!\n");
		system("pause");
		return 0;
	}

	//读取拟合参数
	for (int index = 0; index < 8; index++)
	{
		in1 >> para[index];
	}
	in1.close();
	in1.clear();

	Ad[0] = para[0];
	Ad[1] = para[1];
	Ad[2] = para[2];
	As[0] = para[3];
	As[1] = para[4];
	As[2] = para[5];
	n = para[6];
	
	bands = light.width / 3;  //得到每个面的宽度

	//开始渲染
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
					//printf("%d**%d**%d**%d\n", l, i, j, k);
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

					Vector3 H;
					H.x = direction.x ;
					H.y = direction.y ;	
					H.z = direction.z + 1;
					double scale = sqrt(H.x*H.x + H.y*H.y + H.z*H.z);
					H.x = H.x / scale;
					H.y = H.y / scale;
					H.z = H.z / scale;
					
					cosine_L = dot(&scene->normals[l*3+0], &direction);
					if ( cosine_L > 0.0 )
					{
						cosine_V = scene->normals[l*3+2];

						double vh = sqrt((1+direction.z)/2);

						double cosine_alpha = dot(&scene->normals[l*3+0], &H);
						double alpha = acos(cosine_alpha);
						if (fabs(cosine_alpha-1.0)< 1e-5)
							alpha = 0.0;

						double G = 1.0 / (cosine_L * cosine_V) / 4 / PI;
	
						double param = exp(-pow(tan(alpha),2)/pow(n,2)) / pow(n,2) * G;

						tempcolor.r = (Ad[0]/PI + As[0] * param) * cosine_L;
						tempcolor.g = (Ad[1]/PI + As[1] * param) * cosine_L*1.15;
						tempcolor.b = (Ad[2]/PI + As[2] * param) * cosine_L*1.66;

						coeffs[l].r += tempcolor.r * light.image[ind*3 + 0];
						coeffs[l].g += tempcolor.g * light.image[ind*3 + 1];
						coeffs[l].b += tempcolor.b * light.image[ind*3 + 2];
					}
				}
			}
		}
		coeffs[l].r = pow(coeffs[l].r, 1.0 / 3.0);
		coeffs[l].g = pow(coeffs[l].g, 1.0 / 3.0);
		coeffs[l].b = pow(coeffs[l].b, 1.0 / 3.0);
	}

	return 1;
}