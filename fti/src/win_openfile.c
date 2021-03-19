//win_openfile.c

#include "freethought.h"
#include "win_ftimain.h"
#include "win_openfile.h"

extern ProgramStatus PStatus;
//extern XFontStruct *font_info;
extern int *Ly1,*Ly2;  //levels for buttons


int fdUSB;
unsigned char *USBbuffer;
#if Linux
struct termios oldtio,newtio;
#endif


int openfile_CreateFTWindow(void)
{
FTWindow twin,*twin2;

twin2=GetFTWindow("click"); 
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"openfile");
  strcpy(twin.title,"Open File");  //unbelievable, numchar<10
  strcpy(twin.ititle,"Open File");
  twin.flags=WOnlyOne;
  twin.w=300;
  twin.h=70;
  twin.Keypress=(FTControlfuncwk *)win1key;
  twin.AddFTControls=(FTControlfuncw *)openfile_AddFTControls;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);

}

//fprintf(stderr,"b4 free %p %p\n",twin->ititle,twin);
return 1;
}  //end openfile_CreateFTWindow

void win1key(FTWindow *twin,unsigned char key)
{

    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
//        break;
      case 0x1b:	/*esc*/
//        CloseFTWindow("openfile");
        CloseFTWindow(twin);
//        DelWindow("openfile");
      default:
        break;
      }
}



int openfile_AddFTControls(FTWindow *twin)
{
FTControl tFTControl;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'openfile'\n");

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"txtGetFile");
tFTControl.type=CTTextBox;//2; //textbox
tFTControl.x1=20;
tFTControl.y1=5;//Ly1[1];
tFTControl.x2=220;
tFTControl.y2=21;//Ly2[1];
//tFTControl.win=1;
tFTControl.tab=0;
tFTControl.Keypress=(FTControlfuncwck *)txtGetFile_Keypress;
//memset(&tFTControl.text,0,FTMedStr-1);
//strcpy(tFTControl.text,"");
tFTControl.color[0]=FT_WHITE;  //bkground
//tFTControl.color[1]=FT_WHITE;//black;  //text
tFTControl.textcolor[0]=FT_BLACK;  //text
tFTControl.textcolor[1]=FT_WHITE;  //text
//memset(&tFTControl.filename,0,FTMedStr-1);
////strcpy(tFTControl.filename,"");


//tFTControl.win=twin->num;
AddFTControl(twin,&tFTControl);



memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"btnGetFile");
tFTControl.type=CTButton;//0;//2;
//tFTControl.win=1;
tFTControl.x1=10;
tFTControl.y1=30;
tFTControl.x2=10+tFTControl.fontwidth*8;
tFTControl.y2=30+tFTControl.fontheight;
strcpy(tFTControl.text,"Get File");
//tFTControl.ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tFTControl.ButtonUp=(FTControlfuncwcxy *)OpenFile;
tFTControl.ButtonDownUp[0]=(FTControlfuncwcxy *)btnGetFile_Click;
//tFTControl.tab=-1;
//strcpy(tFTControl.filename,"");

tFTControl.color[0]=FT_LIGHT_GRAY;
tFTControl.color[1]=0;
//tFTControl.textcolor
//tFTControl.win=twin->num;
AddFTControl(twin,&tFTControl);



memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"btnSendUSB");
tFTControl.type=CTButton;//0;//2;
//tFTControl.win=1;
tFTControl.x1=10;
tFTControl.y1=50;
tFTControl.x2=10+tFTControl.fontwidth*7;
tFTControl.y2=50+tFTControl.fontheight;
strcpy(tFTControl.text,"SendUSB");
//tFTControl.ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tFTControl.ButtonUp=(FTControlfuncwcxy *)OpenFile;
tFTControl.ButtonDownUp[0]=(FTControlfuncwcxy *)btnSendUSB_Click;
//tFTControl.tab=-1;
//strcpy(tFTControl.filename,"");

tFTControl.color[0]=FT_LIGHT_GRAY;
tFTControl.color[1]=0;
//tFTControl.textcolor
//tFTControl.win=twin->num;
AddFTControl(twin,&tFTControl);

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"btnGetUSB");
tFTControl.type=CTButton;//2;
//tFTControl.win=1;
tFTControl.x1=120;
tFTControl.y1=50;
tFTControl.x2=120+tFTControl.fontwidth*7;
tFTControl.y2=50+tFTControl.fontheight;
strcpy(tFTControl.text,"Get USB");
//tFTControl.ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tFTControl.ButtonUp=(FTControlfuncwcxy *)OpenFile;
tFTControl.ButtonDownUp[0]=(FTControlfuncwcxy *)btnGetUSB_Click;
//tFTControl.tab=-1;
//strcpy(tFTControl.filename,"");

tFTControl.color[0]=FT_LIGHT_GRAY;
tFTControl.color[1]=0;
//tFTControl.textcolor
//tFTControl.win=twin->num;
AddFTControl(twin,&tFTControl);
//#endif

if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to openfile.\n");
return 1;
}//end if openfile_AddFTControls


void btnSendUSB_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
FTControl *tFTControl2;



//tFTControl=GetFTControl("btnSendUSB");
//fprintf(stderr,"USB Clicked %p\n",tFTControl);
if (tFTControl!=0)
  {
  tFTControl2=GetFTControl("txtGetFile");
  //open /dev/ttyUSB0
  //send


       fprintf(stderr,"will write %s\n",tFTControl2->text);
       write(fdUSB,tFTControl2->text,sizeof(tFTControl2->text));
       fprintf(stderr,"wrote out %s\n",tFTControl2->text);

//  DrawFTControls(tFTControl->win,0);
  DrawFTControls(tFTControl->window,0);
  }
}  //end btnSendUSB_Click



int InitUSB(void)
{

#if Linux
       //fdUSB = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
        fdUSB = open("/dev/ttyUSB0", O_RDONLY );
       //fprintf(stderr,"Opened USB = %d\n",fdUSB);
        if (fdUSB <= 0)
	  {
	  //perror(MODEMDEVICE);
	  fprintf(stderr,"Error opening /dev/ttyUSB0\n");
	  return -1;//exit(-1);
	  }
	else
	  {

        tcgetattr(fdUSB,&oldtio); /* save current port settings */

        bzero(&newtio, sizeof(newtio));
        //newtio.c_cflag = CLOCAL;
	//newtio.c_cflag&= ~CRTSCTS;
//	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

	newtio.c_iflag = IGNPAR;

	newtio.c_oflag = 0;

        /* set input mode (non-canonical, no echo,...) */
        //newtio.c_lflag = ICANON;

        //newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        //newtio.c_cc[VMIN]     = 255;   /* blocking read until 5 chars received */

        tcflush(fdUSB, TCIFLUSH);
        tcsetattr(fdUSB,TCSANOW,&newtio);

        //if ((USBbuffer=malloc(1500000))==0)  //allocate 1.5MB
	if ((USBbuffer=malloc(100000))==0)  //allocate 100kB
          fprintf(stderr,"Could not malloc USB buffer\n");
	else {
    //fprintf(stderr,"malloc USBbuffer 100000 %p",USBbuffer);
    return 1;
  }


        }  //end if usb device available
#endif
		return -1;

}  //end InitUSB

void CloseUSB(void)
{
#if Linux
close(fdUSB);
//fprintf(stderr,"Closed USB\n");
tcsetattr(fdUSB,TCSANOW,&oldtio);
if (USBbuffer>0) {free(USBbuffer); USBbuffer=0;}

#endif
}  //end CloseUSB


void btnGetUSB_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
#if Linux
FTControl *tFTControl2;
unsigned int nb,tb;
FILE *fptr;

//tFTControl=GetFTControl("btnGetUSB");

//fprintf(stderr,"USB Clicked %p\n",tFTControl);
if (tFTControl!=0)
  {
  //put data in a file
  //time to sample

if (InitUSB()!=-1)
{

  tFTControl2=GetFTControl("txtGetFile");

  if (strlen(tFTControl2->text))
    {
    fptr=fopen(tFTControl2->text,"wb");
    }
  else
    {
    //fprintf(stderr,"%d\n",strlen(tFTControl2->text));
    fptr=fopen("/root/usb.dat","wb");
    }


  //USB buffer is filled very fast
  //fprintf(stderr,"USB Clicked with value-%d\n",tFTControl->value);
  fprintf(stderr,"reading USB ...\n");

  //read(fdUSB,&USBbuffer[0],1);
//384 byte tx, 128 byte rx
/*  for(a=0;a<5000;a++)  //5000 reads of 200 samples
    {
    //read(fdUSB,USBbuffer,1000000);  //read 1 MB  only gets 4ms~ 4k
    read(fdUSB,&USBbuffer[a*200],200);  //read 1 MB
    fwrite(&USBbuffer[a*200],200,1,fptr);
    }
*/

  //  read(fdUSB,USBbuffer,1000000);  //read 1 MB  only gets 4ms~ 4k

//can only read 40ms at a time
///*
//  for(a=0;a<250;a++)  //5000 reads of 200 samples

fprintf(stderr,"usb=%d\n",fdUSB);
tb=0;
while (tb<500000)
  {
  //read(fdUSB,USBbuffer,1000000);  //read 1 MB  only gets 4ms~ 4k
  nb=read(fdUSB,USBbuffer,1000);  //read 4kb
  //nb=read(fdUSB,USBbuffer,100);  //reads 62b
  if (nb>0)
    {
//    fprintf(stderr,"nb=%d tb=%d\t",nb,tb);
    fwrite(USBbuffer,nb,1,fptr);
    //tb+=100;
    tb+=nb;
    }
    //usleep(2000);  //4000usec while driver buffer fills
    //500=4.5ms
    //1000=8.5ms***, 4.5ms
    //2000=5.5
    //3000=5.5
    //4000=5ms
    //10000=4.5ms
    }

//*/

  fprintf(stderr,"done getting USB data: %x\n",USBbuffer[0]);


  if (fptr>0)
    {
    //fwrite(USBbuffer,1000000,1,fptr);
    //fclose(fptr);
    CloseUSB();  //should close fptr?!
    if (strlen(tFTControl2->text))
      {
      fprintf(stderr,"Data written to file %s.\n",tFTControl2->text);
      }
    else
      {
      fprintf(stderr,"Data written to file /root/usb.dat.\n");
      }
    }
  else
    {
    fprintf(stderr,"File %s not found.\n",tFTControl2->text);
    }


  //fprintf(stderr,"now free\n");
  //fprintf(stderr,"now draw\n");
  //DrawFTControls(tFTControl->win,0);
	DrawFTControls(tFTControl->window,0);
  }   //tFTControl!=0

}//end if InitUSB was success
#endif
}  //end btnGetUSB_Click



void btnGetFile_Click(FTWindow *twin,FTControl *tFTControl,int x, int y)
{


}//end btnGetFile


void txtGetFile_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
{  //key was pressed in open file textbox

}//end txtGetFile_Keypress


int  LoadDataFile(char *tstr,FTControl *tFTControl)
{
return(1);
}  //end LoadDataFile




void txtOpen_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
{  //key was pressed in open file textbox


if (key==13) //enter
{
btnOpenProject_Click(twin,tFTControl,0,0);

}  //end if key==13

}//end txtOpen_Keypress





