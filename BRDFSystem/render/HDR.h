/* THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.
 * WHILE THE AUTHORS HAVE TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY,
 * IT IS STRICTLY USE AT YOUR OWN RISK.  */

/* utility for reading and writing Ward's rgbe image format.
   See rgbe.txt file for more details.
*/

/* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

/* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

/* offsets to red, green, and blue components in a data (float) pixel */
#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
/* number of floats per pixel */
#define RGBE_DATA_SIZE   3

typedef struct 
{
  int valid;            /* indicate which fields are valid */
  char programtype[16]; /* listed at beginning of file to identify it 
                         * after "#?".  defaults to "RGBE" */ 
  float gamma;          /* image has already been gamma corrected with 
                         * given gamma.  defaults to 1.0 (no correction) */
  float exposure;       /* a value of 1.0 in an image corresponds to
    * <exposure> watts/steradian/m^2. 
    * defaults to 1.0 */
} rgbe_header_info;

enum rgbe_error_codes {
  rgbe_read_error,
  rgbe_write_error,
  rgbe_format_error,
  rgbe_memory_error,
};

class HDR_Image
{
public:
	/* read or write headers */
	/* you may set rgbe_header_info to null if you want to */
	int RGBE_WriteHeader(FILE *fp);
	int RGBE_ReadHeader(FILE *fp);

	/* read or write pixels */
	/* can read or write pixels in chunks of any size including single pixels*/
	int RGBE_WritePixels(FILE *fp, unsigned int numpixels);
	int RGBE_ReadPixels(FILE *fp, unsigned int numpixels);

	/* read or write run length encoded files */
	/* must be called to read or write whole scanlines */
	int RGBE_WritePixels_RLE(FILE *fp, float *data, int scanline_width,
		int num_scanlines);
	int RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width,
	   int num_scanlines);

	/*load or save HDR file*/
	bool Load(char *filename);
	bool Save(char *filename);

	/*-------*/
	int rgbe_error(int rgbe_error_code, char *msg);
	int RGBE_WriteBytes_RLE(FILE *fp, unsigned char *data, int numbytes);
	void float2rgbe(unsigned char rgbe[4], float red, float green, float blue);
	void rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4]);



public:
	int width;
	int height;
	unsigned int TotalPixels;
	rgbe_header_info* info;
	float* image;
};


