/*   sim2bmp.c
 *   convert a text-file from VHDL-testbench simulation to a bitmap-file
 *     Image size must be 720p, i.e. 1280x720 Pixel
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
long count;
int  x, y;
long pixel;
long r, g, b;
int  status;
long **image_out;
FILE *f_in;
char c_line[256];

printf("SIM2BMP (convert text-file to bitmap-file)\n");
printf("=======\n\n");

if (argc != 2)
  {
  printf("USAGE: %s <input file base>\n",argv[0]);
  exit(1);
  }

sprintf(f_name ,"%s.txt",argv[1]);
f_in = fopen(f_name,"r");

if (f_in==NULL)
    {
    printf("\n\nERROR: File %s does not exist\n",f_name);
    exit(0);
    }

fgets(c_line,200,f_in); /* read and ignore first line */
bmp24_alloc(&image_out,1280,720);

    for (count=0;count<(1280*720);count++)
        {
        status = fscanf(f_in,"%s\n",&c_line);
        if ( status == -1)
            {
            printf("\nERROR: Unexpected End of file %s\n       Output image has invalid pixel\n\n\n",f_name);
            break; /* exit, if file is smaller than 1280x720 */
            }
        r = strtol(c_line,NULL,16);

        fscanf(f_in,"%s\n",&c_line);
        g = strtol(c_line,NULL,16);

        fscanf(f_in,"%s\n",&c_line);
        b = strtol(c_line,NULL,16);

        /* write image */
        x = count%1280;
        y = count/1280;
        bmp24_put(image_out,r,g,b,x,y,1280,720);
        }

sprintf(f_name ,"%s.bmp",argv[1]);
bmp24_close(f_name,image_out,1280,720);

printf("\nOK\n");

}
