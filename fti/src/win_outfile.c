//win_outfile.c
#include "freethought.h"
#include "win_outfile.h"

extern ProgramStatus PStatus;
//extern XFontStruct *font_info;
extern int *Ly1,*Ly2;  //levels for buttons


int outfile_CreateFTWindow(void)
{
FTWindow twin,*twin2;

twin2=GetFTWindow("outfile");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"outfile");
  strcpy(twin.title,"Make File");  //unbelievable, numchar<10
  strcpy(twin.ititle,"Make File");
  twin.w=300;
  twin.h=300;
  twin.Keypress=(FTControlfuncwk *)outfile_Keypress;
  twin.AddFTControls=(FTControlfuncw *)outfile_AddFTControls;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);


//Add Combo submenu windows
//wincmbtest1_CreateFTWindow();


  //fprintf(stderr,"b4 free %p %p\n",twin->ititle,twin);
  return 1;
  }
return 0;
}  //end outfile_CreateFTWindow


void outfile_Keypress(FTWindow *twin,unsigned char key)
{


    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
 //       break;
      case 0x1b:	/*esc*/
	      //CloseFTWindow(twin);
        break;
      default:
        break;
      }
}


int outfile_AddFTControls(FTWindow *twin)
{
FTControl tFTControl;
FTItem tFTItem;
int OptionGroup;


if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'outfile'\n");

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"chkAudio");
tFTControl.type=CTCheckBox;
tFTControl.x1=110;
tFTControl.y1=10;//Ly1[1];
tFTControl.x3=125;
tFTControl.y3=25;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=1;
strcpy(tFTControl.text,"Audio");

AddFTControl(twin,&tFTControl);

OptionGroup=GetFreeOptionGroupNumber(twin);
memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"rad48k");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=30;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=45;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=1;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"48000 Hz");

AddFTControl(twin,&tFTControl);

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"rad44k");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=50;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=65;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=0;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"44000 Hz");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);

OptionGroup=GetFreeOptionGroupNumber(twin);
memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"rad16bit");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=70;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=85;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=1;
tFTControl.OptionGroup=OptionGroup;
//fprintf(stderr,"add rad16 text=%p\n",tFTControl.text);
strcpy(tFTControl.text,"16 bits/sample");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
fprintf(stderr,"add rad16 2\n");
AddFTControl(twin,&tFTControl);

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"rad8bit");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=90;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=105;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=0;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"8 bits/sample");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);

OptionGroup=GetFreeOptionGroupNumber(twin);
memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"radstereo");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=110;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=125;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=1;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"Stereo");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"radmono");
tFTControl.type=CTRadio;
tFTControl.x1=120;
tFTControl.y1=130;//Ly1[1];
tFTControl.x3=135;
tFTControl.y3=145;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
tFTControl.value=0;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"Mono");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);

memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"chkDec");
tFTControl.type=CTCheckBox;
tFTControl.x1=110;
tFTControl.y1=150;//Ly1[1];
tFTControl.x3=125;
tFTControl.y3=165;//Ly2[1];
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
//tFTControl.value=1;
strcpy(tFTControl.text,"Decrease");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);



//test combo
memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"cmbTest");
tFTControl.type=CTDropdown;
tFTControl.x1=40;//120;
tFTControl.y1=200;
tFTControl.x2=200;  //right of combo
tFTControl.y2=225;
//tFTControl.x3=120;  //left of box with triangle
//tFTControl.y3=225;
//tFTControl.Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
//tFTControl.color[0]=FT_WHITE;  //bkground
//tFTControl.color[1]=FT_BLACK;  //text
//tFTControl.color[2]=FT_LIGHT_GRAY;
tFTControl.color[0]=FT_WHITE;  //background of combo text area 
tFTControl.color[1]=FT_DARK_GRAY;  //background (area)
tFTControl.color[2]=FT_BLUE;  //mouse over
tFTControl.color[3]=FT_GRAY;  //bkground press
tFTControl.value=0;
tFTControl.OptionGroup=OptionGroup;
strcpy(tFTControl.text,"Video Type");  //current value
strcpy(tFTControl.text2,"Video Type");  //original value
//strcpy(tFTControl.submenu,"wincmbtest1");  //submenu

memset(&tFTItem,0,sizeof(FTItem));
strcpy(tFTItem.name,"AVI (uncompressed)");
AddFTItem(&tFTControl,&tFTItem);
strcpy(tFTItem.name,"AVI (MJPEG)");
AddFTItem(&tFTControl,&tFTItem);
strcpy(tFTItem.name,"AVI (MJPEG-1)");
AddFTItem(&tFTControl,&tFTItem);
strcpy(tFTItem.name,"AVI (MJPEG-2)");
AddFTItem(&tFTControl,&tFTItem);


tFTControl.flags=CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);



memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"txtOutFile");
tFTControl.type=CTTextBox;//2; //textbox
tFTControl.x1=10;
tFTControl.y1=270;//Ly1[1];
tFTControl.x2=190;
tFTControl.y2=290;//Ly2[1];
//tFTControl.win=1;
//tFTControl.tab=2;
tFTControl.Keypress=(FTControlfuncwck *)txtOutFile_Keypress;
//memset(&tFTControl.text,0,FTMedStr-1);
//strcpy(tFTControl.text,"");
tFTControl.color[0]=FT_WHITE;  //bkground
tFTControl.color[1]=FT_BLACK;  //text
//memset(&tFTControl.filename,0,FTMedStr-1);
////strcpy(tFTControl.filename,"");
tFTControl.flags=CGrowY1|CGrowX2|CGrowY2;
AddFTControl(twin,&tFTControl);


memset(&tFTControl,0,sizeof(FTControl));
strcpy(tFTControl.name,"btnOutFile");
tFTControl.type=CTButton;//0;//2;
//tFTControl.win=1;
tFTControl.x1=200;
tFTControl.y1=270;
tFTControl.x2=200+tFTControl.fontwidth*13;
tFTControl.y2=290;
strcpy(tFTControl.text,"Make Out File");
tFTControl.ButtonDownUp[0]=(FTControlfuncwcxy *)btnOutFile_Click;
//tFTControl.tab=-1;
//strcpy(tFTControl.filename,"");
tFTControl.flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
tFTControl.color[0]=FT_LIGHT_GRAY;
tFTControl.color[1]=0;
//tFTControl.textcolor
AddFTControl(twin,&tFTControl);


if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to outfile.\n");

//#endif
return 1;
} //end outfile_AddFTControls






void txtOutFile_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
{
if (key==13) //enter
{
btnOutFile_Click(twin,tFTControl,0,0);

}  //end if key==13

}



void btnOutFile_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//FTControl *tFTControl;

tFTControl=GetFTControl("txtOutFile");
//MakeOutFile(tFTControl->text);

}  //btnOutFile_Click



#if 0
void MakeOutFile(char *name)
{  //make an output file (.wav) of all data on tracks
FTControl **tFTControl,*tFTControl2,*tFTControl3;
//i64 buffersize,buffertotal;
i64 numdata,maxdatasize,a,b,c,StartTime,endtime,tx;
//i64 audiobuffersize
i64 inc,mask;
i16 fsample16[MaxNumChannels];
char tstr[FTMedStr];//,*buffer;
int outsamplerate,outchannels,bitspersample;
int ex,maxchan,tlen;
int decrease;
FILE *fptr;
WaveHdr wh;
WaveDataHdr wdh;


tlen=strlen(name);


if (tlen>0)
{
if (tlen>4)
{
if (strcasecmp(&name[tlen-4],".wav"))
  strcat(name,".wav");
//fprintf(stderr,"file extension: %s %d\n",tstr,tlen);
}
else
{
strcat(name,".wav");
}

fprintf(stderr,"\nSave wave file: \"%s\".\n",name);

if ((fptr=fopen(name,"wb")))
{
//outfile
//should check to see if any data FTControls have track 1,2,3...
//delete bit tests

//go thru each FTControl in window 0
//mark in a 64 bit flag


//outsamplerate=44100;
//outsamplerate=44100;
outsamplerate=48000;
outchannels=1;//1;  should get this from wave files
bitspersample=16;
//audiobuffersize=2048;

//GetTracksMaxChannels()
//GetTracksMaxSampleRate()
//GetTrackMaxbitspersample()

decrease=0;
//these FTControls will only exist if Out Window is open
if (GetFTWindow("outfile"))
{
tFTControl2=GetFTControl("rad44k");
if (tFTControl2->value==1) outsamplerate=44100;
tFTControl2=GetFTControl("rad8bit");
if (tFTControl2->value==1) bitspersample=8;
tFTControl2=GetFTControl("radmono");
if (tFTControl2->value==1) outchannels=1;
tFTControl2=GetFTControl("chkDec");
if (tFTControl2->value==1) decrease=1;
}


fprintf(stderr,"Make Out File %s\n",name);

numdata=0;
maxdatasize=0;
//get first FTControl of window 0
tFTControl2=PStatus.iwindow->icontrol;
tFTControl=(FTControl **)malloc(sizeof(FTControl *)*MaxNumData);
if (tFTControl2>0)
{
ex=0;
while (ex==0)
{
if (tFTControl2->track>0)
  {
//  tFTControl[numdata]=malloc(sizeof(FTControl *));
  tFTControl[numdata]=tFTControl2;
  fprintf(stderr,"sps of data=%i\n",tFTControl[numdata]->nSamplesPerSec);
//  if (tFTControl[numdata]->filesize>maxdatasize)
//    maxdatasize=tFTControl[numdata]->filesize;

  numdata++;
  }

if (tFTControl2->next==0) ex=1;
else tFTControl2=tFTControl2->next;
}  //end while ex==0
}  //end if there are FTControls tFTControl2>0

if (numdata>0)  //there is data to write
{

//in play/make file window:
//play entire project or selection
//48000,44100,...
//channels 1,2,...


//use mark in/out
tFTControl3=GetFTControl("btnMark");
//StartTime=PStatus.markin;
//endtime=PStatus.markout;
StartTime=tFTControl3->ia[0];  //markin
endtime=tFTControl3->ia[1];//markout;

//filesize has to be exact
inc=1000000000/outsamplerate;  //in ns
//maxdatasize=(i64)((long double)(endtime-StartTime+inc)*2.0*outsamplerate/1000000000.0);
//maxdatasize=(i64)((long double)(endtime-StartTime+inc)/inc)*2;
maxdatasize=((endtime-StartTime)/inc)*2*outchannels;
//maxdatasize&=~(0x1);

//ns*bps*sps*chan/1e9
//=total bytes (not samples)
//fprintf(stderr,"maxdata=%lli\n",maxdatasize);

//fptr=fopen(name,"wb");

//if (fptr!=0)
//{

strcpy(tstr,"chmod 777 ");
strcat(tstr,name);
system(tstr);

//if exists?
//freq=48000, bps=16, channels=1
wh.ID=0x46464952;//RIFF
wh.hssize=sizeof(WaveHdr);//some wav files have size of data+header (0x24)
wh.hssize2=0x45564157;//WAVE
wh.ckID=0x20746d66;//"fmt ";
wh.nchunkSize=0x00000010;
wh.wFormatTag=1;
wh.nChannels=outchannels;
wh.nSamplesPerSec=outsamplerate;//48000;//samplerate;
wh.nAvgBytesPerSec=outsamplerate*2;//48000*2;//samplerate*2;
wh.nBlockAlign=2;
wh.nflagsPerSample=16;




wdh.ID=0x61746164;//data
wdh.chunkSize=maxdatasize;  //length of data in bytes

fwrite(&wh,sizeof(WaveHdr),1,fptr);
fwrite(&wdh,sizeof(WaveDataHdr),1,fptr);

fprintf(stderr,"number of pieces of data on tracks numdata=%lli\n",numdata);

//tFTControl2=GetFTControl("btnTimeUnit");

//fprintf(stderr,"inc=%lli\n",inc);
//fprintf(stderr,"numdata=%lli\n",numdata);

fprintf(stderr,"st=%lli\t",StartTime);
fprintf(stderr,"et=%lli\n",endtime);
fprintf(stderr,"inc=%lli\n",inc);
//for(a=StartTime+inc;a<endtime;a+=inc)
//c=0;
//for(a=StartTime+inc;a<endtime;a+=inc)
for(a=StartTime+inc;a<endtime;a+=inc)
 {
 //c++;
  //go thru data on tracks and see if any is at this time
  for(b=0;b<MaxNumChannels;b++)
    {
    fsample16[b]=0;
    }

 maxchan=0;
 for(b=0;b<numdata;b++)  //for each piece of data
  {
  if (tFTControl[b]->StartTime<a && a<=(tFTControl[b]->StartTime+tFTControl[b]->Duration))
    {  //ns
    if (maxchan<tFTControl[b]->nChannels) maxchan=tFTControl[b]->nChannels;
//    rx=tFTControl[b]->nSamplesPerSec;//48000
//    rx*=tFTControl->nflagsPerSample/8.0;  //0.0096

    //tx=(i64)(((long double)(a-tFTControl[b]->StartTime)*outsamplerate*2.0)/1000000000.0);
    //may need to change from 44100 to 48000 for example

//    fprintf(stderr,"%i\t",tFTControl[b]->nSamplesPerSec);
//do not need the output sample rate, that will automatically be calculated
//by the for a loop interval
//for example if output is 44100 and sample is 22050
//the sample 2050 sample will be written twice because the a interval will be
//twice as fast.  Where there was 1 sample in 1 ms, there will now be 2 (duplicate) samples

    mask=~(i64)(pow(2.0,(long double)tFTControl[b]->nChannels)-1);
    //tx=(i64)((a-tFTControl[b]->StartTime+tFTControl[b]->oStartTime)/inc)*2;
    tx=(i64)((long double)((a-tFTControl[b]->StartTime+tFTControl[b]->oStartTime)*tFTControl[b]->nSamplesPerSec*tFTControl[b]->nChannels)/1000000000.0)*2;
    //tx=(i64)((long double)((a-tFTControl[b]->StartTime)*(long double)outsamplerate*((long double)tFTControl[b]->nSamplesPerSec)*2.0)/1000000000.0);
    //tx&=(~0x1);  //make even will be even because *2

    tx&=mask;
//    if (tx<0) tx=0;
    //fprintf(stderr,"%lli:%llx\t",a,tx);
    //if ((tx+1)<tFTControl[b]->filesize)  //just in case
     // for(c=0;c<tFTControl[b]->nChannels;c++)
     //   {

c=tFTControl[b]->Channel;	fsample16[c]+=((tFTControl[b]->data[tx+1+c*2]<<0x8)|(unsigned char)tFTControl[b]->data[tx+c*2]);
	//
	//}
    //fprintf(stderr,"%i\t",fsample16);
    }
  }  //end b for each piece of data
  for(c=0;c<outchannels;c++)
    {
    if (decrease) {fsample16[c]=(i16)((float)fsample16[c]*.8);}  //decrease volume by 20%
    fwrite(&fsample16[c],2,1,fptr);
    }
 }  //end a for each ns


fclose(fptr);

fprintf(stderr,"Done making file %s\n",name);

CloseFTWindowByName("outfile");



}  //fopen==0
else
{
fprintf(stderr,"Could not open file %s\n",name);
}

}  //end if numdata>0

}  //end if tlen>0

}  //end MakeOutFile
#endif
