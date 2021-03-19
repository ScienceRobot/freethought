//win_prop.c
#include "freethought.h"
#include "win_prop.h"
#include "win_ftimain.h"

extern ProgramStatus PStatus;
extern FTIStatus FStatus;


int prop_CreateFTWindow(void)
{
FTWindow twin,*twin2;//,*twin3;

twin2=GetFTWindow("prop");
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"prop");
  strcpy(twin.title,"Properties"); 
  strcpy(twin.ititle,"Properties");
//  twin.x1=0;
//  twin.y1=0;
  twin.w=200;
  twin.h=400;
  //todo: account for windows decorations (width)
	twin.x=805; //(800,100)
#if WIN32
	twin.x=830; //(800,100)
#endif
	twin.y=100;

/*
  twin3=GetFTWindow("main");
  if (twin3!=0) {
    twin.x=twin3->x+twin3->w+10;  //+border of window.  how to get?
    twin.y=twin3->y;
  }
*/
//  twin.Keypress=(FTControlfuncwk *)prop_Keypress;
  twin.AddFTControls=(FTControlfuncw *)prop_AddFTControls;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);


  //fprintf(stderr,"b4 free %p %p\n",twin->ititle,twin);
  return 1;
  }
return 0;
}  //end prop_CreateFTWindow


#if 0 
void prop_Keypress(FTWindow *twin,unsigned char key)
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

int prop_AddFTControls(FTWindow *twin)
{
//FTControl tFTControl;
FTControl *tcontrol;
FTItem lFTItem;
int w,h,lw,lh;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window '%s'\n",twin->name);


w=200;
h=400;
lw=w/2;

tcontrol=malloc(sizeof(FTControl));

memset(tcontrol,0,sizeof(FTControl));
lh=tcontrol->fontheight;

strcpy(tcontrol->name,"lblwprop1l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=0;
tcontrol->x2=lw;
tcontrol->y2=lh;
tcontrol->textcolor[0]=FT_BLACK;  //text
tcontrol->textcolor[1]=FT_LIGHT_GRAY;  //bkground
strcpy(tcontrol->text,"Window");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblwprop1r");
tcontrol->type=CTLabel;
tcontrol->x1=lw;
tcontrol->y1=0;
tcontrol->x2=w;
tcontrol->y2=lh;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblwprop2l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh;
tcontrol->x2=lw;
tcontrol->y2=lh*2;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Name");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop2r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh;
tcontrol->x2=w;
tcontrol->y2=lh*2;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
//tcontrol->Keypress=(FTControlfuncwck *)txtprop2r_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop2r_LostFocus;
//tcontrol->Keypress=(FTControlfuncwk *)txtprop2r_OnChange;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblwprop3l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*2;
tcontrol->x2=lw;
tcontrol->y2=lh*3;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Title");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop3r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*2;
tcontrol->x2=w;
tcontrol->y2=lh*3;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop3r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblwprop4l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*3;
tcontrol->x2=lw;
tcontrol->y2=lh*4;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"x");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop4r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*3;
tcontrol->x2=w;
tcontrol->y2=lh*4;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop4r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblwprop5l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*4;
tcontrol->x2=lw;
tcontrol->y2=lh*5;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"y");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop5r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*4;
tcontrol->x2=w;
tcontrol->y2=lh*5;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop5r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblwprop6l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*5;
tcontrol->x2=lw;
tcontrol->y2=lh*6;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"w");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop6r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*5;
tcontrol->x2=w;
tcontrol->y2=lh*6;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop6r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblwprop7l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*6;
tcontrol->x2=lw;
tcontrol->y2=lh*7;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"h");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop7r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*6;
tcontrol->x2=w;
tcontrol->y2=lh*7;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop7r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblwprop8l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*7;
tcontrol->x2=lw;
tcontrol->y2=lh*8;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Exclusive");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop8r");
tcontrol->type=CTDropdown;
tcontrol->x1=lw;
tcontrol->y1=lh*7;
tcontrol->x2=w;
tcontrol->y2=lh*8;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop8r_LostFocus;
memset(&lFTItem,0,sizeof(FTItem));
//lFTItem.name=malloc(100);
strcpy(lFTItem.name,"Yes");
AddFTItem(tcontrol,&lFTItem);
strcpy(lFTItem.name,"No");
AddFTItem(tcontrol,&lFTItem);
//free(lFTItem.name);
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_LIGHT_GRAY;  //combo box square
tcontrol->color[2]=FT_BLUE;  //mouseover
tcontrol->color[3]=FT_GRAY;  //bkground press
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblwprop9l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*8;
tcontrol->x2=lw;
tcontrol->y2=lh*9;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Icon Title");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtwprop9r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*8;
tcontrol->x2=w;
tcontrol->y2=lh*9;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtwprop9r_LostFocus;
tcontrol->value=2;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

//===========================
//FTControl Properties
strcpy(tcontrol->name,"lblcprop1l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=0;
tcontrol->x2=lw;
tcontrol->y2=lh;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"FTControl");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblcprop1r");
tcontrol->type=CTLabel;
tcontrol->x1=lw;
tcontrol->y1=0;
tcontrol->x2=w;
tcontrol->y2=lh;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblcprop2l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh;
tcontrol->x2=lw;
tcontrol->y2=lh*2;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Name");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop2r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh;
tcontrol->x2=w;
tcontrol->y2=lh*2;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
//tcontrol->Keypress=(FTControlfuncwck *)txtprop2r_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop2r_LostFocus;
//tcontrol->Keypress=(FTControlfuncwk *)txtprop2r_OnChange;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblcprop3l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*2;
tcontrol->x2=lw;
tcontrol->y2=lh*3;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Title");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop3r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*2;
tcontrol->x2=w;
tcontrol->y2=lh*3;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop3r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblcprop4l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*3;
tcontrol->x2=lw;
tcontrol->y2=lh*4;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"x");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop4r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*3;
tcontrol->x2=w;
tcontrol->y2=lh*4;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop4r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblcprop5l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*4;
tcontrol->x2=lw;
tcontrol->y2=lh*5;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"y");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop5r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*4;
tcontrol->x2=w;
tcontrol->y2=lh*5;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop5r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblcprop6l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*5;
tcontrol->x2=lw;
tcontrol->y2=lh*6;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"w");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop6r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*5;
tcontrol->x2=w;
tcontrol->y2=lh*6;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop6r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"lblcprop7l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*6;
tcontrol->x2=lw;
tcontrol->y2=lh*7;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"h");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop7r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*6;
tcontrol->x2=w;
tcontrol->y2=lh*7;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop7r_LostFocus;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblcprop8l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*7;
tcontrol->x2=lw;
tcontrol->y2=lh*8;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Exclusive");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop8r");
tcontrol->type=CTDropdown;
tcontrol->x1=lw;
tcontrol->y1=lh*7;
tcontrol->x2=w;
tcontrol->y2=lh*8;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop8r_LostFocus;
memset(&lFTItem,0,sizeof(FTItem));
//lFTItem.name=malloc(100);
strcpy(lFTItem.name,"Yes");
AddFTItem(tcontrol,&lFTItem);
strcpy(lFTItem.name,"No");
AddFTItem(tcontrol,&lFTItem);
//free(lFTItem.name);
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_LIGHT_GRAY;  //combo box square
tcontrol->color[2]=FT_BLUE;  //mouseover
tcontrol->color[3]=FT_GRAY;  //bkground press
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


strcpy(tcontrol->name,"lblcprop9l");
tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=lh*8;
tcontrol->x2=lw;
tcontrol->y2=lh*9;
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"Icon Title");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

strcpy(tcontrol->name,"txtcprop9r");
tcontrol->type=CTTextBox;
tcontrol->x1=lw;
tcontrol->y1=lh*8;
tcontrol->x2=w;
tcontrol->y2=lh*9;
tcontrol->LostFocus=(FTControlfuncwc *)txtcprop9r_LostFocus;
tcontrol->value=2;
tcontrol->color[0]=FT_WHITE;//wingray;  //bkground
tcontrol->color[1]=FT_BLACK;  //text
strcpy(tcontrol->text,"");
tcontrol->flags=CNotVisible;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


if (tcontrol!=0) {
	free(tcontrol);
}


if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to Properties Window.\n");

//#endif
return 1;
} //end prop_AddFTControls


void UpdatePropertyFTWindow(void)
{
FTWindow *twin;
FTControl *tcontrol;

fprintf(stderr,"UpdatePropertyFTWindow\n");

//find if window of FTControl has focus
//if (PStatus.focus!=0) {
if (PStatus.FocusFTWindow!=0) {
//  fprintf(stderr,"1\n");
  //twin=GetFTWindowN(PStatus.focus);
	twin=PStatus.FocusFTWindow;
  if (strcmp(twin->name,"main") && strcmp(twin->name,"prop") && strcmp(twin->name,"FTControl")) {
    //fprintf(stderr,"window name=%s focus=%d\n",twin->name,twin->focus);
//    if (twin->focus==0) { //no FTControl has focus
    if (twin->FocusFTControl==0) { //no FTControl has focus
      DrawFTWindowProperties(twin);
    }  //twin->FocusFTControl==0
    else {  //a FTControl does have focus
      //fprintf(stderr,"window FTControl with tab %d has focus\n",twin->focus);
      //tcontrol=GetFTControlWithTab(twin,twin->focus);
			tcontrol=twin->FocusFTControl;
      if (tcontrol==0) DrawFTWindowProperties(twin); //error with tabs, no FTControls have this tab, select window
      else {
        DrawFTControlProperties(tcontrol);
      }
    }

  }  //not main, properties or FTControl window has focus
  
}  //PStatus.focus!=0
}  //UpdatePropertyFTWindow


void DrawFTControlProperties(FTControl *tFTControl)
{
FTWindow *twin;
FTControl *tFTControl2;

fprintf(stderr,"Draw FTControl '%s' Properties\n",tFTControl->name);

ClearPropertyWindow();

tFTControl2=GetFTControl("lblcprop1l");
tFTControl2->flags&=~CNotVisible;


tFTControl2=GetFTControl("lblcprop1r");
strcpy(tFTControl2->text,tFTControl->name);

tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("lblcprop2l");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop2r");
tFTControl2->flags&=~CNotVisible;
strcpy(tFTControl2->text,tFTControl->name);

tFTControl2=GetFTControl("lblcprop3l");
sprintf(tFTControl2->text,"Text");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop3r");
tFTControl2->flags&=~CNotVisible;
strcpy(tFTControl2->text,tFTControl->text);

tFTControl2=GetFTControl("lblcprop4l");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop4r");
tFTControl2->flags&=~CNotVisible;
sprintf(tFTControl2->text,"%d",tFTControl->x1);

tFTControl2=GetFTControl("lblcprop5l");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop5r");
tFTControl2->flags&=~CNotVisible;
sprintf(tFTControl2->text,"%d",tFTControl->y1);

tFTControl2=GetFTControl("lblcprop6l");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop6r");
tFTControl2->flags&=~CNotVisible;
sprintf(tFTControl2->text,"%d",tFTControl->x2-tFTControl->x1);

tFTControl2=GetFTControl("lblcprop7l");
tFTControl2->flags&=~CNotVisible;

tFTControl2=GetFTControl("txtcprop7r");
tFTControl2->flags&=~CNotVisible;
sprintf(tFTControl2->text,"%d",tFTControl->y2-tFTControl->y1);


twin=GetFTWindow("prop");
DrawFTWindow(twin);  //refresh properties window
} //DrawFTControlProperties


void DrawFTWindowProperties(FTWindow *twin)
{
FTControl *tFTControl;
FTWindow *twin2;
char tstr[FTMedStr];

fprintf(stderr,"DrawFTWindowProperties\n");
fprintf(stderr,"x=%d y=%d\n",twin->x,twin->y);

ClearPropertyWindow();

tFTControl=GetFTControl("lblwprop1l");
strcpy(tFTControl->text,"Window");
tFTControl->flags&=~CNotVisible;
//sprintf(tFTControl->text,"Window %s",twin->name);

tFTControl=GetFTControl("lblwprop1r");
strcpy(tFTControl->text,twin->name);
tFTControl->flags&=~CNotVisible;


tFTControl=GetFTControl("lblwprop2l");
tFTControl->flags&=~CNotVisible;
//strcpy(tFTControl->text,"Name");

tFTControl=GetFTControl("txtwprop2r");
tFTControl->flags&=~CNotVisible;
strcpy(tFTControl->text,twin->name);

tFTControl=GetFTControl("lblwprop3l");
sprintf(tFTControl->text,"Title");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop3r");
tFTControl->flags&=~CNotVisible;
strcpy(tFTControl->text,twin->title);

tFTControl=GetFTControl("lblwprop4l");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop4r");
tFTControl->flags&=~CNotVisible;
sprintf(tFTControl->text,"%d",twin->x);

tFTControl=GetFTControl("lblwprop5l");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop5r");
tFTControl->flags&=~CNotVisible;
sprintf(tFTControl->text,"%d",twin->y);

tFTControl=GetFTControl("lblwprop6l");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop6r");
tFTControl->flags&=~CNotVisible;
sprintf(tFTControl->text,"%d",twin->w);

tFTControl=GetFTControl("lblwprop7l");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop7r");
tFTControl->flags&=~CNotVisible;
sprintf(tFTControl->text,"%d",twin->h);

tFTControl=GetFTControl("lblwprop8l");
tFTControl->flags&=~CNotVisible;

//get from dup win
tFTControl=GetFTControl("txtwprop8r");
tFTControl->flags&=~CNotVisible;
sprintf(tstr,"%s%s",FStatus.Prefix,twin->name);
twin2=GetFTWindow(tstr);
if (twin2!=0 && twin2->flags&WExclusive) tFTControl->value=1;

tFTControl=GetFTControl("lblwprop9l");
tFTControl->flags&=~CNotVisible;

tFTControl=GetFTControl("txtwprop9r");
tFTControl->flags&=~CNotVisible;
strcpy(tFTControl->text,twin->ititle);




twin2=GetFTWindow("prop");
DrawFTWindow(twin2);  //refresh properties window



}  //DrawFTWindowProperties



void ClearPropertyWindow(void)
{
FTControl *tFTControl;
FTWindow *twin;

twin=GetFTWindow("prop");
if (twin!=0) {
  tFTControl=twin->icontrol;
  while (tFTControl!=0) {
    //tFTControl=GetFTControl("txtprop8r");
    tFTControl->flags|=CNotVisible;
  tFTControl=tFTControl->next;
  }
DrawFTWindow(twin);
}  //if twin!=0

}  //ClearWindowProperties



//=======================
//Begin Windows Properties Functions
//=============================
void txtwprop2r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2,*tFTControlr,*tFTControl3;
char tstr[FTMedStr];

//fprintf(stderr,"txtwprop2r lost focus\n");
//Window Name

tFTControl=GetFTControl("lblwprop1l");
tFTControlr=GetFTControl("lblwprop1r");
tFTControl2=GetFTControl("txtwprop2r");
if (tFTControl!=0 && tFTControlr!=0 && tFTControl2!=0) {

  twin=GetFTWindow(tFTControlr->text);
  if (twin!=0)  {
    strcpy(twin->name,tFTControl2->text);
 // fprintf(stderr,"name=%s\n",tFTControl->name);

    //copy to duplicate window
    sprintf(tstr,"%s%s",FStatus.Prefix,tFTControlr->text);  //old dup name

    //rename right click FTControl
    sprintf(tstr,"%s%s",FStatus.PrefixRClick,tFTControlr->text);
    tFTControl3=GetFTControl(tstr);
    if (tFTControl3!=0) {
      sprintf(tstr,"%s%s",FStatus.PrefixRClick,tFTControl2->text);
      strcpy(tFTControl3->name,tstr);
    }

    //copy new name to top label in prop window
    strcpy(tFTControlr->text,tFTControl2->text);

    twin=GetFTWindow(tstr);
    if (twin!=0) {  //copy new name to dup win
      sprintf(twin->name,"%s%s",FStatus.Prefix,tFTControlr->text);  //text2?
    }
    DrawFTControl(tFTControlr); //update name of window
  }  //twin!=0


}  //tFTControl!=0

}  //txtwprop2r_LostFocus


void txtwprop3r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2,*tFTControlr;
//char tstr[FTMedStr];

//fprintf(stderr,"txtwprop3r lost focus\n");
//Window Title (in menu bar)/FTControl Text
tFTControl=GetFTControl("lblwprop1l");
tFTControlr=GetFTControl("lblwprop1r");
tFTControl2=GetFTControl("txtwprop3r");
if (tFTControl!=0 && tFTControlr!=0 && tFTControl2!=0) {

  twin=GetFTWindow(tFTControlr->text);
  if (twin!=0)  {
    strcpy(twin->title,tFTControl2->text);  //and the duplicate window?
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
//    strcpy(tFTControl->text,tFTControl2->text);
//    DrawFTControl(tFTControl); //update name of window
    //have to refresh title of window
#if Linux
   //maybe should put in DrawFTWindow
   XSetStandardProperties(PStatus.xdisplay,twin->xwindow,twin->title,twin->ititle,None,NULL,0,NULL);
#endif
  }

} //tFTControl!=0 tFTControlr!=0
}  //txtwprop3r_LostFocus



void txtwprop4r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window location x
//update window property
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop4r");
  if (twin!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
   twin->x=atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
#if Linux
   XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);
#endif
  }
}  //tFTControl!=0
}  //txtwprop4r_Keypress


void txtwprop5r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window location y
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop5r");
  if (twin!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
   twin->y=atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
#if Linux
   XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);
#endif
  }
}  //tFTControl!=0
}  //txtwprop5r_Keypress


void txtwprop6r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window width
//update window property
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop6r");
  if (twin!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
   twin->w=atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
#if Linux
   XResizeWindow(PStatus.xdisplay,twin->xwindow,twin->w,twin->h);
#endif
  }
}  //tFTControl!=0
}  //txtwprop6r_Keypress

void txtwprop7r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window height
//update window property
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop7r");
  if (twin!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
   twin->h=atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
#if Linux
   XResizeWindow(PStatus.xdisplay,twin->xwindow,twin->w,twin->h);
#endif
  }
}  //tFTControl!=0
}  //txtwprop7r_Keypress


void txtwprop8r_LostFocus(void)
{
FTWindow *twin;//,twin2;
FTControl *tFTControl,*tFTControl2;
char tstr[FTMedStr];

//fprintf(stderr,"FTControl lost focus\n");
//Exclusive yes/no
//this comes from dup window (because fti window cannot be exclusive)

//update window property
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop8r");
  if (twin!=0 && tFTControl2!=0)  {

    //all bits are stored in the copy window
    //because we do not want these bits for the window in fti
//get duplicate window
     sprintf(tstr,"%s%s",FStatus.Prefix,twin->name);

     twin=GetFTWindow(tstr);
     if (twin!=0) {
       if (tFTControl->value==1) twin->flags|=WExclusive;
       else twin->flags&=~WExclusive;
     }
  }
}  //tFTControl!=0
}  //txtwprop8r_Keypress


void txtwprop9r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window ititle (icon title) (not sure where this is)
//update window property
tFTControl=GetFTControl("lblwprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtwprop9r");
  if (twin!=0 && tFTControl2!=0)  {
    strcpy(twin->ititle,tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
   //maybe should put in DrawFTWindow
#if Linux
   XSetStandardProperties(PStatus.xdisplay,twin->xwindow,twin->title,twin->ititle,None,NULL,0,NULL);
#endif
  }
}  //tFTControl!=0
}  //txtwprop9r_Keypress


//============================
//====FTControl Property functions
//==========================
void txtcprop2r_LostFocus(void)
{
//FTWindow *twin;
FTControl *tFTControl,*tFTControl2,*tFTControl3,*tFTControlr;
char tstr[FTMedStr];

//fprintf(stderr,"txtcprop2r lost focus\n");
//Window/FTControl Name

tFTControl=GetFTControl("lblcprop1l");
tFTControlr=GetFTControl("lblcprop1r");
tFTControl2=GetFTControl("txtcprop2r");
if (tFTControl!=0 && tFTControlr!=0 && tFTControl2!=0) {

  tFTControl3=GetFTControl(tFTControlr->text);
  if (tFTControl3!=0)  {
    strcpy(tFTControl3->name,tFTControl2->text);

    //copy to duplicate window
    sprintf(tstr,"%s%s",FStatus.Prefix,tFTControlr->text);  //old dup name

    //copy new name to top label in prop window
    strcpy(tFTControlr->text,tFTControl2->text);

    tFTControl=GetFTControl(tstr);  //GetFTControlWin?
//fprintf(stderr,"Here3\n");   
    if (tFTControl!=0) {  //copy new name to dup FTControl
      sprintf(tFTControl->name,"%s%s",FStatus.Prefix,tFTControl2->text);
    }
    DrawFTControl(tFTControlr); //update name of FTControl
    //and duplicate FTControl

    sprintf(tstr,"%s%s",FStatus.Prefix,tFTControlr->text);  
    tFTControl=GetFTControl(tstr);
//    fprintf(stderr,"tstr=%s\n",tstr);
    if (tFTControl!=0) strcpy(tFTControl->name,tFTControl2->text);

  }  //tFTControl3!=0

}  //tFTControl!=0

}  //txtcprop2r_LostFocus


void txtcprop3r_LostFocus(void)
{
//FTWindow *twin;
FTControl *tFTControl,*tFTControl2,*tFTControlr;
char tstr[FTMedStr];

//fprintf(stderr,"txtcprop3r lost focus\n");
//Window Title (in menu bar)/FTControl Text
tFTControl=GetFTControl("lblcprop1l");
tFTControlr=GetFTControl("lblcprop1r");
tFTControl2=GetFTControl("txtcprop3r");
if (tFTControl!=0 && tFTControlr!=0 && tFTControl2!=0) {

    tFTControl=GetFTControl(tFTControlr->text);
    if (tFTControl!=0) {
//      fprintf(stderr,"text=%s\n",tFTControl->text);
      strcpy(tFTControl->text,tFTControl2->text);
//      fprintf(stderr,"text=%s\n",tFTControl->text);
      DrawFTControl(tFTControl);  //refresh FTControl text
     //and the duplicate FTControl
    sprintf(tstr,"%s%s",FStatus.Prefix,tFTControlr->text);  
    tFTControl=GetFTControl(tstr);
    fprintf(stderr,"tstr=%s\n",tstr);
    if (tFTControl!=0) strcpy(tFTControl->text,tFTControl2->text);
    }

} //tFTControl!=0 tFTControlr!=0
}  //txtcprop3r_LostFocus



void txtcprop4r_LostFocus(void)
{
FTControl *tFTControl,*tFTControl2,*tFTControl3;
FTWindow *twin;
int nx,w;

//fprintf(stderr,"FTControl lost focus\n");
//Window location x
//update window property
tFTControl=GetFTControl("lblcprop1r");
if (tFTControl!=0) {
  tFTControl3=GetFTControl(tFTControl->text);
//  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop4r");
  if (tFTControl3!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
//   twin=tFTControl3->window;
   twin=tFTControl3->window;
   EraseFTControl(tFTControl3);
   nx=atoi(tFTControl2->text);
   w=tFTControl3->x2-tFTControl3->x1;
   tFTControl3->x1=nx;
   tFTControl3->x2=tFTControl3->x1+w;
   
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
   DrawFTControl(tFTControl3);  //move FTControl
    //now duplicate FTControl ?   - no need to only flags and functions are in duplicate wins/FTControls
//   XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);
  }
}  //tFTControl!=0
}  //txtcprop4r_Keypress


void txtcprop5r_LostFocus(void)
{
FTControl *tFTControl,*tFTControl2,*tFTControl3;
FTWindow *twin;
int ny,h;

//fprintf(stderr,"FTControl lost focus\n");
//Window location y
tFTControl=GetFTControl("lblcprop1r");
if (tFTControl!=0) {
  tFTControl3=GetFTControl(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop5r");
  if (tFTControl3!=0 && tFTControl2!=0)  {
    twin=tFTControl3->window;
    EraseFTControl(tFTControl3);
//    strcpy(twin->ititle,tFTControl2->text);
    h=tFTControl3->y2-tFTControl3->y1;
    ny=atoi(tFTControl2->text);
    tFTControl3->y1=ny;
    tFTControl3->y2=tFTControl3->y1+h;
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
   //XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);
    DrawFTControl(tFTControl3);
  }
}  //tFTControl!=0
}  //txtcprop5r_Keypress


void txtcprop6r_LostFocus(void)
{
FTControl *tFTControl,*tFTControl2,*tFTControl3;
FTWindow *twin;

//fprintf(stderr,"FTControl lost focus\n");
//Window width
//update window property
tFTControl=GetFTControl("lblcprop1r");
if (tFTControl!=0) {
  tFTControl3=GetFTControl(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop6r");
  if (tFTControl3!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
    twin=tFTControl3->window;
    EraseFTControl(tFTControl3);

    tFTControl3->x2=tFTControl3->x1+atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
    DrawFTControl(tFTControl3);
//   XResizeWindow(PStatus.xdisplay,twin->xwindow,twin->w,twin->h);
  }
}  //tFTControl!=0
}  //txtcprop6r_Keypress

void txtcprop7r_LostFocus(void)
{
FTControl *tFTControl,*tFTControl2,*tFTControl3;
FTWindow *twin;

//fprintf(stderr,"FTControl lost focus\n");
//Window height
//update window property
tFTControl=GetFTControl("lblcprop1r");
if (tFTControl!=0) {
  tFTControl3=GetFTControl(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop7r");
  if (tFTControl3!=0 && tFTControl2!=0)  {
//    strcpy(twin->ititle,tFTControl2->text);
    twin=tFTControl3->window;
    EraseFTControl(tFTControl3);
    tFTControl3->y2=tFTControl3->y1+atoi(tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
//   XResizeWindow(PStatus.xdisplay,twin->xwindow,twin->w,twin->h);
    DrawFTControl(tFTControl3);
  }
}  //tFTControl!=0
}  //txtcprop7r_Keypress


void txtcprop8r_LostFocus(void)
{
FTWindow *twin;//,twin2;
FTControl *tFTControl,*tFTControl2;
char tstr[FTMedStr];

//fprintf(stderr,"FTControl lost focus\n");
//Exclusive yes/no
//this comes from dup window (because fti window cannot be exclusive)

//update window property
tFTControl=GetFTControl("lblcprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop8r");
  if (twin!=0 && tFTControl2!=0)  {

    //all bits are stored in the copy window
    //because we do not want these bits for the window in fti
//get duplicate window
     sprintf(tstr,"%s%s",FStatus.Prefix,twin->name);

     twin=GetFTWindow(tstr);
     if (twin!=0) {
       if (tFTControl->value==1) twin->flags|=WExclusive;
       else twin->flags&=~WExclusive;
     }
  }
}  //tFTControl!=0
}  //txtcprop8r_Keypress


void txtcprop9r_LostFocus(void)
{
FTWindow *twin;
FTControl *tFTControl,*tFTControl2;

//fprintf(stderr,"FTControl lost focus\n");
//Window ititle (icon title) (not sure where this is)
//update window property
tFTControl=GetFTControl("lblcprop1r");
twin=0;
if (tFTControl!=0) {
  twin=GetFTWindow(tFTControl->text);
  tFTControl2=GetFTControl("txtcprop9r");
  if (twin!=0 && tFTControl2!=0)  {
    strcpy(twin->ititle,tFTControl2->text);
  //  fprintf(stderr,"name=%s\n",tFTControl->name);
   //maybe should put in DrawFTWindow
#if Linux
   XSetStandardProperties(PStatus.xdisplay,twin->xwindow,twin->title,twin->ititle,None,NULL,0,NULL);
#endif
  }
}  //tFTControl!=0
}  //txtcprop9r_Keypress



