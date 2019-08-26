#include "stdlib.h"
#include "math.h"

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)
#define M_PI	3.1415926535897932384626433832795



// cross product of two vectors
void cross_product (float* v1, float* v2, float* out)
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

// normalize vector
void normalize(float* v)
{
	// normalize
	float len = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0] = v[0] / len;
	v[1] = v[1] / len;
	v[2] = v[2] / len;
}

// rotate vector along one axis
void rotate_vector(float* vector, float* axis, float angle, float* out)
{
	float temp;
	float cross[3];
	float cos_ang = cos(angle);
	float sin_ang = sin(angle);

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


// convert standard coordinates to half vector/difference vector coordinates
void std_coords_to_half_diff_coords(float theta_in, float fi_in, float theta_out, float fi_out,
								float& theta_half,float& fi_half,float& theta_diff,float& fi_diff )
{

	// compute in vector
	float in_vec_z = cos(theta_in);
	float proj_in_vec = sin(theta_in);
	float in_vec_x = proj_in_vec*cos(fi_in);
	float in_vec_y = proj_in_vec*sin(fi_in);
	float in[3]= {in_vec_x,in_vec_y,in_vec_z};
	normalize(in);


	// compute out vector
	float out_vec_z = cos(theta_out);
	float proj_out_vec = sin(theta_out);
	float out_vec_x = proj_out_vec*cos(fi_out);
	float out_vec_y = proj_out_vec*sin(fi_out);
	float out[3]= {out_vec_x,out_vec_y,out_vec_z};
	normalize(out);


	// compute halfway vector
	float half_x = (in_vec_x + out_vec_x)/2.0f;
	float half_y = (in_vec_y + out_vec_y)/2.0f;
	float half_z = (in_vec_z + out_vec_z)/2.0f;
	float half[3] = {half_x,half_y,half_z};
	normalize(half);

	// compute  theta_half, fi_half
	if (fabs(half[2] - 1.0) < 1e-6)
	{
		theta_half = 0.0;
		fi_half = 0.0;
	}
	else
	{
		theta_half = acos(half[2]);
		fi_half = atan2(half[1], half[0]);
	}

	float bi_normal[3] = {0.0, 1.0, 0.0};
	float normal[3] = { 0.0, 0.0, 1.0 };
	float temp[3];
	float diff[3];

	// compute diff vector
	rotate_vector(in, normal , -fi_half, temp);
	rotate_vector(temp, bi_normal, -theta_half, diff);
	
	// compute  theta_diff, fi_diff	
	theta_diff = acos(diff[2]);
	fi_diff = atan2(diff[1], diff[0]);

}


// Lookup theta_half index
// This is a non-linear mapping!
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_half_index(float theta_half)
{
	if (theta_half <= 0.0)
		return 0;
	double theta_half_deg = ((theta_half / (M_PI/2.0))*BRDF_SAMPLING_RES_THETA_H);
	double temp = theta_half_deg*BRDF_SAMPLING_RES_THETA_H;
	temp = sqrt(temp);
	int ret_val = (int)temp;
	if (ret_val < 0) ret_val = 0;
	if (ret_val >= BRDF_SAMPLING_RES_THETA_H)
		ret_val = BRDF_SAMPLING_RES_THETA_H-1;
	return ret_val;
}


// Lookup theta_diff index
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_diff_index(float theta_diff)
{
	int tmp = int(theta_diff / (M_PI * 0.5) * BRDF_SAMPLING_RES_THETA_D);
	if (tmp < 0)
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_THETA_D - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_THETA_D - 1;
}


// Lookup phi_diff index
inline int phi_diff_index(float phi_diff)
{
	// Because of reciprocity, the BRDF is unchanged under
	// phi_diff -> phi_diff + M_PI
	if (phi_diff < 0.0)
		phi_diff += M_PI;

	// In: phi_diff in [0 .. pi]
	// Out: tmp in [0 .. 179]
	int tmp = int(phi_diff / M_PI * BRDF_SAMPLING_RES_PHI_D / 2);
	if (tmp < 0)	
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_PHI_D / 2 - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_PHI_D / 2 - 1;
}


// Given a pair of incoming/outgoing angles, look up the BRDF.
void lookup_brdf_val(double* brdf, float theta_in, float fi_in,
			  float theta_out, float fi_out, 
			  double& red_val,double& green_val,double& blue_val)
{
	// Convert to halfangle / difference angle coordinates
	float theta_half, fi_half, theta_diff, fi_diff;
	
	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		       theta_half, fi_half, theta_diff, fi_diff);


	// Find index.
	// Note that phi_half is ignored, since isotropic BRDFs are assumed
	int ind = phi_diff_index(fi_diff) +
		  theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		  theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
					         BRDF_SAMPLING_RES_THETA_D;

	red_val = brdf[ind] * RED_SCALE;
	green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2] * GREEN_SCALE;
	blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;

	
	if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0)
		fprintf(stderr, "Below horizon.\n");

}


// Read BRDF data
bool read_brdf(CString filename, double* &brdf)
{
	ifstream in(filename, ios::in | ios::binary);
	if (!in)
		return false;

	int dims[3];
	in.read((char*)dims, sizeof(int)*3);
	int n = dims[0] * dims[1] * dims[2];
	if (n != BRDF_SAMPLING_RES_THETA_H *
		 BRDF_SAMPLING_RES_THETA_D *
		 BRDF_SAMPLING_RES_PHI_D / 2) 
	{
		fprintf(stderr, "Dimensions don't match\n");
		in.close();
		return false;
	}

	brdf = (double*) malloc (sizeof(double)*3*n);
	in.read((char*)brdf, sizeof(double)*3*n);

	in.close();

	return true;
}