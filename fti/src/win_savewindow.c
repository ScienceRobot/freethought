//win_savewindow.c
#include "freethought.h"
#include "win_savewindow.h"

extern ProgramStatus PStatus;
//extern XFontStruct *font_info;
extern int *Ly1,*Ly2;  //levels for buttons


int savewindow_CreateFTWindow(void)
{
FTWindow twin,*twin2;

twin2=GetFTWindow("savewindow");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"savewindow");
  strcpy(twin.title,"Save Window");
  strcpy(twin.ititle,"Save Window");
//  twin.x1=0;
//  twin.y1=0;
  twin.w=300;
  twin.h=70;
  twin.AddFTControls=(FTControlfuncw *)savewindow_AddFTControls;
  twin.Keypress=(FTControlfuncwk *)savewindow_Keypress;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);
  return 1;
  }
return 0;
}  //end savewindow_CreateFTWindow



void savewindow_Keypress(FTWindow *twin,unsigned char key)
{

    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
//        break;
      case 0x1b:	/*esc*/
        CloseFTWindow(twin);
        break;
      default:
        break;
      }
}


int savewindow_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'savewindow'\n");

tcontrol=malloc(sizeof(FTControl));
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"txtSaveWinName");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
//if (tcontrol->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
tcontrol->type=CTTextBox;//2; //textbox
tcontrol->x1=80;
tcontrol->y1=45;//Ly1[1];
tcontrol->x2=280;
tcontrol->y2=101;//Ly2[1];
//tcontrol->win=1;
//tcontrol->tab=2;
tcontrol->Keypress=(FTControlfuncwck *)txtSaveWinName_Keypress;
//memset(&tcontrol->text,0,FTMedStr-1);
//strcpy(tcontrol->text,"");
tcontrol->color[0]=FT_WHITE;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
//memset(&tcontrol->filename,0,FTMedStr-1);
////strcpy(tcontrol->filename,"");

//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnSaveWin");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
//if (tcontrol->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
tcontrol->type=CTButton;//0;//2;
//tcontrol->win=1;
tcontrol->x1=30;
tcontrol->y1=80;
//tcontrol->x2=130+tcontrol->fontwidth*4;
//tcontrol->y2=100+tcontrol->fontheight;
tcontrol->x2=130+tcontrol->fontwidth*4;
tcontrol->y2=100+tcontrol->fontheight;

strcpy(tcontrol->text,"Save");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnSaveWindow_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=0;
//tcontrol->textcolor
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

if (tcontrol!=0) {
	free(tcontrol);
}

if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to savewindow.\n");

//#endif
return 1;
} //end saveproject_AddFTControls


void txtSaveWinName_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
{  //key was pressed in open file textbox

if (key==13) //enter
{
btnSaveWindow_Click(twin,tFTControl,0,0);

}  //end if key==13

}//end txtSave_Keypress


void btnSaveWindow_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
int a,tlen,numdata,ex;
FTControl **tFTControl2,*tFTControl3;
FILE *fptr;


tFTControl=GetFTControl("txtSaveWinName");
tlen=strlen(tFTControl->text);


if (tlen>0)
{



if (tlen>4)
{

#if Linux
if (strcasecmp(&tFTControl->text[tlen-4],".win"))  //.ftw
#endif
#if WIN32
if (stricmp(&tFTControl->text[tlen-4],".win")==0)  //.ftw
#endif
  strcat(tFTControl->text,".win");
//fprintf(stderr,"file extension: %s %d\n",tstr,tlen);
}
else
{
strcat(tFTControl->text,".win");
}

fprintf(stderr,"\nSave Window: \"%s\".\n",tFTControl->text);

if ((fptr=fopen(tFTControl->text,"wb")))
{
//btnTimeLine
//btnMarkIn
//btnMarkOut
//num data FTControls
//data FTControls (data FTControl 1, data FTControl 2, ....)
//tFTControl3=GetFTControl("btnTimeLine");
//fwrite(tcontrol3->StartTime,sizeof(long long),1,fptr);
//fwrite(&PStatus.timescale,sizeof(long long),1,fptr);
//tFTControl3=GetFTControl("btnMark");
//fwrite(tcontrol3->ia[0],sizeof(long long),1,fptr);//markin
//fwrite(tcontrol3->ia[1],sizeof(long long),1,fptr);//markout

tFTControl3=PStatus.iwindow->icontrol;
tFTControl2=(FTControl **)malloc(sizeof(MaxNumData));
ex=0;
numdata=0;
if (tFTControl3>0)
{
while (ex==0)
{
if (tFTControl3->track>0)
  {
  tFTControl2[numdata]=tFTControl3;
//  fprintf(stderr,"track=%s.\n",tFTControl2[numdata]);
  numdata++;
  }

if (tFTControl3->next==0) ex=1;
else tFTControl3=tFTControl3->next;
}  //end while ex==0
}  //end if tFTControl3>0

//fprintf(stderr,"numdata=%d.\n",numdata);
fwrite(&numdata,sizeof(int),1,fptr);
for(a=0;a<numdata;a++)
{
fwrite(&tFTControl2[a]->filename,FTMedStr,1,fptr);
fwrite(&tFTControl2[a]->track,sizeof(int),1,fptr);
fwrite(&tFTControl2[a]->Channel,sizeof(int),1,fptr);
fwrite(&tFTControl2[a]->StartTime,sizeof(long long),1,fptr);
fwrite(&tFTControl2[a]->EndTime,sizeof(long long),1,fptr);
//fwrite(&tFTControl2[a]->oStartTime,sizeof(long long),1,fptr);
}  //for a

free(tFTControl2);

fclose(fptr);

CloseFTWindowByName("savewindow");
//fprintf(stderr,"\nWindow \"%s\" saved.\n",tFTControl->text);


strcpy(PStatus.Project,tFTControl->text);
strcpy(PStatus.iwindow->title,"Video Edit (");
strcat(PStatus.iwindow->title,tFTControl->text);
strcat(PStatus.iwindow->title,")");
strcpy(PStatus.iwindow->ititle,"Video Edit (");
strcat(PStatus.iwindow->ititle,tFTControl->text);
strcat(PStatus.iwindow->ititle,")");
PStatus.flags&=~(EChanged);

#if Linux
XSetStandardProperties(PStatus.xdisplay,PStatus.iwindow->xwindow,PStatus.iwindow->title,PStatus.iwindow->ititle,None,NULL,0,NULL);
XFlush (PStatus.xdisplay);
#endif



}  //fopen==0
else
{
fprintf(stderr,"Could not open file %s\n",tFTControl->text);
}

}  //end if tlen>0


} //btnSaveWindow_Click

