/* THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.
 * WHILE THE AUTHORS HAVE TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY,
 * IT IS STRICTLY USE AT YOUR OWN RISK.  */

#include "stdafx.h"

#include "HDR.h"
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>


/* This file contains code to read and write four byte rgbe file format
 developed by Greg Ward.  It handles the conversions between rgbe and
 pixels consisting of floats.  The data is assumed to be an array of floats.
 By default there are three floats per pixel in the order red, green, blue.
 (RGBE_DATA_??? values control this.)  Only the mimimal header reading and 
 writing is implemented.  Each routine does error checking and will return
 a status value as defined below.  This code is intended as a skeleton so
 feel free to modify it to suit your needs.

 (Place notice here if you modified the code.)
 posted to http://www.graphics.cornell.edu/~bjw/
 written by Bruce Walter  (bjw@graphics.cornell.edu)  5/26/95
 based on code written by Greg Ward
*/
/* default error routine.  change this to change error handling */
int HDR_Image::rgbe_error(int rgbe_error_code, char *msg)
{
  switch (rgbe_error_code) {
  case rgbe_read_error:
    perror("RGBE read error");
    break;
  case rgbe_write_error:
    perror("RGBE write error");
    break;
  case rgbe_format_error:
    fprintf(stderr,"RGBE bad file format: %s\n",msg);
    break;
  default:
  case rgbe_memory_error:
    fprintf(stderr,"RGBE error: %s\n",msg);
  }
  return RGBE_RETURN_FAILURE;
}

/* standard conversion from float pixels to rgbe pixels */
/* note: you can remove the "inline"s if your compiler complains about it */
void HDR_Image::float2rgbe(unsigned char rgbe[4], float red, float green, float blue)
{
  float v;
  int e;

  v = red;
  if (green > v) v = green;
  if (blue > v) v = blue;
  if (v < 1e-32)
  {
    rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
  }
  else 
  {
    v = frexp(v,&e) * 256.0/v;
    rgbe[0] = (unsigned char) (red * v);
    rgbe[1] = (unsigned char) (green * v);
    rgbe[2] = (unsigned char) (blue * v);
    rgbe[3] = (unsigned char) (e + 128);

//	printf("%f, %f, %f, %f\n", rgbe[0], rgbe[1], rgbe[2], rgbe[3]);
  }
}

/* standard conversion from rgbe to float pixels */
/* note: Ward uses ldexp(col+0.5,exp-(128+8)).  However we wanted pixels */
/*       in the range [0,1] to map back into the range [0,1].            */
void HDR_Image::rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
{
  float f;

  if (rgbe[3]) {   /*nonzero pixel*/
   // f = ldexp(1.0,rgbe[3]-(int)(128+8));
	f = pow(2.0, rgbe[3] - int(128+8));
//  printf("%d, %d, %d, %d\n", rgbe[0], rgbe[1], rgbe[2], rgbe[3]);
    *red = rgbe[0] * f;
    *green = rgbe[1] * f;
    *blue = rgbe[2] * f;
  }
  else
    *red = *green = *blue = 0.0;
}

/* default minimal header. modify if you want more information in header */
int HDR_Image::RGBE_WriteHeader(FILE *fp)
{
  char *programtype = "RGBE";

  if (info && (info->valid & RGBE_VALID_PROGRAMTYPE))
    programtype = info->programtype;
  if (fprintf(fp,"#?%s\n",programtype) < 0)
    return rgbe_error(rgbe_write_error,NULL);
  /* The #? is to identify file type, the programtype is optional. */
  if (info && (info->valid & RGBE_VALID_GAMMA)) {
    if (fprintf(fp,"GAMMA=%g\n",info->gamma) < 0)
      return rgbe_error(rgbe_write_error,NULL);
  }
  if (info && (info->valid & RGBE_VALID_EXPOSURE)) {
    if (fprintf(fp,"EXPOSURE=%g\n",info->exposure) < 0)
      return rgbe_error(rgbe_write_error,NULL);
  }
  if (fprintf(fp,"FORMAT=32-bit_rle_rgbe\n\n") < 0)
    return rgbe_error(rgbe_write_error,NULL);
  if (fprintf(fp, "-Y %d +X %d\n", height, width) < 0)
    return rgbe_error(rgbe_write_error,NULL);
  return RGBE_RETURN_SUCCESS;
}

/* minimal header reading.  modify if you want to parse more information */
int HDR_Image::RGBE_ReadHeader(FILE *fp)
{
  char buf[128];
  int found_format;
  float tempf;
  int i;

  info = (rgbe_header_info*)malloc(sizeof(rgbe_header_info));

  found_format = 0;
  if (info) 
  {
    info->valid = 0;
    info->programtype[0] = 0;
    info->gamma = info->exposure = 1.0;
  }
  if (fgets(buf,sizeof(buf)/sizeof(buf[0]),fp) == NULL)
    return rgbe_error(rgbe_read_error,NULL);
  if ((buf[0] != '#')||(buf[1] != '?')) {
   
  }
  else if (info) {
    info->valid |= RGBE_VALID_PROGRAMTYPE;
    for(i=0;i<sizeof(info->programtype)-1;i++) {
      if ((buf[i+2] == 0) || isspace(buf[i+2]))
 break;
      info->programtype[i] = buf[i+2];
    }
    info->programtype[i] = 0;
    if (fgets(buf,sizeof(buf)/sizeof(buf[0]),fp) == 0)
      return rgbe_error(rgbe_read_error,NULL);
  }
  while(1)
  {
    if ((buf[0] == 0)||(buf[0] == '\n'))
		break;
    else if (strcmp(buf,"FORMAT=32-bit_rle_rgbe\n") != 0)
	{
	}
    else if (info && (sscanf(buf,"GAMMA=%g",&tempf) == 1)) {
      info->gamma = tempf;
      info->valid |= RGBE_VALID_GAMMA;
    }
    else if (info && (sscanf(buf,"EXPOSURE=%g",&tempf) == 1)) {
      info->exposure = tempf;
      info->valid |= RGBE_VALID_EXPOSURE;
    }

    if (fgets(buf,sizeof(buf)/sizeof(buf[0]),fp) == 0)
      return rgbe_error(rgbe_read_error,NULL);
  }

  if (fgets(buf,sizeof(buf)/sizeof(buf[0]),fp) == 0)
    return rgbe_error(rgbe_read_error,NULL);
  if (sscanf(buf,"-Y %d +X %d",&height, &width) < 2)
    return rgbe_error(rgbe_format_error,"missing image size specifier");

  TotalPixels = width * height;
  return RGBE_RETURN_SUCCESS;


}

/* simple write routine that does not use run length encoding */
/* These routines can be made faster by allocating a larger buffer and
   fread-ing and fwrite-ing the data in larger chunks */
int HDR_Image::RGBE_WritePixels(FILE *fp, unsigned int numpixels)
{
  unsigned char rgbe[4];

  while (numpixels-- > 0)
  {
    float2rgbe(rgbe,image[RGBE_DATA_RED],
        image[RGBE_DATA_GREEN],image[RGBE_DATA_BLUE]);
    image += RGBE_DATA_SIZE;
    if (fwrite(rgbe, sizeof(rgbe), 1, fp) < 1)
      return rgbe_error(rgbe_write_error,NULL);
  }
  return RGBE_RETURN_SUCCESS;
}

/* simple read routine.  will not correctly handle run length encoding */
int HDR_Image::RGBE_ReadPixels(FILE *fp, unsigned int numpixels)
{
  unsigned char rgbe[4];
  unsigned int i;

  for(i=0; i<numpixels; i++)
  {
	  if (fread(rgbe, sizeof(rgbe), 1, fp) < 1)
		  return rgbe_error(rgbe_read_error,NULL);
	//  rgbe2float(&image[i*3+0], &image[i*3+1], &image[i*3+2], rgbe);
//	  printf("%f, %f, %f\n", image[i*3+0], image[i*3+1], image[i*3+2]);

	  float f;

	  if (rgbe[3])
	  {  
		/*nonzero pixel*/
	   // f = ldexp(1.0,rgbe[3]-(int)(128+8));
		f = pow(2.0, rgbe[3] - int(128+8));
//		printf("%d, %d, %d, %d\n", rgbe[0], rgbe[1], rgbe[2], rgbe[3]);
		image[i*3+0] = rgbe[0] * f;
		image[i*3+1] = rgbe[1] * f;
		image[i*3+2] = rgbe[2] * f;
	  }
	  else
	  {
		image[i*3+0] = 0.0;
		image[i*3+1] = 0.0;
		image[i*3+2] = 0.0;
	  }
  }
}

/* The code below is only needed for the run-length encoded files. */
/* Run length encoding adds considerable complexity but does */
/* save some space.  For each scanline, each channel (r,g,b,e) is */
/* encoded separately for better compression. */

int HDR_Image::RGBE_WriteBytes_RLE(FILE *fp, unsigned char *data, int numbytes)
{
#define MINRUNLENGTH 4
  int cur, beg_run, run_count, old_run_count, nonrun_count;
  unsigned char buf[2];

  cur = 0;
  while(cur < numbytes) {
    beg_run = cur;
    /* find next run of length at least 4 if one exists */
    run_count = old_run_count = 0;
    while((run_count < MINRUNLENGTH) && (beg_run < numbytes)) {
      beg_run += run_count;
      old_run_count = run_count;
      run_count = 1;
      while( (beg_run + run_count < numbytes) && (run_count < 127)
             && (data[beg_run] == data[beg_run + run_count]))
 run_count++;
    }
    /* if data before next big run is a short run then write it as such */
    if ((old_run_count > 1)&&(old_run_count == beg_run - cur)) {
      buf[0] = 128 + old_run_count;   /*write short run*/
      buf[1] = data[cur];
      if (fwrite(buf,sizeof(buf[0])*2,1,fp) < 1)
 return rgbe_error(rgbe_write_error,NULL);
      cur = beg_run;
    }
    /* write out bytes until we reach the start of the next run */
    while(cur < beg_run) {
      nonrun_count = beg_run - cur;
      if (nonrun_count > 128) 
 nonrun_count = 128;
      buf[0] = nonrun_count;
      if (fwrite(buf,sizeof(buf[0]),1,fp) < 1)
 return rgbe_error(rgbe_write_error,NULL);
      if (fwrite(&data[cur],sizeof(data[0])*nonrun_count,1,fp) < 1)
 return rgbe_error(rgbe_write_error,NULL);
      cur += nonrun_count;
    }
    /* write out next run if one was found */
    if (run_count >= MINRUNLENGTH) {
      buf[0] = 128 + run_count;
      buf[1] = data[beg_run];
      if (fwrite(buf,sizeof(buf[0])*2,1,fp) < 1)
 return rgbe_error(rgbe_write_error,NULL);
      cur += run_count;
    }
  }
  return RGBE_RETURN_SUCCESS;
#undef MINRUNLENGTH
}

int HDR_Image::RGBE_WritePixels_RLE(FILE *fp, float *data, int scanline_width,
    int num_scanlines)
{
  unsigned char rgbe[4];
  unsigned char *buffer;
  int i, err;

  if ((scanline_width < 8)||(scanline_width > 0x7fff))
    /* run length encoding is not allowed so write flat*/
    return RGBE_WritePixels(fp, scanline_width*num_scanlines);
  buffer = (unsigned char *)malloc(sizeof(unsigned char)*4*scanline_width);
  if (buffer == NULL) 
    /* no buffer space so write flat */
    return RGBE_WritePixels(fp, scanline_width*num_scanlines);
  while(num_scanlines-- > 0) {
    rgbe[0] = 2;
    rgbe[1] = 2;
    rgbe[2] = scanline_width >> 8;
    rgbe[3] = scanline_width & 0xFF;
    if (fwrite(rgbe, sizeof(rgbe), 1, fp) < 1) {
      free(buffer);
      return rgbe_error(rgbe_write_error,NULL);
    }
    for(i=0;i<scanline_width;i++) {
      float2rgbe(rgbe,data[RGBE_DATA_RED],
   data[RGBE_DATA_GREEN],data[RGBE_DATA_BLUE]);
      buffer[i] = rgbe[0];
      buffer[i+scanline_width] = rgbe[1];
      buffer[i+2*scanline_width] = rgbe[2];
      buffer[i+3*scanline_width] = rgbe[3];
      data += RGBE_DATA_SIZE;
    }
    /* write out each of the four channels separately run length encoded */
    /* first red, then green, then blue, then exponent */
    for(i=0;i<4;i++) {
      if ((err = RGBE_WriteBytes_RLE(fp,&buffer[i*scanline_width],
         scanline_width)) != RGBE_RETURN_SUCCESS) {
 free(buffer);
 return err;
      }
    }
  }
  free(buffer);
  return RGBE_RETURN_SUCCESS;
}
      
int HDR_Image::RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width,
   int num_scanlines)
{
  unsigned char rgbe[4], *scanline_buffer, *ptr, *ptr_end;
  int i, count;
  unsigned char buf[2];

  if ((scanline_width < 8)||(scanline_width > 0x7fff))
    /* run length encoding is not allowed so read flat*/
    return RGBE_ReadPixels(fp,scanline_width*num_scanlines);
  scanline_buffer = NULL;
  /* read in each successive scanline */
  while(num_scanlines > 0) {
    if (fread(rgbe,sizeof(rgbe),1,fp) < 1) {
      free(scanline_buffer);
      return rgbe_error(rgbe_read_error,NULL);
    }
    if ((rgbe[0] != 2)||(rgbe[1] != 2)||(rgbe[2] & 0x80)) {
      /* this file is not run length encoded */
      rgbe2float(&data[0],&data[1],&data[2],rgbe);
      data += RGBE_DATA_SIZE;
      free(scanline_buffer);
      return RGBE_ReadPixels(fp,scanline_width*num_scanlines-1);
    }
    if ((((int)rgbe[2])<<8 | rgbe[3]) != scanline_width) {
      free(scanline_buffer);
      return rgbe_error(rgbe_format_error,"wrong scanline width");
    }
    if (scanline_buffer == NULL)
      scanline_buffer = (unsigned char *)
 malloc(sizeof(unsigned char)*4*scanline_width);
    if (scanline_buffer == NULL) 
      return rgbe_error(rgbe_memory_error,"unable to allocate buffer space");
    
    ptr = &scanline_buffer[0];
    /* read each of the four channels for the scanline into the buffer */
    for(i=0;i<4;i++) {
      ptr_end = &scanline_buffer[(i+1)*scanline_width];
      while(ptr < ptr_end) {
 if (fread(buf,sizeof(buf[0])*2,1,fp) < 1) {
   free(scanline_buffer);
   return rgbe_error(rgbe_read_error,NULL);
 }
 if (buf[0] > 128) {
   /* a run of the same value */
   count = buf[0]-128;
   if ((count == 0)||(count > ptr_end - ptr)) {
     free(scanline_buffer);
     return rgbe_error(rgbe_format_error,"bad scanline data");
   }
   while(count-- > 0)
     *ptr++ = buf[1];
 }
 else
 {
   /* a non-run */
   count = buf[0];
   if ((count == 0)||(count > ptr_end - ptr))
   {
     free(scanline_buffer);
     return rgbe_error(rgbe_format_error,"bad scanline data");
   }
   *ptr++ = buf[1];
   if (--count > 0) {
     if (fread(ptr,sizeof(*ptr)*count,1,fp) < 1) {
       free(scanline_buffer);
       return rgbe_error(rgbe_read_error,NULL);
     }
     ptr += count;
   }
 }
      }
    }
    /* now convert data from buffer into floats */
    for(i=0;i<scanline_width;i++)
	{
      rgbe[0] = scanline_buffer[i];
      rgbe[1] = scanline_buffer[i+scanline_width];
      rgbe[2] = scanline_buffer[i+2*scanline_width];
      rgbe[3] = scanline_buffer[i+3*scanline_width];
      rgbe2float(&data[RGBE_DATA_RED],&data[RGBE_DATA_GREEN], &data[RGBE_DATA_BLUE],rgbe);
      data += RGBE_DATA_SIZE;
    }
    num_scanlines--;
  }
  free(scanline_buffer);
  return RGBE_RETURN_SUCCESS;
}

bool HDR_Image::Load(char *filename)
{
	char* pch = strstr(filename,".hdr");
 
    if (pch == NULL)
		return false;

    FILE* file = fopen(filename,"rb");
	if (!file)
		return false;

	int i = RGBE_ReadHeader(file);
	if (i != 0)
	{
		printf("Read HDR Header Error!\n");
		return false;
	}

    image = (float *)malloc(sizeof(float)*3*width*height);

	printf("%d %d\n", width, height);
//  RGBE_ReadPixels(file, TotalPixels);
 	RGBE_ReadPixels_RLE(file, image, width, height);
    fclose(file);

	return true;
	
}
bool HDR_Image::Save(char *filename)
{
	char* pch = strstr(filename,".hdr");
 
    if (pch == NULL)
		return false;

	FILE* file= fopen(filename,"wb");
    RGBE_WriteHeader(file);
	RGBE_WritePixels_RLE(file, image, width, height);
//    RGBE_WritePixels(file, TotalPixels);
    fclose(file);

	return true;
}

