/*   lane_testbench.c
 *   2-dimensional edge filter for lane detection
 *      calculation with fixed point values
 *      shift output by one pixel down and right to match hardware implementation
 *
 *   FPGA Vision Remote Lab http://h-brs.de/fpga-vision-lab
 *   (c) Marco Winzker, Hochschule Bonn-Rhein-Sieg, 03.01.2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp24_io.c"

void main(argc,argv)
int argc;
char *argv[];
{
char  f_name[100];
int   x,y;
long  pixel_lt,pixel_ct,pixel_rt;
long  pixel_lc,         pixel_rc;
long  pixel_lb,pixel_cb,pixel_rb;
long  lum_lt,lum_ct,lum_rt;
long  lum_lc,     lum_rc;
long  lum_lb,lum_cb,lum_rb;
long  sum_x, sum_y;
long  sum_x_2, sum_y_2;
long  g_square_256;
long  g_square;
long  g_limit;
long  g_limit_13bit;
long  g_int;
long  lum_new;
long  **image_in;
int   y_size, x_size;
long  **image_out;
long  pixel;
long  r, g, b;
FILE  *f_stimuli, *f_expected;

if (argc != 2)
  {
  printf("USAGE: %s <input file base>\n",argv[0]);
  exit(1);
  }

printf("Sobel-Filter\n");
printf("============\n\n");

sprintf(f_name ,"%s.bmp",argv[1]);
bmp24_open(f_name,&image_in,&x_size,&y_size);

bmp24_alloc(&image_out,x_size,y_size);

    for (y=0;y<y_size;y++)
      for (x=0;x<x_size;x++)
        {
        pixel_lt = bmp24_get(image_in,x-1,y-1,x_size,y_size); /* left   top  */
        pixel_ct = bmp24_get(image_in,x  ,y-1,x_size,y_size); /* center top  */
        pixel_rt = bmp24_get(image_in,x+1,y-1,x_size,y_size); /* right  top  */
        pixel_lc = bmp24_get(image_in,x-1,y  ,x_size,y_size); /* left   center  */
        pixel_rc = bmp24_get(image_in,x+1,y  ,x_size,y_size); /* right  center  */
        pixel_lb = bmp24_get(image_in,x-1,y+1,x_size,y_size); /* left   bottom  */
        pixel_cb = bmp24_get(image_in,x  ,y+1,x_size,y_size); /* center bottom  */
        pixel_rb = bmp24_get(image_in,x+1,y+1,x_size,y_size); /* right  bottom  */

        lum_lt = 5*bmp24_r(pixel_lt) + 9*bmp24_g(pixel_lt) + 2*bmp24_b(pixel_lt);
        lum_ct = 5*bmp24_r(pixel_ct) + 9*bmp24_g(pixel_ct) + 2*bmp24_b(pixel_ct);
        lum_rt = 5*bmp24_r(pixel_rt) + 9*bmp24_g(pixel_rt) + 2*bmp24_b(pixel_rt);
        lum_lc = 5*bmp24_r(pixel_lc) + 9*bmp24_g(pixel_lc) + 2*bmp24_b(pixel_lc);
        lum_rc = 5*bmp24_r(pixel_rc) + 9*bmp24_g(pixel_rc) + 2*bmp24_b(pixel_rc);
        lum_lb = 5*bmp24_r(pixel_lb) + 9*bmp24_g(pixel_lb) + 2*bmp24_b(pixel_lb);
        lum_cb = 5*bmp24_r(pixel_cb) + 9*bmp24_g(pixel_cb) + 2*bmp24_b(pixel_cb);
        lum_rb = 5*bmp24_r(pixel_rb) + 9*bmp24_g(pixel_rb) + 2*bmp24_b(pixel_rb);

        sum_x = (lum_rt + 2*lum_rc + lum_rb) - (lum_lt + 2*lum_lc + lum_lb);
        sum_y = (lum_lt + 2*lum_ct + lum_rt) - (lum_lb + 2*lum_cb + lum_rb);

        sum_x_2 = (sum_x*sum_x);
        sum_y_2 = (sum_y*sum_y);

        g_square_256 = (sum_x_2 + sum_y_2);
        g_square = g_square_256/256;
        if (g_square > 262143)
            g_limit = 262143;
        else
            g_limit = g_square;
        g_limit_13bit = 32*(g_limit/32);
        g_int    = sqrt(g_limit_13bit)/2;

        lum_new = 255 - g_int;

        bmp24_put(image_out,lum_new,lum_new,lum_new,x,y,x_size,y_size);
        }

sprintf(f_name ,"%s_edge_fixed.bmp",argv[1]);
bmp24_close(f_name,image_out,x_size,y_size);


printf("Generating testbench data\n");
/* stimuli data */
sprintf(f_name ,"%s_stimuli.txt",argv[1]);
f_stimuli = fopen(f_name,"w");
if (f_stimuli==NULL)
    {
    printf("\n\nERROR: File %s can not be written\n",f_name);
    exit(0);
    }

fprintf(f_stimuli,"# %s - RGB pixel in hex\n",f_name);
    for (y=0;y<y_size;y++)
      for (x=0;x<x_size;x++)
        {
        pixel = bmp24_get(image_in,x,y,x_size,y_size);
        r = bmp24_r(pixel);
        g = bmp24_g(pixel);
        b = bmp24_b(pixel);
        fprintf(f_stimuli,"%02X %02X %02X\n",r,g,b);
        }
fclose(f_stimuli);

/* expected data */
sprintf(f_name ,"%s_expected.txt",argv[1]);
f_expected = fopen(f_name,"w");
if (f_expected==NULL)
    {
    printf("\n\nERROR: File %s can not be written\n",f_name);
    exit(0);
    }

fprintf(f_expected,"# %s - RGB pixel in hex\n",f_name);
    for (y=0;y<y_size;y++)
      for (x=0;x<x_size;x++)
        {
        /* impossible result for border region: different values for RGB  */
        if (x<2) pixel = 0x55AAEE;  else
        if (y<2) pixel = 0x55AAEE;  else
            pixel = bmp24_get(image_out,x-1,y-1,x_size,y_size);
        r = bmp24_r(pixel);
        g = bmp24_g(pixel);
        b = bmp24_b(pixel);
        fprintf(f_expected,"%02X %02X %02X\n",r,g,b);
        }
fclose(f_expected);


printf("OK ...\n");
}
