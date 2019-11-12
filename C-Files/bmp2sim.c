/*   bmp2sim.c
 *   convert a bitmap-file to a text-file for simulation with VHDL-testbench
 *
 *   FPGA Vision Remote Lab http://h-brs.de/fpga-vision-lab
 *   (c) Marco Winzker, Hochschule Bonn-Rhein-Sieg, 03.01.2018
 */

#include <stdio.h>
#include <stdlib.h>

#include "bmp24_io.c"

void main(argc,argv)
int argc;
char *argv[];
{
char f_name[100];
int  x, y;
long pixel;
long r, g, b;
long **image_in;
int  y_size, x_size;
int  status;
FILE *f_out;

printf("BMP2SIM (convert bitmap-file to text-file)\n");
printf("=======\n\n");

if (argc != 2)
  {
  printf("USAGE: %s <input file base>\n",argv[0]);
  exit(1);
  }

sprintf(f_name ,"%s.bmp",argv[1]);

bmp24_open(f_name,&image_in,&x_size ,&y_size );

sprintf(f_name ,"%s.txt",argv[1]);
f_out = fopen(f_name,"w");
if (f_out==NULL)
    {
    printf("\n\nERROR: File %s can not be written\n",f_name);
    exit(0);
    }

fprintf(f_out,"# %s - RGB pixel in hex\n",f_name);

    for (y=0;y<y_size;y++)
      for (x=0;x<x_size;x++)
        {
        pixel = bmp24_get(image_in,x,y,x_size,y_size);
        r = bmp24_r(pixel);
        g = bmp24_g(pixel);
        b = bmp24_b(pixel);
        fprintf(f_out,"%02X %02X %02X\n",r,g,b);
        }

printf("\nOK\n");

}
