//win_saveproject.c
#include "freethought.h"
#include "win_saveproject.h"

extern ProgramStatus PStatus;
//extern XFontStruct *font_info;
extern int *Ly1,*Ly2;  //levels for buttons


int saveproject_CreateFTWindow(void)
{
FTWindow twin,*twin2;

twin2=GetFTWindow("saveproject");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"saveproject");
  strcpy(twin.title,"Save Project");  //unbelievable, numchar<10
  strcpy(twin.ititle,"Save Project");
  //twin.x1=0;
  //twin.y1=0;
  twin.w=300;
  twin.h=70;
  twin.AddFTControls=(FTControlfuncw *)saveproject_AddFTControls;
  twin.Keypress=(FTControlfuncwk *)win5key;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);
  return 1;
  }
return 0;
}  //end openproject_CreateFTWindow



//void win5key(FTWindow *twin,unsigned char key)
void win5key(FTWindow *twin,KeySym key)
{

    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
//        break;
//      case 0x1b:	/*esc*/
      case XK_Escape:	/*esc*/
        CloseFTWindow(twin);
//        DelWindow("saveproject");
        break;
      default:
        break;
      }
}


int saveproject_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'saveproject'\n");

tcontrol=malloc(sizeof(FTControl));
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"txtSave");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
//if (tcontrol->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
tcontrol->type=CTTextBox;//2; //textbox
tcontrol->x1=80;
tcontrol->y1=45;//Ly1[1];
tcontrol->x2=280;
tcontrol->y2=101;//Ly2[1];
//tcontrol->win=1;
//tcontrol->tab=2;
tcontrol->Keypress=(FTControlfuncwck *)txtSave_Keypress;
//memset(tcontrol.text,0,FTMedStr-1);
//strcpy(tcontrol->text,"");
tcontrol->color[0]=FT_WHITE;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
//memset(tcontrol.filename,0,FTMedStr-1);
////strcpy(tcontrol->filename,"");

//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnSaveProject");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
//if (tcontrol->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
tcontrol->type=CTButton;//0;//2;
//tcontrol->win=1;
tcontrol->x1=30;
tcontrol->y1=80;
tcontrol->x2=130+tcontrol->fontwidth*4;
tcontrol->y2=100+tcontrol->fontheight;
strcpy(tcontrol->text,"Save");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnSaveProject_Click;
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
  fprintf(stderr,"done with adding FTControls to saveproject.\n");
 
  
//#endif
return 1;
} //end saveproject_AddFTControls


//void txtSave_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
void txtSave_Keypress(FTWindow *twin,FTControl *tFTControl,KeySym key)
{  //key was pressed in open file textbox

//if (key==13) //enter
if (key==XK_Return) //enter
{
btnSaveProject_Click(twin,tFTControl,0,0);

}  //end if key==13

}//end txtSave_Keypress


void btnSaveProject_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
int a,tlen,numdata,ex;
FTControl **tFTControl2,*tFTControl3;
FILE *fptr;


tFTControl=GetFTControl("txtSave");
tlen=strlen(tFTControl->text);


if (tlen>0)
{



if (tlen>4)
{
#if Linux
if (strcasecmp(&tFTControl->text[tlen-4],".prj"))
#endif
#if WIN32
if (stricmp(&tFTControl->text[tlen-4],".prj")==0)
#endif
	strcat(tFTControl->text,".prj");
//fprintf(stderr,"file extension: %s %d\n",tstr,tlen);
}
else
{
strcat(tFTControl->text,".prj");
}

fprintf(stderr,"\nSave Project: \"%s\".\n",tFTControl->text);

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

fprintf(stderr,"numdata=%d.\n",numdata);
fwrite(&numdata,sizeof(int),1,fptr);
for(a=0;a<numdata;a++)
{
fwrite(&tFTControl2[a]->filename,FTMedStr,1,fptr);
fwrite(&tFTControl2[a]->track,sizeof(int),1,fptr);
fwrite(&tFTControl2[a]->Channel,sizeof(int),1,fptr);
fwrite(&tFTControl2[a]->StartTime,sizeof(long long),1,fptr);
fwrite(&tFTControl2[a]->EndTime,sizeof(long long),1,fptr);
//fwrite(tFTControl2[a]->oStartTime,sizeof(long long),1,fptr);
}  //for a

free(tFTControl2);

fclose(fptr);

CloseFTWindowByName("saveproject");
fprintf(stderr,"\nProject \"%s\" saved.\n",tFTControl->text);


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


} //btnSaveProject_Click

