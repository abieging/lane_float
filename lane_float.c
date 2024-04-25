/*   lane_float.c
 *   2-dimensional edge filter for lane detection
 *      calculation with floating-point values
 *
 *   FPGA Vision Remote Lab http://h-brs.de/fpga-vision-lab
 *   (c) Marco Winzker, Hochschule Bonn-Rhein-Sieg, 03.01.2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

unsigned int srcImage[1280 * 720];
unsigned int outImage[1280 * 720];

int main(int argc, char *argv[])
{
  char  f_name[100];
  int x, y;

  long  pixel_lt, pixel_ct, pixel_rt;
  long  pixel_lc,           pixel_rc;
  long  pixel_lb, pixel_cb, pixel_rb;

  float lum_lt, lum_ct, lum_rt;
  float lum_lc,         lum_rc;
  float lum_lb, lum_cb, lum_rb;

  float sum_x, sum_y;
  float g_square;
  float g_root;
  long  g_int;
  long  lum_new;
  long  **image_in;
  int   y_size, x_size;
  long  **image_out;

  if (argc != 2)
  {
    printf("USAGE: %s <input file base>\n",argv[0]);
    exit(1);
  }

  printf("Sobel-Filter\n");
  printf("============\n\n");

  sprintf(f_name ,"%s.bmp",argv[1]);

  int width, height, channels;
  unsigned char *img = stbi_load(f_name, &width, &height, &channels, 0);
  
  if(img == NULL)
  {
     printf("Error in loading the image\n");
     exit(1);
  }

  printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

  int srcImageIdx = 0;

  for (int i = 0; i < 1280 * 720 * 3;)
  {
    unsigned char b = img[i++];
    unsigned char g = img[i++];
    unsigned char r = img[i++];
    srcImage[srcImageIdx++] = r << 16 | g << 8 | b;
  }

  int rewrIdx = 0;

  for (int y = 0; y < height - 0; ++y)
  {
    for (int x = 0; x < width - 0; ++x)
    {
      int idx = x + (y * width);

      pixel_lt = srcImage[idx - 1 - width]; /* left   top  */
      pixel_ct = srcImage[idx     - width]; /* center top  */
      pixel_rt = srcImage[idx + 1 - width]; /* right  top  */
      pixel_lc = srcImage[idx - 1        ]; /* left   center  */
      pixel_rc = srcImage[idx + 1        ]; /* right  center  */
      pixel_lb = srcImage[idx - 1 + width]; /* left   bottom  */
      pixel_cb = srcImage[idx     + width]; /* center bottom  */
      pixel_rb = srcImage[idx + 1 + width]; /* right  bottom  */

      float rMult = 0.299;
      float gMult = 0.587;
      float bMult = 0.114;

      lum_lt = rMult * ((pixel_lt >> 16) & 255) + gMult * ((pixel_lt >> 8) & 255) + bMult * (pixel_lt & 255);
      lum_ct = rMult * ((pixel_ct >> 16) & 255) + gMult * ((pixel_ct >> 8) & 255) + bMult * (pixel_ct & 255);
      lum_rt = rMult * ((pixel_rt >> 16) & 255) + gMult * ((pixel_rt >> 8) & 255) + bMult * (pixel_rt & 255);
      lum_lc = rMult * ((pixel_lc >> 16) & 255) + gMult * ((pixel_lc >> 8) & 255) + bMult * (pixel_lc & 255);
      lum_rc = rMult * ((pixel_rc >> 16) & 255) + gMult * ((pixel_rc >> 8) & 255) + bMult * (pixel_rc & 255);
      lum_lb = rMult * ((pixel_lb >> 16) & 255) + gMult * ((pixel_lb >> 8) & 255) + bMult * (pixel_lb & 255);
      lum_cb = rMult * ((pixel_cb >> 16) & 255) + gMult * ((pixel_cb >> 8) & 255) + bMult * (pixel_cb & 255);
      lum_rb = rMult * ((pixel_rb >> 16) & 255) + gMult * ((pixel_rb >> 8) & 255) + bMult * (pixel_rb & 255);

      sum_x = (lum_rt + (2.0 * lum_rc) + lum_rb) - (lum_lt + (2.0 * lum_lc) + lum_lb);
      sum_y = (lum_lt + (2.0 * lum_ct) + lum_rt) - (lum_lb + (2.0 * lum_cb) + lum_rb);

      g_square = (sum_x * sum_x) + (sum_y * sum_y);
      g_root   = sqrt(g_square);
      g_int    = g_root / 2.0;

      if (g_int > 255)
      {
          g_int = 255;
      }

      lum_new = 255 - g_int;

      img[rewrIdx++] = lum_new;
      img[rewrIdx++] = lum_new;
      img[rewrIdx++] = lum_new;
    }
  }

  sprintf(f_name ,"%s_edge_float.bmp",argv[1]);

  stbi_write_png(f_name, width, height, channels, img, width * channels);

  printf("OK ...\n");
}