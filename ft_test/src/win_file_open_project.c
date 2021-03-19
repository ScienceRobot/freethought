//win_file_open_project.c
#include "freethought.h"
#include "win_file_open_project.h"

extern ProgramStatus PStatus;
//FTIStatus FStatus;  //FTI global variables

//PStatus.FirstWindow=(FTControlfunc *)main_CreateFTWindow;

//Open Window
int win_file_open_project_CreateFTWindow(void)
{
FTWindow twin,*twin2;
int fh,a;

memset(&twin,0,sizeof(FTWindow));
strcpy(twin.name,"file_open_project");
strcpy(twin.title,"Open Project");
strcpy(twin.ititle,"Open Project");
twin.x=200;
twin.y=200;
twin.w=600;
twin.h=400;
twin.Keypress=(FTControlfuncwk *)win_file_open_project_Keypress;
//twin.OnOpen=(FTControlfuncw *)main_OnOpen;
//twin.GotFocus=(FTControlfuncw *)main_GotFocus;
//twin.ButtonDownUpOnFTControl[0]=(FTControlfuncwcxy *)main_ButtonDownUpOnFTControl;
twin.AddFTControls=(FTControlfuncw *)file_open_project_AddFTControls;

fprintf(stderr,"create window file_open_project_CreateFTWindow\n");
CreateFTWindow(&twin);
twin2=GetFTWindow(twin.name);
DrawFTWindow(twin2);

fprintf(stderr,"end win_file_open_project_CreateFTWindow\n");
return 1;
} //end win_file_open_project_CreateFTWindow



void file_open_project_Keypress(FTWindow *twin,unsigned char key)
{
}  //file_open_project_Keypress



//Add FTControls
int file_open_project_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol2,*tcontrol;
FTItem titem,*titem2;
int fw,fh;
char *sptr;
//int fw,fh;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'file_open_project'\n");


sptr=(char *)malloc(100);  //reuse this pointer
tcontrol=(FTControl *)malloc(sizeof(FTControl));


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"foFileOpenProject");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");
#if WIN32
		tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		if (tcontrol->font==0) {
		}
		GetTextMetrics(twin->hdcWin,&tcontrol->textmet);
		tcontrol->fontwidth=tcontrol->textmet.tmMaxCharWidth;
		tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
#if Linux
tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
if (tcontrol->font==0) {
	fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
		//use any font
}
tcontrol->fontwidth=tcontrol->font->max_bounds.width;
fw=tcontrol->font->max_bounds.width;
fh=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
	fw=tcontrol->fontheight;
	fh=tcontrol->fontwidth;
#endif
tcontrol->type=CTFileOpen;//6;  //menu FTItem
tcontrol->x1=fw;
tcontrol->y1=fh*3;
tcontrol->x2=fw*30;
tcontrol->y2=fh*30;
//tcontrol->textcolor[0]=0;
//tcontrol->textcolor[1]=0xffffff; //background text color= white
//tcontrol->TextColor=0;
//tcontrol->TextBackgroundColor=0xffffff; //background text color= white
//tcontrol->color[0]=0; //font
//tcontrol->color[1]=0xffffff; //control background color= white
//tcontrol->ControlBorderColor=0; 
//tcontrol->ControlBackgroundColor=0xffffff; 

//
//tcontrol->flags=CVScroll;
tcontrol->flags=CVScroll|CFOName|CFOSize|CFODate;
//tcontrol->hotkey[0]=PAlt|0x66;  //alt-f
//tcontrol->hotkey[1]=PAlt|0x46;  //alt-F
//tcontrol->LinkedDirTextbox=
//tcontrol->LinkedFilenameTextbox=
AddFTControl(twin,tcontrol);




//Sample buttons
//Exit button
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->text,"&Close");
strcpy(tcontrol->name,"btn_file_open_project_Exit");
tcontrol->type=CTButton;
tcontrol->x1=400;
tcontrol->y1=300;
tcontrol->x2=500;
tcontrol->y2=350;

tcontrol->hotkey[0]=PAlt|0x78;  //alt-x
tcontrol->hotkey[1]=PAlt|0x58;  //alt-X
tcontrol->ButtonClick[0]=(FTControlfuncwcxy *)btn_file_open_project_Exit_Click;
AddFTControl(twin,tcontrol);


if (tcontrol!=0) {
	free(tcontrol);
}

if (PStatus.flags&PInfo)
  fprintf(stderr,"Done adding FTControls for window 'main'\n");

//#endif
return 1;
} //end main_AddFTControls




void btn_file_open_project_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"Exit program\n");
//PStatus.exitprogram=1;
//PStatus.flags|=PExit;
	CloseFTWindow(twin);
}


void win_file_open_project_Keypress(FTWindow *twin,KeySym key)
{
//fprintf(stderr,"Exit program\n");
//PStatus.exitprogram=1;
	if (key==XK_Escape && twin->FocusFTControl==0) {  //esc key and no control has focus
		PStatus.flags|=PExit;
	}
}
