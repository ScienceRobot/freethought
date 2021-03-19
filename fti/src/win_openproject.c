//win_openproject.c
#include "freethought.h"
#include "win_openproject.h"
#include "win_openfile.h"
#include "win_ftimain.h"

extern ProgramStatus PStatus;
//extern XFontStruct *font_info;
extern int *Ly1,*Ly2;  //levels for buttons


int openproject_CreateFTWindow(void)
{
FTWindow twin,*twin2;

//can add more than 1 window
//perhaps should add number to name
//or and int called "inum" or "instance" 
twin2=GetFTWindow("openproject");
//inst=GetFTWindowInstance("openproject"); 
if (twin2==0) {
  memset(&twin,0,sizeof(FTWindow));
  strcpy(twin.name,"openproject");
  strcpy(twin.title,"Open Project");  //unbelievable, numchar<10
  strcpy(twin.ititle,"Open Project");
  twin.flags=WOnlyOne|WExclusive;  //WOnlyOne=technically not used at this time
//WExclusive=only FTControls on this window can be clicked on (and mouse over change) when open
  twin.w=400;
  twin.h=300;
  twin.AddFTControls=(FTControlfuncw *)openproject_AddFTControls;
  twin.Keypress=(FTControlfuncwk *)win4key;
  twin.OnOpen=(FTControlfuncw *)openproject_OnOpen;
//  twin.OnDraw=(FTControlfuncwc *)openproject_OnDraw;  //is window function
  twin.GotFocus=(FTControlfuncw *)openproject_GotFocus;
//  twin.OnResize=(FTControlfuncw *)openproject_OnResize;
  CreateFTWindow(&twin);
  twin2=GetFTWindow(twin.name);
  DrawFTWindow(twin2);

  return 1;
  }

return 0;
}  //end openproject_CreateFTWindow


//start windows functions
//void win4key(FTWindow *twin,u8 key)
void win4key(FTWindow *twin,KeySym key)
{


if (PStatus.flags&PInfo)
  fprintf(stderr,"win4key\n");

    switch (key)
      {
//      case -30:
//      case -31:  //ignore shift keys
      case XK_Shift_L:
      case XK_Shift_R:
        break;
//      case 0x1b:	/*esc*/
      case XK_Escape:	/*esc*/
//also may need instance of that window, since many can be open
//        CloseFTWindowI("openproject",twin->inst);
        CloseFTWindow(twin);
        CloseFTWindowByName("winImage");  //possibly should be subwindow
      default:
        break;
      }
if (PStatus.flags&PInfo)
  fprintf(stderr,"end win4key\n");
}


int openproject_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol;
//char path[512];
//FTItem tFTItem;
int fw,fh;

if (PStatus.flags&PInfo)
  fprintf(stderr,"Adding FTControls for window 'openproject'\n");

tcontrol=malloc(sizeof(FTControl));
  
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"foOpenFile2");  //could have duplicate name, check window too
tcontrol->type=CTFileOpen;//0;//2;
tcontrol->x1=10;
tcontrol->y1=50+(tcontrol->fontheight);
//are replaced by grow
tcontrol->x2=10+tcontrol->fontwidth*50;
tcontrol->y2=50+(tcontrol->fontheight)*10+4;
strcpy(tcontrol->text,"Sample File1");
//tcontrol->ButtonDown[0]=(FTControlfuncwcxy *)OpenFile;
//tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)foOpenFile2_DownUp;
tcontrol->ButtonDoubleClick[0]=(FTControlfuncwcxy *)foOpenFile2_DoubleClick;
//tcontrol->ButtonUp=(FTControlfuncwcxy *)OpenFile;
tcontrol->Keypress=(FTControlfuncwck *)foOpenFile2_Keypress;
//this click has some internal processing (getting filename and putting filename in filename field)
//and can have human (user/external) processing too
//tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnGetFile_Click;
//tcontrol->OnDraw=(FTControlfuncwc *)foOpenFile2_OnDraw;
tcontrol->OnChange=(FTControlfuncwc *)foOpenFile2_OnChange;
//ButtonDown up will select a file
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CShowText|CVScroll;
//very interesting - current directory changes, use absolute paths
//should not be done by user
//perhaps actual images should be in library? - this way people could easily customize - but I worry about broken links
//File open needs to be done differently - add and delete associated buttons using prefix+control name
/*
strcpy(path,PStatus.ProjectPath);
strcat(path,"images/folder.bmp");
tcontrol->image=FT_LoadBitmapAndScale(path,tcontrol->fontheight,tcontrol->fontheight);
strcpy(path,PStatus.ProjectPath);
strcat(path,"images/file.bmp");
tcontrol->image2=FT_LoadBitmapAndScale(path,tcontrol->fontheight,tcontrol->fontheight);
*/
//make FTControl have empty list
//tcontrol->ilist=malloc(sizeof(FTItemlist));
//memset(tcontrol->ilist,0,sizeof(FTItemlist));  //FTItemlist will be filled with file and folder names
/*
memset(&tFTItem,0,sizeof(FTItem));  //add button names to ilist2
//I think associated buttons may be done with a flags bit now - 
//should be done internally - user should not have to add
//would need AddFTItem2 or AddFTItem(control,itemlist,num)
//tFTItem.name=malloc(100);
strcpy(tFTItem.name,"btnfoName");
//AddFTItem(tcontrol->ilist2,&tFTItem);
AddFTItem(tcontrol->ilist2,&tFTItem);
strcpy(tFTItem.name,"btnfoSize");
AddFTItem(tcontrol->ilist2,&tFTItem);
strcpy(tFTItem.name,"btnfoDate");
AddFTItem(tcontrol->ilist2,&tFTItem);
*/
//free(tFTItem.name);
//tcontrol->i[1]=-1; //no file selected
//tcontrol->flags=CShowText|CVScroll|CGrowX2|CGrowY2;//|CResize;
tcontrol->flags=CVScroll|CFOName|CFOSize|CFODate|CScaleX2|CScaleY2;//|CResize;
tcontrol->sx2=1.0;
tcontrol->sy2=1.0;
//tcontrol->gx2=-40;
//tcontrol->gy2=-70;
//tcontrol->color[0]=FT_WHITE;
//tcontrol->color[1]=FT_BLACK;
//tcontrol->textcolor
tcontrol->textcolor[0]=FT_BLACK;  //text color
tcontrol->textcolor[1]=FT_WHITE;  //text background color
tcontrol->textcolor[2]=FT_WHITE;  //selected text color
tcontrol->textcolor[3]=FT_BLACK;  //selected text background color
tcontrol->color[0]=FT_WHITE;  //bkground
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

/*
//3 Name,Size,Date buttons
memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnfoName");  
tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");
tcontrol->type=CTButton;
tcontrol->x1=10;  //these will be changed in human func on resize
tcontrol->y1=50;
tcontrol->x2=10+tcontrol->fontwidth*20;
tcontrol->y2=50+(tcontrol->fontheight); 
tcontrol->x3=tcontrol->x2-tcontrol->x1;  //hold on to original size
strcpy(tcontrol->text,"Name");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoName_DownUp;
tcontrol->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoName_DownUp;
//tcontrol->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
//tcontrol->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
tcontrol->OnResize=(FTControlfuncwcxy *)btnfoName_OnResize;
tcontrol->flags=CShowText|CResizeX2|CGrowX2;
tcontrol->sx2=0.4;
//
//tcontrol->gx2=-40;
//tcontrol->gy2=-70;
tcontrol->color[0]=FT_GRAY;
tcontrol->color[1]=FT_LIGHT_GRAY;
tcontrol->color[2]=FT_DARK_GRAY;
//tcontrol->textcolor
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnfoSize");  
tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");
tcontrol->type=CTButton;
tcontrol->x1=10+tcontrol->fontwidth*20;
tcontrol->y1=50;
tcontrol->x2=10+tcontrol->fontwidth*28;
tcontrol->y2=50+(tcontrol->fontheight); 
tcontrol->x3=tcontrol->x2-tcontrol->x1;  //hold on to original size
strcpy(tcontrol->text,"Size");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoSize_DownUp;
tcontrol->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoSize_DownUp;
tcontrol->OnResize=(FTControlfuncwcxy *)btnfoSize_OnResize;
//tcontrol->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
//tcontrol->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
//tcontrol->flags=CShowText|CResizeX1|CResizeX2;
tcontrol->flags=CShowText|CResizeX1|CResizeX2|CGrowX1|CGrowX2;
tcontrol->sx1=.3;
tcontrol->sx2=.3;
//tcontrol->gx2=-40;
//tcontrol->gy2=-70;
tcontrol->color[0]=FT_GRAY;
tcontrol->color[1]=FT_LIGHT_GRAY;
tcontrol->color[2]=FT_DARK_GRAY;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnfoDate");  
tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");
tcontrol->type=CTButton;
tcontrol->x1=10+tcontrol->fontwidth*28;
tcontrol->y1=50;
tcontrol->x2=10+tcontrol->fontwidth*50;
tcontrol->y2=50+(tcontrol->fontheight); 
tcontrol->x3=tcontrol->x2-tcontrol->x1;  //hold on to original size
strcpy(tcontrol->text,"Date");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoDate_DownUp;
tcontrol->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoDate_DownUp;
//tcontrol->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
//tcontrol->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
tcontrol->OnResize=(FTControlfuncwcxy *)btnfoDate_OnResize;
tcontrol->flags=CShowText|CResizeX1|CGrowX1|CGrowX2;
tcontrol->sx1=.3;
tcontrol->sx2=.3;
//tcontrol->gx2=-40;
//tcontrol->gy2=-70;
tcontrol->color[0]=FT_GRAY;
tcontrol->color[1]=FT_LIGHT_GRAY;
tcontrol->color[2]=FT_DARK_GRAY;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);


//end 3 Name, Size, Date buttons

*/


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"txtOpen");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

tcontrol->type=CTTextBox;//2; //textbox
tcontrol->x1=10;
tcontrol->y1=200;//Ly1[1];
tcontrol->x2=10+tcontrol->fontwidth*50;
tcontrol->y2=215;//Ly2[1];
//tcontrol->win=1;
//tcontrol->tab=2;
tcontrol->Keypress=(FTControlfuncwck *)txtOpen_Keypress;
//memset(tcontrol->text,0,FTMedStr-1);
//strcpy(tcontrol->text,"");
tcontrol->textcolor[0]=FT_BLACK;  //text color
tcontrol->textcolor[1]=FT_WHITE;  //text background color
tcontrol->textcolor[2]=FT_WHITE;  //selected text color
tcontrol->textcolor[3]=FT_BLUE;  //selected text background color
tcontrol->color[0]=FT_WHITE;  //bkground

//tcontrol->color[0]=FT_WHITE;  //bkground
//tcontrol->color[1]=FT_BLACK;  //text
//tcontrol->color[2]=FT_BLUE;  //highlighted text background
//memset(tcontrol->filename,0,FTMedStr-1);
////strcpy(tcontrol->filename,"");
//tcontrol->flags=CShowText|CGrowX2|CGrowY1|CGrowY2;
tcontrol->flags=CGrowY1|CGrowX2|CGrowY2;
//tcontrol->gx2=-100;
//tcontrol->gy1=-40;
//tcontrol->gy2=-25;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);



memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"lblDir");
fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;
tcontrol->type=CTLabel; //text label
strcpy(tcontrol->text,"Location:");
tcontrol->x1=10;
tcontrol->y1=20;//5;//Ly1[1];
tcontrol->x2=10+fw*strlen(tcontrol->text);
tcontrol->y2=20+fh;//21;//Ly2[1];
//tcontrol->win=1;
//tcontrol->tab=0;
//tcontrol->Keypress=(FTControlfuncwck *)txtMessage_Keypress;
tcontrol->textcolor[0]=FT_BLACK;  //text color
tcontrol->textcolor[1]=FT_LIGHT_GRAY;  //text background color
tcontrol->color[0]=FT_LIGHT_GRAY;  //bkground
//tcontrol->color[1]=FT_LIGHT_GRAY;  //background text

tcontrol->window=twin;
AddFTControl(twin,tcontrol);



memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"txtDir");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;
tcontrol->type=CTTextBox;
tcontrol->x1=10+fw*10;
tcontrol->y1=20;//Ly1[1];
tcontrol->x2=10+fw*40;
tcontrol->y2=20+fh;//Ly2[1];
//tcontrol->win=1;
//tcontrol->tab=2;
tcontrol->Keypress=(FTControlfuncwck *)txtDir_Keypress;
//memset(tcontrol->text,0,FTMedStr-1);
//strcpy(tcontrol->text,"");
tcontrol->textcolor[0]=FT_BLACK;  //text color
tcontrol->textcolor[1]=FT_WHITE;  //text background color
tcontrol->textcolor[2]=FT_WHITE;  //selected text color
tcontrol->textcolor[3]=FT_BLUE;  //selected text background color
tcontrol->color[0]=FT_WHITE;  //bkground
//tcontrol->color[0]=FT_WHITE;//defgray;//white;  //bkground
//tcontrol->color[1]=FT_BLACK;  //text
//tcontrol->color[2]=FT_BLUE;  //highlighted text background
//memset(tcontrol->filename,0,FTMedStr-1);
////strcpy(tcontrol->filename,"");
//tcontrol->flags=CGrowX2;
tcontrol->flags=CGrowX2;
//tcontrol->gx2=-40;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);

memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"chkPreview");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;
tcontrol->type=CTCheckBox;
//x2 is box+ length of text
tcontrol->x1=250;
tcontrol->y1=280;
tcontrol->x2=250+10+fw*7;
tcontrol->y2=280+fh+2;
tcontrol->x3=260;
tcontrol->y3=280+fh+2;
//tcontrol->Keypress=(FTControlfuncwk *)txtOutFile_Keypress;
tcontrol->textcolor[0]=FT_BLACK;  //text color
tcontrol->textcolor[1]=FT_WHITE;  //text background color
tcontrol->textcolor[2]=FT_WHITE;  //selected text color
tcontrol->textcolor[3]=FT_BLACK;  //selected text background color
tcontrol->color[0]=FT_WHITE;  //bkground
tcontrol->color[0]=FT_WHITE;  //bkground
tcontrol->color[1]=FT_BLACK;  //color of check mark "x"
tcontrol->color[2]=FT_DARK_GRAY;  //
tcontrol->value=1;
strcpy(tcontrol->text,"Preview");
//tcontrol->flags=CShowText|CGrowX1|CGrowX3|CGrowY1|CGrowY3;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
//tcontrol->gx1=-30-strlen(tcontrol->text)*tcontrol->fontwidth;
//tcontrol->gx3=-20-strlen(tcontrol->text)*tcontrol->fontwidth;
//tcontrol->gy1=-20;
//tcontrol->gy3=-10;
//tcontrol->win=twin->num;
AddFTControl(twin,tcontrol);



memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnOpenProject");
//tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*courier*12*");

fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;
tcontrol->type=CTButton;//0;//2;
//tcontrol->win=1;
tcontrol->x1=240;
tcontrol->y1=220;
tcontrol->x2=240+fw*10;
tcontrol->y2=260+fh;
strcpy(tcontrol->text,"Open");
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnOpenProject_Click;
//tcontrol->tab=-1;
//strcpy(tcontrol->filename,"");
//tcontrol->flags=CShowText|CGrowX1|CGrowX2|CGrowY1|CGrowY2;
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
//tcontrol->gx1=-60-tcontrol->fontwidth*4;
//tcontrol->gx2=-20;
//tcontrol->gy1=-50-tcontrol->font->max_bounds.ascent-tcontrol->font->max_bounds.descent;
//tcontrol->gy2=-30;
tcontrol->color[0]=FT_LIGHT_GRAY;
tcontrol->color[1]=FT_DARK_GRAY;  
tcontrol->color[2]=FT_GRAY;  //mouseover
//tcontrol->textcolor
//tcontrol->win=twin->num;
//AddFTControl(twin,tcontrol);
AddFTControl(twin,tcontrol);


memset(tcontrol,0,sizeof(FTControl));
strcpy(tcontrol->name,"btnOPExit");
tcontrol->type=CTButton;//0;//2;

fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;
tcontrol->x1=240+fw*10+1;//305;
tcontrol->y1=220;//50;
tcontrol->x2=240+fw*20;//380;//140;//280*fw*4+1;  //grow?
tcontrol->y2=260+fh;//50+bh-1;
strcpy(tcontrol->text,"Exit");
//tcontrol->image=FT_LoadBitmap("images/ctl_button.bmp",tcontrol,bw,bh);
//tcontrol->image2=FT_LoadBitmapFileAndShade("images/ctl_button.bmp",tcontrol,bw,bh,0x3f3f3f);
//tcontrol->image3=FT_LoadBitmapFileAndShade("images/ctl_button.bmp",tcontrol,bw,bh,0x8080ff);
tcontrol->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2;
tcontrol->color[0]=FT_LIGHT_GRAY;  //button color background no press
tcontrol->color[1]=FT_DARK_GRAY;  //button down color (and toggle color)
tcontrol->color[2]=FT_GRAY;  //mouse over color
tcontrol->window=twin;
tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)btnOPExit_ButtonDownUp;
//tcontrol->tab=9;
AddFTControl(twin,tcontrol);

if (tcontrol!=0) {
	free(tcontrol);
}


if (PStatus.flags&PInfo)
  fprintf(stderr,"done with adding FTControls to open project.\n");

//#endif
return 1;
} //end openproject_AddFTControls


/*
//currently this function is unused because of X error in resize
void openproject_OnResize(void)
{
FTControl *tcontrol,*tcontrol2,*tcontrol3,*tcontrol4;
FTItem *tFTItem;
float r1,r2;
int bw;

tcontrol=GetFTControl("foOpenFile2");
DrawFTControl(tcontrol);  //to get new size
//resize 3 buttons
if (tcontrol->ilist2!=0) {
 tFTItem=tcontrol->ilist2->iFTItem;
 if (tFTItem!=0) {
   tcontrol2=GetFTControl(tFTItem->name);
   if (tcontrol2!=0) {
     tFTItem=(FTItem *)tFTItem->next;
     if (tFTItem!=0) {
       tcontrol3=GetFTControl(tFTItem->name);
       if (tcontrol3!=0) {
         tFTItem=(FTItem *)tFTItem->next;
         if (tFTItem!=0) {
           tcontrol4=GetFTControl(tFTItem->name);
           if (tcontrol4!=0) {
//             r1=(float)tcontrol3->x1/(float)tcontrol4->x2;
//             r2=(float)tcontrol3->x2/(float)tcontrol4->x2;
//             r1=(float)tcontrol3->x1/(float)tcontrol->x2;
//             r2=(float)tcontrol3->x2/(float)tcontrol->x2;

//I am not sure why, but I have to add .5 (or some amount) to offset loss
//the ration contrinues to get smaller otherwise
//I finally gave up trying to fix this problem
             r1=(float)(tcontrol2->x3+0.5)/(float)(tcontrol4->x2-tcontrol2->x1);
             r2=(float)(tcontrol3->x3+0.5)/(float)(tcontrol4->x2-tcontrol2->x1);
             //fprintf(stderr,"r1=%f r2=%f\n",r1,r2);
//             fprintf(stderr,"x1,x2=%d,%d\n",tcontrol->x1,tcontrol->x2);
             bw=tcontrol->x2-tcontrol->x1;
             tcontrol2->x2=tcontrol2->x1+(int)(r1*(float)bw);
             tcontrol3->x1=tcontrol2->x2;
             tcontrol3->x2=tcontrol3->x1+(int)(r2*(float)bw);
             tcontrol4->x1=tcontrol3->x2;
             tcontrol4->x2=tcontrol->x2;           
//adjust x3
             tcontrol2->x3=tcontrol2->x2-tcontrol2->x1;
             tcontrol3->x3=tcontrol3->x2-tcontrol3->x1;
             tcontrol4->x3=tcontrol4->x2-tcontrol4->x1;

           }
         }
       }
     }
   } //tcontrol2!=0
 }  //tFTItem!=0

}  //ilist2!=0

//if (tcontrol->ilist!=0) tcontrol->ilist->bits|=IL_REREAD;
//DrawFTControl(tcontrol);
}  //end openproject_OnResize
*/


//end windows functions

//start window FTControls functions


//start openproject FTControl functions

/*
*/

//decide to use func_DownUp or func_Click
//downup is more accurate
//click is more intuitive, standard (with MS VB), and simple
/*
void foOpenFile2_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2;

//put filename in the textbox
//tcontrol=GetFTControl("foOpenFile2");
tcontrol2=GetFTControl("txtOpen");


strcpy(tcontrol2->text,tcontrol->filename);
DrawFTControl(tcontrol2);  //redraw FTControl
tcontrol2=GetFTControl("chkPreview");
if (tcontrol2->value!=0) FTI_DrawImage();


//fprintf(stderr,"put filename in textbox\n");
}//end foOpenFile2_DownUp
*/


void foOpenFile2_DoubleClick(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2;//,*tcontrol2,*tcontrol3;
//FTItem *lFTItem;

//put filename in the textbox
  tcontrol2=GetFTControl("txtDir");

  getcwd(tcontrol2->text,FTMedStr);  //put cwd in label above fileopen
  DrawFTControl(tcontrol2);  //redraw FTControl, will draw scroll FTControls

//  tcontrol2=GetFTControl("sb_foOpenFile2");
//  tcontrol3=GetFTControl("sm_foOpenFile2");
//  tcontrol2->y1=tcontrol3->y1;
//  update_sb_foOpenFile2();  //update scroll button (in DrawFTControl i[0] is set to 0)
//    UpdateScrollSlide(tcontrol); //update scroll button (in DrawFTControl i[0] is set to 0)

//fprintf(stderr,"put filename in textbox\n");
}//end foOpenFile2_DoubleClick

/*
void s1_foOpenFile2_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
//FTControl *tcontrol;//,*tcontrol2;
//FTWindow *twin;

tcontrol=GetFTControl("foOpenFile2");

if (tcontrol->i[0]>0) {
  tcontrol->i[0]--;
  update_sb_foOpenFile2();  //update scroll button
  DrawFTControl(tcontrol);  //redraw FTControl
  }

//twin=GetFTWindowN(tcontrol->win);
if (twin!=0) twin->focus=tcontrol->tab;
}


void s2_foOpenFile2_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
//FTControl *tcontrol;//,*tcontrol2;
//FTWindow *twin;
//Cursor watch;

tcontrol=GetFTControl("foOpenFile2");

//fprintf(stderr,"i[0]=%d numFTItem=%d-numvis=%d \n",tcontrol->i[0],tcontrol->numFTItems,tcontrol->numvisible);

if (tcontrol->i[0]<tcontrol->ilist->numFTItems-tcontrol->numvisible) {
  tcontrol->i[0]++;
//  fprintf(stderr,"i[0]=%d\n",tcontrol->i[0]);
  update_sb_foOpenFile2();  //update scroll button
  DrawFTControl(tcontrol);  //redraw FTControl
  }

//twin=GetFTWindowN(tcontrol->win);
if (twin!=0) twin->focus=tcontrol->tab;

//watch = XCreateFontCursor (PStatus.xdisplay, XC_watch);
//XDefineCursor(PStatus.xdisplay,twin->xwindow,watch);


//fprintf(stderr,"scroll fileopen\n");
} //end s2_foOpenFile2_DownUp



void sm_foOpenFile2_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2,*tcontrol3;
//FTWindow *twin;

fprintf(stderr,"in sm_openfile2\n");


tcontrol3=GetFTControl("foOpenFile2");
//twin=GetFTWindowN(tcontrol3->win);
tcontrol2=GetFTControl("sb_foOpenFile2");

//if (y>tcontrol2->y2) foOpenFile2_Keypress(twin,tcontrol,PGDN);  //PageDown
//if (y<tcontrol2->y1) foOpenFile2_Keypress(twin,tcontrol,PGUP);  //PageUp
SetFocus(twin,tcontrol3);
if (y>tcontrol2->y2) CheckFTControlKeypress(twin,PGDN);  //PageDown
if (y<tcontrol2->y1) CheckFTControlKeypress(twin,PGUP);  //PageUp

//put back focus to fileopen FTControl
//SetFocus(twin,tcontrol3);
//if (twin!=0) twin->focus=tcontrol->tab;

//fprintf(stderr,"scroll fileopen\n");
} //end sm_foOpenFile2_DownUp
*/


void foOpenFile2_OnChange(FTWindow *twin,FTControl *tcontrol)
{
FTControl *tcontrol2;


  tcontrol2=GetFTControl("txtDir");

	if (tcontrol2!=0) {
		getcwd(tcontrol2->text,FTMedStr);  //put cwd in label above fileopen
		DrawFTControl(tcontrol2);  //redraw FTControl, will draw scroll FTControls
	}


tcontrol2=GetFTControl("txtOpen");
if (tcontrol2!=0) {
	//update_sb_foOpenFile2();  //update scroll button
	//UpdateScrollSlide(tcontrol);
	strcpy(tcontrol2->text,tcontrol->filename);  
	DrawFTControl(tcontrol2);  //redraw FTControl
}

tcontrol2=GetFTControl("chkPreview");
if (tcontrol2!=0) {
	if (tcontrol2->value!=0) {
		FTI_DrawImage();  //if preview==1
	}
}


} //foOpenFile2_Keypress


//void foOpenFile2_Keypress(FTWindow *twin,FTControl *tcontrol,u8 key)
void foOpenFile2_Keypress(FTWindow *twin,FTControl *tcontrol,KeySym key)
{
//FTControl *tcontrol2;//,*tcontrol3;
//FTItem *lFTItem;

//tcontrol=GetFTControl("foOpenFile2");
//tcontrol2=GetFTControl("txtOpen");

//fprintf(stderr,"Keypress func %d\n",key);

switch (key) {
/*
case DARROW:
//start autogenerated code
  if (tcontrol->ilist!=0) {
   if (tcontrol->ilist->curFTItem<tcontrol->ilist->numFTItems-1) {  
//  fprintf(stderr,"Keypress func up top=%d cur=%d numFTItem=%d\n",tcontrol->i[0],tcontrol->i[1],tcontrol->ilist->numFTItems);
    if (tcontrol->ilist->curFTItem==(tcontrol->i[0]+tcontrol->numvisible-1))
      tcontrol->i[0]++;
    tcontrol->ilist->curFTItem++;

    lFTItem=GetFTItem(tcontrol->ilist,0,tcontrol->ilist->curFTItem);
    if (!(lFTItem->bits&I_FOLDER))
      strcpy(tcontrol->filename,lFTItem->name);
    else
      strcpy(tcontrol->filename,"");  

    update_sb_foOpenFile2();  //foOpenFile_OnChange() update scroll button
    DrawFTControl(tcontrol);  //redraw FTControl
  }  //tcontrol->ilist->curFTItem<tcontrol->ilist->numFTItems-1
//end autogenerated code



    strcpy(tcontrol2->text,tcontrol->filename);  //need foOpenFile_OnChange() func to update txtOpen
    DrawFTControl(tcontrol2);  //redraw FTControl
    tcontrol2=GetFTControl("chkPreview");
    if (tcontrol2->value!=0) FTI_DrawImage();  //if preview==1
  } //ilist!=0
break;
case UARROW:
//start autogenerated code
//  fprintf(stderr,"Keypress func up top=%d cur=%d\n",tcontrol->i[0],tcontrol->i[1]);
  if (tcontrol->ilist!=0) {
    if (tcontrol->ilist->curFTItem>0) {
      if (tcontrol->ilist->curFTItem==tcontrol->i[0])
        tcontrol->i[0]--;
      tcontrol->ilist->curFTItem--;

      lFTItem=GetFTItem(tcontrol->ilist,0,tcontrol->ilist->curFTItem);
      if (!(lFTItem->bits&I_FOLDER))
        strcpy(tcontrol->filename,lFTItem->name);
      else
        strcpy(tcontrol->filename,"");  

      update_sb_foOpenFile2();  //update scroll button
//    fprintf(stderr,"Keypress func up i0=%d i1=%d\n",tcontrol->i[0],tcontrol->i[1]);
      DrawFTControl(tcontrol);  //redraw FTControl
    //strcpy(tcontrol2->text,tcontrol->filename);  //need onChange() func to update txtOpen
      }
//end autogenerated code

      strcpy(tcontrol2->text,tcontrol->filename);
      DrawFTControl(tcontrol2);  //redraw FTControl
      tcontrol2=GetFTControl("chkPreview");
      if (tcontrol2->value!=0) FTI_DrawImage();  //if preview==1

  } //tcontrol->ilist->curFTItem
break;
*/
case XK_Return:
//start autogenerated code
//for now, double clicking or pressing enter to go into a folder is done in human code

/*
//  if (tcontrol->ilist!=0) {
//  fprintf(stderr,"enter key\n");
   foOpenFile2_DoubleClick(0,0);
  lFTItem=GetFTItem(tcontrol->ilist,0,tcontrol->ilist->curFTItem);
//if this is a folder, go in to the folder
  if (lFTItem!=0 && lFTItem->bits&I_FOLDER) {
    //SetFileMenuName(tcontrol);
    tcontrol->ilist->curFTItem=0;
    tcontrol->i[0]=0;
    chdir(lFTItem->name);
    tcontrol->ilist->bits|=IL_REREAD;
    strcpy(tcontrol->filename,"");  
    DrawFTControl(tcontrol);  //redraw FTControl
    update_sb_foOpenFile2();  //update scroll button (in DrawFTControl i[0] is set to 0)
  }
//end autogenerated code
*/

//move into freethought.c

//  fprintf(stderr,"enter key\n");
  foOpenFile2_DoubleClick(twin,tcontrol,0,0);

//  }  //ilist!=0
break;
/*
case PGDN:
//start autogenerated code
// if (tcontrol->i[1]<(tcontrol->ilist->numFTItems-1-tcontrol->numvisible)) {
  if (tcontrol->ilist!=0) {
    if (tcontrol->i[0]<tcontrol->ilist->numFTItems-1) {
//  fprintf(stderr,"Keypress func up top=%d cur=%d numFTItem=%d\n",tcontrol->i[0],tcontrol->i[1],tcontrol->ilist->numFTItems);
//    fprintf(stderr,"tcontrol->i[0]=%d numFTItems=%d\n",tcontrol->i[0],tcontrol->ilist->numFTItems);
      if ((tcontrol->i[0]+2*tcontrol->numvisible)<tcontrol->ilist->numFTItems-1) {
        tcontrol->i[0]+=tcontrol->numvisible;
//        if (tcontrol->i[0]>tcontrol->numvisible) tcontrol->i[0]=tcontrol->numvisible-1;
        tcontrol->ilist->curFTItem+=tcontrol->numvisible;
      }
      else {
        //go to last FTItem
        tcontrol->ilist->curFTItem=tcontrol->ilist->numFTItems-1;
        tcontrol->i[0]=tcontrol->ilist->curFTItem-tcontrol->numvisible+1;
        //number of FTItems is less than number of FTItems allowed to be visible
        if (tcontrol->i[0]<0) tcontrol->i[0]=0;
//        tcontrol->i[0]=tcontrol->ilist->numFTItems-tcontrol->ilist->curFTItem+1;
//        fprintf(stderr,"curFTItem=%d numFTItems=%d\n",tcontrol->ilist->curFTItem,tcontrol->ilist->numFTItems);

      }
   
      lFTItem=GetFTItem(tcontrol->ilist,0,tcontrol->ilist->curFTItem);
      if (!(lFTItem->bits&I_FOLDER))
        strcpy(tcontrol->filename,lFTItem->name);
      else
        strcpy(tcontrol->filename,"");  

      update_sb_foOpenFile2();  //update scroll button

      DrawFTControl(tcontrol);  //redraw FTControl
    }  
//end autogenerated code

      strcpy(tcontrol2->text,tcontrol->filename);  //need onChange() func to update txtOpen
      DrawFTControl(tcontrol2);  //redraw FTControl
      tcontrol2=GetFTControl("chkPreview");
      if (tcontrol2->value!=0) FTI_DrawImage();
  }  //ilist!=0
break;
case PGUP:
//start autogenerated code
//  fprintf(stderr,"Keypress func up top=%d cur=%d\n",tcontrol->i[0],tcontrol->i[1]);
  if (tcontrol->ilist!=0) {
    if (tcontrol->ilist->curFTItem>0) {
      if (tcontrol->i[0]-tcontrol->numvisible>0) {
        tcontrol->i[0]-=tcontrol->numvisible;
        tcontrol->ilist->curFTItem-=tcontrol->numvisible;
        }
      else  {
        tcontrol->i[0]=0;
        tcontrol->ilist->curFTItem=0;
      }
//    fprintf(stderr,"Keypress func up i0=%d i1=%d\n",tcontrol->i[0],tcontrol->i[1]);

      lFTItem=GetFTItem(tcontrol->ilist,0,tcontrol->ilist->curFTItem);
      if (!(lFTItem->bits&I_FOLDER))
        strcpy(tcontrol->filename,lFTItem->name);
      else
        strcpy(tcontrol->filename,"");  

      update_sb_foOpenFile2();  //update scroll button

      DrawFTControl(tcontrol);  //redraw FTControl
      }  //if tcontrol->i[1]>0

//end autogenerated code

      strcpy(tcontrol2->text,tcontrol->filename);  //need onChange() func to update txtOpen
      DrawFTControl(tcontrol2);  //redraw FTControl
      tcontrol2=GetFTControl("chkPreview");
      if (tcontrol2->value!=0) FTI_DrawImage();
  }  //ilist!=0

break;
*/
} //end switch

}  //end foOopenFile2_Keypress

/*
void foOpenFile2_OnDraw(FTWindow *twin,FTControl *tcontrol)
{
FTControl *tcontrol2,*tcontrol3;
float ratio,pos;
char tstr[FTMedStr];

sprintf(tstr,"%s%s",PStatus.PrefixScrollSlide,tcontrol->name);
tcontrol2=GetFTControl(tstr);
//  tcontrol2=GetFTControl("sb_foOpenFile2");
sprintf(tstr,"%s%s",PStatus.PrefixScrollBack,tcontrol->name);
tcontrol3=GetFTControl(tstr);
//  tcontrol3=GetFTControl("sm_foOpenFile2");

//#if 0 
//resize middle scroll bar button
//ratio=numvisible/numFTItems
  if (tcontrol->ilist!=0) {
    //determine size and position of scroll bar slider button
    ratio=(float)tcontrol->numvisible/(float)tcontrol->ilist->numFTItems;
    if (ratio>1.0) {
    tcontrol2->y1=tcontrol3->y1;
    tcontrol2->y2=tcontrol3->y2;
    }
    else {
      pos=(float)tcontrol->i[0]/(float)tcontrol->ilist->numFTItems;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
//      tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;
      tcontrol2->y2=tcontrol2->y1+((float)(tcontrol3->y2-tcontrol3->y1))*ratio+1;


//#if 0 
      //make sure middle button is minimum of 15 pixels in height
      if (tcontrol2->y2<(tcontrol2->y1+15)) {  //middle button is less than 15 pixels {
//        fprintf(stderr,"here0\n");
        if ((tcontrol3->y2-tcontrol2->y1)>15) {
          tcontrol2->y2=tcontrol2->y1+15;
        }
        else {
          tcontrol2->y2=tcontrol3->y2;

          if ((tcontrol3->y2-tcontrol3->y1)<15) {
            tcontrol2->y1=tcontrol3->y1;  //width is less than 15, make button 15
            //tcontrol2->y2=tcontrol3->y2;
          }
          else {
            tcontrol2->y1=tcontrol2->y2-15;  //there is space above
          }

        }
       }  //if y2<y1+15
//#endif

//    fprintf(stderr,"y1=%d y2=%d\n",tcontrol2->y1,tcontrol2->y2);
    }  //end else ratio>1.0
    DrawFTControl(tcontrol3);
    DrawFTControl(tcontrol2);
  } //end if tcontrol->ilist!=0


//btnfoName_OnResize(0,0);
//btnfoSize_OnResize(0,0);
//btnfoDate_OnResize(0,0);


//#endif
}  //end foOpenFile2_OnDraw
*/




void openproject_GotFocus(void)
{
FTWindow *twin;
FTControl *tcontrol;

  tcontrol=GetFTControl("foOpenFile2");
//  twin=GetFTWindowN(tcontrol->win);
  twin=tcontrol->window;
//  twin->focus=tcontrol->tab;  //set window focus to FTControl
  twin->FocusFTControl=tcontrol;//focus=tcontrol->tab;  //set window focus to FTControl

}

void openproject_OnOpen(FTWindow *twin)
{
FTControl *tcontrol;
FTWindow *twin2;

  tcontrol=GetFTControl("txtDir");
  getcwd(tcontrol->text,FTMedStr);  //put cwd in label above fileopen

  tcontrol=GetFTControl("foOpenFile2");
  //twin=GetFTWindowN(tcontrol->win);
  twin2=tcontrol->window;
//  twin2->focus=tcontrol->tab;  //set window focus to FTControl
  twin2->FocusFTControl=tcontrol;  //set window focus to FTControl
//  DrawFTControls(0,tcontrol->name,0);  //redraw FTControl
}  //end openproject_OnOpen


//void txtDir_Keypress(FTWindow *twin,FTControl *tcontrol,u8 key)
void txtDir_Keypress(FTWindow *twin,FTControl *tcontrol,KeySym key)
{
FTControl *tcontrol2;
FTWindow *twin2;

//tcontrol=GetFTControl("txtDir");
tcontrol2=GetFTControl("foOpenFile2");

//fprintf(stderr,"Keypress func %d\n",key);
if (key==XK_Return && strlen(tcontrol->text)>0) {
//  fprintf(stderr,"chdir %s\n",tcontrol->text);
  chdir(tcontrol->text);  

  if (tcontrol2->ilist!=0) tcontrol2->ilist->flags|=FTIL_REREAD;
  fprintf(stderr,"txtDir drawFTControl\n");
  DrawFTControl(tcontrol2);  //redraw FTControl
  //twin2=GetFTWindowN(tcontrol2->win);
  twin2=tcontrol2->window;
  if (twin2!=0) {
		//twin2->focus=tcontrol2->tab;
		twin2->FocusFTControl=tcontrol2;
	}
  //need SetFocus(tcontrol2->name);
}

}  //end txtDir_Keypress


void FTI_DrawImage(void)
{
FTWindow *twin,*twin2;
FTControl *tcontrol;
int imgtype,result;
//u8 *bmp;
//BITMAPINFOHEADER *bmpinfo;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"FTI_DrawImage\n");
}

tcontrol=GetFTControl("foOpenFile2");

imgtype=0;
//    fprintf(stderr,"get bmp %s=.bmp?\n",tcontrol->filename[strlen(tcontrol->filename)-5]);
#if Linux
    if (tcontrol->filename!=0 && (strcasecmp(&tcontrol->filename[strlen(tcontrol->filename)-4],".bmp")==0)) {
#endif
#if WIN32
    if (tcontrol->filename!=0 && (stricmp(&tcontrol->filename[strlen(tcontrol->filename)-4],".bmp")==0)) {
#endif
 //      fprintf(stderr,"get bmp and open window\n");
       imgtype=1;
    }
#if Linux
    if (tcontrol->filename!=0 && (strcasecmp(&tcontrol->filename[strlen(tcontrol->filename)-4],".jpg")==0)) {
#endif
#if WIN32
    if (tcontrol->filename!=0 && (stricmp(&tcontrol->filename[strlen(tcontrol->filename)-4],".jpg")==0)) {
#endif
 //      fprintf(stderr,"get jpg and open window\n");
       imgtype=2;
    }

if (imgtype==0) return;

      //open a window to show the bmp image
        //should a window have an image?
      //first get image
//      timage=FT_LoadBitmap(tcontrol->filename,0,0);
      //get width height
      twin=GetFTWindow("winImage");
      if (twin==0) { 
   //make a new window
        twin=(FTWindow *)malloc(sizeof(FTWindow));
//        fprintf(stderr,"malloc twin %d %p\n",sizeof(FTWindow),twin);
        //fprintf(stderr,"&twin=%p twin=%p\n",&twin,twin);
        memset(twin,0,sizeof(FTWindow));
	      strcpy(twin->name,"winImage");
	      strcpy(twin->title,"Image");
	      strcpy(twin->ititle,"Image");
	      //twin->x1=0;
	      //twin->y1=0;
				//set at 100x100 initially
	      CreateFTWindow(twin);
	      free(twin);
	      twin=0;
				twin=GetFTWindow("winImage");
//have to get the bitmap dimensions first
//GetBmpWidth()

        if (imgtype==1) {
//scales to window size
/*
          bmp=FT_LoadBitmap(tcontrol->filename);
					if (bmp==0) {
						fprintf(stderr,"Could not load Bitmap %s\n",tcontrol->filename);
						return;
					}
					//need to get dimensions of window
					bmpinfo=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
	        twin->w=bmpinfo->biWidth;
	        twin->h=bmpinfo->biHeight;
				//LoadBitmapFileToFTWindow also does DrawFTWindow - which is probably not good
*/
           result=LoadBitmapFileToFTWindow(tcontrol->filename,twin);
           if (!result) { //FT_LoadBitmaptoWindow(twin,tcontrol->filename)!=1) {
             fprintf(stderr,"Error loading bitmap %s\n",tcontrol->filename);
             return;
           }
					DrawFTWindow(twin);
        }
        if (imgtype==2) {
					LoadJPGFileToFTWindow(tcontrol->filename,twin);
					//twin->image=LoadJPGFile(tcontrol->filename);
//        fprintf(stderr,"&twin=%p twin=%p\n",&twin,twin);
           if (twin->image==0) { //FT_LoadBitmaptoWindow(twin,tcontrol->filename)!=1) {
             fprintf(stderr,"Error loading jpg %s\n",tcontrol->filename);
             return;
           }
        twin->w=twin->bmpinfo->biWidth;
        twin->h=twin->bmpinfo->biHeight;
	      DrawFTWindow(twin);
				} //imgtype==2

  //      twin=GetFTWindow("winImage");
      }  //twin==0
    else
      {  //there is an image window open
			//we never add or draw this temporary window
      twin2=malloc(sizeof(FTWindow));  //malloc a temporary window
        //fprintf(stderr,"malloc twin %d %p\n",sizeof(FTWindow),twin);
        //fprintf(stderr,"&twin=%p twin=%p\n",&twin,twin);
      memset(twin2,0,sizeof(FTWindow));
      if (imgtype==1) {
         twin2->image=LoadBitmapFileToFTWindow(tcontrol->filename,twin2); 
         if (twin2->image==0) {
//         if (FT_LoadBitmaptoWindow(twin2,tcontrol->filename)!=1) {
           fprintf(stderr,"Error loading bitmap %s\n",tcontrol->filename);
           return;
         }
	        twin2->w=twin->bmpinfo->biWidth;
	        twin2->h=twin->bmpinfo->biHeight;
//					DrawFTWindow(twin2);

      }
      if (imgtype==2) {
				twin2->image=LoadJPGFileToFTWindow(tcontrol->filename,twin2);
				//DrawFTWindow(twin2);
			}


//fprintf(stderr,"here 0\n");
//      if (twin->w==twin2->bmpinfo->biWidth &&  twin->h==twin2->bmpinfo->biHeight)
      if (twin->bmpinfo->biWidth==twin2->bmpinfo->biWidth && twin->bmpinfo->biHeight==twin2->bmpinfo->biHeight) {
//fprintf(stderr,"here 1\n");
        //simply copy image to current window
        if (twin->image!=0) { //free any image already drawn
          free(twin->image);
					twin->image=0;
        }
#if Linux
        if (twin->ximage!=0) { //free any ximage already drawn
          XDestroyImage(twin->ximage);
					twin->ximage=0;
        }
#endif

//fprintf(stderr,"here 2\n");
        twin->image=twin2->image;
//fprintf(stderr,"here 3\n");
        free(twin2);  //free temp window (not image)
        DrawFTWindow(twin);
      }  //if bitmap dimensions are the same as last image
      else {  //else bitmap has different dimensions
				//This and last image have different dimensions
        //Del the last window
//fprintf(stderr,"here 4\n");
//        DestroyFTWindowN(twin->num);
        DestroyFTWindow(twin);
        strcpy(twin2->name,"winImage");
        strcpy(twin2->title,"Image");
        strcpy(twin2->ititle,"Image");
        //twin2->x1=0;
        //twin2->y1=0;
        twin2->w=twin2->bmpinfo->biWidth;
        twin2->h=twin2->bmpinfo->biHeight;
        CreateFTWindow(twin2);
        free(twin2);
        twin2=0;

				twin=GetFTWindow("winImage");
				DrawFTWindow(twin);
      }  //last bitmap was different from this bitmap
 
      }  //end if window existed twin==0
     twin=GetFTWindow("openproject");//  XMapRaised (PStatus.xdisplay, twin->xwindow);
#if Linux
     XSetInputFocus(PStatus.xdisplay,twin->xwindow,0,0);//RevertToPointerRoot, 17000);
#endif

if (PStatus.flags&PInfo)
  fprintf(stderr,"End FTI_DrawImage\n");
}  //end FTI_DrawImage

void btnOPExit_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
	CloseFTWindow(twin);
} //btnOPExit_ButtonDownUp

/*
void sb_foOpenFile2_MouseMove(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2,*tcontrol3;
int tnum,bw,bh;
//FTWindow *twin;
float ratio;

//fprintf(stderr,"foOpenFile2_MouseMove\n");
//fprintf(stderr,"%s\n",tcontrol->name);

//#if 0 

//tcontrol=GetFTControl("foOpenFile2");
tcontrol2=GetFTControl("foOpenFile2");  //==tcontrol?
tcontrol3=GetFTControl("sm_foOpenFile2");

//fprintf(stderr,"Ly=%d y=%d\n",PStatus.LastMouseY,y);

if (tcontrol->flags&CButtonDown(0)) {// && y<tcontrol3->y2 && y>tcontrol3->y1) { //only move if within y of middle button

//  tnum=(int)((float)(y-PStatus.LastMouseY)*(float)tcontrol3->time/(float)PStatus.timespace);
  tnum=(y-PStatus.LastMouseY);
  bw=(tcontrol->x2-tcontrol->x1);
  bh=(tcontrol->y2-tcontrol->y1);
  

//possible timesaver (needed because scroll bar is slow)=
//no erase, just draw two FTControls

//  EraseFTControl(twin,tcontrol);

  if (tnum>0) {  //going down
    if (tcontrol->y2+tnum>tcontrol3->y2) {
//      tcontrol->y2+=tcontrol3->y2-tcontrol->y2;
//      tcontrol->y1+=tcontrol3->y2-tcontrol->y2;
      tcontrol->y2=tcontrol3->y2;
      tcontrol->y1=tcontrol3->y2-(tcontrol->y2-tcontrol->y1);
    }
    else {
      tcontrol->y1+=tnum;
      tcontrol->y2+=tnum;
    }
  }
  else {  //going up
    if (tcontrol->y1+tnum<tcontrol3->y1) {
//      tcontrol->y1+=tcontrol->y1-tcontrol3->y1;
//      tcontrol->y2+=tcontrol->y1-tcontrol3->y1;
      tcontrol->y1=tcontrol3->y1;
      tcontrol->y2=tcontrol3->y1+(tcontrol->y2-tcontrol->y1);
    }
    else {
      tcontrol->y1+=tnum;
      tcontrol->y2+=tnum;
    }
  }

//  GetFTControlBackGround(twin,tcontrol);
  DrawFTControl(tcontrol3);
  DrawFTControl(tcontrol);

  
//fprintf(stderr,"tnum=%d\n",tnum);
//fprintf(stderr,"tcontrol2->y1=%d\n",tcontrol2->y1);

//adjust i[0] and i[1] in fileopen FTControl
if (tcontrol2->ilist!=0) {
  //get ratio of scroll button to middle bar
  ratio=1.0-((float)(tcontrol->y2-tcontrol->y1)/(float)(tcontrol3->y2-tcontrol3->y1));
//  fprintf(stderr,"ratio=%f\n",ratio);
  if (ratio<1.0 && ratio!=0) {
//    ratio+=(1.0-(float)tcontrol2->y1/(float)tcontrol3->y1);
    ratio=(1.0-((float)(tcontrol3->y2-tcontrol->y1)/(float)(tcontrol3->y2-tcontrol3->y1)))/ratio;
//    ratio=1.0-((float)(tcontrol3->y2-tcontrol2->y1)/(float)(tcontrol3->y2-tcontrol3->y1));
    tcontrol2->i[0]=ratio*(tcontrol2->ilist->numFTItems-tcontrol2->numvisible);
//    tcontrol->i[0]=ratio*tcontrol->ilist->numFTItems;
    //tcontrol->i[1]=((float)tcontrol2->y1/(float)tcontrol3->y2)*tcontrol->ilist->numFTItems;
 //   fprintf(stderr,"ratio=%f tcontrol->i[0]=%d\n",ratio,tcontrol->i[0]);
    DrawFTControl(tcontrol2);
  }
}




//  if (tnum<0) s1_foOpenFile2_DownUp(0,0);
//  else s2_foOpenFile2_DownUp(0,0);

}  //end if within middle button

//twin=GetFTWindow("openproject");
//have to put back focus on sliding button, scroll button puts back on fileopen FTControl
twin->focus=tcontrol->tab;

//#endif

} //end sb_foOpenFile2_MouseMove


//update scroll bar button
void update_sb_foOpenFile2(void)
{
float pos,ratio;
FTControl *tcontrol,*tcontrol2,*tcontrol3;

tcontrol=GetFTControl("foOpenFile2");
tcontrol2=GetFTControl("sb_foOpenFile2");
tcontrol3=GetFTControl("sm_foOpenFile2");


if (tcontrol->ilist!=0) {
  pos=(float)tcontrol->i[0]/(float)tcontrol->ilist->numFTItems;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
  tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;

//now calculate y2 (size) of scroll bar button
    ratio=(float)tcontrol->numvisible/(float)tcontrol->ilist->numFTItems;
    if (ratio>1.0) {
    tcontrol2->y1=tcontrol3->y1;
    tcontrol2->y2=tcontrol3->y2;
    }
    else {
//      pos=(float)tcontrol->i[0]/(float)tcontrol->ilist->numFTItems;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
//      tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;
      tcontrol2->y2=tcontrol2->y1+((float)(tcontrol3->y2-tcontrol3->y1))*ratio+1;
//    fprintf(stderr,"y1=%d y2=%d\n",tcontrol2->y1,tcontrol2->y2);
    }  //if ratio else

     DrawFTControl(tcontrol3);
     DrawFTControl(tcontrol2);

  }  //if ilist!=0
}

//end window FTControls functions
*/
