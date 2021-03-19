//FT_Graphics.c
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
*/
// FT_Graphics.c: FreeThought Library Graphics functions.
//


#include "freethought.h"
#include "FT_Graphics.h"

extern ProgramStatus PStatus;

#if Linux
#if USE_FFMPEG
#include "libavcodec/avcodec.h"  //for MJPEG needs to be on top because sizeof(AVFrame) !=
#include "libswscale/swscale.h" //for MJPEG
#endif //#if USE_FFMPEG
#endif  //Linux


//Global variables
//for JPEG
unsigned char *jpgdest;
int jpglen;
int jpgdestlen;

//===========
//GRAPHICS FUNCTIONS
//=============
//FT_CreateBitmap
//mallocs memory and initializes bitmap parameters
//this function expects caller to free memory when done
//probably should add 32-bit aligned padding
unsigned char *FT_CreateBitmap(int w,int h,int bpp,unsigned char *data)
{
unsigned char *bmp;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
int bmpsize,Bytespp;

bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*bpp);

bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

Bytespp=bpp>>3;

bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*Bytespp;

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=w;
bmi->biHeight=h;
bmi->biPlanes=1;
bmi->biBitCount=bpp;
bmi->biSizeImage=Bytespp*w*h;

//probably a better way of creating a variable based on passed data type
/*
switch(bpp) {
	case	8:
	case	24:
		memcpy((unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
	case	16:
		memcpy((unsigned short *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
	case	32:
		memcpy((unsigned int *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
} //switch(bpp)
*/

//for now just copy directly - ignore byte order
		memcpy((unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),data,Bytespp*w*h);

return(bmp);
} //FT_CreateBitmap

//FT_CreateBitmapHeader - Malloc memory, and initialize a bitmap header only, caller copies data into
//note that memory is allocated for the bitmap data
unsigned char *FT_CreateBitmapHeader(int w,int h,int bpp) 
{
unsigned char *bmp;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
int bmpsize,Bytespp;

bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*bpp);

bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

Bytespp=bpp>>3;
bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*Bytespp;

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=w;
bmi->biHeight=h;
bmi->biPlanes=1;
bmi->biBitCount=bpp;
bmi->biSizeImage=Bytespp*w*h;

return(bmp);
} //FT_CreateBitmapHeader(int w,int h,int bpp)

#if Linux
//Load Bitmap and return XImage
XImage * FT_LoadBitmapXImage(char *filename)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int nw,i,j,bc,x,y;
XImage *img;
unsigned char *offset;
float rx,ry;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapXImage\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  fprintf(stderr,"Error opening %s\n",filename);
else
{
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

x=bmi.biWidth;
y=bmi.biHeight;

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0) {
	nw=bmi.biWidth*bc+4-nw;
} else {
  nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
    *(bdata2+i*x*4+j*4)=*offset;
    *(bdata2+i*x*4+j*4+1)=*(offset+1);
    *(bdata2+i*x*4+j*4+2)=*(offset+2);
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,visual,depth,ZPixmap,0,bdata2,x,y,8,x*3);
img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);



free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

return(img);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapXImage\n");
}

return(0);
} //XImage * FT_LoadBitmapXImage(char *filename);
#endif 

//Load Bitmap and return unsigned char *
unsigned char * FT_LoadBitmap(char *filename)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,x,y,bmphsize;
unsigned char *b1offset,*b2offset;
/*
float rx,ry;
unsigned char *offset;
int bidx;
*/

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmap\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

x=bmi.biWidth;
y=bmi.biHeight;

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0)  {
	nw=bmi.biWidth*bc+4-nw;
} else {
  nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"image bitcount=%d\n",bmi.biBitCount);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);
//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
//bdata2=(unsigned char *)malloc(x*y*4);
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now make enough for a 32-bit bmp
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;


#if 0
rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);

	//bidx=i*x*4+j*4;
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
	*(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl
#endif

//#if 0 

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {

	  //nw includes bc
	  //b1offset=(y-i-1)*nw+j*bc;
	  //b1offset=i*x*4+j*4;
	  b1offset=bdata+(y-i-1)*nw+j*bc;
	  //b1offset=bdata+i*nw+j*bc;
	  b2offset=bdata2+i*x*4+j*4;

	   memcpy(b2offset,b1offset,3);
	  *(b2offset+3)=0;
//	  *(b2offset)=*(b1offset);
//	  *(b2offset+1)=*(b1offset+1);
//	  *(b2offset+2)=*(b1offset+2);
//	  *(b2offset+3)=0;

//	  memcpy(b2offset,b1offset,3);
//	  *(b2offset+3)=0;
/*	
	  *(bdata2+b2offset)=*(bdata+b1offset);
	  *(bdata2+b2offset+1)=*(bdata+b1offset+1);
	  *(bdata2+b2offset+2)=*(bdata+b1offset+2);
	  *(bdata2+b2offset+3)=0;
*/
/*
    //offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
	  offset=bdata+(y-i-1)*nw+j*bc;
  //  fprintf(stderr,"%p ",offset);
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
	*(bdata2+bidx+3)=0;
*/
  }  //end j
}  //end i
//copy image to 32-bit image on FTControl

//#endif
//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//#endif


free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

//return(img);
return(bmp);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmap\n");
}

return(0);
} //unsigned char * FT_LoadBitmap(char *filename);

//currently only for 24 bit images
//returns pointer to a bitmap (including file header, info header and data)
//XImage * FT_LoadBitmapFileAndScale(char *filename,int x,int y)
unsigned char * FT_LoadBitmapFileAndScale(char *filename,int x,int y)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,BitmapDepthInBytes,bmphsize,bidx;
unsigned char *offset;
float rx,ry;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapFileAndScale\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	//fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

BitmapDepthInBytes=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*BitmapDepthInBytes,4.0);
if (nw>0)  {
	nw=bmi.biWidth*BitmapDepthInBytes+4-nw;
} else {
  nw=bmi.biWidth*BitmapDepthInBytes;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"image bitcount=%d\n",bmi.biBitCount);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
//bdata2=(unsigned char *)malloc(x*y*4);
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now make enough for a 32-bit bmp
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

rx=(float)nw/((float)x*(float)BitmapDepthInBytes);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*BitmapDepthInBytes;
  //  fprintf(stderr,"%p ",offset);

	//bidx=i*x*4+j*4;
		bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
		*(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);



free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

//return(img);
//return(bdata2);
return(bmp);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapFileAndScale\n");
}

return(0);
}  //FT_LoadBitmapFileAndScale


//Load a Bitmap file and shade with mixcolor - for selected buttons
unsigned char *FT_LoadBitmapFileAndShade(char *filename,int x,int y,unsigned int mixcolor)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,bidx,bmphsize;
unsigned char *offset;
float rx,ry;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapFileAndShade\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0)  {
	nw=bmi.biWidth*bc+4-nw;
} else {
	nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now only use depth 3
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=(*offset+ (mixcolor&0x000000ff))/2;
    *(bdata2+bidx+1)=(*(offset+1)+ ((mixcolor&0x0000ff00)>>8))/2;
    *(bdata2+bidx+2)=(*(offset+2)+ ((mixcolor&0x00ff0000)>>16))/2;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4); //32-bit

free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

return(bmp);
//return(img);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapFileAndShade\n");
}

return(0);
}  //FT_LoadBitmapFileAndShade

//load bitmap data to a control and shade (for mouse over and button click)
//presumes no resize or 4-byte alignment is needed and source bmp is 32-bit image
//this is mostly called internally by FreeThought to shade bitmaps on buttons
unsigned char * FT_LoadBitmapAndShade(unsigned char *bmfile,unsigned int mixcolor)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdataBMP,*bdataNEWBMP,*bmp;
int i,j,bmphsize,bidx;
unsigned char *offsetBMP,*offsetNEWBMP;
int bmpx,bmpy;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapAndShade\n");
}

if (bmfile==0) {
	fprintf(stderr,"Error: FT_LoadBitmapAndShade called with bitmap data=0\n");
	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End FT_LoadBitmapAndShade\n");
	}

}

bmf=(BITMAPFILEHEADER *)bmfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",bmfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));
bdataBMP=(unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER));

//bc=bmi->biBitCount/8;

bmpx=bmi->biWidth;
bmpy=bmi->biHeight;

//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(bmpx*bmpy*4+bmphsize); //for now only use depth 3
memcpy(bmp,(unsigned char *)bmf,sizeof(BITMAPFILEHEADER));
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)bmi,sizeof(BITMAPINFOHEADER));
bdataNEWBMP=bmp+bmphsize;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<bmpy;i++) {
  for(j=0;j<bmpx;j++) {
	bidx=i*bmpx*4+j*4;
    offsetBMP=bdataBMP+bidx;
	offsetNEWBMP=bdataNEWBMP+bidx;

	*(offsetNEWBMP)=(unsigned char)(*offsetBMP+ (mixcolor&0x000000ff))/2;
    *(offsetNEWBMP+1)=(unsigned char)(*(offsetBMP+1)+ ((mixcolor&0x0000ff00)>>8))/2;
    *(offsetNEWBMP+2)=(unsigned char)(*(offsetBMP+2)+ ((mixcolor&0x00ff0000)>>16))/2;
	*(offsetNEWBMP+3)=0;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//tcontrol->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,nw,y,32,nw*4);
#endif
#if WIN32
//CreateBitmap x,y is actual (not padded)
//tcontrol->hbmp[0]= CreateBitmap(x,y,1,32,bdataCTL);
#endif

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapAndShade\n");
}

return(bmp);
} //FT_LoadBitmapAndShade


//Load an image to one of the FTControl images (and resize, or shade depending on flags)
int LoadImageFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
	char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadImageFileToFTControl\n");
}

#if Linux
	if (strcasecmp(&filename[strlen(filename)-3],"bmp")==0) {
#endif
#if WIN32
if (stricmp(&filename[strlen(filename)-3],"bmp")==0) {
#endif
	//image if bitmap
	if (!(LoadBitmapFileToFTControl(filename,tcontrol,flags,mixcolor))) {
		fprintf(stderr,"Error: LoadBitmapFileToFTControl failed\n");
		return(0);
	} else {
		return(1);
	}
} else { //if (stricmp(filename,strlen(filename)-3,"bmp")==0) {
#if Linux
	if (strcasecmp(&filename[strlen(filename)-3],"jpg")==0) {
#endif
#if WIN32
	if (stricmp(&filename[strlen(filename)-3],"jpg")==0) {
#endif
		if (!(LoadJPGFileToFTControl(filename,tcontrol,flags,mixcolor))) {
			fprintf(stderr,"Error: LoadJPGFileToFTControl failed\n");
			return(0);
		} else {
			return(1);
		}
	} else { //if (stricmp(filename,strlen(filename)-3,"jpg")==0) {
		fprintf(stderr,"Error: In LoadImageFileToFTControl(): Support for file type for %s not implemented.\n",filename);
		sprintf(tstr,"Error: In LoadImageFileToFTControl(): Support for file type for %s not implemented.\n",filename);
		FTMessageBox(tstr,FTMB_OK,"Error",0);
		return(0);
	} ////if (stricmp(filename,strlen(filename)-3,"jpg")==0) {
} //if (stricmp(filename,strlen(filename)-3,"bmp")==0) {

}  //int FT_LoadImageFileToFTControl(char *filename,FTControl *tcontrol,unsigned int Flags)

//currently only for 24 bit images
//not scalable
//maybe make FT_LoadBitmapToFTControlScale - Scale to FTControl
//int FT_LoadBitmapToFTControl(FTControl *tcontrol,unsigned char *bmfile,int ResizeImage)
int LoadBitmapToFTControl(unsigned char *bmpfile,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdataBMP,*bdataCTL,*bmp;
int PaddedBitmapWidth,PaddedControlBitmapWidth,i,j,bmphsize,bidx,BitmapDepthInBytes,DisplayDepthInBytes;
unsigned char *offsetBMP,*offsetCTL;
int x,y,bmpx,bmpy,wordalign;
float rx,ry;
int ImageNum;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapToFTControl\n");
}

if (bmpfile==0) {
	fprintf(stderr,"Error: LoadBitmapToFTControl called with bitmap data=0\n");
	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End LoadBitmapToFTControl\n");
	}

}


bmf=(BITMAPFILEHEADER *)bmpfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",bmpfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmpfile+sizeof(BITMAPFILEHEADER));
bdataBMP=(unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER));

BitmapDepthInBytes=bmi->biBitCount/8;

bmpx=bmi->biWidth;
bmpy=bmi->biHeight;

PaddedBitmapWidth=bmpx*BitmapDepthInBytes;
wordalign=(int)fmodf((float)PaddedBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedBitmapWidth+=4-wordalign;
}

x=tcontrol->x2-tcontrol->x1;
y=tcontrol->y2-tcontrol->y1;

//do not scale
if (!(flags&FT_SCALE_IMAGE)) {
	rx=1.0;
	ry=1.0;
	//clip bmp to control if bitmap is larger than control
	if (bmpx>=x) {
		bmpx=x;
		PaddedBitmapWidth=x*BitmapDepthInBytes;
	} else {
		//clip control to bitmap
		x=bmpx;
	}
	if (bmpy>=y) {
		bmpy=y;
	} else {
	//clip control to bitmap
		y=bmpy;
	}


} else {	
//rx=ratio of x of original bitmap and destination (x or 0 if not scaled to control)
//ry=ratio of y of original bitmap and destination (y or 0 if not scaled to control)
	rx=(float)bmpx/(float)x;
	ry=(float)bmpy/(float)y;
} //!ResizeImage


//  fprintf(stderr,"depth=%d\n",depth);

ImageNum=flags&0x3;

//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
//see if we need to pad control bitmap
#if Linux
//XCreateImage only works for 8, 16, or 32 bit depths (not 24)
DisplayDepthInBytes=4;
#endif
#if WIN32
DisplayDepthInBytes=PStatus.depth/8;
#endif
PaddedControlBitmapWidth=x*DisplayDepthInBytes;
wordalign=(int)fmodf((float)PaddedControlBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedControlBitmapWidth+=4-wordalign;
}

bmp=(unsigned char *)malloc(PaddedControlBitmapWidth*y+bmphsize); //control bitmap has the same depth as the current display setting

tcontrol->image[ImageNum]=bmp;
tcontrol->bmpinfo[ImageNum]=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
//add pointer to bitmap data to control data structure
tcontrol->bmpdata[ImageNum]=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//bmf->bfSize=bmphsize+x*y*4;
bmf->bfSize=bmphsize+PaddedControlBitmapWidth*y;
memcpy(bmp,(unsigned char *)bmf,sizeof(BITMAPFILEHEADER));
//adjust bpp
#if Linux
bmi->biBitCount=32;//can only be 32 for createximage was-PStatus.depth;
#endif
#if WIN32
bmi->biBitCount=PStatus.depth;//can only be 32 for createximage was-PStatus.depth;
#endif
bmi->biWidth=x;
bmi->biHeight=y;
//bmi->biSizeImage=x*y*4;
bmi->biSizeImage=PaddedControlBitmapWidth*y;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)bmi,sizeof(BITMAPINFOHEADER));
bdataCTL=bmp+bmphsize;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//copy, scale, and shade bitmap data to control image
if (flags&FT_SHADE_IMAGE) {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
		offsetBMP=bdataBMP+((bmpy-(int)(i*ry))-1)*PaddedBitmapWidth+((int)(j*rx))*BitmapDepthInBytes;
		offsetCTL=bdataCTL+bidx;
		*(offsetCTL)=(*offsetBMP+ (mixcolor&0x000000ff))/2;
		*(offsetCTL+1)=(*(offsetBMP+1)+ ((mixcolor&0x0000ff00)>>8))/2;
		*(offsetCTL+2)=(*(offsetBMP+2)+ ((mixcolor&0x00ff0000)>>16))/2;
		if (DisplayDepthInBytes==4) {
			*(offsetCTL+3)=0;
		}
	  }  //end j
	}  //end i
} else {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
		offsetBMP=bdataBMP+((bmpy-(int)(i*ry))-1)*PaddedBitmapWidth+((int)(j*rx))*BitmapDepthInBytes;
		offsetCTL=bdataCTL+bidx;
		memcpy(offsetCTL,offsetBMP,3);
		if (DisplayDepthInBytes==4) {
			*(offsetCTL+3)=0;
		}
	  }  //end j
	}  //end i
} //if (flags&FT_SHADE_IMAGE) {

//add border to control bitmap
if (flags&FT_ADD_BORDER_TO_IMAGE) {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		  if (i==0 || i==y-1 || j==0 || j==x-1) {
			bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
			offsetCTL=bdataCTL+bidx;
			*(unsigned int *)offsetCTL=mixcolor;
		  } //if (i==0 || i==y-1 || j==0 || j==x-1) {
	  }  //end j
	}  //end i

} //if (flags&FT_ADD_BORDER_TO_IMAGE) {

//add dashed-border
if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {

} //if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {


//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,nw,y,32,nw*4);
//nw is now the 32-bit aligned width*bpp
//tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,nw/4,y,32,nw);
//bitmap must  be 32 it depth, but first depth in XCreateImage must match the display depth which can be 24
tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,x,y,32,PaddedControlBitmapWidth);

#endif
#if WIN32
//CreateBitmap x,y is actual (not padded)
tcontrol->hbmp[ImageNum]= CreateBitmap(x,y,1,PStatus.depth,bdataCTL);
#endif


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapToFTControl\n");
}

//    fprintf(stderr,"Got Image %p\n",img);
//return(tximage);
//return(bmp);
return(1);
} //FT_LoadBitmapToFTControl



//currently only for 16, 24 or 32 bit images
//Resize=1, bitmap is scaled to fit control
//int FT_LoadBitmapFileToFTControl(FTControl *tcontrol,char *bmpname,int ResizeImage)
int LoadBitmapFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bmp,*bdata;
int PaddedBitmapWidth,wordalign,BitmapDepthInBytes,bmpx,bmpy;
char tstr[FTMedStr];

//NOTE - this probably needs to be changed - see LoadBitmapToFTWindow for working version

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapFileToFTControl\n");
}


fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"Done LoadBitmapFileToFTControl\n");
	}
	return(0);
}


//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

BitmapDepthInBytes=bmi.biBitCount/8;

bmpx=bmi.biWidth;
bmpy=bmi.biHeight;

PaddedBitmapWidth=bmpx*BitmapDepthInBytes;
wordalign=(int)fmodf((float)PaddedBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedBitmapWidth+=4-wordalign;
}

//allocate bitmap in memory
bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PaddedBitmapWidth*bmi.biHeight);
//and copy file and info header into memory
memcpy(bmp,&bmf,sizeof(BITMAPFILEHEADER));
memcpy(bmp+sizeof(BITMAPFILEHEADER),&bmi,sizeof(BITMAPINFOHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//read data into memory
fread(bdata,1,PaddedBitmapWidth*bmi.biHeight,fptr);
fclose(fptr);

//load the bitmap in memory to the control, scaling to control if ResizeImage is set
LoadBitmapToFTControl(bmp,tcontrol,flags,mixcolor);

free(bmp); //free original image

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Done LoadBitmapFileToFTControl\n");
}

//return(tximage);
return(1);
}  //FT_LoadBitmapFileToFTControl


void WriteBitmap(unsigned char *name, unsigned char *bmfile) 
{
FILE *fptr;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmd;
int i,bpp,h,linelen;

bmf=(BITMAPFILEHEADER *)bmfile;
bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));
bmd=(unsigned char *)bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

//fprintf(stdout,"bitmap %s file size= %d\n",name,bmf->bfSize);
//fprintf(stdout,"bitmap image size= %d\n",bmi->biSizeImage);

fptr=fopen((char*)name,"wb");
if (fptr==0) {
	fprintf(stdout,"Could not open file %s for writing bitmap\n",name);
} else {
//have to write in little endian - fwrite does not know to use structures, such as the BITMAPFILEHEADER structure

//fprintf(stdout,"bmf->bfType=%x fptr=%x\n",bmf->bfType,fptr);
	fwrite(&bmf->bfType,1,2,fptr);
//fprintf(stdout,"bmf->bfType=%x\n",bmf->bfType);
	fwrite(&bmf->bfSize,1,4,fptr);
	fwrite(&bmf->bfReserved1,1,2,fptr);
	fwrite(&bmf->bfReserved2,1,2,fptr);
	fwrite(&bmf->bfOffBits,1,4,fptr);

//fprintf(stdout,"bmi->biSize=%x\n",bmi->biSize);
	fwrite(&bmi->biSize,1,4,fptr);
	fwrite(&bmi->biWidth,1,4,fptr);
	fwrite(&bmi->biHeight,1,4,fptr);
	fwrite(&bmi->biPlanes,1,2,fptr);
	fwrite(&bmi->biBitCount,1,2,fptr);
	fwrite(&bmi->biCompression,1,4,fptr);
	fwrite(&bmi->biSizeImage,1,4,fptr);
	fwrite(&bmi->biXPelsPerMeter,1,4,fptr);
	fwrite(&bmi->biYPelsPerMeter,1,4,fptr);
	fwrite(&bmi->biClrUsed,1,4,fptr);
	fwrite(&bmi->biClrImportant,1,4,fptr);

	//fwrite(bmd,bmi->biSizeImage,1,fptr);
//turn bmpdata upside down
bpp=bmi->biBitCount/8;
h=bmi->biHeight;
linelen=bmi->biWidth*bpp;
for(i=0;i<h;i++) {
	fwrite(bmd+(h-i-1)*linelen,linelen,1,fptr);
} //i

} //fopen
	fclose(fptr);

//fprintf(stdout,"Done WriteBitmap\n");
} //WriteBitmap

//currently only for 24 bit images
//ResizeWindow=0 resize bmp to fit window, 1= resize window to fit bmp
//possibly change ResizeWindow to flags or Settings and (FT_LoadBitmap_RESIZEWINDOW)
//probably need faster function, without malloc, 
//possibly use sws_scale to scale BMP
int LoadBitmapToFTWindow(unsigned char *bmfile,FTWindow *twin,int ResizeWindow)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,bmphsize,bidx;
unsigned char *offset;
int x,y;
float rx,ry;

//add scaling (rx,ry)

//bitmap should already have all header info

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapToFTWindow\n");
}

if (bmfile==0 || twin<=0) {
  fprintf(stderr,"Error: LoadBitmapToFTWindow passed NULL pointer\n"); 
	return 0;
}

bmf=(BITMAPFILEHEADER *)bmfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"bmp does not start with 0x4d42 (but with %x).\n",*(unsigned short *)bmfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));

//if (twin->bmpinfo!=0) free(twin->bmpinfo);
//bmpinfo should probably be static - not pointer
//if (twin->bmpinfo==0) {  //don't bother to free+malloc again if memory is already malloc'd
//	twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//}
//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));

//fprintf(stderr,"w=%d h=%d\n",bmi->biWidth,bmi->biHeight);
//fprintf(stderr,"bitcount=%d\n",bmi->biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi->biBitCount/8;

nw=(int)fmodf((float)bmi->biWidth*bc,4.0);
if (nw>0) {
	nw=bmi->biWidth+4-nw;
} else {
  nw=bmi->biWidth;
}

if (ResizeWindow) {
	if (twin->w!=bmi->biWidth || twin->h!=bmi->biHeight) {
		twin->w=bmi->biWidth;
		twin->h=bmi->biHeight;
#if Linux
		if (twin->flags&WOpen && twin->xwindow!=0) {
#endif
#if WIN32
		if (twin->flags&WOpen && twin->hwindow!=0) {
#endif
			//we need to resize an existing window
			ResizeFTWindow(twin);
		} //WOpen

	}
}

//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi->biWidth;
  twin->h=bmi->biHeight;
}

//twin at 32 bpp should always be 4-byte aligned - but could be a problem on other display resolutions
x=twin->w;
y=twin->h;


//if (!ResizeWindow) {
	//rx=(float)nw/((float)twin->x*(float)bc);
	//1.0 so no overlap
	rx=((float)nw-1.0)/(float)x;
	ry=((float)bmi->biHeight-1.0)/(float)y;

//fprintf(stderr,"rx=%f ry=%f w=%d h=%d\n",rx,ry,x,y);
//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//} 

bdata=(unsigned char *)bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned

//if (twin->bmpdata!=0) {
//	free(twin->bmpdata); //free any previous mallc'd bitmap data
//}
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now only use depth 3
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)((unsigned char *)bmp+sizeof(BITMAPFILEHEADER));
bmf->bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi->biBitCount=32;
bmi->biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

twin->bmpdata=bdata2; //store in order to track to free
//memset(bdata2,0,sizeof(x*y*4));


//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);

//#if PINFO
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work
		//offset=bdata;


for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
		bidx=i*x*4+j*4;
    
		if (!ResizeWindow) {
			offset=bdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
		}		else {
			offset=bdata+i*nw*bc+j*bc;
		}

  //  fprintf(stderr,"%p ",offset);
//		memcpy(bdata2+x4+y4,offset,3);
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
    *(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl



//fprintf(stderr,"before XCreateImage\n");
//display and bitmap must be same depth (bits per pixel)?
//XCreateImage does not allocate space for the image itself
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(nw,y,1,32,bdata2);
#endif

//twin->image[0]=tximage;
//and DrawFTWindow?
//DrawFTWindow(twin);

//can free bdata and bdata2 - does XCreateImage do its own malloc?
//yes I think so, use XDestroyImage to free XImage
//but free(bdata2) causes X crashes
//free(bdata); //presumes bdata was malloc'd
//free(bdata2); //free original image

//how and when can we free bdata2?


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapToFTWindow\n");
}

//    fprintf(stderr,"Got Image %p\n",img);
//return(tximage);
return(1);

} //LoadBitmapToFTWindow



//todo: Needs to create BITMAPFILEHEADER assign to twin->image[0], BITMAPINFOHEADER assign to twin->bmpinfo

//LoadBitmapDataToFTWindow() - Loads Bitmap data to window (expects bmfile to have no header info as LoadBitmapToFTWindow does).
//currently only for 24 bit images
//ResizeWindow=0 resize bmp to fit window, 1= resize window to fit bmp
//possibly change ResizeWindow to flags or Settings and (FT_LoadBitmap_RESIZEWINDOW)
//possibly use sws_scale to scale BMP
unsigned char * LoadBitmapDataToFTWindow(unsigned char *bmpdata,FTWindow *twin,int ResizeWindow,int width,int height,int depth)
{
//BITMAPFILEHEADER *bmf;
//BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdata2;//,*bdata;
int nw,i,j,bc,bidx;
//XImage *img;
unsigned char *offset;
int x,y;
float rx,ry;

//add scaling (rx,ry)


if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapDataToFTWindow\n");
}

if (bmpdata==0 || twin<=0) {
  fprintf(stderr,"Error: LoadBitmapDataToFTWindow passed NULL pointer\n"); 
	return 0;
}




//fprintf(stderr,"w=%d h=%d\n",bmi->biWidth,bmi->biHeight);
//fprintf(stderr,"bitcount=%d\n",bmi->biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=depth/8;

nw=(int)fmodf((float)width*bc,4.0);
if (nw>0) {
	nw=width+4-nw;
} else {
  nw=width;
}

if (ResizeWindow) {
	if (twin->w!=width || twin->h!=height) {
		twin->w=width;
  		twin->h=height;
#if Linux
		if (twin->flags&WOpen && twin->xwindow!=0) {
#endif
#if WIN32
		if (twin->flags&WOpen && twin->hwindow!=0) {
#endif
			//we need to resize an existing window
			ResizeFTWindow(twin);
		} //WOpen

	}
}

//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=width;
  twin->h=height;
}

//twin at 32 bpp should always be 4-byte aligned - but could be a problem on other display resolutions
x=twin->w;
y=twin->h;


//if (!ResizeWindow) {
	//rx=(float)nw/((float)twin->x*(float)bc);
	//1.0 so no overlap
	rx=((float)nw-1.0)/(float)x;
	ry=((float)height-1.0)/(float)y;

//fprintf(stderr,"rx=%f ry=%f w=%d h=%d\n",rx,ry,x,y);
//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//} 

//bdata=bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned

if (twin->bmpdata!=0) {
	free(twin->bmpdata); //free any previous mallc'd bitmap data
}
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
twin->bmpdata=bdata2; //store in order to track to free
//memset(bdata2,0,sizeof(x*y*4));


//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);

//#if PINFO
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work
		//offset=bdata;
//do 8 and 16 bit seperately

switch(depth) {
	case 8:	//8 bit
		for(i=0;i<y;i++) {
					//  fprintf(stderr,"\n\n");
					for(j=0;j<x;j++) {
						bidx=i*x*4+j*4;
				
						if (!ResizeWindow)
							offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
						else 
							offset=bmpdata+i*nw*bc+j*bc;

					//  fprintf(stderr,"%p ",offset);
				//		memcpy(bdata2+x4+y4,offset,3);
						*(bdata2+bidx)=*offset;
						*(bdata2+bidx+1)=*(offset);
						*(bdata2+bidx+2)=*(offset);
				    *(bdata2+bidx+3)=0;
					}  //end j
				}  //end i
				//scale image to FTControl
		break;
	case 16: //16 bit
		for(i=0;i<y;i++) {
			//  fprintf(stderr,"\n\n");
			for(j=0;j<x;j++) {
				bidx=i*x*4+j*4;
				
				if (!ResizeWindow)
					offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
				else 
					offset=bmpdata+i*nw*bc+j*bc;

			//  fprintf(stderr,"%p ",offset);
		//		memcpy(bdata2+x4+y4,offset,3);
					//presumes 454
				*(bdata2+bidx)=*offset&0xf;
				*(bdata2+bidx+1)=((*(offset)&0xd0)>>5)|((*(offset+1)&0x3)<<5);
				*(bdata2+bidx+2)=((*(offset+1)&0xfc)>>2);
		    *(bdata2+bidx+3)=0;
			}  //end j
		}  //end i
		//scale image to FTControl
		break;
	case 24:
	case 32:

		for(i=0;i<y;i++) {
			//  fprintf(stderr,"\n\n");
			for(j=0;j<x;j++) {
				bidx=i*x*4+j*4;
				
				if (!ResizeWindow)
					offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
				else 
					offset=bmpdata+i*nw*bc+j*bc;

			//  fprintf(stderr,"%p ",offset);
		//		memcpy(bdata2+x4+y4,offset,3);
				*(bdata2+bidx)=*offset;
				*(bdata2+bidx+1)=*(offset+1);
				*(bdata2+bidx+2)=*(offset+2);
		    *(bdata2+bidx+3)=0;
			}  //end j
		}  //end i
		//scale image to FTControl
		break; //24,32
	} //switch


//fprintf(stderr,"before XCreateImage\n");
//display and bitmap must be same depth (bits per pixel)?
//XCreateImage does not allocate space for the image itself
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,nw*4);
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(nw,y,1,32,bdata2);
#endif

//if (twin->image[0]!=0) {
//	XDestroyImage(twin->image[0]);  //causes segfault
//}

//and DrawFTWindow?
//DrawFTWindow(twin);

//can free bdata and bdata2 - does XCreateImage do its own malloc?
//yes I think so, use XDestroyImage to free XImage
//but free(bdata2) causes X crashes
//free(bdata); //presumes bdata was malloc'd
//free(bdata2); //free original image


//Presumably DrawWindow will draw image
//perhaps DrawFTWindow should be called here

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapDataToFTWindow\n");
}

return(bdata2);
} //LoadBitmapDataToFTWindow


//currently only for 24 bit images
int LoadBitmapFileToFTWindow(char *filename,FTWindow *twin)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int PaddedBitmapWidth,PaddedWindowBitmapWidth,i,j,BitmapDepthInBytes,WindowDepthInBytes,bmphsize;
//XImage *img;
unsigned char *offset;
int x,y,bidx;
float rx,ry;
char tstr[FTMedStr];


//NOTE THIS CODE IS INACCURATE - remove rx,ry - fix nw and loop

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapFileToFTWindow %s\n",filename);
}

fptr=fopen(filename,"rb");
if (fptr==0) {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End LoadBitmapFileToFTWindow\n");
	}
	return(0);
}


//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,2,1,fptr);
if (bmf.bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",filename);
  return 0;
}
fread(&bmf.bfSize,4,1,fptr);
fread(&bmf.bfReserved1,2,1,fptr);
fread(&bmf.bfReserved2,2,1,fptr);
fread(&bmf.bfOffBits,4,1,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,4,1,fptr);
fread(&bmi.biWidth,4,1,fptr);
fread(&bmi.biHeight,4,1,fptr);
fread(&bmi.biPlanes,2,1,fptr);
fread(&bmi.biBitCount,2,1,fptr);
fread(&bmi.biCompression,4,1,fptr);
fread(&bmi.biSizeImage,4,1,fptr);
fread(&bmi.biXPelsPerMeter,4,1,fptr);
fread(&bmi.biYPelsPerMeter,4,1,fptr);
fread(&bmi.biClrUsed,4,1,fptr);
fread(&bmi.biClrImportant,4,1,fptr);

BitmapDepthInBytes=bmi.biBitCount/8;
PaddedBitmapWidth=(int)fmodf((float)bmi.biWidth*BitmapDepthInBytes,4.0);
if (PaddedBitmapWidth>0)	{
	PaddedBitmapWidth=bmi.biWidth+4-PaddedBitmapWidth;
} else {
  PaddedBitmapWidth=bmi.biWidth;
}

//read data
bdata=(unsigned char *)malloc(PaddedBitmapWidth*bmi.biHeight);
fread(bdata,1,PaddedBitmapWidth*bmi.biHeight,fptr);
fclose(fptr);



//now determine bitmap for window padding
//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi.biWidth;
  twin->h=bmi.biHeight;
}

x=twin->w;
y=twin->h;

#if Linux
//XCreateImage only works for 8, 16, or 32 bit depths (not 24)
WindowDepthInBytes=4;
#endif
#if WIN32
WindowDepthInBytes=PStatus.depth/8;
#endif
PaddedWindowBitmapWidth=(int)fmodf((float)twin->x*WindowDepthInBytes,4.0);
if (PaddedWindowBitmapWidth>0)	{
	PaddedWindowBitmapWidth=twin->x+WindowDepthInBytes-PaddedWindowBitmapWidth;
} else {
  PaddedWindowBitmapWidth=twin->x*WindowDepthInBytes;
}

bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(PaddedWindowBitmapWidth*y+bmphsize); 
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
twin->bmpdata=(unsigned char *)twin->bmpinfo+sizeof(BITMAPINFOHEADER);
bmf.bfSize=bmphsize+PaddedWindowBitmapWidth*y;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=PStatus.depth;
bmi.biSizeImage=PaddedWindowBitmapWidth*y;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

//rx=(float)PaddedBitmapWidth/((float)x*(float)bc);
//ry=(float)bmi.biHeight/(float)y;
rx=(float)PaddedBitmapWidth/(float)PaddedWindowBitmapWidth;
ry=(float)bmi.biHeight/(float)y;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
#if PINFO
fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work


//for(i=y-1;i>-1;i--) {
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
	//for(j=x-1;j>-1;j--) {
//    offset=bdata+((int)((y-i-1)*ry))*PaddedBitmapWidth+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
//    *(bdata2+i*x*4+j*4)=*offset;
//    *(bdata2+i*x*4+j*4+1)=*(offset+1);
//    *(bdata2+i*x*4+j*4+2)=*(offset+2);
		bidx=(i*x+j)*WindowDepthInBytes;
    
		offset=bdata+((int)((float)(y-i-1)*ry))*PaddedBitmapWidth*BitmapDepthInBytes+(((int)((float)j*rx))*BitmapDepthInBytes);

  //  fprintf(stderr,"%p ",offset);
    //*(bdata2+x4+y4)=*offset;
    //*(bdata2+x4+y4+1)=*(offset+1);
    //*(bdata2+x4+y4+2)=*(offset+2);
		*(bdata2+bidx)=*offset;
		*(bdata2+bidx+1)=*(offset+1);
		*(bdata2+bidx+2)=*(offset+2);
		*(bdata2+bidx+3)=0;

  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,WindowDepthInBytes,PaddedWindowBitmapWidth);
#endif
#if WIN32
twin->hbmp= CreateBitmap(PaddedWindowBitmapWidth,y,1,WindowDepthInBytes,bdata2);
#endif

free(bdata); //free original image


//    fprintf(stderr,"Got Image %p\n",img);
if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapFileToFTWindow\n");
}

return 1;
} //LoadBitmapFileToFTWindow




//NOTE: I was initially going to convert all JPG encoding/decoding to FFMPEG Libraries 
//but libjpeg will function for compressing/decompressing jpg and making and reading mjpg files
#if 0 
//currently only for 24 bit images
XImage * FT_LoadJPGFile(char *filename)
{
//BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int i,j;
//int nw,i,j,bc;
//XImage *img;
//unsigned char *offset;
//float rx,ry;
int x,y;
//JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
//int image_height;	/* Number of rows in image */
//int image_width;		/* Number of columns in image */
JSAMPARRAY buffer;		/* Output row buffer */
//JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
int row_stride;		/* physical row width in image buffer */
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
//struct jpeg_decompress_struct cinfo;
struct jpeg_decompress_struct dinfo;
//struct my_error_mgr jerr;
struct jpeg_error_mgr jerr;
//typedef struct my_error_mgr *my_error_ptr;
//struct my_error_mgr           jerr;
XImage *tximage;


if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow %s\n",filename);


if ((fptr = fopen(filename, "rb")) == NULL) {
            fprintf(stderr, "can't open %s\n", filename);
            return(0);
        }



	dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
//fprintf(stderr,"before jpeg_stdio_src\n");
        dinfo.output_components = 3;
        dinfo.out_color_space = JCS_RGB;

        dinfo.dct_method = JDCT_FASTEST;  //need?

        jpeg_stdio_src(&dinfo, fptr);
//fprintf(stderr,"before read header\n");
        (void) jpeg_read_header(&dinfo, TRUE);
//fprintf(stderr,"before decompress\n");
        jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width * dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);


  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*dinfo.mem->alloc_sarray)
		((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//   buffer=malloc(x*4);



//fprintf(stderr,"before while\n");
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (dinfo.output_scanline < dinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&dinfo, buffer, 1);
//    (void) jpeg_read_scanlines(&cinfo, bdata+cinfo.output_scanline*x, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//    put_scanline_someplace(buffer[0], row_stride);
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
    memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
  }

   jpeg_finish_decompress(&dinfo);

   fclose(fptr);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

memset(&bmi,0,sizeof(BITMAPINFOHEADER));
//only need width and height
bmi.biWidth=x;
bmi.biHeight=y;

//if (twin->bmpinfo!=0) free(twin->bmpinfo);
//twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));

//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));
//
//scale image to contorl
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    *(bdata2+i*x*4 + j*4+2)=*(bdata+i*x*3 + j*3);
    *(bdata2+i*x*4 + j*4+1)=*(bdata+i*x*3 + j*3+1);
    *(bdata2+i*x*4 + j*4)=*(bdata+i*x*3 + j*3+2);
    *(bdata2+i*x*4 + j*4+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?
/*
     The XCreateImage function allocates the memory needed for an XImage
       structure for the specified display but does not allocate space for the
       image itself.  Rather, it initializes the structure byte-order, bit-
       order, and bitmap-unit values from the display and returns a pointer to
       the XImage structure.
*/
tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
/*
      Note that when the image is created using XCreateImage, XGetImage, or
       XSubImage, the destroy procedure that the XDestroyImage function calls
       frees both the image structure and the data pointed to by the image
       structure.
*/
free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);


//}//end if bitmap file found
if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow complete\n");

return(tximage);

}  //end FT_LoadJPGFile
#endif


//currently only for 24 bit images
int LoadJPGFileToFTWindow(char *filename,FTWindow *twin)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*offset,*offset2;
int i,j;
int x,y,bmpsize;
//JSAMPARRAY buffer;		/* Output row buffer */
int row_stride;		/* physical row width in image buffer */
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
struct jpeg_decompress_struct dinfo;
struct jpeg_source_mgr mgr;
struct jpeg_error_mgr jerr;
int numbytes,jpglen,ex;
unsigned char *jpgdata,*bmp;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadJPGFileToFTWindow %s\n",filename);
}


if ((fptr=fopen(filename,"rb")) == NULL) {
	sprintf(tstr,"LoadJPGFileToFTWindow error: %s could not be opened.\n",filename);
	FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);

    fprintf(stderr,"Can't open %s\n",filename);
    return(0);
    }

//read in JPEG data
jpglen=0;
numbytes=0;
ex=0;
jpgdata=(unsigned char *)malloc(FT_MAXJPGSIZE);
while(!ex) {
	numbytes=fread(jpgdata+jpglen,1,512,fptr);
	if (numbytes<=0) {
		ex=1;
	} else {
		jpglen+=numbytes;
	}
} //while !ex
fclose(fptr);


PStatus.jpgbufdata=jpgdata;
PStatus.jpgbufsize=jpglen;


memset(&dinfo,0,sizeof(dinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));


  jpeg_create_decompress(&dinfo);

	mgr.next_input_byte=jpgdata;
  mgr.bytes_in_buffer=jpglen;
  mgr.init_source=mem_init_source;
  mgr.fill_input_buffer=mem_fill_input_buffer;
  mgr.skip_input_data=mem_skip_input_data;
  mgr.resync_to_restart=mem_resync_to_restart;
  mgr.term_source=mem_term_source;
//fprintf(stderr,"before jpeg_stdio_src\n");

	dinfo.err=jpeg_std_error(&jerr);
	dinfo.src=&mgr;

	dinfo.output_components = 3;
  dinfo.out_color_space = JCS_RGB;
	dinfo.dct_method = JDCT_FASTEST;  //need?

	//jpeg_stdio_src(&dinfo,fptr);
//fprintf(stderr,"before read header\n");
	jpeg_read_header(&dinfo,TRUE);
//fprintf(stderr,"before decompress\n");
  jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width*dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);  //*4 for word-align padding

	for(j=0;j<y;j++) {
		offset=bdata+x*j*3;
		jpeg_read_scanlines(&dinfo,(JSAMPARRAY)&offset,1);
	}  //for j


  /* Make a one-row-high sample array that will go away when done with image */
  //buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//fprintf(stderr,"before while\n");
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
//  while (dinfo.output_scanline < dinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
//    jpeg_read_scanlines(&dinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
 //   memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
 // }

   jpeg_finish_decompress(&dinfo);
	 jpeg_destroy_decompress(&dinfo);
   
	 free(jpgdata);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

//fprintf(stderr,"jpg x=%d y=%d\n",x,y);
bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+x*y*4);
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)(twin->image+sizeof(BITMAPFILEHEADER));
//add pointer to bitmap data to FTWindow data structure
twin->bmpdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));


bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+x*y*4;
//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=x;
bmi->biHeight=y;
bmi->biPlanes=1;
bmi->biBitCount=32;
bmi->biSizeImage=4*x*y;

bdata2=bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);



//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi->biWidth;
  twin->h=bmi->biHeight;
}

//if (twin->bmpinfo!=0) {
//	free(twin->bmpinfo);
//}
//twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,bmi,sizeof(BITMAPINFOHEADER));

//scale image to control

//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
		offset=bdata+(i*x + j)*3;
		offset2=bdata2+(i*x + j)*4;
    *(offset2+2)=*bdata;
    *(offset2+1)=*(bdata+1);
    *(offset2)=*(bdata+2);
    *(offset2+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?
/*
     The XCreateImage function allocates the memory needed for an XImage
       structure for the specified display but does not allocate space for the
       image itself.  Rather, it initializes the structure byte-order, bit-
       order, and bitmap-unit values from the display and returns a pointer to
       the XImage structure.
*/
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(x,y,1,32,bdata2);
//twin->hbmp=CreateDIBSection(twin->hdcWin,(BITMAPINFO *)twin->bmpinfo,DIB_RGB_COLORS,(void **)bdata2,NULL,NULL);
//SelectObject(twin->hdcWin,
//twin->hbmp=CreateCompatibleBitmap(twin->hdcWin,x,y);
//SetDIflags(twin->hdcWin,twin->hbmp,0,y,bdata2,(BITMAPINFO *)twin->bmpinfo,DIB_RGB_COLORS);
#endif
//twin->image=bmp;
/*
      Note that when the image is created using XCreateImage, XGetImage, or
       XSubImage, the destroy procedure that the XDestroyImage function calls
       frees both the image structure and the data pointed to by the image
       structure.
*/
free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);



//}//end if bitmap file found
if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadJPGFileToFTWindow complete\n");
}

return(1);
//return(bmp);
}  //end LoadJPGFileToFTWindow


//need to convert to ffmpeg
//currently only for 24 bit images
int LoadJPGFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
	int numbytes, jpglen, ex;
	unsigned char *jpgdata;
	FILE *fptr;
	char tstr[FTMedStr];

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"LoadJPGFileToFTControl %s\n",filename);
	}

	if ((fptr = fopen(filename, "rb")) == NULL) {
		sprintf(tstr,"LoadJPGFileToFTWindow error: %s could not be opened.\n",filename);
		FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);
		fprintf(stderr, "can't open %s\n", filename);
		return(0);
		}

	//read in JPEG data
	jpglen=0;
	numbytes=0;
	ex=0;
	jpgdata=(unsigned char *)malloc(FT_MAXJPGSIZE);
	while(!ex) {
		numbytes=fread(jpgdata+jpglen,1,512,fptr);
		if (numbytes<=0) {
			ex=1;
		} else {
			jpglen+=numbytes;
		}
	} //while !ex
	fclose(fptr);

	LoadJPGToFTControl(jpgdata, jpglen, tcontrol, flags, mixcolor);

	free(jpgdata);

//    fprintf(stderr,"Got Image %p\n",img);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"LoadJPGFileToFTControl complete\n");
	}

	return(1);
}  //end LoadJPGFileToFTControl


//load a jpg from memory to a control (currently only control->image[0])
int LoadJPGToFTControl(unsigned char *jpgdata, int jpglen, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor)
{
	BITMAPFILEHEADER *bmf;
	BITMAPINFOHEADER *bmi;
	unsigned char *bdata, *bdata2, *offset, *offsetCTL, *bmp;
	int i, j;
	int x, y, jpgx, jpgy, bc, bc2, bmpsize;
	//JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in image buffer */
	//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
	struct jpeg_decompress_struct dinfo;
	struct jpeg_source_mgr mgr;
	struct jpeg_error_mgr jerr;
	int numbytes, ex;
	float rx, ry;
	int ImageNum;
#if 0//WIN32
	DWORD dwWaitResult;
#endif


	/*
	if (!CheckJPG(jpgdata, jpglen)) {
	return(0);
	}
	*/

	//we need to lock the DrawFTControls mutex so that we are not writing over image data that is currently being drawn
#if 0//WIN32
	//request the mutex
	dwWaitResult = WaitForSingleObject(
		PStatus.DrawFTControlsMutex,    // handle to mutex
		INFINITE);  // no time-out interval

	if (dwWaitResult != WAIT_OBJECT_0) {
		return;
	}
#endif


	PStatus.jpgbufdata = jpgdata;
	PStatus.jpgbufsize = jpglen;


	memset(&dinfo, 0, sizeof(dinfo));
	memset(&jerr, 0, sizeof(jerr));
	memset(&mgr, 0, sizeof(mgr));


	jpeg_create_decompress(&dinfo);

	mgr.next_input_byte = jpgdata;
	mgr.bytes_in_buffer = jpglen;
	mgr.init_source = mem_init_source;
	mgr.fill_input_buffer = mem_fill_input_buffer;
	mgr.skip_input_data = mem_skip_input_data;
	mgr.resync_to_restart = mem_resync_to_restart;
	mgr.term_source = mem_term_source;

	dinfo.err = jpeg_std_error(&jerr);
	dinfo.src = &mgr;

	dinfo.output_components = 3;
	dinfo.out_color_space = JCS_RGB;
	dinfo.dct_method = JDCT_FASTEST;  //need?

	//jpeg_stdio_src(&dinfo,fptr);
	//fprintf(stderr,"before read header\n");
	jpeg_read_header(&dinfo, TRUE);
	//fprintf(stderr,"before decompress\n");
	jpeg_start_decompress(&dinfo);

	row_stride = dinfo.output_width*dinfo.output_components;
	//fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

	jpgx = dinfo.output_width;
	jpgy = dinfo.output_height;
	bdata = (unsigned char *)malloc(jpgx*jpgy * 4);  //just to take care of word align padding

	for (j = 0; j < jpgy; j++) {
		offset = bdata + jpgx*j * 3;
		jpeg_read_scanlines(&dinfo, (JSAMPARRAY)&offset, 1);
	}  //for j


	/* Make a one-row-high sample array that will go away when done with image */
	//buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

	//fprintf(stderr,"before while\n");
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	//  while (dinfo.output_scanline < dinfo.output_height) {
	/* jpeg_read_scanlines expects an array of pointers to scanlines.
	* Here the array is only one element long, but you could ask for
	* more than one scanline at a time if that's more convenient.
	*/
	//    jpeg_read_scanlines(&dinfo, buffer, 1);
	/* Assume put_scanline_someplace wants a pointer and sample count. */
	//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
	//  fprintf(stderr,"row_stride:%d\n",row_stride);
	//   memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
	//fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
	// }

	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);

	//free(jpgdata);

	x = tcontrol->x2 - tcontrol->x1;
	y = tcontrol->y2 - tcontrol->y1;

	ImageNum = flags & 0x3;
	//fprintf(stderr,"jpg x=%d y=%d\n",x,y);
	bmp = (unsigned char *)malloc(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y * 4);
	tcontrol->image[ImageNum] = bmp;
	tcontrol->bmpinfo[ImageNum] = (BITMAPINFOHEADER *)(bmp + sizeof(BITMAPFILEHEADER));
	//add pointer to bitmap data to control data structure
	tcontrol->bmpdata[ImageNum] = (unsigned char *)(bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	bmf = (BITMAPFILEHEADER *)bmp;
	bmi = (BITMAPINFOHEADER *)(bmp + sizeof(BITMAPFILEHEADER));
	bmpsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y * 4;
	//memset(bmf,0,sizeof(BITMAPFILEHEADER));
	bmf->bfType = 0x4d42;
	bmf->bfSize = bmpsize;
	bmf->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	memset(bmi, 0, sizeof(BITMAPINFOHEADER));
	//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
	bmi->biSize = sizeof(BITMAPINFOHEADER);
	bmi->biWidth = x;
	bmi->biHeight = y;
	bmi->biPlanes = 1;
	bmi->biBitCount = 32;
	bmi->biSizeImage = 4 * x*y;

	bdata2 = bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Note: jpeglib does not byte-align RGB data

	bc = 3; //jpg is always 3 bytes/pixel bmi->biBitCount/8;
	/*
	nw=(int)fmodf((float)jpgx*bc,4.0);
	if (nw>0)  {
	nw=jpgx*bc+4-nw;
	} else {
	nw=jpgx*bc;
	}
	*/

	bc2 = 4;
	//nw2=x*4;

	//  if (cx==0 && y==0) //scale to FTControl

	//scale image to FTControl
	//rx=(float)nw/((float)jx*(float)bc);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)nw/(float)(jx*bc2);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)(jx*bc2)/(float)nw;
	//rx=(float)nw2/(float)nw;
	//if (ResizeImage) {
	if (!(flags&FT_SCALE_IMAGE)) {
		rx = (float)jpgx / (float)x;
		ry = (float)jpgy / (float)y;//bmi.biHeight;
	}
	else {
		rx = 1.0;
		ry = 1.0;
		//if control is bigger than jpg
		if (x > jpgx) {
			x = jpgx;
		}
		if (y > jpgy) {
			y = jpgy;
		}
	}


	//todo: make a function for below and call - see FTLoadBitmapFileToControl


	if (flags&FT_SHADE_IMAGE) {
		//windows can stretchblt, if xwindows can too, then this is unnecessary
		//have to copy to 32 bit, even though display is 24 bit
		for (i = 0; i < y; i++) {
			//  fprintf(stderr,"\n\n");
			for (j = 0; j < x; j++) {
				offsetCTL = bdata2 + (i*x + j)*bc2;
				offset = bdata + (((int)(i*ry))*jpgx + ((int)(j*rx)))*bc;
				//offset=bdata+(i*jpgx+j)*bc;
				//  fprintf(stderr,"%p ",offset);
				*(offsetCTL) = (*offset + (mixcolor & 0x000000ff)) / 2;
				*(offsetCTL + 1) = (*(offset + 1) + ((mixcolor & 0x0000ff00) >> 8)) / 2;
				*(offsetCTL + 2) = (*(offset + 2) + ((mixcolor & 0x00ff0000) >> 16)) / 2;
				*(offsetCTL + 3) = 0;
			}  //j
		}  //i
	}
	else {
		for (i = 0; i < y; i++) {
			//  fprintf(stderr,"\n\n");
			for (j = 0; j < x; j++) {
				offsetCTL = bdata2 + (i*x + j)*bc2;
				offset = bdata + (((int)(i*ry))*jpgx + ((int)(j*rx)))*bc;
				//offset=bdata+(i*jpgx+j)*bc;
				//  fprintf(stderr,"%p ",offset);
				*offsetCTL = *(offset + 2);
				*(offsetCTL + 1) = *(offset + 1);
				*(offsetCTL + 2) = *offset;
				*(offsetCTL + 3) = 0;
			}  //j
		}  //i
	} //if (flags&FT_SHADE_IMAGE) {


	//add border
	if (flags&FT_ADD_BORDER_TO_IMAGE) {
		for (i = 0; i < y; i++) {
			for (j = 0; j < x; j++) {
				if (i == 0 || i == y - 1 || j == 0 || j == x - 1) {
					offsetCTL = bdata2 + (i*x + j)*bc2;
					*(unsigned int *)offsetCTL = mixcolor;
				} //if (i==0 || i==y-1 || j==0 || j==x-1) {
			}  //end j
		}  //end i

	} //if (flags&FT_ADD_BORDER_TO_IMAGE) {

	//add dashed-border
	if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {

	} //if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {



	//display and bitmap must be same depth (bits per pixel)?
	/*
	The XCreateImage function allocates the memory needed for an XImage
	structure for the specified display but does not allocate space for the
	image itself.  Rather, it initializes the structure byte-order, bit-
	order, and bitmap-unit values from the display and returns a pointer to
	the XImage structure.
	*/

#if Linux
	//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
	if (tcontrol->ximage[ImageNum]!=0) {
		XDestroyImage(tcontrol->ximage[ImageNum]);
	}
	tcontrol->ximage[ImageNum] = XCreateImage(PStatus.xdisplay, PStatus.visual, PStatus.depth, ZPixmap, 0, (char *)bdata2, x, y, 32, x * 4);
#endif
#if WIN32
	//free any existing image
	if (tcontrol->hbmp[ImageNum] != 0) {
		DeleteBitmap(tcontrol->hbmp[ImageNum]);
	}

	tcontrol->hbmp[ImageNum] = CreateBitmap(x, y, 1, 32, bdata2);
	//tcontrol->hbmp[ImageNum]=CreateDIBSection(PStatus.iwindow->hdcWin,(BITMAPINFO *)tcontrol->bmpinfo,DIB_RGB_COLORS,(void **)bdata2,NULL,NULL);
#endif
	//tcontrol->image=bmp;

	
#if 0//WIN32
	//end wait for mutex
	// Release ownership of the mutex object
	if (!ReleaseMutex(PStatus.DrawFTControlsMutex))
	{
		// Handle error.
	}
#endif //WIN32
	


	/*
	Note that when the image is created using XCreateImage, XGetImage, or
	XSubImage, the destroy procedure that the XDestroyImage function calls
	frees both the image structure and the data pointed to by the image
	structure.
	*/
	free(bdata); //free original image


	if (PStatus.flags&PInfo) {
		fprintf(stderr, "LoadJPGToFTControl complete\n");
	}

	return(1);
} //int LoadJPGToFTControl(char *ImageData, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor)

int CheckJPG(unsigned char *jpgdata, int jpglen)
{
	int i,endloop,len;

	//verify that jpgdata ends with FF D9
	if (jpglen <= 0) {
		fprintf(stderr, "Jpeg length <= 0");
		return(0);
	}

	//verify that jpgdata ends with FF D9
	if (jpgdata[0] != 0xff || jpgdata[1] != 0xd8) {
		fprintf(stderr, "Jpeg doesn't start with ffd8");
		return(0);
	}

	//verify that jpgdata ends with FF D9
	if (jpgdata[jpglen - 1] != 0xd9 || jpgdata[jpglen - 2] != 0xff) {
		fprintf(stderr, "Jpeg doesn't end with ffd9");
		return(0);
	}

	//check other markers
	i = 2;
	endloop = 0;
	while (!endloop) {
		switch (jpgdata[i]) {
		case 0xff: //marker
			switch (jpgdata[i + 1]) {
			case 0x00: //
			case 0x01: //
			case 0xd0: // 0xdn n=0-7 restart
			case 0xd1: //
			case 0xd2: //
			case 0xd3: //
			case 0xd4: //
			case 0xd5: //
			case 0xd6: //
			case 0xd7: //
				i+=2; //skip- no length
				break;
			case 0xd8: //start of image
				return(0); //two SOI markers
				break;
			case 0xdd: //DRI- payload=four bytes
				i += 5;
			case 0xd9: //EOI
				return(1);
				break;
			case 0xff: //every marker can be padded with any number of 0xff's
				i += 1; //skip ahead one for any 0xffff
				break;
			case 0xda: //start of scan
				//SOS, start of scan, 0xffda is followed by header length, but the size of the entropy data is not given
				//and there is no way I currently know of to verify the length of the "entropy" data
				//for now just exit once ffda is found
				/* I found that the below code, doesn't catch the jpeg library "premature end of data segment" popup window
				//for now scan to the end of the file, just to check against the known file length
				while (!endloop) {
					if (i+1 > jpglen) {
						return(0);
					}
					if (jpgdata[i] == 0xff && jpgdata[i + 1] == 0xd9) {
						if (i + 2 == jpglen) {
							return(1);
						}
						else {
							//ffd9 can be in the image data
							if (i+1 < jpglen) {
								i++;
							}
							else { //but if we are past the jpeglen
								return(0); //length scanned doesn't match file size
							}
						}
					}
					else {
						i++;
					} //if (jpgdata[i] = 0xff && jpgdata[i + 1] == 0xd9) {
				} //while
				*/
				return(1);
				break;
			default:

				//get length of marker block - is Big Endian
				len = (unsigned short int) ((jpgdata[i + 2] << 8) | jpgdata[i + 3]);
				i += 2 + len; //move to the next marker (header length includes 2 len bytes)
				
				break;
			} //switch (jpgdata[i + 1]) {
			break;
		default:
			fprintf(stderr, "JPG file error: failed markers + sizes check");
			return(0);
		} //switch (jpgdata[i]) {
	}

	return(1);
} //int CheckJPG(unsigned char *jpgdata, int jpglen)

/*
//currently only for 24 bit images
//XImage * FT_LoadJPGFileToFTWindow(FTWindow *twin,char *filename)
XImage * FT_LoadJPGFileToFTWindow(FTWindow *twin,unsigned char *jpg)
{
//BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int i,j;
//int nw,i,j,bc;
//XImage *img;
//unsigned char *offset;
//float rx,ry;
int x,y;
//JSAMPLE * image_buffer;	// Points to large array of R,G,B-order data 
JSAMPARRAY buffer;		// Output row buffer 
//JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s] 
int row_stride;		// physical row width in image buffer 
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
//struct jpeg_decompress_struct cinfo;
struct jpeg_decompress_struct dinfo;
//struct my_error_mgr jerr;
struct jpeg_error_mgr jerr;
//typedef struct my_error_mgr *my_error_ptr;
//struct my_error_mgr           jerr;
XImage *tximage;


if (PStatus.flags&PInfo)
  fprintf(stderr,"LoadJPGToFTWindow %s\n",filename);




	dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
//fprintf(stderr,"before jpeg_stdio_src\n");
        dinfo.output_components = 3;
        dinfo.out_color_space = JCS_RGB;

        dinfo.dct_method = JDCT_FASTEST;  //need?


//need to just send jpeg data directly
        jpeg_stdio_src(&dinfo, fptr);
//				dinfo
//fprintf(stderr,"before read header\n");
        (void) jpeg_read_header(&dinfo, TRUE);
//fprintf(stderr,"before decompress\n");
        jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width * dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);


  // Make a one-row-high sample array that will go away when done with image 
  buffer = (*dinfo.mem->alloc_sarray)
		((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//   buffer=malloc(x*4);



//fprintf(stderr,"before while\n");
  // Here we use the library's state variable cinfo.output_scanline as the
   // loop counter, so that we don't have to keep track ourselves.
   
  while (dinfo.output_scanline < dinfo.output_height) {
    // jpeg_read_scanlines expects an array of pointers to scanlines.
    // Here the array is only one element long, but you could ask for
     // more than one scanline at a time if that's more convenient.
     
    (void) jpeg_read_scanlines(&dinfo, buffer, 1);
//    (void) jpeg_read_scanlines(&cinfo, bdata+cinfo.output_scanline*x, 1);
    // Assume put_scanline_someplace wants a pointer and sample count. 
//    put_scanline_someplace(buffer[0], row_stride);
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
    memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
  }

   jpeg_finish_decompress(&dinfo);

   fclose(fptr);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

memset(&bmi,0,sizeof(BITMAPINFOHEADER));
//only need width and height
bmi.biWidth=x;
bmi.biHeight=y;


//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi.biWidth;
  twin->h=bmi.biHeight;
}

if (twin->bmpinfo!=0) free(twin->bmpinfo);
twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//copy bitmap info header to FTControl
memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));

//scale image to contorl
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    *(bdata2+i*x*4 + j*4+2)=*(bdata+i*x*3 + j*3);
    *(bdata2+i*x*4 + j*4+1)=*(bdata+i*x*3 + j*3+1);
    *(bdata2+i*x*4 + j*4)=*(bdata+i*x*3 + j*3+2);
    *(bdata2+i*x*4 + j*4+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?

//     The XCreateImage function allocates the memory needed for an XImage
//       structure for the specified display but does not allocate space for the
//       image itself.  Rather, it initializes the structure byte-order, bit-
//       order, and bitmap-unit values from the display and returns a pointer to
//       the XImage structure.

tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);

//      Note that when the image is created using XCreateImage, XGetImage, or
//       XSubImage, the destroy procedure that the XDestroyImage function calls
//       frees both the image structure and the data pointed to by the image
//       structure.

free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);


//}//end if bitmap file found
if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow complete\n");

return(tximage);

}  //end FT_LoadJPGFileToFTWindow
*/

/*
//currently only for 24 bit images
//ResizeWindow=0 resize jpg to fit window, =1 resize window to fit jpg
void FT_LoadJPGToFTWindow(FTWindow *twin,unsigned char *jpg,int ResizeWindow)
{
if (PStatus.flags&PInfo)
  fprintf(stderr,"LoadJPGToFTWindow\n");


  if (twin!=0) {
		//initialize FFMPEG (AVLibs) if not done already
		if (!(twin->flags&WAVCodec_Init)) {
			//FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			if (ResizeWindow) {
				//determine jpg width and height
				//set windows new dimensions
				//twin->w=;
				FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			} else {
				FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			}
		} //WAVCodec_Init
		//this 
		//ConvertFTWinBMPtoJPG(twin);
		//twin->jpgdata=jpg;
		ConvertFTWinJPGtoBMP(twin);

	} //twin!=0

//ConvertJPGToBMP
//LoadBMPToFTWindow


}  //end FT_LoadJPGToFTWindow
*/


void mem_init_source (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr = cinfo->src;
    mgr->next_input_byte = PStatus.jpgbufdata;
    mgr->bytes_in_buffer = PStatus.jpgbufsize;
    //printf("init %d\n", size - mgr->bytes_in_buffer);
}

boolean mem_fill_input_buffer (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr;

		mgr = cinfo->src;
    //printf("fill %d\n", PStatus.jpgbufsize - mgr->bytes_in_buffer);
    return 0;
}

void mem_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr* mgr;
		
		mgr = cinfo->src;
    //printf("skip %d +%d\n", PStatus.jpgbufsize - mgr->bytes_in_buffer, num_bytes);
    if(num_bytes<=0)
	return;
    mgr->next_input_byte += num_bytes;
    mgr->bytes_in_buffer -= num_bytes;
}

boolean mem_resync_to_restart (j_decompress_ptr cinfo, int desired)
{
    struct jpeg_source_mgr* mgr;

		mgr=cinfo->src;
    //printf("resync %d\n", size - mgr->bytes_in_buffer);
    mgr->next_input_byte = PStatus.jpgbufdata;
    mgr->bytes_in_buffer = PStatus.jpgbufsize;
    return 1;
}

void mem_term_source (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr;
		mgr=cinfo->src;
    //printf("term %d\n", size - mgr->bytes_in_buffer);
}



#if Linux
#if USE_FFMPEG
//InitAVCodec
//initialize AVCodec part of FFMPEG - using the MJPEG encoder and decoder
//Each video may have a different size and so needs a different avcodec and avcodeccontext
void FTInit_AVCodec(FTWindow *twin) 
{
int width,height,depth;

/* find the mpeg1 video encoder */
//    codec_out = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
//fprintf(stderr,"CODEC_ID_MJPEG=%x\n",CODEC_ID_MJPEG);
//twin->avc = avcodec_find_encoder(CODEC_ID_MJPEG);
twin->avc = avcodec_find_decoder(CODEC_ID_MJPEG);
//codec_out = avcodec_find_encoder(CODEC_ID_NONE);
if (!twin->avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
//exit(1);
return;
}


//twin->avcc=avcodec_alloc_context3();
avcodec_alloc_context3(twin->avcc);


//need for each camera in which video will be saved or sent
//will get moved to iipvp[call]
twin->mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
twin->mjpeg_yuv= avcodec_alloc_frame();

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
twin->mjpeg_bmp->quality=1; 
twin->mjpeg_yuv->quality=1;

//camera dimensions must be defined before this
width=twin->w;
height=twin->h;
depth=PStatus.depth;//24;//

/* put sample parameters */
twin->avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
twin->avcc->width = width;
twin->avcc->height = height;
/* frames per second */
twin->avcc->time_base= (AVRational){1,30}; //{1,25};
twin->avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(twin->avcc,twin->avc) < 0) {
if (avcodec_open2(twin->avcc,twin->avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return;
} else {
	twin->flags|=WAVCodec_Init;
}



/* alloc image and output buffer */
twin->mjpeg_frame_size = width*height*(depth/8); //allocate full uncompressed image size
twin->mjpeg_frame = (uint8_t *)malloc(twin->mjpeg_frame_size);


//fprintf(stderr,"icam.swsctx set\n");
//yuv 2 1 1 16bpp = 44 4 4
//initialize sws_scale conversion
//adjust for PStatus.depth
twin->swsctx = sws_getContext(
width,  //source width, height, format
height,
PIX_FMT_BGR24,//PIX_FMT_BGR24, //PIX_FMT_RGB24, //source format  //to me still a mystery why BGR instead of RGB
width, //dest width, height, format
height,
PIX_FMT_YUV422P,  //dest format  
SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//image is backwards

if (twin->swsctx == 0) {
	fprintf(stderr,"Error initializing the Scaler conversion context.\n");
} //icam.swsctx

twin->mjpeg_yuv->data[0] =(uint8_t *) malloc(2*width*height); //
twin->mjpeg_yuv->data[1]=twin->mjpeg_yuv->data[0]+width*height; //not /2
twin->mjpeg_yuv->data[2]=twin->mjpeg_yuv->data[1]+width*height/2;
twin->mjpeg_yuv->linesize[0]=width;
twin->mjpeg_yuv->linesize[1]=width/2;
twin->mjpeg_yuv->linesize[2]=width/2;


//important to provide addresses to data[0],[1] and [2]
//I guess these are used even though [0][1][2] point to continuous memory
twin->mjpeg_bmp->data[0]=(uint8_t *)(twin->bmpdata);  //set bitmap data to AVFrame
twin->mjpeg_bmp->data[1]=0;
twin->mjpeg_bmp->data[2]=0;
twin->mjpeg_bmp->linesize[0] = width*3;//c->width;
twin->mjpeg_bmp->linesize[1] = 0;
twin->mjpeg_bmp->linesize[2] = 0;

} //FTInit_AVCodec
#endif //USE_FFMPEG
#endif //Linux


#if Linux
#if USE_FFMPEG
//This converts the jpg attached to a twin to the bmp in the twin structure
//this is the fastest method of drawing a jpg to a window
int ConvertFTWinJPGtoBMP(FTWindow *twin)
{
int len;
//FILE *fptr;

//possibly add sws_getContext here
//and malloc

if (twin!=0) {

//	fprintf(stderr,"sws_scale\n");
	//convert image data in bgr24 format to scaled yuv format (no header info is involved only image data)
	sws_scale(twin->swsctx,twin->mjpeg_bmp->data,twin->mjpeg_bmp->linesize,0,twin->h,twin->mjpeg_yuv->data,twin->mjpeg_yuv->linesize);


//this is avcodec_decode_video
//	fprintf(stderr,"avcodec_encode_video\n");
	//convert yuv to jpg image (I think this adds header info, huffman table, etc)
	//this returns a writable jpg image file in icam.mjpeg_frame
	len = avcodec_encode_video(twin->avcc,twin->mjpeg_frame,twin->mjpeg_frame_size,twin->mjpeg_yuv);

//	fprintf(stderr,"converted %d bytes to JPEG\n",len);

	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

	return(len);
} //twin!=0
return(0);
} //ConvertFTWinBMPtoJPG(FTWindow *twin)


//ConvertJPGtoBMPmalloc
//possibly go back to libjpeg
//This converts a jpg file data to a bmp file data
//jpg is the jpg file, bmp includes header info, the bmp length is returned
//bmp is malloc'd in this function - otherwise jpg dimensions need to be passed
int ConvertJPGtoBMPmalloc(unsigned char *jpg,int jpglen,unsigned char **bmp)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmpdata;
int len,bmpsize;//,jpglen;
//FILE *fptr;
//int width,height,depth;
AVCodec *avc;  //format to save video calls - currently set to MJPEG
AVCodecContext *avcc;
struct SwsContext  *swsctx;
//uint8_t *sws_data;
uint8_t *sws_dest[3];
int sws_stride[3];
int got_picture;
AVFrame *mjpeg_bmp;  //
AVFrame *mjpeg_yuv; 
//unsigned short *jmarker;
AVPacket avpkt;


//need jpg size for decoder
//jpg should have a header with size, width, height but doesn't
//parse jpg until ffd9 to get size
//could get jpg size from ipvp packet data size



/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(jpg,1,jpglen,fptr);
	fclose(fptr);
	} //fptr!=0
*/


/*
fprintf(stderr,"Determining jpg size\n");
jmarker=(unsigned short *)jpg;
jpglen=0;
//no jpeg larger than 999,999,999 ~1GB
while(*jmarker!=0xd9ff && jpglen<999999999) {
	//fprintf(stderr,"%x ",*jmarker);
	jmarker++;
	jpglen+=2;
}

jpglen+=2;

if (jpglen>999999999) {
	fprintf(stderr,"Error: ConvertJPGtoBMP: JPG either has no ffd9 end marker, or is too large >1GB\n");
}
fprintf(stderr,"Got size %d\n",jpglen);
*/

/*
bmf=(BITMAPFILEHEADER *)bmp;
bmi=bmp+sizeof(BITMAPFILEHEADER);
bmpdata=bmi=sizeof(BITMAPINFOHEADER);
width=bmi.biWidth;
height=bmi.biHeight;
depth=bmi.biBitCount;
*/


//avcodec_init();
//avcodec_register_all();

avc = avcodec_find_decoder(CODEC_ID_MJPEG);
//avc = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO);
if (!avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
	//fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MPEG2VIDEO);
return(0);
}


//avcc=avcodec_alloc_context3();
avcodec_alloc_context3(avcc);

mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
mjpeg_yuv= avcodec_alloc_frame();


//need?
//if(avc->capabilities&CODEC_CAP_TRUNCATED)
//	avcc->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
mjpeg_bmp->quality=1; 
mjpeg_yuv->quality=1;

//memset(avcc,0,sizeof(AVCodecContext)); //need?

/* put sample parameters */
//avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
//avcc->width = width;
//avcc->height = height;
//jpg or bmp dimensions?
//just make larger than biggest possible jpg
//avcc->width = 320;//352;//320;//352;//640;//3000;//640;//352;//320;//640;//352;//320;//width;
//avcc->height = 240;//288;//240;//288;//480;//2000;//480;//288;//240;//480;//288;//240;//height;

/* frames per second */
//avcc->time_base= (AVRational){1,30}; //{1,25};

//icam.avcc->pix_fmt = PIX_FMT_NONE;
//avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
//icam.avcc->pix_fmt = PIX_FMT_YUV422P;  //this is correct format for some digital cameras
//icam.avcc->pix_fmt = PIX_FMT_YU4422P;  //this is correct format for some digital cameras

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(avcc,avc) < 0) {
if (avcodec_open2(avcc,avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return(0);
}

//possibly want to use:
//FF_BUFFER_TYPE_INTERNAL or FF_BUFFER_TYPE_USER (user deallocated buffer)
memset(&avpkt,0,sizeof(AVPacket));
avpkt.data=(uint8_t *)jpg;
avpkt.size=jpglen+FF_INPUT_BUFFER_PADDING_SIZE;


//   fprintf(stderr,"before avcc->width=%d height=%d\n",avcc->width,avcc->height);
//    len = avcodec_decode_video(avcc,mjpeg_yuv,&got_picture,(uint8_t *)jpg,jpglen);
	len = avcodec_decode_video2(avcc,mjpeg_yuv,&got_picture,&avpkt);

//decode_video2 adjusts avcc width and height - but it is better to know before what dimensions are

//	fprintf(stderr,"decode returns %d bytes\n",len);

//	if (mjpeg_yuv->pan_scan!=0) {
//		fprintf(stderr,"mjpeg_yuv->pan_scan->height=%d\n",mjpeg_yuv->pan_scan->height);
//	}
//	fprintf(stderr," linesize[0]=%d\n",mjpeg_yuv->linesize[0]);
	 // printf("after avcode_decode_video\n");
	  //printf("lossless=%d\n",mjpeg->ls);

//   fprintf(stderr,"after avcc->width=%d height=%d\n",avcc->width,avcc->height);

//	  fprintf(stderr,"mjpeg_yuv->linesize[0]=%d\n",mjpeg_yuv->linesize[0]);



	  //fprintf(stderr,"after avcode_decode_video len=0x%x (%d)\n",len,len);
	  if (got_picture) {
//	   fprintf(stderr,"got picture len=0x%x (%d)\n",len,len);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//    fprintf(stderr, "(avcod)c->pix_fmt=%x=%x(PIX_FMT_YUVJ422P)\n",c->pix_fmt,PIX_FMT_YUVJ422P);
	   //fprintf(stderr,"c->width %d c->height %d\n",c->width,c->height);

//static	int		    sws_context_flags_ = 0;
//SwsContext  *swsctx;
//uint8_t *sws_data;
//uint8_t *sws_dest[3];
//int sws_stride[3];

	switch(PStatus.depth) {
		case 24:
	//			PIX_FMT_YUVJ422P, //source format
	swsctx = sws_getContext(
				avcc->width,  //source
				avcc->height,
				PIX_FMT_YUV422P, //source format
				avcc->width,  //dest
				avcc->height,//c->height,
				PIX_FMT_BGR24,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
		break;
		case 32:
	//			PIX_FMT_YUVJ422P, //source format
	swsctx = sws_getContext(
				avcc->width,  //source
				avcc->height,
				PIX_FMT_YUV422P, //source format
				avcc->width,  //dest
				avcc->height,//c->height,
				PIX_FMT_RGB32,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
			break;
		default:
				fprintf(stderr,"Error: Unknown display depth.\n");
				return(0);
	} //switch
		

//image is backwards
	   
//fprintf(stderr,"after sws_getContext\n");

//remember to free at some point

bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(PStatus.depth/8)*avcc->width*avcc->height;
//*bmp = malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);
*bmp = (unsigned char*)malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);

bmf=(BITMAPFILEHEADER *)*bmp;
bmi=(BITMAPINFOHEADER *)(*bmp+sizeof(BITMAPFILEHEADER));
bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));




//sws_scale appears to return many zeros (empty header info)?

//uint8_t *dest[3]= {data, data+c->width*c->height, data+c->width*c->height*2};
//sws_dest[0]=bmpdata;
sws_dest[0]=*bmp;
sws_dest[1]=0;//sws_data+c->width*c->height;
sws_dest[2]=0;//sws_data+c->width*c->height*2;
//int stride[3]={c->width*3,0,0};  //put data in one channel w*3
sws_stride[0]=avcc->width*(PStatus.depth/8);
sws_stride[1]=0;
sws_stride[2]=0;
//void *testdst;


//printf("important: picture->data[0]=%x ls[0]=%d d[1]=%x l[1]=%d d[2]=%x l[2]=%d\n",picture->data[0],picture->linesize[0],picture->data[1],picture->linesize[1],picture->data[2],picture->linesize[2]);

//testdst=malloc(320*240*4);
//fprintf(stderr,"before sws_scale\n");
//fprintf(stderr,"&dst->data=%x =%x\n",&dst->data,dst->data);
//int sws_scale(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,
//              int srcSliceH, uint8_t* dst[], int dstStride[]){
//sws_scale( ctx, src->data, src->stride,0, src->height,dst->data, dst->stride );
//fprintf(stderr,"before sws_scale\n");
//can possibly write directly to bmpaddress if not reversed
//	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
//fprintf(stderr,"before sws_free\n");
	//free later sws_freeContext(swsctx);
//sws_scale apparently returns an empty header

//this is just data - needs header info to

//fprintf(stderr,"after sws_scale\n");

//return 1560 of 0s padding? 0x618+36 unknown bytes=1596 bytes

//*bmp+=1596; //cant keep because of malloc

bmf=(BITMAPFILEHEADER *)*bmp;
bmi=(BITMAPINFOHEADER *)(*bmp+sizeof(BITMAPFILEHEADER));

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=avcc->width;
bmi->biHeight=avcc->height;
bmi->biPlanes=1;
bmi->biBitCount=PStatus.depth;
bmi->biSizeImage=(PStatus.depth/8)*avcc->width*avcc->height;




	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

//add bmp header info?


/*
	//write bmp to file
	fptr=fopen("testjpg.bmp","wb");
	if (fptr!=0) {
//		fwrite(*bmp,1,len,fptr);
	fwrite(*bmp,1,bmpsize,fptr);
	fclose(fptr);
	} //fptr!=0
*/

	return(len);


		} else { //got_picture
	fprintf(stderr,"avcodec_decode_video2 did not return a picture\n");
} //got_picture


/*

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);
*/

return(0);

} //ConvertJPGtoBMPmalloc


//ConvertJPGtoBMPData
//possibly go back to libjpeg
//This converts a jpg file data to a bmp file data
//jpg is the jpg file, bmp includes header info, the bmp length is returned
//bmp is not malloc'd in this function - so bmp needs to contain enough allocated memory
//dimensions are taken from jpg
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int *w,int *h, int *d)
//int ConvertJPGtoBMP(unsigned char *jpg,int jpglen,unsigned char *bmp)
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d)
//**bmp because bmp needs to change the address of bmp forward after the header
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char **bmp,int w,int h, int d)
int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d)
{
	//BITMAPFILEHEADER *bmf;
	//BITMAPINFOHEADER *bmi;
	//unsigned char *bmpdata;
	int len;//,bmpsize;//,jpglen;
	//FILE *fptr;
	//int width,height,depth;
	AVCodec *avc;  //format to save video calls - currently set to MJPEG
	AVCodecContext *avcc;
	struct SwsContext  *swsctx;
	//uint8_t *sws_data;
	uint8_t *sws_dest[3];
	int sws_stride[3];
	int got_picture;
	AVFrame *mjpeg_bmp;  //
	AVFrame *mjpeg_yuv; 
	//unsigned short *jmarker;
	AVPacket avpkt;


	//need jpg size for decoder
	//jpg should have a header with size, width, height but doesn't
	//parse jpg until ffd9 to get size
	//could get jpg size from ipvp packet data size



	/*
		//write jpg to file
		fptr=fopen("test.jpg","wb");
		if (fptr!=0) {
			fwrite(jpg,1,jpglen,fptr);
		fclose(fptr);
		} //fptr!=0
	*/


	/*
	fprintf(stderr,"Determining jpg size\n");
	jmarker=(unsigned short *)jpg;
	jpglen=0;
	//no jpeg larger than 999,999,999 ~1GB
	while(*jmarker!=0xd9ff && jpglen<999999999) {
		//fprintf(stderr,"%x ",*jmarker);
		jmarker++;
		jpglen+=2;
	}

	jpglen+=2;

	if (jpglen>999999999) {
		fprintf(stderr,"Error: ConvertJPGtoBMP: JPG either has no ffd9 end marker, or is too large >1GB\n");
	}
	fprintf(stderr,"Got size %d\n",jpglen);
	*/

	/*
	bmf=(BITMAPFILEHEADER *)bmp;
	bmi=bmp+sizeof(BITMAPFILEHEADER);
	bmpdata=bmi=sizeof(BITMAPINFOHEADER);
	width=bmi.biWidth;
	height=bmi.biHeight;
	depth=bmi.biBitCount;
	*/


	//avcodec_init();
	//avcodec_register_all();

	//add this to PStatus

	avc = avcodec_find_decoder(CODEC_ID_MJPEG);
	//avc = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO);
	if (!avc) {
		fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
		//fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MPEG2VIDEO);
	return(0);
	}


	//avcc=avcodec_alloc_context3();
	avcodec_alloc_context3(avcc);

	mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
	mjpeg_yuv= avcodec_alloc_frame();


	//need?
	//if(avc->capabilities&CODEC_CAP_TRUNCATED)
	//	avcc->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

	/*
	* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
	* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
	* - decoding: Set by libavcodec.\
	*/
	mjpeg_bmp->quality=1; 
	mjpeg_yuv->quality=1;

	//memset(avcc,0,sizeof(AVCodecContext)); //need?

	/* put sample parameters */
	//avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
	/* resolution must be a multiple of two */
	//dimensions of output video are for now window size
	//VIDWIDTH, VIDHEIGHT maybe is future
	avcc->width = w;
	avcc->height = h;
	//jpg or bmp dimensions?
	//just make larger than biggest possible jpg
	//avcc->width = 320;//352;//320;//352;//640;//3000;//640;//352;//320;//640;//352;//320;//width;
	//avcc->height = 240;//288;//240;//288;//480;//2000;//480;//288;//240;//480;//288;//240;//height;

	/* frames per second */
	//avcc->time_base= (AVRational){1,30}; //{1,25};

	//icam.avcc->pix_fmt = PIX_FMT_NONE;
	avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
	//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
	//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
	//icam.avcc->pix_fmt = PIX_FMT_YUV422P;  //this is correct format for some digital cameras
	//icam.avcc->pix_fmt = PIX_FMT_YU4422P;  //this is correct format for some digital cameras

	//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
	//PIX_FMT_RGB24 gives:
	//colorspace is not supported in jpeg
	//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

	/* open it */
	//if (avcodec_open(avcc,avc) < 0) {
	if (avcodec_open2(avcc,avc,0) < 0) {
	fprintf(stderr, "could not open codec for output\n");
	//exit(1);
	return(0);
	}

	//possibly want to use:
	//FF_BUFFER_TYPE_INTERNAL or FF_BUFFER_TYPE_USER (user deallocated buffer)
	memset(&avpkt,0,sizeof(AVPacket));
	avpkt.data=(uint8_t *)jpg;
	avpkt.size=jpglen+FF_INPUT_BUFFER_PADDING_SIZE;


	//   fprintf(stderr,"before avcc->width=%d height=%d\n",avcc->width,avcc->height);
	//    len = avcodec_decode_video(avcc,mjpeg_yuv,&got_picture,(uint8_t *)jpg,jpglen);
	len = avcodec_decode_video2(avcc,mjpeg_yuv,&got_picture,&avpkt);

	//decode_video2 adjusts avcc width and height - but it is better to know before what dimensions are

	//	fprintf(stderr,"decode returns %d bytes\n",len);

	//	if (mjpeg_yuv->pan_scan!=0) {
	//		fprintf(stderr,"mjpeg_yuv->pan_scan->height=%d\n",mjpeg_yuv->pan_scan->height);
	//	}
	//	fprintf(stderr," linesize[0]=%d\n",mjpeg_yuv->linesize[0]);
		 // printf("after avcode_decode_video\n");
			//printf("lossless=%d\n",mjpeg->ls);

	//   fprintf(stderr,"after avcc->width=%d height=%d\n",avcc->width,avcc->height);

	//	  fprintf(stderr,"mjpeg_yuv->linesize[0]=%d\n",mjpeg_yuv->linesize[0]);



	  //fprintf(stderr,"after avcode_decode_video len=0x%x (%d)\n",len,len);
	  if (got_picture) {
//	   fprintf(stderr,"got picture len=0x%x (%d)\n",len,len);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//    fprintf(stderr, "(avcod)c->pix_fmt=%x=%x(PIX_FMT_YUVJ422P)\n",c->pix_fmt,PIX_FMT_YUVJ422P);
	   //fprintf(stderr,"c->width %d c->height %d\n",c->width,c->height);

//static	int		    sws_context_flags_ = 0;
//SwsContext  *swsctx;
//uint8_t *sws_data;
//uint8_t *sws_dest[3];
//int sws_stride[3];

		switch(PStatus.depth) {
			case 24:
		//			PIX_FMT_YUVJ422P, //source format
					swsctx = sws_getContext(
					w,  //source
					h,
					PIX_FMT_YUV422P, //source format
					w,  //dest
					h,//c->height,
					PIX_FMT_BGR24,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
					SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
					);
			break;
			case 32:
		//			PIX_FMT_YUVJ422P, //source format
					swsctx = sws_getContext(
					w,  //source
					h,
					PIX_FMT_YUV422P, //source format
					w,  //dest
					h,//c->height,
					PIX_FMT_BGR32,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
					SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
					);
				break;
			default:
					fprintf(stderr,"Error: Unknown display depth.\n");
					return(0);
		} //switch
		

	//image is backwards
			 
	//fprintf(stderr,"after sws_getContext\n");

	//remember to free at some point

	//bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(PStatus.depth/8)*avcc->width*avcc->height;
	//*bmp = malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);

	//bmf=(BITMAPFILEHEADER *)bmp;
	//bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
	//bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));




	//sws_scale appears to return many zeros (empty header info)?

	//uint8_t *dest[3]= {data, data+c->width*c->height, data+c->width*c->height*2};
	//sws_dest[0]=bmpdata;
	sws_dest[0]=bmp;
	sws_dest[1]=0;//sws_data+c->width*c->height;
	sws_dest[2]=0;//sws_data+c->width*c->height*2;
	//int stride[3]={c->width*3,0,0};  //put data in one channel w*3
	sws_stride[0]=w*(PStatus.depth>>3);
	sws_stride[1]=0;
	sws_stride[2]=0;
	//void *testdst;


	//printf("important: picture->data[0]=%x ls[0]=%d d[1]=%x l[1]=%d d[2]=%x l[2]=%d\n",picture->data[0],picture->linesize[0],picture->data[1],picture->linesize[1],picture->data[2],picture->linesize[2]);

	//testdst=malloc(320*240*4);
	//fprintf(stderr,"before sws_scale\n");
	//fprintf(stderr,"&dst->data=%x =%x\n",&dst->data,dst->data);
	//int sws_scale(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,
	//              int srcSliceH, uint8_t* dst[], int dstStride[]){
	//sws_scale( ctx, src->data, src->stride,0, src->height,dst->data, dst->stride );
	//fprintf(stderr,"before sws_scale\n");
	//can possibly write directly to bmpaddress if not reversed
	//	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,h,sws_dest,sws_stride);
	//fprintf(stderr,"before sws_free\n");
		//free later sws_freeContext(swsctx);
	//sws_scale apparently returns an empty header

	//this is just data - needs header info to

	//fprintf(stderr,"after sws_scale\n");

	//return 1560 of 0s padding? 0x618+36 unknown bytes=1596 bytes

	//*bmp+=1596; //cant keep because of malloc
#if 0 
	bmf=(BITMAPFILEHEADER *)bmp;
	bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

	//memset(bmf,0,sizeof(BITMAPFILEHEADER));
	bmf->bfType=0x4d42;
	bmf->bfSize=bmpsize;
	bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


	//memset(bmi,0,sizeof(BITMAPINFOHEADER));
	//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
	bmi->biSize=sizeof(BITMAPINFOHEADER);
	bmi->biWidth=avcc->width;
	bmi->biHeight=avcc->height;
	bmi->biPlanes=1;
	bmi->biBitCount=PStatus.depth;
	bmi->biSizeImage=(PStatus.depth/8)*avcc->width*avcc->height;
#endif



		/*
		//write jpg to file
		fptr=fopen("test.jpg","wb");
		if (fptr!=0) {
			fwrite(icam.mjpeg_frame,1,len,fptr);
		fclose(fptr);
		} //fptr!=0
		*/

	//add bmp header info?


/*
	//write bmp to file
	fptr=fopen("testjpg.bmp","wb");
	if (fptr!=0) {
//		fwrite(*bmp,1,len,fptr);
	fwrite(*bmp,1,bmpsize,fptr);
	fclose(fptr);
	} //fptr!=0
*/



//	bmp+=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		av_free(mjpeg_bmp);
		av_free(mjpeg_yuv);

		//free avcodec
		avcodec_close(avcc);

		return(len);


		} else { //got_picture
			fprintf(stderr,"avcodec_decode_video2 did not return a picture\n");
		} //got_picture


/*

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);
*/

	return(0);

} //ConvertJPGtoBMPData

#if 0 //now done with libjpeg

//ConvertBMPtoJPG
//This converts a bmp file to a jpg file
//bmp includes header info, jpg is the jpg file, the jpeg length is returned
//needs to pass address pointer **jpg
int ConvertBMPtoJPG(unsigned char *bmp,unsigned char *jpg)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmpdata;
int len;
//FILE *fptr;
int width,height,depth;
AVCodec *avc;  //format to save video calls - currently set to MJPEG
AVCodecContext *avcc;
struct SwsContext  *swsctx;
int mjpeg_frame_size;
AVFrame *mjpeg_bmp;  //
AVFrame *mjpeg_yuv; 



bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));
width=bmi->biWidth;
height=bmi->biHeight;
depth=bmi->biBitCount;


//avcodec_init();
//avcodec_register_all();

avc = avcodec_find_encoder(CODEC_ID_MJPEG);
if (avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
return(0);
}


//avcc=avcodec_alloc_context3();
avcodec_alloc_context3(avcc);

mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
mjpeg_yuv= avcodec_alloc_frame();

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
mjpeg_bmp->quality=1; 
mjpeg_yuv->quality=1;

/* put sample parameters */
avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
avcc->width = width;
avcc->height = height;
/* frames per second */
avcc->time_base= (AVRational){1,30}; //{1,25};
//c->gop_size = 10; /* emit one intra frame every ten frames */
//c->max_b_frames=1;
//c->pix_fmt = PIX_FMT_YUV420P;
//    c->pix_fmt = PIX_FMT_YUVJ440P;
//    c->pix_fmt = PIX_FMT_YUVJ444P;

avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common?

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(avcc,avc) < 0) {
if (avcodec_open2(avcc,avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return(0);
}


/* alloc image and output buffer */
mjpeg_frame_size = width*height*(depth/8); //allocate full uncompressed image size

//malloc here or expect malloc'd memory?
//if here, caller needs to free
jpg = (unsigned char *)malloc(mjpeg_frame_size+FF_INPUT_BUFFER_PADDING_SIZE);


//PIX_FMT_YUVJ422P,  //dest format  
//yuv 2 1 1 16bpp = 44 4 4
//initialize sws_scale conversion
swsctx = sws_getContext(
width,  //source width, height, format
height,
PIX_FMT_BGR24,//PIX_FMT_BGR24, //PIX_FMT_RGB24, //source format  //to me still a mystery why BGR instead of RGB
width, //dest width, height, format
height,
PIX_FMT_YUVJ422P,  //dest format  
SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//image is backwards

if (swsctx == 0) {
	fprintf(stderr,"Error initializing the Scaler conversion context.\n");
} //swsctx

mjpeg_yuv->data[0] =(uint8_t *)malloc(2*width*height);
mjpeg_yuv->data[1]=mjpeg_yuv->data[0]+width*height;
mjpeg_yuv->data[2]=mjpeg_yuv->data[1]+width*height/2;
mjpeg_yuv->linesize[0]=width;
mjpeg_yuv->linesize[1]=width/2;
mjpeg_yuv->linesize[2]=width/2;


//important to provide addresses to data[0],[1] and [2]
//I guess these are used even though [0][1][2] point to continuous memory
mjpeg_bmp->data[0]=(uint8_t *)(bmpdata);  //set bitmap data to AVFrame
mjpeg_bmp->data[1]=0;
mjpeg_bmp->data[2]=0;
mjpeg_bmp->linesize[0] = width*3;//c->width;
mjpeg_bmp->linesize[1] = 0;
mjpeg_bmp->linesize[2] = 0;


	fprintf(stderr,"sws_scale\n");
	//convert image data in bgr24 format to scaled yuv format (no header info is involved only image data)
	sws_scale(swsctx,mjpeg_bmp->data,mjpeg_bmp->linesize,0,height,mjpeg_yuv->data,mjpeg_yuv->linesize);

//	fprintf(stderr,"avcodec_encode_video\n");
	//convert yuv to jpg image (I think this adds header info, huffman table, etc)
	//this returns a writable jpg image file in mjpeg_frame
//	len = avcodec_encode_video(avcc,mjpeg_frame,mjpeg_frame_size,mjpeg_yuv);
len = avcodec_encode_video(avcc,(uint8_t *)jpg,mjpeg_frame_size,mjpeg_yuv);


//	fprintf(stderr,"converted %d bytes to JPEG\n",len);

	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);

	return(len);
} //ConvertBMPtoJPG
#endif //0
#endif //USE_FFMPEG
#endif  //Linux


void mem_init_destination(j_compress_ptr cinfo) 
{ 
  struct jpeg_destination_mgr *dmgr = 
      (struct jpeg_destination_mgr *)(cinfo->dest);
  dmgr->next_output_byte = jpgdest;
  dmgr->free_in_buffer = jpgdestlen;
}

boolean mem_empty_output_buffer(j_compress_ptr cinfo)
{ 
    printf("jpeg mem overflow!\n");
    exit(1);
}

void mem_term_destination(j_compress_ptr cinfo) 
{ 
  struct jpeg_destination_mgr *dmgr = 
      (struct jpeg_destination_mgr *)(cinfo->dest);
  jpglen = jpgdestlen - dmgr->free_in_buffer;
  dmgr->free_in_buffer = 0;
}


void ConvertBMPtoJPGFile(unsigned char *bmp,char *name,int quality)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
FILE *jpgfile;
int offset1;
unsigned char *offset2;


jpgfile=fopen(name,"wb");
if (!jpgfile) {
	return;
}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=25;//100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


//jpeg_stdio_dest(&cinfo,jpgfile);
cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);


fwrite(jpgdest,1,jpglen,jpgfile);

fclose(jpgfile);
free(jpgdest);
} //void ConvertBMPtoJPGFile(u8 *bmp,char *name,int quality)


//Currently have to use global variables because of destination manager
void ConvertBMPtoJPG(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;




//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;

		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);


*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);
//*len=jpglen;

free(jpgdest);

//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPtoJPG(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)


//Currently have to use global variables because of destination manager
void ConvertBMPDatatoJPG(unsigned char *bdata,int quality,int w,int h,int BytesPerPixel,unsigned char *jpg,int *jlen)
{
//BITMAPINFOHEADER *bmi;
//unsigned char *bdata;
//int w,h,d;
	int d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;




//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

//bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
//bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//w=bmi->biWidth ;
//h=bmi->biHeight;
d=BytesPerPixel;//bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);

//*len=jpglen;

*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);

free(jpgdest);
//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPDatatoJPG(unsigned char *bdata,int quality,int w,int h,int BytesPerPixel,unsigned char *jpg,int *jlen)


//Currently have to use global variables because of destination manager
//MakeMJPG - make an mjpg frame
void ConvertBMPtoMJPGFrame(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
unsigned char marker[60];
int i;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;

//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=25;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;

jpeg_set_defaults(&cinfo);
jpeg_set_colorspace (&cinfo,JCS_YCbCr);
jpeg_set_quality(&cinfo,quality,TRUE);

	cinfo.dc_huff_tbl_ptrs[0]->sent_table = TRUE;
	cinfo.dc_huff_tbl_ptrs[1]->sent_table = TRUE;
	cinfo.ac_huff_tbl_ptrs[0]->sent_table = TRUE;
	cinfo.ac_huff_tbl_ptrs[1]->sent_table = TRUE;

	cinfo.comp_info[0].component_id = 0;
	cinfo.comp_info[0].v_samp_factor = 1;
	cinfo.comp_info[1].component_id = 1;
	cinfo.comp_info[2].component_id = 2;

	cinfo.write_JFIF_header = FALSE;

//cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,FALSE);



i=0;
	marker[i++] = 'A';
	marker[i++] = 'V';
	marker[i++] = 'I';
	marker[i++] = '1';
	marker[i++] = 0;
	while (i<60)
		marker[i++] = 32;

	jpeg_write_marker (&cinfo, JPEG_APP0, marker, 60);

	i=0;
	while (i<60)
		marker[i++] = 0;

	jpeg_write_marker (&cinfo, JPEG_COM, marker, 60);



row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);

	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);

//*len=jpglen;

*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);

free(jpgdest);

//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPtoMJPGFrame(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)

//currently x,y and d are ignored and 32-bit bitmap data is the only option
int ConvertJPGtoBMPData_LibJPEG(unsigned char *jpg,int jpglen,unsigned char *bmpdata,int w,int h, int d)
{
//int LoadJPGToFTControl(unsigned char *jpgdata, int jpglen, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor)
//{
	BITMAPFILEHEADER *bmf;
	BITMAPINFOHEADER *bmi;
	unsigned char *bdata, *bdata2, *offset, *offsetCTL, *bmp;
	int i, j;
	int x,y,jpgx, jpgy, bc, bc2, bmpsize;
	//JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in image buffer */
	//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
	struct jpeg_decompress_struct dinfo;
	struct jpeg_source_mgr mgr;
	struct jpeg_error_mgr jerr;
	int numbytes, ex;
	float rx, ry;
	int ImageNum;
#if 0//WIN32
	DWORD dwWaitResult;
#endif


	/*
	if (!CheckJPG(jpgdata, jpglen)) {
	return(0);
	}
	*/

	//we need to lock the DrawFTControls mutex so that we are not writing over image data that is currently being drawn
#if 0//WIN32
	//request the mutex
	dwWaitResult = WaitForSingleObject(
		PStatus.DrawFTControlsMutex,    // handle to mutex
		INFINITE);  // no time-out interval

	if (dwWaitResult != WAIT_OBJECT_0) {
		return;
	}

#endif


	PStatus.jpgbufdata = jpg;
	PStatus.jpgbufsize = jpglen;


	memset(&dinfo, 0, sizeof(dinfo));
	memset(&jerr, 0, sizeof(jerr));
	memset(&mgr, 0, sizeof(mgr));


	jpeg_create_decompress(&dinfo);

	mgr.next_input_byte = jpg;
	mgr.bytes_in_buffer = jpglen;
	mgr.init_source = mem_init_source;
	mgr.fill_input_buffer = mem_fill_input_buffer;
	mgr.skip_input_data = mem_skip_input_data;
	mgr.resync_to_restart = mem_resync_to_restart;
	mgr.term_source = mem_term_source;

	dinfo.err = jpeg_std_error(&jerr);
	dinfo.src = &mgr;

	dinfo.output_components = 3;
	dinfo.out_color_space = JCS_RGB;
//	dinfo.output_components = 4;
//	dinfo.out_color_space = JCS_RGBA;

	dinfo.dct_method = JDCT_FASTEST;  //need?

	//jpeg_stdio_src(&dinfo,fptr);
	//fprintf(stderr,"before read header\n");
	jpeg_read_header(&dinfo, TRUE);
	//fprintf(stderr,"before decompress\n");
	jpeg_start_decompress(&dinfo);

	row_stride = dinfo.output_width*dinfo.output_components;
	//fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

	jpgx = dinfo.output_width;
	jpgy = dinfo.output_height;
	bdata = (unsigned char *)malloc(jpgx*jpgy * 4);  //just to take care of word align padding

	for (j = 0; j < jpgy; j++) {
		offset = bdata + jpgx*j * 3;
		jpeg_read_scanlines(&dinfo, (JSAMPARRAY)&offset, 1);
	}  //for j


	/* Make a one-row-high sample array that will go away when done with image */
	//buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

	//fprintf(stderr,"before while\n");
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	//  while (dinfo.output_scanline < dinfo.output_height) {
	/* jpeg_read_scanlines expects an array of pointers to scanlines.

	* Here the array is only one element long, but you could ask for
	* more than one scanline at a time if that's more convenient.
	*/
	//    jpeg_read_scanlines(&dinfo, buffer, 1);
	/* Assume put_scanline_someplace wants a pointer and sample count. */
	//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
	//  fprintf(stderr,"row_stride:%d\n",row_stride);
	//   memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
	//fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
	// }

	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);

	//free(jpgdata);


//	bdata2 = bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Note: jpeglib does not byte-align RGB data

	bc = 3; //jpg is always 3 bytes/pixel bmi->biBitCount/8;
	/*
	nw=(int)fmodf((float)jpgx*bc,4.0);
	if (nw>0)  {
	nw=jpgx*bc+4-nw;
	} else {
	nw=jpgx*bc;
	}
	*/

	//bc2 = 4;
	bc2 = d>>3; //32 >>3=4  24BitsPerPixel >>3=3
	//nw2=x*4;

	//  if (cx==0 && y==0) //scale to FTControl

	//scale image to FTControl
	//rx=(float)nw/((float)jx*(float)bc);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)nw/(float)(jx*bc2);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)(jx*bc2)/(float)nw;
	//rx=(float)nw2/(float)nw;
	//if (ResizeImage) {
//	if (!(flags&FT_SCALE_IMAGE)) {
//		rx = (float)jpgx / (float)x;
//		ry = (float)jpgy / (float)y;//bmi.biHeight;
//	}
//	else {
		rx = 1.0;
		ry = 1.0;

		x=jpgx;//w;
		y=jpgy;//h;

		//if control is bigger than jpg
		if (x > jpgx) {
			x = jpgx;
		}
		if (y > jpgy) {
			y = jpgy;
		}
//	}


	//todo: make a function for below and call - see FTLoadBitmapFileToControl
	for (i = 0; i < y; i++) {
		//  fprintf(stderr,"\n\n");
		for (j = 0; j < x; j++) {
			offsetCTL = bmpdata + (i*x + j)*bc2;
			offset = bdata + (((int)(i*ry))*jpgx + ((int)(j*rx)))*bc;
			//offset=bdata+(i*jpgx+j)*bc;
			//  fprintf(stderr,"%p ",offset);
			*offsetCTL = *(offset + 2);
			*(offsetCTL + 1) = *(offset + 1);
			*(offsetCTL + 2) = *offset;
			if (bc2>3) {
				*(offsetCTL + 3) = 0;
			} 
		}  //j
	}  //i



	/*
	Note that when the image is created using XCreateImage, XGetImage, or
	XSubImage, the destroy procedure that the XDestroyImage function calls
	frees both the image structure and the data pointed to by the image
	structure.
	*/
	free(bdata); //free original image


	if (PStatus.flags&PInfo) {
		fprintf(stderr, "ConvertJGPtoBMPData_LibJPEG complete\n");
	}

	return(1);
} //int ConvertJPGtoBMPData_LibJPEG(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d)

