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
float lum_lt,lum_ct,lum_rt;
float lum_lc,     lum_rc;
float lum_lb,lum_cb,lum_rb;
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

        lum_lt = 0.299*bmp24_r(pixel_lt) + 0.587*bmp24_g(pixel_lt) + 0.114*bmp24_b(pixel_lt);
        lum_ct = 0.299*bmp24_r(pixel_ct) + 0.587*bmp24_g(pixel_ct) + 0.114*bmp24_b(pixel_ct);
        lum_rt = 0.299*bmp24_r(pixel_rt) + 0.587*bmp24_g(pixel_rt) + 0.114*bmp24_b(pixel_rt);
        lum_lc = 0.299*bmp24_r(pixel_lc) + 0.587*bmp24_g(pixel_lc) + 0.114*bmp24_b(pixel_lc);
        lum_rc = 0.299*bmp24_r(pixel_rc) + 0.587*bmp24_g(pixel_rc) + 0.114*bmp24_b(pixel_rc);
        lum_lb = 0.299*bmp24_r(pixel_lb) + 0.587*bmp24_g(pixel_lb) + 0.114*bmp24_b(pixel_lb);
        lum_cb = 0.299*bmp24_r(pixel_cb) + 0.587*bmp24_g(pixel_cb) + 0.114*bmp24_b(pixel_cb);
        lum_rb = 0.299*bmp24_r(pixel_rb) + 0.587*bmp24_g(pixel_rb) + 0.114*bmp24_b(pixel_rb);

        sum_x = (lum_rt + 2.0*lum_rc + lum_rb) - (lum_lt + 2.0*lum_lc + lum_lb);
        sum_y = (lum_lt + 2.0*lum_ct + lum_rt) - (lum_lb + 2.0*lum_cb + lum_rb);

        g_square = sum_x*sum_x + sum_y*sum_y;
        g_root   = sqrt(g_square);
        g_int    = g_root/2;
        if (g_int > 255)
            g_int = 255;

        lum_new = 255 - g_int;

        bmp24_put(image_out,lum_new,lum_new,lum_new,x,y,x_size,y_size);

        if (0) /* for debugging */
        if (x==690) if (y==277) {
            bmp24_put(image_out,255,0,0,x,y,x_size,y_size);
            printf("lum_lt = %f\n",lum_lt);
            printf("lum_ct = %f\n",lum_ct);
            printf("lum_rt = %f\n",lum_rt);
            printf("lum_lc = %f\n",lum_lc);
            printf("lum_rc = %f\n",lum_rc);
            printf("lum_lb = %f\n",lum_lb);
            printf("lum_cb = %f\n",lum_cb);
            printf("lum_rb = %f\n",lum_rb);
            printf("\n");
            printf("sum_x  = %f\n",sum_x);
            printf("sum_y  = %f\n",sum_y);
            printf("\n");
            printf("g_square = %f\n",g_square);
            printf("g_root   = %f\n",g_root);
            printf("g_int    = %d\n",g_int);
            printf("lum_new  = %d\n",lum_new);
            }
        }

sprintf(f_name ,"%s_edge_float.bmp",argv[1]);
bmp24_close(f_name,image_out,x_size,y_size);

printf("OK ...\n");
}
