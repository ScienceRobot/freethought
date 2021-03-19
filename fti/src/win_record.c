//win_record.c
#include "freethought.h"
#include "win_record.h"


extern ProgramStatus PStatus;
//extern XFontStruct *font_info;

//should be bit flag
int stoprecord;


int record_CreateFTWindow(void)
{
FTWindow twin,*twin2;

twin2=GetFTWindow("record");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"record");
  strcpy(twin.title,"Record File");  //unbelievable, numchar<10
  strcpy(twin.ititle,"Record File");
  //twin.x1=0;
  //twin.y1=0;
  twin.w=300;
  twin.h=70;
  twin.AddFTControls=(FTControlfuncw *)record_AddFTControls;
  twin.Keypress=(FTControlfuncwk *)win2key;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);

  return 1;
  }
return 0;
}  //end record_CreateFTWindow

void win2key(FTWindow *twin,unsigned char key)
{

    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
//        break;
      case 0x1b:	/*esc*/
	      CloseFTWindow(twin);
//        DestroyFTWindow("record");
        break;
      default:
        break;
      }
}



int record_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol;

fprintf(stderr,"Adding FTControls for window 'record'\n");

tcontrol=malloc(sizeof(FTControl));

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"txtRecordFile");
tcontrol->type=CTTextBox; //textbox
tcontrol->x1=20;
tcontrol->y1=5;//Ly1[1];
tcontrol->x2=220;
tcontrol->y2=5+tcontrol->fontheight;
//tcontrol->KeyPress=(FTControlfuncwk *)txtRecord_KeyPress;
strcpy(tcontrol->text,"");
tcontrol->tab=0;
tcontrol->color[0]=FT_WHITE;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
//strcpy(tcontrol->filename,"");

//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
tcontrol->type=0;
strcpy(tcontrol->name,"btnStartRecord");
tcontrol->x1=20;
tcontrol->y1=50;
tcontrol->x2=20+tcontrol->fontwidth*6;
tcontrol->y2=50+tcontrol->fontheight;
strcpy(tcontrol->text,"Record");
//tcontrol->ButtonDown=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnStartRecord_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=0;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnStopRecord");
tcontrol->x1=100;
tcontrol->y1=50;
tcontrol->x2=100+tcontrol->fontwidth*6;
tcontrol->y2=50+tcontrol->fontheight;
strcpy(tcontrol->text,"Stop");
//tcontrol->ButtonDown=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnStopRecord_Click;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=0;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnCloseRecord");
tcontrol->x1=150;
tcontrol->y1=50;
tcontrol->x2=150+tcontrol->fontwidth*6;
tcontrol->y2=50+tcontrol->fontheight;
strcpy(tcontrol->text,"Close");
//tcontrol->ButtonDown=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnCloseRecord_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=0;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

if (tcontrol!=0) {
	free(tcontrol);
}

//#endif
return 1;
} //end record_AddFTControls


void btnStartRecord_Click(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
#if Linux
//unsigned short int buffer[10000];
unsigned char *buffer;
unsigned int buffersize,a;
//FTControl *tFTControl;//,*tFTControl2;
char * tstr;
int outsamplerate,audiobuffersize,outchannels,len;
WaveHdr wh1;
WaveDataHdr wh1d;
int audio_fptr;
FILE *fptr,*fptr2;
int format;



outsamplerate=48000;
audiobuffersize=4096;
outchannels=1;

fprintf(stderr,"Setting mixer to line in.\n");

#if 0
mixerfd = open(mixer, O_RDWR);
     if (mixerfd == -1) {
         perror("Can't open mixer");
         exit(1);
     }


     ioctl(mixerfd, MIXER_READ(SOUND_MIXER_DEVMASK), &devmask);
 res = ioctl(mixerfd, MIXER_READ(SOUND_MIXER_VOLUME), &vol);//1
  res = ioctl(mixerfd, MIXER_READ(SOUND_MIXER_LINE), &vol); //2
 res = ioctl(mixerfd, MIXER_READ(SOUND_MIXER_PCM), &vol); //3
res = ioctl(mixerfd, MIXER_READ(SOUND_MIXER_CD), &vol); //8

     //if (res==-1) {
                 /* An undefined mixer channel was accessed - mark it*/
/* Get current input source */
         res = ioctl(mixerfd, MIXER_READ(SOUND_MIXER_RECSRC), &indevice);
     volume = vol*100/32768;


     res = ioctl(mixerfd, MIXER_READ(device), &vol);

   ioctl(mixerfd, MIXER_WRITE(device), &vol);
#endif


fprintf(stderr,"In start record.\n");


if (PStatus.flags&ERecording)
{
fprintf(stderr,"Already recording.\n");
}
else
{
//fprintf(stderr,"in start recording\n");
PStatus.flags|=ERecording;


//open sound device
//audio_fptr= open("/dev/dsp",O_WRONLY);
audio_fptr= open("/dev/dsp",O_RDONLY);
if (audio_fptr == -1)
  {
  //perror("open /dev/dsp");
  fprintf(stderr,"Problem opening /dev/dsp\n");
  //
  }
fprintf(stderr,"opened\n");


if (ioctl(audio_fptr, SNDCTL_DSP_RESET)==-1)
{ /* Fatal error */
  fprintf(stderr,"error resetting sound card\n");
  //perror("SNDCTL_DSP_CHANNELS");
  close(audio_fptr);
//  return -1;
//

}


  //set 16 bit little endian format
//format = AFMT_unsigned char;
//format = AFMT_U16_LE;
format = AFMT_S16_LE;

if (ioctl(audio_fptr, SNDCTL_DSP_SETFMT, &format)==-1)
  { /* Fatal error */
  fprintf(stderr,"error setting format\n");
  perror("SNDCTL_DSP_SETFMT");

  close(audio_fptr);
  //
  }

  //set channels 1=mono 2=stereo

if (ioctl(audio_fptr, SNDCTL_DSP_CHANNELS, &outchannels)==-1)
{ /* Fatal error */
  fprintf(stderr,"error setting format\n");
  //perror("SNDCTL_DSP_CHANNELS");
  close(audio_fptr);
//  return -1;
//
}

//fprintf(stderr,"set channels complete\n");
//set freq  44100
//speed=samplerate;
if (ioctl(audio_fptr, SNDCTL_DSP_SPEED, &outsamplerate)==-1)
{ /* Fatal error */
perror("SNDCTL_DSP_SPEED");
  close(audio_fptr);
  //return -1;

}



fptr=fopen("temp.wav","wb");
if (fptr!=0)
{
system("chmod 777 temp.wav");

buffersize=0;
//while(buffersize<1000000 && stoprecord!=1)
buffer=malloc(audiobuffersize);
fprintf(stderr,"\n\nStart recording\n");
while(PStatus.flags&ERecording)
{
  if ((len=read(audio_fptr,buffer,audiobuffersize))==-1)
    {
    fprintf(stderr,"error audio read\n");
    }
  //fprintf(stderr,"read 2048\n");
  //fwrite(&buffer,2,1024,fptr);
  fwrite(buffer,audiobuffersize,1,fptr);
  buffersize+=audiobuffersize;

  FT_GetInput();
//end if xevent pending

}  //end while
close(audio_fptr);
fclose(fptr);
fprintf(stderr,"\n\nStop recording\n");


//now add the header
fptr=fopen("temp.wav","rb");
//fptr2=fopen("final.wav","wb");
tcontrol=GetFTControl("txtRecordFile");
if (tcontrol==0 || strlen(tcontrol->text)==0)
{
GetTime();
tstr=malloc(FTMedStr);
sprintf(tstr,"%04d-%02d-%02d_%02d_%02d_%02d.wav",PStatus.ctime->tm_year+1900,PStatus.ctime->tm_mon,PStatus.ctime->tm_mday,PStatus.ctime->tm_hour,PStatus.ctime->tm_min,PStatus.ctime->tm_sec);

//later put timestamp
fprintf(stderr,"Writing recorded data to file %s\n",tstr);
//timestamp
fptr2=fopen(tstr,"wb");
free(tstr);
}
else
{
tstr=malloc(FTMedStr);
if (strlen(tcontrol->text)>4)
{
strcpy(tstr,tcontrol->text);
if (strcasecmp(&tcontrol->text[strlen(tcontrol->text)-4],".wav"))
  {
  strcat(tstr,".wav");
  }
}
else
{
strcpy(tstr,tcontrol->text);
strcat(tstr,".wav");
}
fprintf(stderr,"Writing recorded data to file %s\n",tstr);
fptr2=fopen(tstr,"wb");
free(tstr);
}

//write wav file header  44.1Mhz, 16 bit
wh1.ID=0x46464952;//RIFF
wh1.hssize=sizeof(WaveHdr);//-8;//header
wh1.hssize2=0x45564157;//WAVE
wh1.ckID=0x20746d66;//"fmt ";
wh1.nchunkSize=0x00000010;
wh1.wFormatTag=1;
wh1.Channels=outchannels;
wh1.SamplesPerSecond=outsamplerate*outchannels;//samplerate;
wh1.AvgBytesPerSecond=outsamplerate*2*outchannels;//samplerate*2;
wh1.nBlockAlign=2;
wh1.BitsPerSample=16;

wh1d.ID=0x61746164;//data
//wh1d.chunkSize=0x00000000;  //length of data in bytes
wh1d.chunkSize=buffersize;  //length of data in bytes

fwrite(&wh1,sizeof(WaveHdr),1,fptr2);
fwrite(&wh1d,sizeof(WaveDataHdr),1,fptr2);

//write data
a=0;
//fprintf(stderr,"\n\ngot here in record=%d\n",buffersize);
while (a<buffersize)
{
//fread(buffer,2,1024,fptr);
//fwrite(buffer,2,1024,fptr2);
fread(buffer,1,audiobuffersize,fptr);
fwrite(buffer,1,audiobuffersize,fptr2);
a+=audiobuffersize;
}
fclose(fptr);
fclose(fptr2);

free(buffer);

fprintf(stderr,"Done writing file.\n");

}  //if fptr!=0

}  //end if already recording
#endif
}


void btnStopRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
PStatus.flags&=~ERecording;
}

void btnCloseRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{

fprintf(stderr,"Close record click\n");
CloseFTWindowByName("record");
}


int record(void)
{
#if Linux
unsigned short int buffer[10000];
int buffersize,a,outchannels,outsamplerate;
WaveHdr wh1;
WaveDataHdr wh1d;
int audio_fptr;
FILE *fptr,*fptr2;
int format;


outsamplerate=48000;
outchannels=1;

//open sound device
//audio_fptr= open("/dev/dsp",O_WRONLY);
audio_fptr= open("/dev/dsp",O_RDONLY);
if (audio_fptr == -1)
  {
  perror("open /dev/dsp");
  return -1;
  }

  //set 16 bit little endian format
//format = AFMT_unsigned char;
//format = AFMT_U16_LE;
format = AFMT_S16_LE;
if (ioctl(audio_fptr, SNDCTL_DSP_SETFMT, &format)==-1)
  { /* Fatal error */
  perror("SNDCTL_DSP_SETFMT");
  close(audio_fptr);
  return -1;
  }

  //set channels 1=mono 2=stereo
if (ioctl(audio_fptr, SNDCTL_DSP_CHANNELS, &outchannels)==-1)
{ /* Fatal error */
  perror("SNDCTL_DSP_CHANNELS");
  close(audio_fptr);
  return -1;
//
}


if (ioctl(audio_fptr, SNDCTL_DSP_SPEED, &outsamplerate)==-1)
{ /* Fatal error */
perror("SNDCTL_DSP_SPEED");
  close(audio_fptr);
  return -1;

}


fptr=fopen("temp.wav","wb");

buffersize=0;
//while(buffersize<1000000 && stoprecord!=1)
fprintf(stderr,"\n\nStart recording\n");
while(stoprecord!=1)
{
  read(audio_fptr,buffer,2048);
  fwrite(&buffer,2,1024,fptr);
  buffersize+=2048;

  FT_GetInput();
//end if xevent pending

}  //end while
close(audio_fptr);
fclose(fptr);
fprintf(stderr,"\n\nStop recording\n");


//now add the header
fptr=fopen("temp.wav","rb");
fptr2=fopen("final.wav","wb");

//write wav file header  44.1Mhz, 16 bit
wh1.ID=0x46464952;//RIFF
wh1.hssize=sizeof(WaveHdr);//-8;//header
wh1.hssize2=0x45564157;//WAVE
wh1.ckID=0x20746d66;//"fmt ";
wh1.nchunkSize=0x00000010;
wh1.wFormatTag=1;
wh1.Channels=outchannels;
wh1.SamplesPerSecond=outsamplerate;
wh1.AvgBytesPerSecond=outsamplerate*2;
wh1.nBlockAlign=2;
wh1.BitsPerSample=16;

wh1d.ID=0x61746164;//data
//wh1d.chunkSize=0x00000000;  //length of data in bytes
wh1d.chunkSize=buffersize;  //length of data in bytes

fwrite(&wh1,sizeof(WaveHdr),1,fptr2);
fwrite(&wh1d,sizeof(WaveDataHdr),1,fptr2);

//write data
a=0;
//fprintf(stderr,"\n\ngot here in record=%d\n",buffersize);
while (a<buffersize)
{
fread(&buffer,2,1024,fptr);
fwrite(&buffer,2,1024,fptr2);
a+=2048;
}
fclose(fptr);
fclose(fptr2);

return(1);
#endif
}  //end record



