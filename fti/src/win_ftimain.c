//win_main.c
#include "freethought.h"
#include "FT_Windows.h"
#include "FT_Controls.h"
#include "FT_UserInput.h"
#include "win_ftimain.h"
#include "win_openproject.h"
#include "win_saveproject.h"
#include "win_openfile.h"
#include "win_outfile.h"
#include "win_record.h"
#include "win_prop.h"
#include "win_control.h"

extern ProgramStatus PStatus;
FTIStatus FStatus;  //FTI global variables
int *Ly1,*Ly2;  //levels for buttons

//PStatus.FirstWindow=(FTControlfunc *)main_CreateFTWindow;

//Open Window
int main_CreateFTWindow(void)
{
FTWindow twin,*twin2;

//Because of focus stealing prevention - we need to create the control and properties window first

cont_CreateFTWindow();
prop_CreateFTWindow();


//twin=GetFTWindowName("main");
memset(&twin,0,sizeof(FTWindow));
strcpy(twin.name,"main");
strcpy(twin.title,"Freethought");
strcpy(twin.ititle,"Freethought");
twin.x=160;
  //todo: account for windows decorations (width)
#if WIN32
twin.x=170; 
#endif
twin.y=100;
twin.w=640;
twin.h=480;
twin.Keypress=(FTControlfuncwk *)main_Keypress;
twin.OnOpen=(FTControlfuncw *)main_OnOpen;
twin.GotFocus=(FTControlfuncw *)main_GotFocus;
//twin.ButtonDownUpOnFTControl[0]=(FTControlfuncwcxy *)main_ButtonDownUpOnFTControl;
twin.AddFTControls=(FTControlfuncw *)main_AddFTControls;

fprintf(stderr,"create window main_CreateFTWindow\n");
CreateFTWindow(&twin);
twin2=GetFTWindow(twin.name);
DrawFTWindow(twin2);

fprintf(stderr,"end main_CreateFTWindow\n");
return 1;
} //end main_CreateFTWindow



void main_Keypress(FTWindow *twin,unsigned char key)
{
}  //main_Keypress



//Open FTControls
int main_AddFTControls(FTWindow *twin)
{
//FTControl tFTControl;
FTControl *tcontrol2,*tcontrol;
FTItem titem,*titem2;
int fh,fw,a;
int Ly1[30],Ly2[30];

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'main'\n");




//there are track levels
//   fh=tFTControl->font->max_bounds.ascent+tFTControl->font->max_bounds.descent;
fw=twin->fontwidth;
fh=twin->fontheight;


//fh=tFTControl->font->max_bounds.ascent;
Ly1[0]=0;
Ly2[0]=fh+4;
Ly1[1]=Ly2[0]+1;
Ly2[1]=Ly2[0]+fh*2;
Ly1[2]=Ly2[1]+1;
Ly2[2]=Ly2[1]+fh*2;
Ly1[3]=Ly2[2]+1;
Ly2[3]=Ly2[2]+fh*2;
//Ly1[4]=Ly2[3]+1;
//Ly2[4]=Ly2[3]+fh*2;
for (a=4;a<20;a++)  //12 levels on window
{
Ly1[a]=Ly2[a-1]+1;
Ly2[a]=Ly2[a-1]+fh*3;
}




tcontrol=(FTControl *)malloc(sizeof(FTControl));


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"fmFile");
tcontrol->type=CTFileMenu;//6;  //menu FTItem
tcontrol->x1=0;
tcontrol->y1=Ly1[0];

tcontrol->hotkey[0]=PAlt|0x66;  //alt-f
tcontrol->hotkey[1]=PAlt|0x46;  //alt-F
strcpy(tcontrol->text,"&File");
AddFTControl(twin,tcontrol);


tcontrol2=GetFTControl("fmFile"); //get a pointer to the above added FTControl
//Add items to File Menu
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&New");
titem.hotkey[0]=0x6e; //n  
titem.hotkey[1]=0x4e; //N
//tittem.click[0]=(FTControlfuncwcxy *)btnNewProject_Click;
AddFTItem(tcontrol2,&titem);

//File, New has 3 subitems: Project, Window, Control
titem2=GetFTItemFromFTControl(tcontrol2,"&New");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Project");
titem.hotkey[0]=0x70; //p  
titem.hotkey[1]=0x50; //P
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Window");
titem.hotkey[0]=0x77; //w  
titem.hotkey[1]=0x57; //W
titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Control");
titem.hotkey[0]=0x65; //c  
titem.hotkey[1]=0x45; //C
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);


memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Open");
titem.hotkey[0]=0x6f; //o
titem.hotkey[1]=0x4f; //O
//tFTItem.click[0]=(FTControlfuncwcxy *)FTItemOpen_Click;
AddFTItem(tcontrol2,&titem);
//File, Open, has 3 subitems: Project, Window, Control
titem2=GetFTItemFromFTControl(tcontrol2,"&Open");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Project");
titem.hotkey[0]=0x70; //p  
titem.hotkey[1]=0x50; //P
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Window");
titem.hotkey[0]=0x77; //w  
titem.hotkey[1]=0x57; //W
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Control"); //ppl just click on control buttons for now
//strcpy(tFTItem.submenu,"winfms2Open");
titem.hotkey[0]=0x65; //c  
titem.hotkey[1]=0x45; //C
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);



memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Save &As");
titem.hotkey[0]=0x62; //a  
titem.hotkey[1]=0x42; //A
AddFTItem(tcontrol2,&titem);
//File, Save As, has 3 subitems: Project, Window, Control
titem2=GetFTItemFromFTControl(tcontrol2,"Save &As");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Project");
titem.hotkey[0]=0x70; //p  
titem.hotkey[1]=0x50; //P
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Window");
titem.hotkey[0]=0x77; //w  
titem.hotkey[1]=0x57; //W
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Control"); //ppl just click on control buttons for now
titem.hotkey[0]=0x65; //c  
titem.hotkey[1]=0x45; //C
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Save");
titem.hotkey[0]=0x73; //s
titem.hotkey[1]=0x53; //S
AddFTItem(tcontrol2,&titem);
//File, Save, has 3 subitems: Project, Window, Control
titem2=GetFTItemFromFTControl(tcontrol2,"&Save");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Project");
titem.hotkey[0]=0x70; //p  
titem.hotkey[1]=0x50; //P
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Window");
titem.hotkey[0]=0x77; //w  
titem.hotkey[1]=0x57; //W
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Control"); //ppl just click on control buttons for now
titem.hotkey[0]=0x65; //c  
titem.hotkey[1]=0x45; //C
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Build Project");
AddFTItem(tcontrol2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Open &Recent Project");
AddFTItem(tcontrol2,&titem);
//File, Open Recent Project, has 3 subitems: file 1, file 2, file 3
titem2=GetFTItemFromFTControl(tcontrol2,"Open &Recent Project");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"file 1");
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"file 2");
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"file 3");
//titem.click[0]=(FTControlfuncwcxy *)FTItemNewWindow_Click;
AddFTSubItem(titem2,&titem);


memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"E&xit");
titem.hotkey[0]=0x78;  //x
titem.hotkey[1]=0x58;  //X
titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_Exit_Click;
AddFTItem(tcontrol2,&titem);

//done adding items to filemenu fmFile



memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"fmEdit");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->type=CTFileMenu;//6;  //menu FTItem
tcontrol->x1=fw*6+1;
tcontrol->y1=Ly1[0];
//tcontrol->x2=tcontrol->fontwidth*12;
//tcontrol->y2=Ly2[0];
//strcpy(tcontrol->text,"File");
////tcontrol->tab=-1;
////strcpy(tcontrol->filename,"");
//
tcontrol->color[0]=FT_LIGHT_GRAY;  //background no press
tcontrol->color[1]=FT_BLACK;//dkgray;  //text
tcontrol->color[2]=FT_BLUE;  //background (mouse over)
tcontrol->color[3]=FT_GRAY;  //bkground press
//tcontrol->win=twin->num;

tcontrol->hotkey[0]=PAlt|0x65;  //alt-e
tcontrol->hotkey[1]=PAlt|0x45;  //alt-E
strcpy(tcontrol->text,"&Edit");
//strcpy(tcontrol->submenu,"winfmsEdit");  //will DrawFTWindow this window
AddFTControl(twin,tcontrol);
//add items

tcontrol2=GetFTControl("fmEdit"); //get a pointer to the above added FTControl
//Add items to File Menu
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&New");
titem.hotkey[0]=PAlt|0x6e; //n  
titem.hotkey[1]=PAlt|0x4e; //N
//tittem.click[0]=(FTControlfuncwcxy *)btnNewProject_Click;
AddFTItem(tcontrol2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Open");
titem.hotkey[0]=PAlt|0x6f; //o
titem.hotkey[1]=PAlt|0x4f; //O
//tFTItem.click[0]=(FTControlfuncwcxy *)FTItemOpen_Click;
AddFTItem(tcontrol2,&titem);

//Done adding items to filemenu "fmEdit"


#if 0 
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScrollBarX");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=TrackX+20;
tcontrol->y1=460;
tcontrol->x2=579;
tcontrol->y2=480;
strcpy(tcontrol->text,"");
//tcontrol->gx1=0;
//tcontrol->gy1=-20;
//tcontrol->gx2=-60;
//tcontrol->gy2=0;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->color[0]=ltblue;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
tcontrol->ButtonDownHold[0]=(FTControlfuncwcxy *)ScrollBarClick;
tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)ScrollBarClick;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)ScrollBarClick;
tcontrol->interval[0].tv_sec=0;
tcontrol->interval[0].tv_usec=50000;//1000=1ms
tcontrol->delay[0].tv_sec=0;
tcontrol->delay[0].tv_usec=500000;//1000=1ms
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScrollBarY");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=620;
tcontrol->y1=128;
tcontrol->x2=640;  //grow?
tcontrol->y2=480;
strcpy(tcontrol->text,"Scroll Bar Y");
//tcontrol->gx1=-20;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowX1|CGrowX2|CGrowY2;
tcontrol->flags=CGrowX1|CGrowX2|CGrowY2;
tcontrol->color[0]=ltblue;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
tcontrol->interval[0].tv_sec=0;
tcontrol->interval[0].tv_usec=50000;//1000=1ms
tcontrol->delay[0].tv_sec=0;
tcontrol->delay[0].tv_usec=500000;//1000=1ms
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScrollRight");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=600;
tcontrol->y1=460;
tcontrol->x2=619;  //grow?
tcontrol->y2=480;
strcpy(tcontrol->text,">");
tcontrol->ButtonDownHold[0]=(FTControlfuncwcxy *)btnScrollRight_Click;
tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)btnScrollRight_Click;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnScrollRight_Click;
//tcontrol->gx1=-40;
//tcontrol->gy1=-20;
//tcontrol->gx2=-20;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//button down last time, last time button down event was called
tcontrol->interval[0].tv_sec=0;
tcontrol->interval[0].tv_usec=50000;//1000=1ms
tcontrol->delay[0].tv_sec=0;
tcontrol->delay[0].tv_usec=500000;//1000=1ms
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScrollLeft");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=580;
tcontrol->y1=460;
tcontrol->x2=599;  //grow?
tcontrol->y2=479;
strcpy(tcontrol->text,"<");
tcontrol->ButtonDownHold[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
//tcontrol->gx1=-60;
//tcontrol->gy1=-20;
//tcontrol->gx2=-40;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
tcontrol->interval[0].tv_sec=0;
tcontrol->interval[0].tv_usec=50000;//1000=1ms
tcontrol->delay[0].tv_sec=0;
tcontrol->delay[0].tv_usec=500000;//1000=1ms
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScrollLeft2");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=TrackX;
tcontrol->y1=460;
tcontrol->x2=TrackX+19;
tcontrol->y2=480;
strcpy(tcontrol->text,"<");
tcontrol->ButtonDownHold[0]=(FTControlfuncwcxy *)btnScrollLeft2_Click;
tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)btnScrollLeft2_Click;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnScrollLeft2_Click;
//tcontrol->gy1=-20;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowY1|CGrowY2|CShowText;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
tcontrol->interval[0].tv_sec=0;
tcontrol->interval[0].tv_usec=50000;//1000=1ms
tcontrol->delay[0].tv_sec=0;
tcontrol->delay[0].tv_usec=500000;//1000=1ms
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnGet");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=0;
tcontrol->y1=Ly1[1];
tcontrol->x2=31;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Get");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnGet_Click;
////tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->textcolor[0]=FT_BLACK;  //text color 
tcontrol->textcolor[1]=FT_LIGHT_GRAY;  //text background color
tcontrol->color[0]=FT_LIGHT_GRAY;  //button color background no press
tcontrol->color[1]=FT_DARK_GRAY;  //button down color (and toggle color)
tcontrol->color[2]=FT_GRAY;  //mouse over color
//tcontrol->win=twin->num;//this number may change during run time
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnOpen");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=32;
tcontrol->y1=Ly1[1];
tcontrol->x2=63;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Open");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnOpen_Click;
////tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;//this number may change during run time
AddFTControl(twin,tcontrol);



//#if 0
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnSave");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=64;
tcontrol->y1=Ly1[1];
tcontrol->x2=95;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Save");
////tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnSave_Click;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnPlay");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=96;
tcontrol->y1=Ly1[1];
tcontrol->x2=127;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Play");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnPlay_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnClick");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=128;
tcontrol->y1=Ly1[1];
tcontrol->x2=159;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Click");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnClick_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnRecord");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=160;
tcontrol->y1=Ly1[1];
tcontrol->x2=191;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Rec");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnRecord_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);



memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnStop");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=192;
tcontrol->y1=Ly1[1];
tcontrol->x2=223;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Stop");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnStop_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnScissor");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->x1=224;
tcontrol->y1=Ly1[1];
tcontrol->x2=255;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"Scissors");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnSplice_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");
tcontrol->flags=CShowText|CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnOut");

tcontrol->x1=256;
tcontrol->y1=Ly1[1];
tcontrol->x2=287;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"&Out");
tcontrol->hotkey[0]=PAlt|0x6f;//111;  //alt-o
tcontrol->hotkey[1]=PAlt|0x4f;//79;  //alt-O

//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnOut_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnTest");

tcontrol->x1=288;//256;
tcontrol->y1=Ly1[1];
tcontrol->x2=320;//287;
tcontrol->y2=Ly2[1];
strcpy(tcontrol->text,"&Test");
//tcontrol->hotkey=PAlt|0x6f;//111;  //alt-o
//tcontrol->hotkey2=PAlt|0x4f;//79;  //alt-O
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnTest_Click;

tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->textcolor[0]
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);
#endif
//fprintf(stderr,"before free\n");
if (tcontrol!=0) {
	free(tcontrol);
}

if (PStatus.flags&PInfo)
  fprintf(stderr,"Done adding FTControls for window 'main'\n");

//#endif
return 1;
} //end main_AddFTControls





void btnRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
record_CreateFTWindow();
}   //end btnRecord_Click




void btnGet_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
openfile_CreateFTWindow();
}//end btnGet_Click


void btnOpen_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{

openproject_CreateFTWindow();
//fprintf(stderr,"b4 free %p %p\n",twin->ititle,twin);
}//end btnOpen_Click

void btnSave_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
saveproject_CreateFTWindow();
}//end btnSave_Click


void btnOut_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
outfile_CreateFTWindow();
}//end btnOut_Click


void ScrollBarClick(FTWindow *twin,FTControl *tFTControl,int x,int y)
{

}//end ScrollBarClick


//ChkUSBClick
void btnScrollRight_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
}//end ScrollRight


void btnScrollLeft_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
}//end btnScrollLeft

void btnScrollLeft2_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
}//end btnScrollLeft2_Click


void btnOpenProject_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
/*
int a,tlen,numdata,ex;
FTControl *tFTControl,*tFTControl2,*tFTControl3;
long long st,tu;
char fn[FTMedStr];
FILE *fin;

//check for need to save



tFTControl=GetFTControl("txtOpen");
tlen=strlen(tFTControl->text);

tFTControl3=GetFTControl("btnMark");

if (tlen>0)
{



if (tlen>4)
{
if (strcasecmp(tcontrol->text[tlen-4],".prj"))
  strcat(tFTControl->text,".prj");
//fprintf(stderr,"file extension: %s %d\n",tstr,tlen);
}
else
{
strcat(tFTControl->text,".prj");
}

fprintf(stderr,"\nOpen Project: %s\n",tFTControl->text);

//delete any current data on tracks
ex=0;
tFTControl2=PStatus.iwindow->iFTControl;
if (tFTControl2>0)
{
while (ex==0)
{
if (tFTControl2->next==0) ex=1;  //before delete

if (tFTControl2->track>0)
  {
  //DelFTControl(tFTControl2->name);
  tFTControl2=DelFTControl(tFTControl2);
  }

if (ex==0) tFTControl2=tFTControl2->next;
}  //end while ex==0
}  //end if tFTControl2>0



if ((fin=fopen(tFTControl->text,"rb")))
{
//btnTimeLine
//btnMarkIn
//btnMarkOut
//num data FTControls
//data FTControls (data FTControl 1, data FTControl 2, ....)

//adjust existing TImeLine FTControl
//tFTControl2=GetFTControl("btnTimeLine");
//st=tFTControl2->starttime;
//tu=tFTControl2->time;
//tu=PStatus.timescale;//tFTControl2->time;
//fprintf(stderr,"pointer b4=%p\n",tFTControl2);
if (fread(&st,sizeof(long long),1,fin)==0)
{
fprintf(stderr,"Error with project file \"%s\" reading timeline starttime\n",tFTControl->text);
}
else
{
tFTControl2->starttime=st;
}


if (fread(&tu,sizeof(long long),1,fin)==0)
{
fprintf(stderr,"Error with project file \"%s\" reading timeline time units\n",tFTControl->text);
}
else
{
//tFTControl2->time=tu;
PStatus.timescale=tu;
}


if (fread(&st,sizeof(long long),1,fin)==0)
{
fprintf(stderr,"Error with project file \"%s\" reading mark in\n",tFTControl->text);
}
else
{
//PStatus.markin=st;
tFTControl3->ia[0]=st;
}


if (fread(&st,sizeof(long long),1,fin)==0)
{
fprintf(stderr,"Error with project file \"%s\" reading mark out\n",tFTControl->text);
}
else
{
//PStatus.markout=st;
tFTControl3->ia[1]=st;
}

if (fread(&numdata,sizeof(int),1,fin)==0)
{
fprintf(stderr,"Error reading number of data files from project file  \"%s\"\n",tFTControl->text);
}
else
{


//now put data in new FTControl
tFTControl2=(FTControl *)malloc(sizeof(FTControl));
fprintf(stderr,"Number of saved data FTControls=%d\n",numdata);
//for(a=0;a<numdata;a++)
for(a=0;a<numdata;a++)
{
//add each data FTControl
fread(fn,FTMedStr,1,fin);
fread(tcontrol2->track,sizeof(int),1,fin);
fread(tcontrol2->Channel,sizeof(int),1,fin);
fread(tcontrol2->starttime,sizeof(long long),1,fin);
fread(tcontrol2->duration,sizeof(long long),1,fin);
fread(tcontrol2->ostarttime,sizeof(long long),1,fin);

//load any data
//.wav or .dat
fprintf(stderr,"Loading data file: \"%s\"\n",fn);
//fprintf(stderr,"starttime:%lli track:%i\n",st,tr);


if (!(LoadDataFile(fn,tFTControl2)))
 {
 fprintf(stderr,"Problem Loading file \"%s\".\n",tFTControl2->filename);
 }
}  //for a

fclose(fin);

free(tFTControl2);

strcpy(PStatus.Project,tFTControl->text);
strcpy(PStatus.iwindow->title,"Video Edit (");
strcat(PStatus.iwindow->title,tFTControl->text);
strcat(PStatus.iwindow->title,")");
strcpy(PStatus.iwindow->ititle,"Video Edit (");
strcat(PStatus.iwindow->ititle,tFTControl->text);
strcat(PStatus.iwindow->ititle,")");
PStatus.flags&=~(EChanged);

DrawFTControls(0,0);  //redraw all the FTControls on the main window
CloseWindowName("openproject");

XSetStandardProperties(PStatus.xdisplay,PStatus.iwindow->xwindow,PStatus.iwindow->title,PStatus.iwindow->ititle,None,NULL,0,NULL);
XFlush (PStatus.xdisplay);


}  //end if error reading numdata

}  //fopen==0
else
{
fprintf(stderr,"Could not open file %s\n",tFTControl->text);
}

}  //end if tlen>0


*/

} //btnOpenProject_Click



void btnPlay_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{  //should have **tFTControl, malloc+free each FTControl
#if Linux
//FTControl **tFTControl,*tFTControl2;
//long long numdata,a,b,tx,inc,starttime,endtime;
//long long maxdatasize,c,
long long audiobuffersize,buffersize,buffertotal;
//i16 fsample16[MaxNumChannels];
char *buffer;
int outsamplerate,outchannels;
//int ex,maxchan;
int audio_fptr,format;
FILE *fptr;
//,*fptr2;
//WaveHdr wh1;
//WaveDataHdr wh1d;
WaveHdr wh;
WaveDataHdr wdh;


fprintf(stderr,"Play data on tracks\n");
outsamplerate=48000;
outchannels=1;//1;
audiobuffersize=2048;


#if 0
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
#endif


//MakeOutFile("_temp.wav");

//fprintf(stderr,"maxdata=%lli\n",maxdatasize);
//fprintf(stderr,"c=%lli\n",c*2);

//now play temp file
//PStatus.flags|=ERecording; EPlaying

//open sound device
fprintf(stderr,"Opening /dev/dsp, this is usually an adventure, some times this takes 5 seconds or more\n");
audio_fptr= open("/dev/dsp",O_WRONLY);
//audio_fptr= open(uipvp.AudioDevice,O_RDWR);
//fprintf(stderr,"Got past opening /dev/dsp, but the adventure is not over.\n");
//audio_fptr= open("/dev/dsp",O_RDONLY);
if (audio_fptr == -1)
  {
  perror("open /dev/dsp");
  //fprintf(stderr,"Problem opening %s\n",uipvp.AudioDevice);
  //
  }
//fprintf(stderr,"opened\n");

  //set 16 bit little endian format
//format = AFMT_U8;
//format = AFMT_U16_LE;
format = AFMT_S16_LE;
if (ioctl(audio_fptr, SNDCTL_DSP_SETFMT, &format)==-1)
  { /* Fatal error */
  perror("SNDCTL_DSP_SETFMT");
  close(audio_fptr);
  //
  }

  //set channels 1=mono 2=stereo
if (ioctl(audio_fptr, SNDCTL_DSP_CHANNELS, &outchannels)==-1)
{ /* Fatal error */
  perror("SNDCTL_DSP_CHANNELS");
  close(audio_fptr);
//  return -1;
//
}
fprintf(stderr,"set channels complete\n");
if (ioctl(audio_fptr, SNDCTL_DSP_SPEED, &outsamplerate)==-1)
{ /* Fatal error */
perror("SNDCTL_DSP_SPEED");
  close(audio_fptr);
  //return -1;
}

fprintf(stderr,"got past sound card part\n");
fptr=fopen("_temp.wav","rb");
//fptr=fopen("test.wav","rb");

if (fptr!=0)
{
system("chmod 777 _temp.wav");


fread(&wh,sizeof(WaveHdr),1,fptr);
fread(&wdh,sizeof(WaveDataHdr),1,fptr);

fprintf(stderr,"\n\nStart playing\n");
////buffersize=1024;
buffersize=audiobuffersize;//2048;
buffertotal=0;
buffer=malloc(buffersize);//
PStatus.flags|=EPlaying;
//may want to try waiting for a second?
//write 0's, for some reason there is noise if no
memset(buffer,0,buffersize);
write(audio_fptr,buffer,buffersize);  //can send pointer to 1 byte, sound card decodes to 2 bytes

while(PStatus.flags&EPlaying && buffersize<wdh.chunkSize)
{

  fread(buffer,buffersize,1,fptr);
  write(audio_fptr,buffer,buffersize);  //can send pointer to 1 byte, sound card decodes to 2 bytes

  if ((buffertotal+buffersize)>wdh.chunkSize)
    {
    buffersize=(wdh.chunkSize-buffertotal);  //get last chunk
    //fread(buffer,2,buffersize,fptr);
    fread(buffer,buffersize,1,fptr);
    write(audio_fptr,buffer,buffersize);
    PStatus.flags&=~EPlaying;
    buffertotal+=buffersize;
    }
  else
    buffertotal+=buffersize;


//fprintf(stderr,"\nwrote a buffer\n");
  FT_GetInput();
//end if xevent pending

//fprintf(stderr,"\nb\n");

}  //end while
close(audio_fptr);
fclose(fptr);

//may need to wait here?
fprintf(stderr,"\nStopped playing\n");
free(buffer);  //caused seg fault because was overwriting malloc address
} //fptr!=0
else
{
fprintf(stderr,"could not open _temp.wav file.\n");
}

#endif
//end play temp.wav
}  //end btnPlay_Click




void btnStop_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{  //stop recording and playing
fprintf(stderr,"Stop Recording/Playing\n");
PStatus.flags&=~(ERecording|EPlaying);
}


void btnClick_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{

}  //click

void btn_fmFile_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"btn_fmFile_Exit_Click()\n");
PStatus.flags|=PExit;
}

void btnExit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"Exit program\n");
//PStatus.exitprogram=1;
PStatus.flags|=PExit;
}


void btnTest_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
unsigned int retval;
//FTWindow *twin;
//int x2,y2;

//twin=GetFTWindow("main");

//x2=twin->x+twin->w/2;
//y2=twin->y+twin->h/2;

//fprintf(stderr,"Test_Click xr=%d\n",xr);
//FTMessageBox("Click button to Close.",MB_OK);//|FTMB_CANCEL);
retval=FTMessageBox("Click button to Close.",FTMB_YES|FTMB_NO|FTMB_CANCEL,"Test",0);
fprintf(stderr,"mb returned %d\n",retval);
}




void NewWin_Keypress(FTWindow *twin,unsigned char key)
{
int i;

if (key==XK_Escape) {
  if (FStatus.flags&FDrawingFTControl) {
    FStatus.flags&=~FDrawingFTControl;
#if Linux
    if (FStatus.erase[0]!=0) {
      EraseFTControlBox(twin,FStatus.LastX,FStatus.LastY);
      for(i=0;i<4;i++) FStatus.erase[i]=0;
    }
#endif
  }
}


}  //NewWin_Keyboard

//pass twin?
void main_GotFocus(void)
{
FTWindow *twin;
FTControl *tcontrol2;

twin=GetFTWindow("main");
tcontrol2=GetFTControl("fmFile");

if (tcontrol2!=0) {
 //set window focus to file menu
	twin->FocusFTControl=tcontrol2;
}

} //main_GotFocus




void NewWin_ButtonDownUp(FTWindow *twin,int x,int y)
{
//check for adding a new FTControl

} //NewWin_ButtonDownUp

void NewWin_ButtonDown(FTWindow *twin,int x,int y)
{
FTWindow *twin2;
FTControl *tFTControl,*tFTControl2;
int i,onFTControl;

fprintf(stderr,"button down in new win\n");

//check for adding a new FTControl
twin2=GetFTWindow("cont");
if (twin2!=0) {
  tFTControl=twin2->icontrol;
  i=0;
  while(tFTControl!=0) {
    if (tFTControl->value) {  //FTControl is active
      //see if button is on FTControl
      tFTControl2=twin->icontrol;
      onFTControl=0;
      while (tFTControl2!=0) {
        if (x>=tFTControl2->x1 && x<=tFTControl2->x2 && y>=tFTControl2->y1 && y<=tFTControl2->y2) {
          onFTControl=1;
        }
        tFTControl2=tFTControl2->next;
      } //while
 
      if (!onFTControl) {
        fprintf(stderr,"Add new FTControl %s\n",tFTControl->name);
        FStatus.flags|=FDrawingFTControl;  
        FStatus.StartX=x;  //start of FTControl box
        FStatus.StartY=y;
        FStatus.CType=i;
      }  //!onFTControl
    } //tFTControl->value

  i++;
  tFTControl=tFTControl->next;
  }  //tFTControl!=0
}  //twin2!=0
} //NewWin_ButtonDown


void EraseFTControlBox(FTWindow *twin,int x,int y) 
{
int x1,y1,bw,bh,i;

//  fprintf(stderr,"mouse move drawing FTControl in new win\n");
//if (x<0) x=0;
//if (y<0) y=0;
//if (x>twin->w-1) x=twin->w-1;
//if (y>twin->h-1) y=twin->h-1;

//if (x!=FStatus.StartX && y!=FStatus.StartY) {
#if Linux
  //Erase earlier Background image
  if (FStatus.erase[0]!=0) {
     fprintf(stderr,"erase\n");
     bw=abs(FStatus.StartX-FStatus.LastX);
     bh=abs(FStatus.StartY-FStatus.LastY);

     if (FStatus.LastX<FStatus.StartX) x1=FStatus.LastX;
     else x1=FStatus.StartX;
     if (FStatus.LastY<FStatus.StartY) y1=FStatus.LastY;
     else y1=FStatus.StartY;

    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,FStatus.erase[0],0,0,x1,y1,bw?bw:1,1);
    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,FStatus.erase[1],0,0,x1+bw,y1,1,bh?bh:1);
    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,FStatus.erase[2],0,0,x1,y1+bh,bw?bw:1,1);
    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,FStatus.erase[3],0,0,x1,y1+1,1,bh-1?bh-1:1);
    for(i=0;i<4;i++) {
      XDestroyImage(FStatus.erase[i]);
    } //for i
  }
#endif

}  //EraseFTControlBox


void ResetFTControlButton(void) 
{
FTWindow *twin;
FTControl *tFTControl;

//reset FTControl button
twin=GetFTWindow("cont");
if (twin!=0) {
  tFTControl=twin->icontrol;
  while(tFTControl!=0) {
    if (tFTControl->value) {  //FTControl is active
      tFTControl->value=0; //reset toggle FTControl button
      DrawFTControl(tFTControl);
    }
    tFTControl=tFTControl->next;
  }  //tFTControl!=0
} //twin!=0

} //ResetFTControlButton


void FTItemNewWindow_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
FTWindow twin2;
FTWindow *twin3;
char winname[FTMedStr];
char tstr[FTMedStr];//,name[FTMedStr];
//FTControl tFTControl2;
FTControl *tcontrol;
int i;
FTItem tFTItem;


//create a new window
memset(&twin2,0,sizeof(FTWindow));
//get free window name
i=0;
sprintf(winname,"tempw%d",i);

while(GetFTWindow(winname)!=0) {
//fprintf(stderr,"here\n");
i++;
sprintf(winname,"tempw%d",i);
}


twin3=GetFTWindow("main");

strcpy(twin2.name,winname);
strcpy(twin2.title,"");
strcpy(twin2.ititle,"");
//twin.flags|=(WExclusive|WMsgBox); //the FTMessageBox traditionally has exclusive FTControl over a program
twin2.x=twin3->x+twin3->w/8;  //location of window
twin2.y=twin3->y+twin3->h/8;
twin2.w=400;
twin2.h=300;
twin2.ButtonDownUp[0]=(FTControlfuncwxy *)NewWin_ButtonDownUp;
twin2.ButtonDown[0]=(FTControlfuncwxy *)NewWin_ButtonDown;
twin2.ButtonDown[2]=(FTControlfuncwxy *)NewWin_Button2Down;
twin2.ButtonUp[0]=(FTControlfuncwxy *)NewWin_ButtonUp;
twin2.ButtonUp[2]=(FTControlfuncwxy *)NewWin_Button2Up;
twin2.MouseMove=(FTControlfuncwxy *)NewWin_MouseMove;
twin2.GotFocus=(FTControlfuncw *)NewWin_GotFocus;
twin2.LostFocus=(FTControlfuncw *)NewWin_LostFocus;
twin2.Keypress=(FTControlfuncwk *)NewWin_Keypress;
twin2.OnMove=(FTControlfuncw *)NewWin_OnMove;
twin2.OnResize=(FTControlfuncw *)NewWin_OnResize;

//twin2.ButtonDownUpOnFTControl[0]=(FTControlfunccxy *)main_ButtonDownUpOnFTControl;
//twin2.AddFTControls=(FTControlfuncw *)mb_AddFTControls;
CreateFTWindow(&twin2);
twin3=GetFTWindow(twin2.name);
DrawFTWindow(twin3);

//make dup win to store flags
memset(&twin2,0,sizeof(FTWindow));
sprintf(twin2.name,"%s%s",FStatus.Prefix,twin3->name);
CreateFTWindow(&twin2);
//do not draw duplicate window



//just make button2 click window function
//add right click FTControl
//this FTControl will be deleted with the rest of the FTControls when win is deleted
//include win instance in name?

tcontrol=(FTControl *)malloc(sizeof(FTControl));

memset(tcontrol,0,sizeof(FTControl));
sprintf(tstr,"%s%s",FStatus.PrefixRClick,winname);
fprintf(stderr,"namew=%s\n",tstr);
strcpy(tcontrol->name,tstr);

tcontrol->type=CTFileMenu;//6;  //menu FTItem - needs to be type CTItemList - 
//tcontrol->flags=CShowText|CNotVisible|CSubMenu;
tcontrol->flags=CNotVisible;
tcontrol->z=-1; 
//forecolor, backcolor, ButtonDowncolor, mouseovercolor?
tcontrol->color[0]=FT_LIGHT_GRAY;  //background no press
tcontrol->color[1]=FT_BLACK;//dkgray;  //text
tcontrol->color[2]=FT_BLUE;  //background (mouse over)
tcontrol->color[3]=FT_GRAY;  //bkground press
//background mouseover
//selectbkcolor
//tcontrol->win=twin3->num;
tcontrol->window=twin3;

AddFTControl(twin,tcontrol);
if (tcontrol!=0) {
	free(tcontrol);
}
tcontrol=GetFTControl(tstr);

memset(&tFTItem,0,sizeof(FTItem));
strcpy(tFTItem.name,"Save &As");
tFTItem.hotkey[0]=0x62; //a  
tFTItem.hotkey[1]=0x42; //A
//tFTItem.submenu=malloc(100);
//strcpy(tFTItem.submenu,"fmsSaveAs");
AddFTItem(tcontrol,&tFTItem);
//free(tFTItem.submenu);

memset(&tFTItem,0,sizeof(FTItem));
strcpy(tFTItem.name,"&Save");
tFTItem.hotkey[0]=0x73; //s
tFTItem.hotkey[1]=0x53; //S
//tFTItem.submenu=malloc(100);
//strcpy(tFTItem.submenu,"fmsSave");
AddFTItem(tcontrol,&tFTItem);
//free(tFTItem.submenu);

memset(&tFTItem,0,sizeof(FTItem));
strcpy(tFTItem.name,"&Build Project");
AddFTItem(tcontrol,&tFTItem);




} //btnNewWindow_Click


void NewWin_ButtonUp(FTWindow *twin,int x,int y)
{
int i,x1,y1,x2,y2;
FTControl *tFTControl,*tcontrol;
char cname[FTMedStr];
char tfile[FTMedStr];

if (PStatus.flags&PInfo) {
	fprintf(stderr,"NewWin_ButtonUp\n");
}

//check for adding a new FTControl
if (FStatus.flags&FDrawingFTControl) {
  FStatus.flags&=~FDrawingFTControl;
  if (x<0) x=0;
  if (y<0) y=0;
  if (x>twin->w-1) x=twin->w-1;
  if (y>twin->h-1) y=twin->h-1;

#if Linux
  if (FStatus.erase[0]!=0) {
    EraseFTControlBox(twin,x,y);
    for(i=0;i<4;i++) FStatus.erase[i]=0;
  }
#endif
 
//   bw=abs(FStatus.StartX-x);
//   bh=abs(FStatus.StartY-y);
   if (x<FStatus.StartX) {
     x1=x;
     x2=FStatus.StartX;
   }
   else {
     x1=FStatus.StartX;
     x2=x;
   }
   if (y<FStatus.StartY) {
     y1=y;
     y2=FStatus.StartY;
   }
   else {
     y1=FStatus.StartY;
     y2=y;
   }


   
   tcontrol=(FTControl *)malloc(sizeof(FTControl));
  //Add and Draw a new FTControl 
  //find FTControl type
  memset(tcontrol,0,sizeof(FTControl));

  tcontrol->type=FStatus.CType;
  //get free FTControl name
  i=0;
  sprintf(cname,"tempc%d",i);
  while(GetFTControl(cname)!=0) {
    //fprintf(stderr,"here\n");
    i++;
    sprintf(cname,"tempc%d",i);
  }
  strcpy(tcontrol->name,cname);
  tcontrol->x1=x1;
  tcontrol->y1=y1;
  tcontrol->x2=x2;  
  tcontrol->y2=y2;
/*
//done in FT
  if (tcontrol->type==CTCheckBox || tcontrol->type==CTRadio) {
    tcontrol->x3=x1+16;  
    tcontrol->y3=y2; 
  }
*/
  strcpy(tcontrol->text,"Enter text");
//  tcontrol->ButtonDownHold[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
//  tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
//  tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnScrollLeft_Click;
  tcontrol->OnMove=(FTControlfuncwcxy *)FTControl_OnMove;
  tcontrol->OnResize=(FTControlfuncwcxy *)FTControl_OnResize;
  tcontrol->Keypress=(FTControlfuncwck *)FTControl_Keypress;
  tcontrol->flags=CMove|CResize;
  if (tcontrol->type==CTTextBox || tcontrol->type==CTCheckBox || tcontrol->type==CTRadio|| tcontrol->type==CTDropdown)
    tcontrol->color[0]=FT_WHITE;
  else
    tcontrol->color[0]=FT_LIGHT_GRAY;
    
  tcontrol->color[1]=FT_DARK_GRAY;
  tcontrol->color[2]=FT_GRAY;

  if (FStatus.CType==CTLabel) {
    tcontrol->type=CTTextBox;
  }

//fprintf(stderr,"ProjectPath=%s\n",PStatus.ProjectPath);
  if (FStatus.CType==CTImage) {
    //strcpy(tcontrol->text,"images/noimage.bmp");
	sprintf(tfile,"%simages/noimage.jpg",PStatus.ProjectPath);
	//tcontrol->image=FT_LoadJPGFileToFTControl(tcontrol,"images/noimage.jpg");
	LoadJPGFileToFTControl(tfile,tcontrol,FT_SCALE_IMAGE,0);
    //tcontrol->image=FT_LoadJPGFileToFTControl(tcontrol,"images/noimage.jpg");
  }

  //fileopen needs to be moved, so cannot select - maybe make button?
//this is done in FT now I think
  if (FStatus.CType==CTFileOpen) {
//    strcpy(path,PStatus.ProjectPath);
	sprintf(tfile,"%simages/folder.bmp",tcontrol,PStatus.ProjectPath);

    FT_LoadBitmapFileAndScale(tfile,tcontrol->fontheight,tcontrol->fontheight);
//    strcpy(path,PStatus.ProjectPath);
sprintf(tfile,"%simages/file.bmp",tcontrol,PStatus.ProjectPath);
    FT_LoadBitmapFileAndScale(tfile,tcontrol->fontheight,tcontrol->fontheight);

    //FT_LoadBitmapFileAndScale("images/file.bmp",tcontrol,tcontrol->fontheight,tcontrol->fontheight);
  } //CTFileOpen


//  tcontrol->interval[0].tv_sec=0;
//  tcontrol->interval[0].tv_usec=50000;//1000=1ms
//  tcontrol->delay[0].tv_sec=0;
//  tcontrol->delay[0].tv_usec=500000;//1000=1ms
  //tcontrol->win=twin->num;
  AddFTControl(twin,tcontrol);
  
  tFTControl=GetFTControl(tcontrol->name);
  DrawFTControl(tFTControl);

//make dup FTControl (FTControls have same name, win has different name, use FTControl prefix?)
  sprintf(cname,"%s%s",FStatus.Prefix,twin->name);
  twin=GetFTWindow(cname);
  if (twin!=0) {
    tcontrol->type=FStatus.CType;
    //tcontrol->win=twin->num; //add to the temp window
    //|CErase;  //default initial FTControl bit
    AddFTControl(twin,tcontrol);
  }  //twin!=0
  else {
    fprintf(stderr,"Error:  No duplicate window\n");
  }

  ResetFTControlButton();


  free(tcontrol);

//redraw prop window

}  //FDrawingFTControl
  
} //NewWin_ButtonUp


void NewWin_Button2Down(FTWindow *twin,int x,int y)
{
FTControl *tFTControl;
char tstr[FTMedStr];
int bw,bh;

fprintf(stderr,"Right Button down\n");

//if right click window open close
//else open
sprintf(tstr,"%s%s",FStatus.PrefixRClick,twin->name);
fprintf(stderr,"name=%s\n",tstr);
tFTControl=GetFTControl(tstr);
if (tFTControl!=0) {
  fprintf(stderr,"FTControl exists\n");
  if (tFTControl->flags&CNotVisible) {
    tFTControl->flags&=~CNotVisible;
//    tFTControl->flags|=CMenuOpen; //open menu
    tFTControl->flags|=CItemListOpen; //open menu
//    twin->flags|=WItemListOpen;
    twin->flags|=WItemList;
    bw=tFTControl->x2-tFTControl->x1;
    bh=tFTControl->y2-tFTControl->y1;  
    tFTControl->x1=x;
    tFTControl->x2=tFTControl->x1+bw;
    tFTControl->y1=y;
    tFTControl->y2=tFTControl->y1+bh;
    tFTControl->y4=tFTControl->y2;
    DrawFTControl(tFTControl);
  } //CNotVisible
  else {
//    tFTControl->flags|=CNotVisible;
//    tFTControl->flags&=~CMenuOpen; //open menu
//    twin->flags&=~WItemListOpen;  
//    DrawFTControl(tFTControl); //eraseFTControl?
//    EraseFTControl(twin,tFTControl); //eraseFTControl?
    //EraseMenu(tFTControl); //eraseFTControl?
		CloseFTItemList(tFTControl->ilist);
  }  //is visible   else CNotVisible
}  //tFTControl!=0

} //NewWin_Button2Down

void NewWin_Button2Up(FTWindow *twin,int x,int y)
{
FTControl *tFTControl;
char tstr[FTMedStr];

//if button up on submenu call FTItem function
sprintf(tstr,"%s%s",FStatus.PrefixRClick,twin->name);
tFTControl=GetFTControl(tstr);
if (tFTControl!=0) {
  if (tFTControl->flags&CNotVisible) {
    if (x>=tFTControl->x1 && x<=tFTControl->x2 && y>=tFTControl->y1 && y>tFTControl->y2) {
      CheckFTWindowButtonDown(twin,x,y,0); //simulate button 0 down
    }  //button up over submenu FTControl
  }  //CNotVisible
}  //tFTControl!=0
} //NewWin_Button2Up


void FTControl_OnMove(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//FTControl was moved
fprintf(stderr,"FTControl moved\n");
UpdatePropertyFTWindow();
}  //FTControl_OnMove


void FTControl_OnResize(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//FTControl was resized
UpdatePropertyFTWindow();
}  //FTControl_OnResize


void NewWin_OnMove(FTWindow *twin)
{
//win moved
fprintf(stderr,"win moved\n");
//UpdatePropertyFTWindow();
DrawFTWindowProperties(twin);
}  //FTControl_OnMove


void NewWin_OnResize(FTWindow *twin)
{
//win was resized
//UpdatePropertyFTWindow();
DrawFTWindowProperties(twin);
}  //FTControl_OnResize


void FTControl_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key)
{
FTWindow *twin2;
FTControl *tFTControl2;
char cname[FTMedStr];

//  fprintf(stderr,"FTControl keypress\n");
if (key==XK_Delete) {  //delete FTControl
  //del duplicate FTControl first
  sprintf(cname,"%s%s",FStatus.Prefix,twin->name);
  twin2=GetFTWindow(cname);
  tFTControl2=GetFTControlW(twin2,tFTControl->name);
//  DelFTControl(&tFTControl2);
  DelFTControl(tFTControl2);

  fprintf(stderr,"Erase FTControl %s\n",tFTControl->name);
  EraseFTControl(tFTControl);
//  DelFTControl(&tFTControl);
  DelFTControl(tFTControl);
  //DrawFTControls(twin->num,0);
  //erase FTControl of draw FTControl
}
}


void FTControl_GotFocus(FTWindow *twin,FTControl *tFTControl)
{
fprintf(stderr,"FTControl_GotFocus: Update Property Window\n");
UpdatePropertyFTWindow();
}


void NewWin_MouseMove(FTWindow *twin,int x,int y)
{
#if Linux
XGCValues gcv;
#endif
int bw,bh,x1,y1,x2,y2;

//fprintf(stderr,"mouse move in new win\n");

//check for adding a new FTControl
if (FStatus.flags&FDrawingFTControl) {

if (x<0) x=0;
if (y<0) y=0;
if (x>twin->w-1) x=twin->w-1;
if (y>twin->h-1) y=twin->h-1;


//  XGetGCValues(PStatus.xdisplay,twin->xgc,GCLineStyle, &gvc)
  EraseFTControlBox(twin,x,y);


  FStatus.LastX=x;
  FStatus.LastY=y;

  //Get current Background image
   bw=abs(FStatus.StartX-x);
   bh=abs(FStatus.StartY-y);
   if (x<FStatus.StartX) {
     x1=x;
     x2=FStatus.StartX;
   }
   else {
     x1=FStatus.StartX;
     x2=x;
   }
   if (y<FStatus.StartY) {
     y1=y;
     y2=FStatus.StartY;
   }
   else {
     y1=FStatus.StartY;
     y2=y;
   }
#if Linux
   //fprintf(stderr,"getimage\n");
   FStatus.erase[0]=XGetImage(PStatus.xdisplay,twin->xwindow,x1,y1,bw+1,1,AllPlanes,ZPixmap);
   FStatus.erase[1]=XGetImage(PStatus.xdisplay,twin->xwindow,x1+bw,y1,1,bh+1,AllPlanes,ZPixmap);
   FStatus.erase[2]=XGetImage(PStatus.xdisplay,twin->xwindow,x1,y1+bh,bw+1,1,AllPlanes,ZPixmap);
   FStatus.erase[3]=XGetImage(PStatus.xdisplay,twin->xwindow,x1,y1+1,1,bh?bh:1,AllPlanes,ZPixmap);


  memset(&gcv,0,sizeof(XGCValues));
  //draw dotted-dashed line
  gcv.foreground = 0;
	gcv.function = GXcopy;
	gcv.plane_mask = AllPlanes;
  gcv.line_style = LineOnOffDash;
	gcv.dashes = 5;
	gcv.dash_offset = 5;
  XChangeGC(PStatus.xdisplay,twin->xgc,GCForeground|GCFunction|GCPlaneMask|
	   GCLineStyle|GCDashList|GCDashOffset,&gcv);
  //draw 4 lines
  XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
  XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2,y1,x2,y2-1);
  XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2,x2-1,y2);
  XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1+1,x1,y2-1);
  gcv.line_style = LineSolid;
//	gcv.dashes = 0;
//	gcv.dash_offset = 0;
  XChangeGC(PStatus.xdisplay,twin->xgc,GCForeground|GCFunction|GCPlaneMask|
  GCLineStyle|GCDashList|GCDashOffset,&gcv);

#endif

// } //x!=StartX y!=StartY

}  //FDrawingFTControl

} //NewWin_MouseMove


void NewWin_GotFocus(FTWindow *twin)
{
//update properties
fprintf(stderr,"Update Property Window\n");
UpdatePropertyFTWindow();

}  //NewWin_GotFocus

void NewWin_LostFocus(FTWindow *twin)
{
fprintf(stderr,"New Window lost focus, raisewindow\n");
//fprintf(stderr,"lostfocus\n");
#if Linux
XRaiseWindow(PStatus.xdisplay,twin->xwindow);
#endif
//update properties
//if prop window has focus, do not erase
//ClearPropertyWindow();
}  //NewWin_LostFocus


void main_OnOpen(FTWindow *twin)
{
//FTWindow *twin2;
FTControl *tcontrol2;
//struct timeval temp1,temp2;
//int ex;


if (PStatus.flags&PInfo) fprintf(stderr,"main_OnOpen\n");

strcpy(FStatus.Prefix,"_t_");
strcpy(FStatus.PrefixRClick,"_rc_"); //right-click windows
//open Properties window
/*
twin=GetFTWindow("prop");
if (twin==0) {
	prop_CreateFTWindow();
}
*/

/*
//fprintf(stderr,"open control window in ftimain\n");
//open Controls window
twin=GetFTWindow("cont");
if (twin==0) {
	cont_CreateFTWindow();
}
//twin=GetFTWindow("cont");
//OpenFTWindow(twin);
*/

/*
gettimeofday(&temp1,NULL);
ex=1;
fprintf(stderr,"before while\n");
while(ex){
//	ProcessEvents(); //blocks
//	XFlush(PStatus.xdisplay);
  gettimeofday(&temp2,NULL);
	if (temp2.tv_sec>=temp1.tv_sec+5) {
		ex=0;
		fprintf(stderr,"time%d=%d+20",(int)temp2.tv_sec,(int)temp1.tv_sec);
	}
//		fprintf(stderr,"while time%d=%d+20\n",(int)temp2.tv_sec,(int)temp1.tv_sec);
} //while
*/

//XFlush(PStatus.XDisplay
//twin=GetFTWindow("main");
//DrawFTWindow(twin);
tcontrol2=GetFTControl("fmFile");

fprintf(stderr,"\n\nSetting focus to main File Menu control\n\n");

//XFlush(PStatus.xdisplay);


//SetFocus(twin,tcontrol2);  //set focus on main, filemenu FTControl
//fprintf(stderr,"after setfocus\n");

} //main_OnOpen()

