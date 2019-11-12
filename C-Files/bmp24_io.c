/*   bmp24_io.c
 *
 *   simple and easy-to-use functions for handling image files
 *     - only for BMP files in 24bit format
 *     - attention: y-index running from top to bottom
 *     - border protection: access outside image area gives black pixel
 *
 *   Version 0.93,14.10.2010
 *
 *   Copyright (C) 2008-2010 Marco Winzker, Hochschule Bonn-Rhein-Sieg
 *       http://fb03.h-brs.de/Marco_Winzker.html
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation; either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, see <http://www.gnu.org/licenses/>.
 */
 
 /* distributed with FPGA Vision Remote Lab, 03.01.2018 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
/* extern int _fmode; */ /* some Compiler need this option */

void bmp24_alloc(long ***image,int x_size,int y_size)
{
long i;
long *c_point;
long **line;
/* allocate y_size pointers to long */
*image = malloc(y_size*sizeof(c_point));
if ( *image == NULL )
  {printf("bmp24_alloc failed. Aborting...\n"); exit(1);}

/* allocate y_size times x_size long */
for (i=0;i<y_size;i++)
  {
  line = *image + i;
  *line = (long *)malloc(x_size*sizeof(i));
  if ( *line == NULL )
    {printf("bmp24_alloc of line failed. Aborting...\n"); exit(1);}
  }
}

long bmp24_get(long **image,int x,int y,int x_size,int y_size)
{
long **line_pos;
long *pix_pos;
long  pix ;

/* border protection: set pixel outside range to black */
if( (y<0) || (y>=y_size) || (x<0) || (x>=x_size) )
     return( (long)0 );

/* OPTION: access outside image area gives nearest pixel */
/*
if (y<0 )      y = 0;
if (y>=y_size) y = y_size-1;
if (x<0 )      x = 0;
if (x>=x_size) x = x_size-1; */


line_pos = image    + y;
pix_pos  = *line_pos + x;
pix  = *pix_pos;
return( pix );
}

int bmp24_r(long pix)
{
return( (pix >>16) & 255 );
}

int bmp24_g(long pix)
{
return( (pix >>8 ) & 255 );
}

int bmp24_b(long pix)
{
return( (pix     ) & 255 );
}

void bmp24_put(long **image,int r,int g,int b,int x,int y,int x_size,int y_size)
{
long **line_pos;
long *pix_pos;
long pix;

if( (y<0) || (y>=y_size) || (x<0) || (x>=x_size) )
  {printf("bmp24_put invoked with address out of range %d,%d (max = %d,%d)\n",
           x,y,x_size,y_size);
   exit(1);}

line_pos = image    + y;
pix_pos  = *line_pos + x;
pix = (r&255)<<16 | (g&255)<<8 | (b&255);
*pix_pos = pix;

return;
}

/* read 24bit BMP-File */
void bmp24_open(char *filename,long ***image,int *x_size,int *y_size)
{
int in1,in2,in3,in4;
int i, x, y;
int  padding;
long l1;
char  r,g,b;
FILE *f_bmp;
/* _fmode = O_BINARY; -- remove this line for Linux */

f_bmp = fopen(filename,"r");
if ( f_bmp == NULL )
  {printf("bmp24_open: %s does not exist. Aborting...\n",filename); exit(1);}

/* BMP-ID */
in1 = fgetc(f_bmp); in2 = fgetc(f_bmp);
if ( (in1!=0x42) || (in2!=0x4d) )
  {printf("bmp24_open: %s is not a BMP-file. Aborting...\n",filename); exit(1);}

for (i=0;i<16;i++)  in1 = fgetc(f_bmp);   /* ignore 16 bytes */

/* width */
in1 = fgetc(f_bmp); in2 = fgetc(f_bmp); in3 = fgetc(f_bmp); in4 = fgetc(f_bmp);
l1 = (in4<<24)|(in3<<16)|(in2<<8)|(in1);
*x_size = l1;

/* height */
in1 = fgetc(f_bmp); in2 = fgetc(f_bmp); in3 = fgetc(f_bmp); in4 = fgetc(f_bmp);
l1 = (in4<<24)|(in3<<16)|(in2<<8)|(in1);
*y_size = l1;

for (i=0;i< 2;i++)  in1 = fgetc(f_bmp);   /* ignore  2 bytes */

/* bits per pixel */
in1 = fgetc(f_bmp); in2 = fgetc(f_bmp);
if ( (in1!=0x18) || (in2!=0x00) )
  {printf("bmp24_open: %s is not a 24bit BMP-file. Aborting...\n",filename); exit(1);}

for (i=0;i<24;i++)  in1 = fgetc(f_bmp);   /* ignore 24 bytes */

printf("Reading file %s (%d*%d pixel)\n",filename,*x_size,*y_size);

/* allocate memory */
bmp24_alloc(image,*x_size,*y_size);

/* each scanline ends on a 4-byte boundary */
padding = *x_size % 4;

for (y=*y_size-1;y>-1;y--)
  {
  for (x=0;x<*x_size;x++)
    {
    b = fgetc(f_bmp);
    g = fgetc(f_bmp);
    r = fgetc(f_bmp);

    bmp24_put(*image,r,g,b,x,y,*x_size,*y_size);
    }
  for (x=0;x<padding;x++)
    in1 = fgetc(f_bmp);
  }

fclose(f_bmp);

}

/* write 24bit BMP-File */
void bmp24_close(char *filename,long **image,int x_size,int y_size)
{
int val;
long bytes, x, y;
int  padding;
char r, g, b;
FILE *f_bmp;
f_bmp = fopen(filename,"w");
if ( f_bmp == NULL )
  {printf("bmp24_close: %s could not be opened. Aborting...\n",filename); exit(1);}

printf("Writing file %s (%d*%d pixel)\n",filename,x_size,y_size);

/* each scanline must end on a 4-byte boundary */
padding = x_size % 4;

/* ID */
val = 0x42;    fputc(val,f_bmp);
val = 0x4d;    fputc(val,f_bmp);

/* filesize */
bytes = 54 + x_size*y_size*3;
val = (bytes    ) & 0xff;    fputc(val,f_bmp);
val = (bytes>>8 ) & 0xff;    fputc(val,f_bmp);
val = (bytes>>16) & 0xff;    fputc(val,f_bmp);
val = (bytes>>24) & 0xff;    fputc(val,f_bmp);

/* reserved 1,2 */
val = 0;    fputc(val,f_bmp);
val = 0;    fputc(val,f_bmp);
val = 0;    fputc(val,f_bmp);
val = 0;    fputc(val,f_bmp);

/* bitmap offset */
val = 0x36;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

/* BMP Version 3 header */
/* this_header size */
val = 0x28;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

/* width */
val = (x_size    ) & 0xff;    fputc(val,f_bmp);
val = (x_size>>8 ) & 0xff;    fputc(val,f_bmp);
val = (x_size>>16) & 0xff;    fputc(val,f_bmp);
val = (x_size>>24) & 0xff;    fputc(val,f_bmp);

/* heigth */
val = (y_size    ) & 0xff;    fputc(val,f_bmp);
val = (y_size>>8 ) & 0xff;    fputc(val,f_bmp);
val = (y_size>>16) & 0xff;    fputc(val,f_bmp);
val = (y_size>>24) & 0xff;    fputc(val,f_bmp);

/* 1 plane */
val = 0x01;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

/* 24 bits per colour */
val = 0x18;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

/* no compression */
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

/* size of bitmap */
bytes = x_size*y_size*3;
val = (bytes    ) & 0xff;    fputc(val,f_bmp);
val = (bytes>>8 ) & 0xff;    fputc(val,f_bmp);
val = (bytes>>16) & 0xff;    fputc(val,f_bmp);
val = (bytes>>24) & 0xff;    fputc(val,f_bmp);

/* 4 longwords resolution and colour */
val = 0xCE;    fputc(val,f_bmp);
val = 0x0E;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

val = 0xC4;    fputc(val,f_bmp);
val = 0x0E;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);
val = 0x00;    fputc(val,f_bmp);

for (y=y_size-1;y>-1;y--)
  {
  for (x=0;x<x_size;x++)
    {
    val = bmp24_get(image,x,y,x_size,y_size);
    r = bmp24_r(val);
    g = bmp24_g(val);
    b = bmp24_b(val);
    fputc(b,f_bmp);
    fputc(g,f_bmp);
    fputc(r,f_bmp);
    }
  for (x=0;x<padding;x++)
    fputc(0,f_bmp);
  }

fclose(f_bmp);

}
