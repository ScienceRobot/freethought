//win_test.c
#include "freethought.h"
#include "FT_Graphics.h"
#include "FT_Windows.h"
#include "FT_Controls.h"
#include "win_test.h"
#include "win_file_open_project.h"

extern ProgramStatus PStatus;   
//FTIStatus FStatus;  //FTI global variables
int *Ly1,*Ly2;  //levels for buttons

//PStatus.FirstWindow=(FTControlfunc *)main_CreateFTWindow;

//Open Window
int main_CreateFTWindow(void)
{
FTWindow twin,*twin2;
//int fh,a;

//twin=GetFTWindowName("main");
memset(&twin,0,sizeof(FTWindow));
strcpy(twin.name,"main");
strcpy(twin.title,"Freethought");
strcpy(twin.ititle,"Freethought");
twin.x=160;
twin.y=100;
twin.w=640;
twin.h=480;
twin.Keypress=(FTControlfuncwk *)winMain_Keypress;
//twin.OnOpen=(FTControlfuncw *)main_OnOpen;
//twin.GotFocus=(FTControlfuncw *)main_GotFocus;
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
char *sptr;
int OptionGroup;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'main'\n");



sptr=(char *)malloc(100);  //reuse this pointer
tcontrol=(FTControl *)malloc(sizeof(FTControl));


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"fmFile");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->type=CTFileMenu;//6;  //menu FTItem
tcontrol->x1=0;
tcontrol->y1=0;

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
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_New_Click;
AddFTItem(tcontrol2,&titem);


//SUBMENU NEW- Project, File
titem2=GetFTItemFromFTControl(tcontrol2,"&New");
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Project");
titem.hotkey[0]=0x70; //p  
titem.hotkey[1]=0x50; //P
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_New_Click;
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_New_Project_Click;
AddFTSubItem(titem2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"File");
//titem.hotkey[0]=0x70; //p  
//titem.hotkey[1]=0x50; //P
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_New_Click;
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_Exit_Click;
AddFTSubItem(titem2,&titem);


memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&Open");
titem.hotkey[0]=0x6f; //o  
titem.hotkey[1]=0x4f; //O
titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_Open_Project_Click;
AddFTItem(tcontrol2,&titem);




memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"&About");
titem.hotkey[0]=0x62; //a  
titem.hotkey[1]=0x42; //A
titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_About_Click;
AddFTItem(tcontrol2,&titem);

memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"E&xit");
titem.hotkey[0]=0x78;  //x
titem.hotkey[1]=0x58;  //X
titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_Exit_Click;
AddFTItem(tcontrol2,&titem);

//fprintf(stderr,"before free\n");
if (sptr!=0) {
	free(sptr);  //free FTItem name memory
}
//fprintf(stderr,"before free\n");


//done adding items to filemenu fmFile


//Sample label and textbox
//Sample buttons
//Exit button
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testLabel1");
tcontrol->type=CTLabel;
tcontrol->x1=10;
tcontrol->y1=110;
tcontrol->color[0]=FT_LIGHT_GRAY;  //background no press
tcontrol->color[1]=FT_BLACK;  //text color
tcontrol->color[2]=FT_BLUE;  //background (mouse over)
tcontrol->color[3]=FT_GRAY;  //bkground press

strcpy(tcontrol->text,"Test Label");
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testTextbox1");
tcontrol->type=CTTextBox;
tcontrol->x1=10;
tcontrol->y1=130;
tcontrol->x2=200;

strcpy(tcontrol->text,"Test textbox");
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testTextArea1");
tcontrol->type=CTTextArea;
tcontrol->x1=10;
tcontrol->y1=160;
tcontrol->x2=210;
tcontrol->y2=260;

strcpy(tcontrol->text,"Test TextArea");
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testDropdown1");
tcontrol->type=CTDropdown;
tcontrol->x1=10;
tcontrol->y1=290;

strcpy(tcontrol->text,"Test Dropdown");

//tcontrol2=GetFTControl("testDropdown1"); //get a pointer to the above added FTControl
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Dropdown option 1");
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_About_Click;
AddFTItem(tcontrol,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Dropdown option 2");
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_About_Click;
AddFTItem(tcontrol,&titem);
AddFTControl(twin,tcontrol);


OptionGroup=GetFreeOptionGroupNumber(twin);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testOption1");
tcontrol->type=CTOption;
tcontrol->x1=10;
tcontrol->y1=320;
tcontrol->x2=120;
tcontrol->y2=340;
tcontrol->OptionGroup=OptionGroup;


strcpy(tcontrol->text,"Option 1");
AddFTControl(twin,tcontrol);
/*
tcontrol2=GetFTControl("testRadio1"); //get a pointer to the above added FTControl
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Option 1");
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_About_Click;
AddFTItem(tcontrol2,&titem);
memset(&titem,0,sizeof(FTItem));
strcpy(titem.name,"Option 2");
//titem.click[0]=(FTControlfuncwcxy *)btn_fmFile_About_Click;
AddFTItem(tcontrol2,&titem);
*/


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testOption2");
tcontrol->type=CTOption;
tcontrol->x1=120;
tcontrol->y1=320;
tcontrol->OptionGroup=OptionGroup;

strcpy(tcontrol->text,"Option 2");
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testOption3");
tcontrol->type=CTOption;
tcontrol->x1=240;
tcontrol->y1=320;
tcontrol->OptionGroup=OptionGroup;

strcpy(tcontrol->text,"Option 3");
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testCheckBox1");
tcontrol->type=CTCheckBox;
tcontrol->x1=10;
tcontrol->y1=350;

strcpy(tcontrol->text,"Test CheckBox");
AddFTControl(twin,tcontrol);


//image
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"testImage1");
tcontrol->type=CTImage;
tcontrol->x1=250;
tcontrol->y1=0;
tcontrol->x2=450;
tcontrol->y2=300;

#if Linux
LoadJPGFileToFTControl("images/test.jpg",tcontrol,FT_SCALE_IMAGE,0);
#endif
#if WIN32
LoadBitmapFileToFTControl("images\\test.bmp",tcontrol,FT_SCALE_IMAGE,0);
#endif
AddFTControl(twin,tcontrol);



//Sample buttons

memset(tcontrol,0,sizeof(FTControl));
//strcpy(tcontrol->text,"&Test");
strcpy(tcontrol->name,"btnOpenTest");
tcontrol->type=CTButton;
tcontrol->x1=210;
tcontrol->y1=360;
tcontrol->x2=310;
tcontrol->y2=410;
#if Linux
LoadBitmapFileToFTControl("images/open2.bmp",tcontrol,FT_SCALE_IMAGE,0);
#endif
#if WIN32
//LoadBitmapFileToFTControl(tcontrol,"images\\open.bmp",0);
LoadBitmapFileToFTControl("images\\open2.bmp",tcontrol,FT_SCALE_IMAGE,0);
#endif
//
//tcontrol->hotkey[0]=PAlt|0x74;  //alt-t
//tcontrol->hotkey[1]=PAlt|0x54;  //alt-T
tcontrol->ButtonClick[0]=(FTControlfuncwcxy *)btnOpenTest_Click;
AddFTControl(twin,tcontrol);


//Exit button
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->text, "Exit");
//strcpy(tcontrol->text,"E&xit");
strcpy(tcontrol->name,"btnExit");
tcontrol->type=CTButton;
tcontrol->x1=400;
tcontrol->y1=360;
tcontrol->x2=500;
tcontrol->y2=410;

tcontrol->hotkey[0]=PAlt|0x78;  //alt-x
tcontrol->hotkey[1]=PAlt|0x58;  //alt-X
tcontrol->ButtonClick[0]=(FTControlfuncwcxy *)btnExit_Click;
AddFTControl(twin,tcontrol);


if (tcontrol!=0) {
	free(tcontrol);
}

if (PStatus.flags&PInfo)
  fprintf(stderr,"Done adding FTControls for window 'main'\n");

//#endif
return 1;
} //end main_AddFTControls



void btnOpenTest_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"Exit program\n");
//win_file_open_project_CreateFTWindow();
}



void btnExit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"Exit program\n");
//PStatus.exitprogram=1;
PStatus.flags|=PExit;
}

void btn_fmFile_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"btn_fmFile_Exit_Click()\n");
PStatus.flags|=PExit;
}

void btn_fmFile_About_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"btn_fmFile_About_Click()\n");
}


void btn_fmFile_New_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"btn_fmFile_New_Click()\n");
}

void btn_fmFile_Open_Project_Click(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
//fprintf(stderr,"btn_fmFile_New_Click()\n");
//	win_file_open_project_CreateFTWindow();
}


void winMain_Keypress(FTWindow *twin,KeySym key)
{
//fprintf(stderr,"Exit program\n");
//PStatus.exitprogram=1;
	if (key==XK_Escape && twin->FocusFTControl==0) {  //esc key and no control has focus
		PStatus.flags|=PExit;
	}
}
