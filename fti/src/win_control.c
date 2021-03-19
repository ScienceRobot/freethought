//win_control.c
#include "freethought.h"
#include "win_control.h"

extern ProgramStatus PStatus;


int cont_CreateFTWindow(void)
{
FTWindow twin,*twin2,*twin3;

twin2=GetFTWindow("cont");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"cont");
  strcpy(twin.title,"FTControls"); 
  strcpy(twin.ititle,"FTControls");
//  twin.x1=0;
//  twin.y1=0;
  twin.w=32*4; //there is a minimum size because of top x,min, file menu, etc...
  twin.h=300;
  twin3=GetFTWindow("main");
/*
  if (twin3!=0) {
		fprintf(stderr,"Main x,y = %d,%d\n",twin3->x,twin3->y);
    twin.x=twin3->x-twin.w-10;  //-border of window.  how to get?
    twin.y=twin3->y;
  }
*/
	twin.x=25;//155-twin.w; //(25,100)
	twin.y=100;
//    twin.x=0;  //-border of window.  how to get?
//  twin.Keypress=(FTControlfuncwk *)prop_Keypress;
  twin.AddFTControls=(FTControlfuncw *)cont_AddFTControls;
//  twin.OnOpen=(FTControlfuncw *)FTControl_OnOpen;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);


	


  //fprintf(stderr,"b4 free %p %p\n",twin->ititle,twin);
  return 1;
  }
return 0;
}  //end cont_CreateFTWindow


#if 0 
void prop_Keypress(FTWindow *twin,u8 key)
{


    switch (key)
      {
      case -30:
      case -31:  //ignore shift keys
        break;
      case 0x1b:	/*esc*/
	      CloseWindow(twin);
//        DelWindow("outfile");
        break;
      default:
        break;
      }
}
#endif

int cont_AddFTControls(FTWindow *twin)
{
//FTControl tFTControl;
FTControl *tcontrol;
//FTItem lFTItem;
int w,h,bw,bh;
char tfile[FTMedStr];

fprintf(stderr,"In cont_AddFTControls\n");

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window '%s'\n",twin->name);


bw=32;
bh=32;
w=bw*3;
h=300;

//---------------


tcontrol=(FTControl *)malloc(sizeof(FTControl));
//tFTControl=*tc;
//memset(tcontrol,0,sizeof(FTControl));
memset(tcontrol,0,sizeof(FTControl));
//#if 0 
strcpy(tcontrol->name,"btnButton");
tcontrol->x1=0;
tcontrol->y1=0;
tcontrol->x2=bw-1;  //grow?
tcontrol->y2=bh-1;
strcpy(tcontrol->text,"Button");
LoadImageFileToFTControl("images/ctl_button.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_button.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_button.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnLabel");

tcontrol->x1=bw;
tcontrol->y1=0;
tcontrol->x2=bw*2-1;  
tcontrol->y2=bh-1;
strcpy(tcontrol->text,"Label");
LoadImageFileToFTControl("images/ctl_lable.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_lable.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_lable.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnText");

tcontrol->x1=bw*2;
tcontrol->y1=0;
tcontrol->x2=bw*3-1;  
tcontrol->y2=bh-1;
strcpy(tcontrol->text,"Text");
LoadImageFileToFTControl("images/ctl_text.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_text.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_text.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnCheckbox");

tcontrol->x1=bw*3;
tcontrol->y1=0;
tcontrol->x2=bw*4-1;  
tcontrol->y2=bh-1;
strcpy(tcontrol->text,"CheckBox");
LoadImageFileToFTControl("images/ctl_checkbox.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_checkbox.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_checkbox.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnOption");

tcontrol->x1=0;
tcontrol->y1=bh;
tcontrol->x2=bw-1;  
tcontrol->y2=bh*2-1;
strcpy(tcontrol->text,"Option");
LoadImageFileToFTControl("images/ctl_option.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_option.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_option.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnDropDown");

tcontrol->x1=bw;
tcontrol->y1=bh;
tcontrol->x2=bw*2-1;  
tcontrol->y2=bh*2-1;
strcpy(tcontrol->text,"DropDown");
LoadImageFileToFTControl("images/ctl_dropdown.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_dropdown.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_dropdown.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

//---------

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnFileMenu");

tcontrol->x1=bw*2;
tcontrol->y1=bh;
tcontrol->x2=bw*3-1;  
tcontrol->y2=bh*2-1;
strcpy(tcontrol->text,"FileMenu");
LoadImageFileToFTControl("images/ctl_filemenu.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_filemenu.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_filemenu.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnFileOpen");

tcontrol->x1=bw*3;
tcontrol->y1=bh-1;
tcontrol->x2=bw*4-1;  
tcontrol->y2=bh*2-1;
strcpy(tcontrol->text,"FileOpen");
LoadImageFileToFTControl("images/ctl_fileopen.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_fileopen.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_fileopen.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);

//
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnImage");

tcontrol->x1=0;
tcontrol->y1=bh*2-1;
tcontrol->x2=bw-1;  
tcontrol->y2=bh*3-1;
strcpy(tcontrol->text,"Image");
LoadImageFileToFTControl("images/ctl_image.bmp",tcontrol,FT_NORMAL_IMAGE|FT_SCALE_IMAGE,0);
LoadImageFileToFTControl("images/ctl_image.bmp",tcontrol,FT_SELECTED_IMAGE|FT_SCALE_IMAGE|FT_ADD_BORDER_TO_IMAGE,0x3f3f3f);
LoadImageFileToFTControl("images/ctl_image.bmp",tcontrol,FT_BUTTONDOWN_IMAGE|FT_SCALE_IMAGE|FT_SHADE_IMAGE,0x8080ff);
tcontrol->flags=CToggle;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;
tcontrol->color[2]=FT_GRAY;
//tcontrol->win=twin->num;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnButton_ButtonDownUp;
AddFTControl(twin,tcontrol);


if (tcontrol!=0) {
	free(tcontrol);
}

if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to FTControls window.\n");

//#endif
return 1;
} //end cont_AddFTControls



void btnButton_ButtonDownUp(FTWindow *twin,FTControl *tFTControl,int x,int y)
{
FTControl *tFTControl2;



//    fprintf(stderr,"here twin->name=%s\n",twin->name);


//untoggle any other FTControl buttons
tFTControl2=twin->icontrol;
while(tFTControl2!=0) {
  if (tFTControl2!=tFTControl && tFTControl2->value) {
  //  fprintf(stderr,"here\n");
    tFTControl2->value=0;
    DrawFTControl(tFTControl2);
  }

tFTControl2=tFTControl2->next;
} //tFTControl2!=0


}

