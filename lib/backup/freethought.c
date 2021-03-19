//freethought.c
//Freethought library functions
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
*/
// freethought.cpp : Defines the entry point for the application.
//

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#if WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Windows.h"
#endif

#include "freethought.h"


//#include "win_ftimain.h"
#if Linux
#if USE_FFMPEG
#include "libavcodec/avcodec.h"  //for MJPEG needs to be on top because sizeof(AVFrame) !=
#include "libswscale/swscale.h" //for MJPEG
#endif //#if USE_FFMPEG
#endif  //Linux


//#include "../fti/win_control.h"  //determine what this is for

//I think drop PStatus.PInfo and just go with a macro
#define PINFO 0  //print info (debug)


//Global variables
ProgramStatus PStatus;
//for JPEG
unsigned char *jpgdest;
int jpglen;
int jpgdestlen;


#if Linux
//time_t ttime;
Atom XA_CLIPBOARD,XA_UTF8,XA_UNICODE,XA_WM_PROTOCOLS,XA_WM_DELETE_WINDOW,XA_WM_TAKE_FOCUS,XA_MOTIF_WM_HINTS,XA_WM_EXTENDED_HINTS;
Atom XA_TASKBAR,XA_NET_WM_WINDOW_TYPE,XA_NET_WM_WINDOW_TYPE_SPLASH,XA_NET_WM_WINDOW_TYPE_DIALOG,XA_NET_WM_WINDOW_TYPE_TOOLBAR,XA_NET_WM_WINDOW_TYPE_UTILITY,XA_NET_WM_WINDOW_TYPE_NORMAL,XA_NET_WM_WINDOW_TYPE_MENU,XA_NET_WM_STATE_SKIP_TASKBAR,XA_NET_WM_STATE;
#endif

/*
_NET_WM_WINDOW_TYPE_DESKTOP, ATOM
_NET_WM_WINDOW_TYPE_DOCK, ATOM
_NET_WM_WINDOW_TYPE_TOOLBAR, ATOM
_NET_WM_WINDOW_TYPE_MENU, ATOM
_NET_WM_WINDOW_TYPE_UTILITY, ATOM
_NET_WM_WINDOW_TYPE_SPLASH, ATOM
_NET_WM_WINDOW_TYPE_DIALOG, ATOM
_NET_WM_WINDOW_TYPE_NORMAL, ATOM
*/



FTControl * GetFTControl(char *name)
{
FTControl *tcontrol;
FTWindow *twin;

//go through all windows
twin=PStatus.iwindow;
tcontrol=0;
while(twin!=0 && tcontrol==0 && name!=0) {
	tcontrol=GetFTControlW(twin,name);
	twin=twin->next;
} //while

return(tcontrol);
}  //end GetFTControl

FTControl * GetFTControlW(FTWindow *twin, char *name)
{
int onewin;
FTControl *tcontrol;

if (PStatus.flags&PInfo)
  fprintf(stderr,"GetFTControl %s\n",name);

if (name == 0) {
	return(0);
}

onewin=0;
if (twin==0) {
	twin=PStatus.iwindow;
} else {
	onewin=1;
}
 
while (twin!=0) { // && ex0==0)
  tcontrol=twin->icontrol;
  //fprintf(stderr,"looking for FTControl %s in window %s\n",name,twin->name);
  while(tcontrol!=0) { // && ex==0)
  //  fprintf(stderr,"tcontrol->name=%p\n",tcontrol->name);
  //  fprintf(stderr,"tcontrol->name=%s\n",tcontrol->name);
    if (tcontrol->name!=0 && strcmp(tcontrol->name,name)==0) {
//  ex=1;
			if (PStatus.flags&PInfo) {
      	fprintf(stderr,"got FTControl %s\n",tcontrol->name);
			}
      return(tcontrol);
    }
//if (tcontrol->next!=0)
 // {
  //fprintf(stderr,"got here %p\n",tcontrol->next);
//fprintf(stderr,"got here0 %p\n",tcontrol);

  //fprintf(stderr,"got here0 %p\n",tcontrol);

    tcontrol=tcontrol->next;

//  if (tcontrol->next==0) {ex=1;}
//  else {tcontrol=(FTControl *)tcontrol->next;}
 // }
//  fprintf(stderr,"got here %p\n",tcontrol);

  }  //end while tcontrol!=0 for all FTControls

  if (onewin) {
		twin=0; //only search 1 window
  } else {
		twin=twin->next;  //go to next window
	} 
//if (twin->next==0) {ex0=1;}
//else {twin=(FTWindow *)twin->next;}
}  //end while twin!=0 for all windows
//fprintf(stderr,"got here return 0\n");
if (PStatus.flags&PInfo) {
  fprintf(stderr,"FTControl %s not found.\n",name);
}

return(0);
}  //end GetFTControlW

int GetFreeOptionGroupNumber(FTWindow *twin) 
{
	FTControl *tcontrol;
	int GroupNum,FoundMatch;

	tcontrol=twin->icontrol;
	//go through controls and find a free group number
	GroupNum=0;
	FoundMatch=0;
	while(!FoundMatch) {
		while (tcontrol!=0) {
			if (tcontrol->type==CTOption && tcontrol->OptionGroup==GroupNum) {
				FoundMatch=1;
			}
			tcontrol=tcontrol->next;
		} //while (tcontrol!=0
		if (FoundMatch) {
			GroupNum++;  //try another GroupNum
			tcontrol=twin->icontrol;
			FoundMatch=0;
		} else {
			FoundMatch=1; //this group number is not used yet
		}
	} //while(!NoMatch)
	return(GroupNum);
} //int GetFreeOptionGroupNumber(FTWindow *twin) 


//Check to see if this tab is taken and return a new tab if yes if no return same tab
int CheckTab(FTControl *c) //int win,int tab)
{
int dup,tab,max;
FTWindow *twin;
FTControl *tcontrol;


		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"CheckTab\n");
		}


//labels and filemenus cannot have tabs
//if (c->type==CTLabel || c->type==CTFileMenu) {
//	return(0);
//}

//fprintf(stderr,"start checktab\n");
//twin=GetFTWindowN(c->win);
twin=c->window;
tcontrol=twin->icontrol;
tab=c->tab;

dup=0;
max=tab;
while (tcontrol!=0) {
//keep trying until find a tab that is free

//c will not = tcontrol if the same FTControl - im not sure why this is
//perhaps because we only pass *FTControl and not **FTControl
//if (strcmp(c->name,tcontrol->name) && tcontrol->tab==tab && tab>0) {
if (c!=tcontrol && tcontrol->tab==tab && tab>0) {
	dup=1;//tab already taken
	//fprintf(stderr,"%s and %s have the same tab (%d).\n",c->name,tcontrol->name,tab);
	//fprintf(stderr,"%p==?%p\n",c,tcontrol);
} 

if (tcontrol->tab>max) max=tcontrol->tab;

tcontrol=tcontrol->next;
} //while


if (dup==0 && tab>0) return(tab);//no duplicate and is not 0 or -1
else return(max+1); //or else return highest known tab+1 (tabs don't need to be sequential)


//if (dup) fprintf(stderr,"Tab %d is already taken. Giving FTControl %s new tab.\n",tab,tcontrol->name);

/*
//fprintf(stderr,"get new tab\n");
//need to get new tab
tcontrol=twin->icontrol;
tab=1;
dup=0;
while(tcontrol!=0) {

if (tcontrol->tab==tab) dup=1; //already taken

tcontrol=tcontrol->next;

if (tcontrol->next==0)
  {
  if (dup)
    {
    dup=0;
    tab++;
    tcontrol=twin->icontrol;
    }
  else
    {
//    fprintf(stderr,"return tab %d\n",tab);
    //fprintf(stderr,"assigned FTControl %s with tab %d\n",tcontrol->name,tab);
    return(tab);
    ex=1;  //need? will return stop while?
    }
  }
else (tcontrol=tcontrol->next);

}//end while ex==0

}//tcontrol not >0

//fprintf(stderr,"end checktab\n");

  //since no FTControls tab is ok
if (tab<1) tab=1;

//fprintf(stderr,"assigned FTControl %s with tab %d\n",tcontrol->name,tab);
return(tab);
*/


}  //end CheckTab


//GetNextTab is different from GotoNextTab in that this just returns the next tab, without setting the twin->focus to the next tab
int GetNextTab(FTWindow *twin,int tab)
{
int min,next;
//FTWindow *twin;
FTControl *tcontrol;


		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"GetNextTab\n");
		}


//twin=GetFTWindowN(win);
tcontrol=twin->icontrol;

if (tcontrol!=0)
{
//ex=0;
min=tab;
next=tab;
while(tcontrol!=0)
{
//skip labels, scroll control buttons, 
if (tcontrol->tab!=0) {//exclude labels
  if (tcontrol->tab>tab && (tcontrol->tab<=next || next==tab)) {
		if (!(tcontrol->flags&CNoTab) && tcontrol->type!=CTLabel && tcontrol->type!=CTImage && tcontrol->type!=CTTimer && 
			!(tcontrol->flags&(CNotVisible|CNotShowing|CVScrollUp|CVScrollDown|CVScrollBack|CVScrollSlide|CHScrollLeft|CHScrollRight|CHScrollBack|CHScrollSlide)) && tcontrol->type!=CTFrame && tcontrol->type!=CTTimeLine && tcontrol->type!=CTFolderTabGroup) {
					if (tcontrol->FolderTabGroup==0 || tcontrol->FolderTabGroup->CurrentFolderTab==tcontrol->FolderTab) { //control is not on a FolderTabGroup or is on a selected FolderTab
					//fprintf(stderr,"next tab=%d\n",tab);
					 next=tcontrol->tab;
					} //tcontrol->FolderTabGroup==0
		} //CNoTab
	} //tcontrol->tab>tab
  if (tcontrol->tab<min) {
		min=tcontrol->tab;
	}
}

tcontrol=tcontrol->next;
}  //end while ex==0
if (next==tab) {
	return(min);  //none higher, go to lower number tab
}
return(next);  //go to next highest tab

}  //end if tcontrol not >0


//fprintf(stderr,"Tried to GetNextTab on a window with no FTControls.\n");
  //since no FTControls tab is ok
return(tab);
}  //end GetNextTab


int GetPrevTab(FTWindow *twin,int tab)
{
int max,prev;
FTControl *tcontrol;

//fprintf(stderr,"GetPrevTab %d\n",tab);

		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"GetPrevTab\n");
		}


tcontrol=twin->icontrol;

if (tcontrol!=0)
{
max=tab;
prev=tab;
//go thru all FTControls
while(tcontrol!=0)
{
if (tcontrol->tab!=0) {//exclude labels

//if this tab is less than the original, and higher than any other previous tab that is not equal to the original tab
//skip labels, scroll control buttons, 
  if (tcontrol->tab<tab && (tcontrol->tab>prev || prev==tab)) {
		if (!(tcontrol->flags&CNoTab) && tcontrol->type!=CTLabel && tcontrol->type!=CTImage && tcontrol->type!=CTTimer && 
			!(tcontrol->flags&(CNotVisible|CNotShowing|CVScrollUp|CVScrollDown|CVScrollBack|CVScrollSlide|CHScrollLeft|CHScrollRight|CHScrollBack|CHScrollSlide)) && tcontrol->type!=CTFrame && tcontrol->type!=CTTimeLine && tcontrol->type!=CTFolderTabGroup) {
					prev=tcontrol->tab;
		} //VScrollUp
	} //tcontrol->tab<tab
  if (tcontrol->tab>max) max=tcontrol->tab; //if this tab is > than max, make this new max
}

tcontrol=tcontrol->next;
}  //end while tcontrol!=0

//fprintf(stderr,"prev=%d tab=%d max=%d\n",prev,tab,max);
if (prev==tab) return(max);  //none lower, go to highest number tab
return(prev);  //go to next lowest tab

}  //end if tcontrol not >0


//fprintf(stderr,"Tried to GetNextTab on a window with no FTControls.\n");
  //since no FTControls tab is ok
return(tab);
}  //end GetPrevTab


//note: tab is number in tab order
FTControl * GetFTControlWithTab(FTWindow *twin,int tab)
{
FTControl *tcontrol;

	if (PStatus.flags&PInfo) {
  	fprintf(stderr,"GetFTControlWithTab\n");
	}


//fprintf(stderr,"GetFTControlWithTab %d\n",tab);

tcontrol=twin->icontrol;
while(tcontrol!=0) {
  if (tcontrol->tab==tab) return(tcontrol);
tcontrol=tcontrol->next;
}  //end while

//fprintf(stderr,"GetFTControlWithTab %d returned 0, tab must not exist\n",tab);
return(0);
} //end GetFTControlWithTab


//are these two functions still used?
FTControl * GetNextTabFTControl(FTWindow *twin,int onlyfm) 
{
int next,start;
FTControl *tcontrol;


		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"GetNextTabFTControl\n");
		}


tcontrol=twin->icontrol;
if (tcontrol!=0) {

//start=twin->focus;
start=0;
if (twin->FocusFTControl!=0) {
	start=twin->FocusFTControl->tab;
} //FocusFTControl
next=GetNextTab(twin,start);
//fprintf(stderr,"0start=%d next=%d\n",start,next);
while (start!=next) {
//  fprintf(stderr,"1start=%d next=%d\n",start,next);

//  fprintf(stderr,"2start=%d next=%d\n",start,next);
  tcontrol=GetFTControlWithTab(twin,next);
//  fprintf(stderr,"3start=%d next=%d\n",start,next);
  if (onlyfm) {
    //if (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu)) return(tcontrol);
		//submenus are not controls anymore
		if (tcontrol->type==CTFileMenu) return(tcontrol);
  }
  else return(tcontrol);

  next=GetNextTab(twin,next);
//	fprintf(stderr,"start=%d next=%d\n",start,next);
}  //end while
//no next tab, return FTControl with initial windows focus(tab)
tcontrol=GetFTControlWithTab(twin,start);
return(tcontrol);

}  //end if tcontrol!=0

//fprintf(stderr,"Tried to GetNextTab on a window with no FTControls.\n");
  //since no FTControls tab is ok
//fprintf(stderr,"exit GetNextTabFTControl\n");
return(0);
}  //end GetNextTabFTControl


FTControl * GetPrevTabFTControl(FTWindow *twin,int onlyfm)
{
int prev,start;
FTControl *tcontrol, *tcontrol2;

		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"GetPrevTabFTControl\n");
		}


tcontrol=twin->icontrol;
if (tcontrol!=0) {
//fprintf(stderr,"here\n");
//start=twin->focus;
start=0;
if (twin->FocusFTControl!=0) {
	start=twin->FocusFTControl->tab;
} //FocusFTControl
prev=GetPrevTab(twin,start);
//prev=start;
//fprintf(stderr,"0start=%d prev=%d\n",start,prev);
while (start!=prev) {
//  fprintf(stderr,"1start=%d next=%d\n",start,next);

//  fprintf(stderr,"2start=%d next=%d\n",start,next);
	//get the control that has the previous tab
  tcontrol2=GetFTControlWithTab(twin,prev);
//  fprintf(stderr,"3start=%d next=%d\n",start,next);
  if (onlyfm) {
//    if (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu)) return(tcontrol);
		//submenus are not controls anymore
    if (tcontrol2!=0) {
			//fprintf(stderr,"tab %d tcontrol=%s\n",prev,tcontrol2->name);
			if (tcontrol2->type==CTFileMenu) return(tcontrol2);
		} //tcontrol!=0
  }
  else return(tcontrol2);

  prev=GetPrevTab(twin,prev);

}  //end while

//no previous tab, return the initial FTControl (that the window had as focus)
//tcontrol=GetFTControlWithTab(twin,start);
return(tcontrol);
}  //end if tcontrol!=0

//fprintf(stderr,"Tried to GetNextTab on a window with no FTControls.\n");
  //since no FTControls tab is ok
//fprintf(stderr,"exit GetPrevTabFTControl\n");

return(0);
}  //end GetPrevTabFTControl



long long GetOpenDataNum(void)
{
long long trynum;
int ex,isfree;
FTControl *tcontrol;
char tstr[255];

tcontrol=PStatus.iwindow->icontrol;

trynum=1;
isfree=1;
if (tcontrol>0)
{
ex=0;

while(ex==0)
{
sprintf(tstr,"%lli",trynum);
//fprintf(stderr,"%s=%s\n",tcontrol->name,tstr);
if (strcmp(tcontrol->name,tstr)==0) isfree=0;

if (tcontrol->next==0)
  {
  if (isfree)
    {
    ex=1;
    return(trynum);
    }
  else
    {
    trynum++;
    tcontrol=PStatus.iwindow->icontrol;
    isfree=1;
    }
  }
else tcontrol=tcontrol->next;
}  //end while ex==0



}  //end if tcontrol not >0


//fprintf(stderr,"End GetOpenDataNum.\n");

return(trynum);  //no FTControls return 1

}  //end GetOpenDataNum

//possibly make AddFTControl
//add from a FTControl **c (the FTControl is already mallc'd)
FTControl *AddFTControl(FTWindow *twin,FTControl *c)
{
//int a;//,ex,nc;
FTControl *tcontrol,*last;
char tpath[FTMedStr];
int x,y;
#if WIN32
//int result;
#endif
//FTWindow *twin;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"add FTControl: %s\n",c->name);
}

if (twin!=0) {
	//fprintf(stderr,"&c=%p\n",&c);
	//fprintf(stderr,"data00=%p\n",c->data);
	//add FTControl will use the existing pointer for data
	//c->data has to be malloc'd by the caller
	c->next=0;  //for now AddFTControl is the only source of next pointers
	//twin=GetFTWindowN(c->winnum);
	c->window=twin;
//	c->winnum=twin->num;
//	nc=0;
	last=0;
	if (twin->icontrol==0) {  //first FTControl for window
		//fprintf(stderr,"first FTControl in window\n");
		twin->icontrol=(FTControl *)malloc(sizeof(FTControl));
		//fprintf(stderr,"malloc %d %p %s\n",sizeof(FTControl),twin->icontrol,c->name);
		tcontrol=twin->icontrol;
		last=0;
	} else { //there is at least one control, go to next available control
		tcontrol=twin->icontrol;
		while(tcontrol->next!=0) {				
			if (!strcmp(tcontrol->name,c->name)) {
				fprintf(stderr,"Error:  Tried to add FTControl with same name \"%s\"\n",tcontrol->name);
				return(0);
				} //!strcmp
			last=tcontrol;
			tcontrol=tcontrol->next;
		} //while tcontrol2!=0		
		tcontrol->next=(FTControl *)malloc(sizeof(FTControl));
		tcontrol=tcontrol->next;
	} //twin->icontrol==0


	memcpy(tcontrol,c,sizeof(FTControl));
	tcontrol->last=last;
	tcontrol->next=0;


	tcontrol->window=twin; //associate this window with this control

	//if no font for control - add default font, and set width and height
	if (tcontrol->font==0) {

		if (PStatus.flags&PInfo) {
		  fprintf(stderr,"Creating font for FTControl: %s\n",tcontrol->name);
		}

		//EnumFontFamiliesEx
		if (!SetDefaultFontForFTControl(tcontrol)) {
			fprintf(stderr,"AddFTControl: Could not set default font for FTControl %s\n",tcontrol->name);
		} //!SetDefaultFontForFTControl

		if (PStatus.flags&PInfo) {
		  fprintf(stderr,"fontwidth=%d fontheight=%d\n",tcontrol->fontwidth,tcontrol->fontheight);
		}

	} else { //if (tcontrol->font==0) {

		//there could be a font but fontwidth and fontheight are not set
		if (tcontrol->fontwidth<=0 || tcontrol->fontheight<=0) {
	#if Linux
			tcontrol->fontwidth=tcontrol->font->max_bounds.width;
			tcontrol->fontheight=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
			twin=tcontrol->window;
			if (twin==0) {
				fprintf(stderr,"AddFTControl: FTControl %s doesn't have an FTWindow using the default window's font.\n",tcontrol->name);
				//this may occur if trying to add an itemlist to a control that hasn't been added yet
			}
			//the problem is that twin->hdcWin has not been definited yet since the window has not been created yet
			if (twin==0) {				
				SelectObject(GetDC(0),tcontrol->font); 
				GetTextMetrics(GetDC(0),&tcontrol->textmet);  //this causes memory to be overwritten in the release .exe
			} else {
				SelectObject(twin->hdcWin,tcontrol->font); 
				GetTextMetrics(twin->hdcWin,&tcontrol->textmet); //this causes memory to be overwritten in the release .exe
			} //twin==0
			tcontrol->fontwidth=tcontrol->textmet.tmAveCharWidth; //using tmMaxCharWidth makes the text too small for labels- there is a large space at the end
			tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
		} //
	} //if (tcontrol->font==0) {

//if (tcontrol->type==CTButton || tcontrol->type==CTLabel || 
//tcontrol->type==CTFileMenu) {

	if (tcontrol->type==CTFolderTab) {  //for foldertab controls without some or all dimensions
		SetFolderTabDimensions(tcontrol);
	} //if (tcontrol->type==CTFolderTab) {  //for foldertab controls without some or all dimensions

	if (tcontrol->type==CTFolderTabGroup && tcontrol->FolderTab!=0) {  //adding a FolderTabGroup that is on a FolderTab
		//we need to make sure that the Z of this control is higher than the FolderTab so a button click will choose it
		if (tcontrol->z<=tcontrol->FolderTab->z) {
			tcontrol->z=tcontrol->FolderTab->z-1;
		}
	}
	if (tcontrol->FolderTabGroup!=0 && tcontrol->type!=CTFolderTabGroup) {  
		//a control that belongs to a FolderTabGroup needs to have the same or lower z than the FolderTabGroup control
		if (tcontrol->z>tcontrol->FolderTabGroup->z) {
			tcontrol->z=tcontrol->FolderTabGroup->z-1;  //need to be above FolderTabGroup control
		}
	}


//Maybe SetDefaultControlSize and SetDefaultControlColors bits?
	//Note that the width of a dropdown control that is not already set (x2<=x1 or CSetDefaultSize is set) is set in AddFTItem
	//only a dropdown control with no items added yet will be resized here
	//Note that unlike the FileMenu itemlist, the DropDown itemlist control must be large enough to fit the selected item's text
		if ((tcontrol->x2<=0 || tcontrol->x2<=tcontrol->x1 || (tcontrol->flags&CSetDefaultSize)) && (tcontrol->type!=CTDropdown || tcontrol->ilist==0)) {
			//tcontrol->x2=tcontrol->x1+tcontrol->fontwidth*(FTstrlen(tcontrol->text)+1)+PStatus.PadX; //PStatus.PadX
			tcontrol->x2=tcontrol->x1+tcontrol->fontwidth*(FTstrlen(tcontrol->text));//+PStatus.PadX; //PStatus.PadX
			if (tcontrol->type==CTDropdown) {
				tcontrol->x2+=FT_DEFAULT_DROPDOWN_WIDTH*tcontrol->fontwidth;
			} //CTDropdown
			if (tcontrol->type==CTFileMenu) {
				tcontrol->x2+=2*tcontrol->fontwidth; //space before and after text
			} //CTFileMenu

		}
		if (tcontrol->y2<=0 || tcontrol->y2<=tcontrol->y1 || (tcontrol->flags&CSetDefaultSize)) {
			tcontrol->y2=tcontrol->y1+tcontrol->fontheight+PStatus.PadY;  //PadY was +2 for underline
		}

		

		if (tcontrol->type==CTDropdown && (tcontrol->x3<=0 || (tcontrol->flags&CSetDefaultSize))) {
			tcontrol->x3=tcontrol->x2-tcontrol->fontwidth*2;
			if (tcontrol->x3<=tcontrol->x1) {
				tcontrol->x3=tcontrol->x2;
			}
		}

//} //if (tcontrol->type==CTButton || tcontrol->type==CTLabel || 

		//color[0]==color[1] or textcolor[0]==textcolor[1] and is not a scrollbar button
		if (tcontrol->color[0]==tcontrol->color[1] || ((tcontrol->textcolor[0]==tcontrol->textcolor[1]) && !(tcontrol->type==CTButton && ((tcontrol->flags&CVScrollSlide) || (tcontrol->flags&CVScrollBack) || (tcontrol->flags&CVScrollUp) || (tcontrol->flags&CVScrollDown) || (tcontrol->flags&CHScrollSlide) || (tcontrol->flags&CHScrollBack) || (tcontrol->flags&CHScrollLeft) || (tcontrol->flags&CHScrollRight))))) {
			tcontrol->flags|=CSetDefaultColors;
		}
		
		if (tcontrol->flags&CSetDefaultColors) {

			if (tcontrol->type==CTButton) {
				tcontrol->ControlBackgroundColor=ltgray;  //background no press
				tcontrol->ControlButtonDownColor=dkgray;  //button down color
				tcontrol->ControlSelectedBackgroundColor=gray;  //background (selected or mouse over)
				tcontrol->ControlBorderColor=dkgray;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black; 
					tcontrol->textcolor[1]=white; 
					tcontrol->textcolor[2]=blue; 
				} 
			}  //CTButton


			if (tcontrol->type==CTLabel) {
				tcontrol->ControlBackgroundColor=wingray;
				tcontrol->ControlButtonDownColor=dkgray;
				tcontrol->ControlSelectedBackgroundColor=gray;
				tcontrol->ControlBorderColor=dkgray;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black;//~tcontrol->textcolor[0];
					tcontrol->textcolor[1]=wingray;//~tcontrol->textcolor[0];
					tcontrol->textcolor[2]=blue;//~tcontrol->textcolor[0];
				} //if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
			}

			if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
				tcontrol->ControlBackgroundColor=white;//ControlBackgroundColor ~tcontrol->textcolor[0];
				tcontrol->ControlBorderColor=black;	//color[1] is ControlBorderColor
				tcontrol->ControlSelectedBackgroundColor=blue;
				tcontrol->ControlBorderColor=dkgray;

				//tcontrol->TextColor=black;//~tcontrol->textcolor[0];
				tcontrol->TextBackgroundColor=white;//~tcontrol->textcolor[0];
				//tcontrol->textcolor[2]=blue; //selected text color PStatus->DefaultSelectedTextColor
				//tcontrol->textcolor[3]=~tcontrol->textcolor[2];
				tcontrol->SelectedTextColor=white; 
				tcontrol->SelectedTextBackgroundColor=blue;
			}

			if (tcontrol->type==CTFileMenu) {
				tcontrol->ControlBackgroundColor=ltgray;  //background no press
				tcontrol->ControlButtonDownColor=black;
				tcontrol->ControlSelectedBackgroundColor=ltblue;//dkgray;  //background (mouse over)
				tcontrol->ControlMouseOverBackgroundColor=gray;  //bkground press
				tcontrol->ControlBorderColor=dkgray;
				
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black;//~tcontrol->textcolor[0]; control and item text color
					tcontrol->textcolor[1]=white;//only item text background color- control text has control color as background
					tcontrol->textcolor[2]=blue;//~tcontrol->textcolor[0];
				}
			} //CTFileMenu

			if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList) {
				tcontrol->ControlBackgroundColor=ltgray;  //background no press-Dropdown button
				tcontrol->ControlButtonDownColor=dkgray;  //control - button down
				tcontrol->ControlSelectedBackgroundColor=blue;  //background (mouse over) - color behind triangle
				tcontrol->ControlMouseOverBackgroundColor=gray;;  //bkground press
				tcontrol->ControlBorderColor=dkgray;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black;//~tcontrol->textcolor[0];
					tcontrol->textcolor[1]=white;//text in comb background color
					tcontrol->textcolor[2]=blue;//~tcontrol->textcolor[0];
				} 
			} //CTDropdown

			if (tcontrol->type==CTOption) {
				tcontrol->ControlBackgroundColor=wingray;
				tcontrol->ControlButtonDownColor=dkgray;
				tcontrol->ControlSelectedBackgroundColor=gray;
				tcontrol->ControlBorderColor=dkgray;
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black; 
					tcontrol->textcolor[1]=wingray; 
					tcontrol->textcolor[2]=blue; 
				} 
			}  //CTOption

			if (tcontrol->type==CTCheckBox) {
				tcontrol->ControlBackgroundColor=ltgray;  //background no press
				tcontrol->ControlButtonDownColor=dkgray;  //button down color
				tcontrol->ControlSelectedBackgroundColor=gray;  //background (selected or mouse over)
				tcontrol->ControlMouseOverBackgroundColor=gray;  //?
				tcontrol->ControlBorderColor=dkgray;
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black; 
					tcontrol->textcolor[1]=wingray; 
					tcontrol->textcolor[2]=blue; 
				} 
			}  //CTCheckBox

			if (tcontrol->type==CTFolderTabGroup) {
				tcontrol->ControlBackgroundColor=wingray;  //background color
				tcontrol->ControlButtonDownColor=ltgray;//button down background color
				tcontrol->ControlSelectedBackgroundColor=gray;//?
				tcontrol->ControlMouseOverBackgroundColor=dkgray;  //?
				tcontrol->ControlBorderColor=dkgray;
				tcontrol->ControlShadedBackgroundColor=ltgray;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black;//~tcontrol->textcolor[0]; control and item text color
					tcontrol->textcolor[1]=white;//only item text background color- control text has control color as background
					tcontrol->textcolor[2]=blue;//~tcontrol->textcolor[0];
				} 
			} //if (tcontrol->type==CTFolderTabGroup || tcontrol->type==CTFolderTab) {
			if (tcontrol->type==CTFolderTab) {  //foldertab button
				tcontrol->ControlBackgroundColor=ltgray;  //background no press
				tcontrol->ControlButtonDownColor=dkgray;  //button down color
				tcontrol->ControlSelectedBackgroundColor=gray;  //background (selected or mouse over)

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=black; 
					tcontrol->textcolor[1]=white; 
					tcontrol->textcolor[2]=blue; 
				} 
				tcontrol->ControlBorderColor=dkgray;
			}  //CTButton

			if (tcontrol->FolderTabGroup!=0) {  //this control belongs to a FolderTabGroup and should have the same background color
				tcontrol->ControlBackgroundColor=tcontrol->FolderTabGroup->ControlBackgroundColor;
			}

		} //(tcontrol->flags&CSetDefaultColors)



	//if people don't create a checkbox x3 (the end of the checkbox and start of checkbox text)
	//then create one
	if ((tcontrol->type==CTCheckBox || tcontrol->type==CTOption) && (tcontrol->x3<=0 || tcontrol->x3>tcontrol->x2)) {
		tcontrol->x3=tcontrol->x1+tcontrol->fontwidth*2;
	}

	//if a checkbox is not long enough for the text widen it- otherwise the outline at focus draws over the text (+2 for space before and after text)
	if ((tcontrol->type==CTCheckBox || tcontrol->type==CTOption) && (strlen(tcontrol->text)+2)*tcontrol->fontwidth>tcontrol->x2-tcontrol->x3) {
		tcontrol->x2=tcontrol->x3+tcontrol->fontwidth*(strlen(tcontrol->text)+2);
	}

	//same for option (radio)- 
	//if an option control is not long enough for the text widen it- otherwise the outline at focus draws over the text
	//if (tcontrol->type==CTOption && strlen(tcontrol->text)*tcontrol->fontwidth>tcontrol->x2-tcontrol->x1)) {
	//	tcontrol->x2=tcontrol->x1+tcontrol->fontwidth*strlen(tcontrol->text);
//	}
	if ((tcontrol->type==CTFolderTabGroup || tcontrol->type==CTFolderTabGroup)  && tcontrol->y3<=0) {
		tcontrol->y3=tcontrol->y1+FT_DEFAULT_FOLDERTAB_HEIGHT;  //if y3 is not defined just set 32 pixels below
		if (tcontrol->z==0) {
			tcontrol->z=1; //so that controls on the FolderTabGroup will have a priotiy on button clicks
		}
	}


	if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
		if (strlen(tcontrol->text)>0 && strlen(tcontrol->text2)==0) {
			strcpy(tcontrol->text2,tcontrol->text); //otherwise OnChange will get called from the initial DrawFTControl because text!=text2
		}
	} //if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {

		//see if this tab is already taken
//what FTControl will not have a tab?
//      if (c->type==CTTextBox || c->type==CTButton)
//      {
  if (tcontrol->type!=CTLabel && tcontrol->type!=CTImage && tcontrol->type!=CTTimer && !(tcontrol->flags&CNoTab)) {
    tcontrol->tab=CheckTab(tcontrol);//->win,c->tab);
#if 0 
	//if this tab is the lowest tab on the window, set the window focus to this control
		//start with no control having focus - user must use hot key or mouse click to select something
		if (twin->FocusFTControl==0) {
			twin->FocusFTControl=tcontrol;
		} else {
			if (tcontrol->tab<twin->FocusFTControl->tab) {
				twin->FocusFTControl=tcontrol;
			} //tcontrol->tab
		} //FocusFTControl==0
#endif
	} //  if (tcontrol->type!=CTLabel && !(tcontrol->flags&CNoTab)) {
//				    tcontrol->tab=CheckTab(c);//->win,c->tab);
		//fprintf(stderr,"%s has tab %d\n",tcontrol->name,tcontrol->tab);
//      }


	if (tcontrol->type==CTFileOpen) { //FileOpen Controls require associated Name, Size and Date buttons

		if (PStatus.flags&PInfo) {
			fprintf(stderr,"CTFileOpen control\n");
		}

		//add Folder and file images - these should perhaps be compiled in libfreethought - but gives people the chance to customize
		strcpy(tpath,PStatus.ProjectPath);
#if Linux
		strcat(tpath,"images/folder.bmp");
		//folder and file image need to be wider
		x=(int)(2*tcontrol->fontwidth);
#endif
#if WIN32
		strcat(tpath,"images\\folder.bmp");
		x=tcontrol->fontwidth;
#endif
//		x=(int)(1.5*tcontrol->fontwidth);
//		y=(int)(1.5*tcontrol->fontheight);
		y=tcontrol->fontheight;

		//Load folder image, and make folder same size as font - this should be done in DrawFTControl probably
		//because the window might not be open, and in windows this means no font is available to get the metrics of
		//Perhaps this should be in DrawFTControl - if (tcontrol->image[0]==0) - load image of folder, etc
		//because here - the font dimensions (metrics) must be known
#if Linux
		tcontrol->image[0]=FT_LoadBitmapFileAndScale(tpath,x,y);
#endif
#if WIN32
		tcontrol->image[0]=FT_LoadBitmap(tpath);
		//tcontrol->image[0]=FT_LoadBitmapFileAndScale(tpath,x,y);
#endif
		tcontrol->bmpinfo[0]=(BITMAPINFOHEADER *)(tcontrol->image[0]+sizeof(BITMAPFILEHEADER));
		tcontrol->bmpdata[0]=((unsigned char *)tcontrol->bmpinfo[0]+sizeof(BITMAPINFOHEADER));
		//tcontrol->imagedata=tcontrol->image[0]+sizeof(FTBITMAPFILEHEADER)+sizeof(FTBITMAPINFOHEADER);
		if (tcontrol->image[0]==0) {
			fprintf(stderr,"Error loading folder.bmp from %s\n",tpath);
		} else {
#if Linux
			tcontrol->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol->bmpdata[0],x,y,32,x*4);
			if (tcontrol->ximage[0]<=0) {
				fprintf(stderr,"XCreateImage for folder.bmp failed\n");
			} 
#endif
#if WIN32
		//note x needs to be 4 byte aligned
			//note that parameters 1 and 2 32,32 are bitmap dimenions and need to match the bitmap
			tcontrol->hbmp[0]= CreateBitmap(32,32,1,32,tcontrol->bmpdata[0]);
			//tcontrol->hbmp[0]= CreateCompatibleBitmap(twin->hdc,x,y);
			//tcontrol->hbmp[0]=CreateDIBSection(twin->hdcWin,(BITMAPINFO *)tcontrol->bmpinfo[0],DIB_RGB_COLORS,(void **)tcontrol->image[0],NULL,NULL);
#endif
		}


		if (tcontrol->TextColor==tcontrol->TextBackgroundColor && tcontrol->TextColor==0) {
			tcontrol->flags|=CSetDefaultColors;
		}
		if (tcontrol->flags&CSetDefaultColors) {
			tcontrol->TextColor=0;
			tcontrol->TextBackgroundColor=0xffffff;
			tcontrol->SelectedTextColor=0xffffff;
			tcontrol->SelectedTextBackgroundColor=blue;
			tcontrol->ControlBorderColor=0;
			tcontrol->ControlBackgroundColor=0xffffff;
		}

		//the file.bmp image goes on tcontrol->ximage[1]
		strcpy(tpath,PStatus.ProjectPath);
#if Linux
		strcat(tpath,"images/file.bmp");
#endif
#if WIN32
		strcat(tpath,"images\\file.bmp");
#endif

#if Linux
		tcontrol->image[1]=FT_LoadBitmapFileAndScale(tpath,x,y);
#endif
#if WIN32
		tcontrol->image[1]=FT_LoadBitmap(tpath);
		//tcontrol->image[1]=FT_LoadBitmapFileAndScale(tpath,x,y);
#endif

		tcontrol->bmpinfo[1]=(BITMAPINFOHEADER *)(tcontrol->image[1]+sizeof(BITMAPFILEHEADER));
		tcontrol->bmpdata[1]=((unsigned char *)tcontrol->bmpinfo[1]+sizeof(BITMAPINFOHEADER));

		if (tcontrol->image[1]==0) {
			fprintf(stderr,"Error loading file.bmp from %s\n",tpath);
		} else {
#if Linux
			tcontrol->ximage[1]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol->bmpdata[1],x,y,32,x*4);
			if (tcontrol->ximage[1]<=0) {
				fprintf(stderr,"XCreateImage for file.bmp failed\n");
			} 
#endif
#if WIN32
			tcontrol->hbmp[1]= CreateBitmap(32,32,1,32,tcontrol->bmpdata[1]);
			//tcontrol->hbmp[1]= CreateCompatibleBitmap(twin->hdc,x,y);
			//tcontrol->hbmp[0]=CreateDIBSection(twin->hdcWin,(BITMAPINFO *)tcontrol->bmpinfo[0],DIB_RGB_COLORS,(void **)tcontrol->image,NULL,NULL);
#endif
		}



		if (tcontrol->flags&CFOName || tcontrol->flags&CFOSize || tcontrol->flags&CFODate) {
			AddFileOpenFTControls(twin,tcontrol);
		}

		//scroll controls are added later to any control with the bit set

	} //CTFileOpen


	//adjust dimensions of data control based on associated timeline control
	if (tcontrol->type==CTData) {
		AdjustDataFTControlDimensions(tcontrol);
		//fprintf(stderr,"Add x1=%d x2=%d\n",tcontrol->x1,tcontrol->x2);
	}


	//Add any scroll bar button controls needed for this control
  if (tcontrol->flags&(CVScroll|CHScroll))	{
		//fprintf(stderr,"Add scroll controls\n");
    AddScrollFTControls(twin,tcontrol);
	}

  //if a button has an image but no shaded image for button press add one
   if (tcontrol->type==CTButton && tcontrol->image[0]!=0 && tcontrol->image[1]==0 && !(tcontrol->flags&CNoColorChangeButton(0))) {
	    tcontrol->image[1]=FT_LoadBitmapAndShade(tcontrol->image[0],0xcfcfcf);
#if WIN32 
		tcontrol->hbmp[1]= CreateBitmap(tcontrol->bmpinfo[0]->biWidth,tcontrol->bmpinfo[0]->biHeight,1,32,tcontrol->image[1]+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
#endif
		tcontrol->bmpinfo[1]=(BITMAPINFOHEADER *)(tcontrol->image[1]+sizeof(BITMAPFILEHEADER));
	   //shade the image for a mouse over
		if (tcontrol->image[2]==0 && !(tcontrol->flags&CNoChangeOnMouseOver)) {
			tcontrol->image[2]=FT_LoadBitmapAndShade(tcontrol->image[0],0x7f7f7f); 
#if WIN32
			tcontrol->hbmp[2]= CreateBitmap(tcontrol->bmpinfo[0]->biWidth,tcontrol->bmpinfo[0]->biHeight,1,32,tcontrol->image[2]+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
#endif
			tcontrol->bmpinfo[2]=(BITMAPINFOHEADER *)(tcontrol->image[2]+sizeof(BITMAPFILEHEADER));

		}//if (!(tcontrol->flags&CNoChangeOnMouseOver)) {
   } //if (tcontrol->type!=CTButton && tcontrol->image[1]==0 && !(tcontrol->flags&CNoColorChangeButton(0))) {

   //if this control has any itemlists we need to update the itemlist->control and itemlist->window pointer (and perhaps font) to this new pointer
   if (tcontrol->ilist!=0) {
	   //this control has an itemlist
	   tcontrol->ilist->control=tcontrol;
	   //note: the ilist->window is it's own FTWindow, not the control's  was: tcontrol->ilist->window=twin;
	   if (tcontrol->ilist->flags&FTIL_USE_CONTROL_FONT) {
#if WIN32
		   //don't DeleteObject the tcontrol->ilist->font because the control inherits the font given to it at the first AddFTItem
#endif
		   tcontrol->ilist->font=tcontrol->font;
		   tcontrol->ilist->fontwidth=tcontrol->fontwidth;
		   tcontrol->ilist->fontheight=tcontrol->fontheight;
#if WIN32
		   //tcontrol->ilist->textmet=tcontrol->textmet;
			memcpy(&tcontrol->ilist->textmet,&tcontrol->textmet,sizeof(TEXTMETRIC));
#endif
		   //possibly this may change the size of the itemlist- but I think that may be calculated at run-time
	   } //
   } //if (tcontrol->ilist!=0) {


   if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList || tcontrol->type==CTFileOpen) {
	   if (tcontrol->ilist!=0) {
			tcontrol->ilist->flags|=FTIL_HAS_NO_HOT_KEYS;  //unlike FileMenus dropdown and itemlist controls do not have to be drawn one character at a time to look for an underlined hotkey
			if (tcontrol->type==CTItemList || tcontrol->type==CTFileOpen) {
				tcontrol->ilist->flags|=FTIL_IS_IN_A_CONTROL; //itemlist is in a control not a window
				tcontrol->ilist->window=twin;  //add this window to the itemlist window - important because DelFTItemList will free any associated itemlist window
			}
			if (tcontrol->FirstItemShowing==0) {
				tcontrol->FirstItemShowing=1;  //otherwise DrawUnselectedFTItem is off by one extra 
			}
	   }
   }


   if (tcontrol->type==CTFolderTabGroup) {
	   //if FolderTabGroup control is on a FolderTab, only show if the FolderTab is visible
	   if (tcontrol->FolderTab!=0) {
			if (tcontrol->FolderTab->flags&CNotShowing) { //FolderTab this FolderTabGroup is on is not showing
				tcontrol->flags|=CNotShowing;
			} 
	   }//if (tcontrol->FolderTab!=0) {
	   //if (tcontrol->CurrentFolderTab==0) { //wait for FolderTab controls to be added to set CurrentFolderTab control

	   //}
   } //if (tcontrol->type==CTFolderTabGroup) {


   if (tcontrol->type==CTFolderTab) {
	   //if no foldertab is selected in FolderTabGroup set the current foldertab to this one
	   if (tcontrol->FolderTabGroup!=0) {
			if (tcontrol->FolderTabGroup->CurrentFolderTab==0) {
				SelectFolderTab(tcontrol); //this will set/unset CNotShowing on all descending controls				
			} else {
				UnselectFolderTab(tcontrol); //this will set CNotShowing on all descending controls				
			}
	   } else {
			fprintf(stderr,"Error: FolderTab %s does not belong to any FolderTabGroup control\n",tcontrol->name);
	   }
   } //


   if (tcontrol->FolderTabGroup!=0) {  //control is a member of a FolderTabGroup
	   //set it's CNotShowing if it's FolderTabGroup or Folder also has it set
	   if (tcontrol->FolderTabGroup->flags&CNotShowing) {
		   tcontrol->flags|=CNotShowing;
	   }
	   if (tcontrol->FolderTab !=0) {
		   if ((tcontrol->FolderTab->flags&CNotShowing) || tcontrol->FolderTab!=tcontrol->FolderTabGroup->CurrentFolderTab) {
			   if (tcontrol->type==CTFolderTabGroup) {
				   UnselectFolderTabGroup(tcontrol);
			   }
			   tcontrol->flags|=CNotShowing;
		   } else {
			   if (tcontrol->type==CTFolderTabGroup) {
				   SelectFolderTabGroup(tcontrol);
			   }
			   tcontrol->flags&=~CNotShowing;
		   } //if ((tcontrol->FolderTab->flags&CNotShowing) || tcontrol->FolderTab!=tcontrol->FolderTabGroup->CurrentFolderTab) {
	   } //	   if (tcontrol->FolderTab !=0) {
   } //   if (tcontrol->FolderTabGroup!=0) {  //control is a member of a FolderTabGroup


  if (PStatus.flags&PInfo) {
    fprintf(stderr,"added FTControl %s\n",tcontrol->name);
	}

	return(tcontrol);

} //twin!=0

if (PStatus.flags&PInfo)
	fprintf(stderr,"end AddFTControl\n");

return(0);
}  //end AddFTControl



//before I was adding the labels above the control, but I think the labels being part of the control is more logical - actually it's true that there may be no filename, date, etc controls
//but it should just take the required space if there are
void AddFileOpenFTControls(FTWindow *twin,FTControl *tcontrol)
{
FTControl *tcontrol2;
char tstr[FTMedStr];
int cw,tx[3],fh,fw;
//int AreLabels;



//FTWindow *twin2;

if (PStatus.flags&PInfo)
	fprintf(stderr,"AddFileOpenFTControls\n");


if (tcontrol!=0) {
	//twin=GetFTWindowN(tcontrol->win);
	tcontrol2=(FTControl *)malloc(sizeof(FTControl));

	cw=tcontrol->x2-tcontrol->x1;
	tx[0]=0; //initial boundaries for fileopen control
	tx[1]=1;
	tx[2]=2;
	


	if (tcontrol->font==0) {
		fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
#if WIN32
		tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		if (tcontrol->font==0) {
		}
		SelectObject(twin->hdcWin,tcontrol->font);
		GetTextMetrics(twin->hdcWin,&tcontrol->textmet);
		tcontrol->fontwidth=tcontrol->textmet.tmAveCharWidth;
		tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
	}
	fh=tcontrol->fontheight;
	fw=tcontrol->fontwidth;


	//Name, Size, Date buttons on the top of a FileOpen control- 
	//note that the actual control Y1 is lowered to accomodate these buttons
	//the buttons should not be located above the control, 
	//and cannot be located ontop of the control because the refresh causes blinking
	//so the control is lowered and they are added in the newly created space above
	if ((tcontrol->flags&CFOName) || (tcontrol->flags&CFOSize)|| (tcontrol->flags&CFODate)) {



	//	AreLabels=1;
//		tcontrol->y3=tcontrol->y1+fh;
		//lower fileopen control top
		//todo: Add PStatus.DefaultTextBoxHeight=(int)(1.5*fh), or twin, or tcontrol variable
		tcontrol->y1=tcontrol->y1+(int)(1.5*fh);
		}
//	} else {
	//	AreLabels=0;
//		tcontrol->y3=tcontrol->y1;
//	}

	if (tcontrol->flags&CFOName) {
		tx[0]=tcontrol->x1; 
		if (tcontrol->flags&CFOSize) {
			tx[1]=(int)((float)(tcontrol->x2-tcontrol->x1)/2.0);
			if (tcontrol->flags&CFODate) { 	
				tx[2]=(int)((float)(tcontrol->x2-tcontrol->x1)*3.0/4.0);
			} else {
				tx[2]=tcontrol->x2;//CFODate
			} //CFODate
		} else {
			tx[1]=tcontrol->x2;
			tx[2]=tcontrol->x2;
		} //CFOSize
	} else {
	 if (tcontrol->flags&CFOSize) {
			tx[1]=tcontrol->x1;
			if (tcontrol->flags&CFODate) { 	
				tx[2]=(int)((float)(tcontrol->x2-tcontrol->x1)/2.0);
			} else tx[2]=tcontrol->x2; //CFODate
		} else {
			if (tcontrol->flags&CFODate) { 	
				tx[2]=tcontrol->x1;
			} else {
				tx[2]=tcontrol->x2;
			} //CFODate
		} //CFOSize
	} //CFOName

//	fprintf(stderr,"x[0]=%d x[1]=%d x[2]=%d cw=%d\n",x[0],x[1],x[2],cw);
	if (tcontrol->flags&CFOName) {  //CFOName
		//3 Name,Size,Date buttons
		memset(tcontrol2,0,sizeof(FTControl));
		sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrol->name);
		//sprintf(tstr,"FO_Name_%s",tcontrol->name);
		strcpy(tcontrol2->name,tstr);  
		//strcpy(tcontrol->name,"btnfoName");  
		tcontrol2->parent=tcontrol;
	#if Linux
		tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
		if (tcontrol2->font==0) {
			fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
		}
	#endif
		tcontrol2->fontheight=fh;
		tcontrol2->fontwidth=fw;
		tcontrol2->type=CTButton;
		tcontrol2->x1=tx[0];//10;  //these will be changed in human func on resize
		//verify y1-fh>0

	//	tcontrol2->y1=tcontrol->y1;
	//	tcontrol2->y2=tcontrol->y1+fh;
		//todo: probably there should be DefaultTextboxHeight- which if not set is fh+1/2fh
		tcontrol2->y1=tcontrol->y1-(int)(1.5*fh);
		tcontrol2->y2=tcontrol->y1;
		tcontrol2->z=-1; //put above fileopen control
	//
	//	if (tcontrol->y1-fh>0) {
	//		tcontrol2->y1=tcontrol->y1-fh;//46;
	//	} else {
	//		fprintf(stderr,"Warning: not enough space for control buttons above fileopen control.\n");
	//	tcontrol2->y1=0;
	//	}
		tcontrol2->x2=tx[1];//10+tcontrol2->font->max_bounds.width*20;
	//	tcontrol2->y2=tcontrol->y1;//46+(tcontrol2->font->max_bounds.ascent+tcontrol2->font->max_bounds.descent+4); 
		tcontrol2->x3=tcontrol2->x2-tcontrol2->x1;  //hold on to original size
		tcontrol2->z=-100; //above other controls
		strcpy(tcontrol2->text,"Name");
		tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoName_DownUp;
		tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoName_DownUp;
		//tcontrol2->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
		//tcontrol2->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
		tcontrol2->OnResize=(FTControlfuncwcxy *)btnfoName_OnResize;
		tcontrol2->flags=CResizeX2|CGrowX2|CNoTab;
		tcontrol2->sx2=(float)(tcontrol2->x2-tcontrol2->x1)/(float)cw;//0.4; //still need?
	//	fprintf(stderr,"sx2=%f\n",tcontrol2->sx2);
		//tcontrol2->gx2=-40;
		//tcontrol2->gy2=-70;
		tcontrol2->color[0]=gray;
		tcontrol2->color[1]=ltgray;
		tcontrol2->color[2]=dkgray;
		//tcontrol2->textcolor[0]
		//tcontrol2->win=twin->num;
	//	fprintf(stderr,"addcontrol\n");
		AddFTControl(twin,tcontrol2);
	//	fprintf(stderr,"after addcontrol\n");
	}  //CFOName
	if (tcontrol->flags&CFOSize) {  //CFOSize
		memset(tcontrol2,0,sizeof(FTControl));
		//sprintf(tstr,"FO_Size_%s",tcontrol->name);
		sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrol->name);
		strcpy(tcontrol2->name,tstr);  
		//strcpy(tcontrol2->name,"btnfoSize");  
		tcontrol2->parent=tcontrol;
	#if Linux
		tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
		if (tcontrol2->font==0) {
			fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
		} 
		fh=tcontrol2->font->max_bounds.ascent+tcontrol2->font->max_bounds.descent+4;
	#endif
	#if WIN32
		fh=tcontrol->fontheight;
	#endif
		tcontrol2->fontheight=fh;
		tcontrol2->fontwidth=fw;
		tcontrol2->type=CTButton;
		tcontrol2->x1=tx[1];//10+tcontrol2->font->max_bounds.width*20;

	//	tcontrol2->y1=tcontrol->y1;
	//	tcontrol2->y2=tcontrol->y1+fh;
		tcontrol2->y1=tcontrol->y1-(int)(1.5*fh);

		tcontrol2->y2=tcontrol->y1;
		tcontrol2->z=-1; //put above fileopen control
	//	if (tcontrol->y1-fh>0) {
	//		tcontrol2->y1=tcontrol->y1-fh;//46;
	//	} else {
	//		fprintf(stderr,"Warning: not enough space for control buttons above fileopen control.\n");
	//	tcontrol2->y1=0;
	//	}
		tcontrol2->x2=tx[2];//10+tcontrol2->font->max_bounds.width*28;
		//tcontrol2->y2=tcontrol->y1;//46+(tcontrol2->font->max_bounds.ascent+tcontrol2->font->max_bounds.descent+4); 
		tcontrol2->x3=tcontrol2->x2-tcontrol2->x1;  //hold on to original size
		tcontrol2->z=-100; //above other controls
		strcpy(tcontrol2->text,"Size");
		tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoSize_DownUp;
		tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoSize_DownUp;
		tcontrol2->OnResize=(FTControlfuncwcxy *)btnfoSize_OnResize;
		//tcontrol2->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
		//tcontrol2->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
		tcontrol2->flags=CResizeX1|CResizeX2|CGrowX1|CGrowX2|CNoTab;
		tcontrol2->sx1=(float)(tcontrol2->x2-tcontrol2->x1)/(float)cw;//.3;
		tcontrol2->sx2=(float)(tcontrol2->x2-tcontrol2->x1)/(float)cw;//.3;
	//	fprintf(stderr,"b sx2=%f cw=%d x2-x1=%d\n",tcontrol2->sx2,cw,tcontrol2->x2-tcontrol2->x1);
		//tcontrol2->gx2=-40;
		//tcontrol2->gy2=-70;
		tcontrol2->color[0]=gray;
		tcontrol2->color[1]=ltgray;
		tcontrol2->color[2]=dkgray;
		//tcontrol2->win=twin->num;
		AddFTControl(twin,tcontrol2);
	}  //CFOSize
	if (tcontrol->flags&CFODate) {  //CFODate
		memset(tcontrol2,0,sizeof(FTControl));
		sprintf(tstr,"%s%s",PStatus.PrefixDate,tcontrol->name);
		//sprintf(tstr,"FO_Date_%s",tcontrol->name);
		strcpy(tcontrol2->name,tstr);  
		//strcpy(tcontrol2->name,"btnfoDate");  
		tcontrol2->parent=tcontrol;
	#if Linux
		tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
		if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
		fh=tcontrol2->font->max_bounds.ascent+tcontrol2->font->max_bounds.descent+4;
	#endif
	#if WIN32
		fh=tcontrol->fontheight;
	#endif
		tcontrol2->fontheight=fh;
		tcontrol2->fontwidth=fw;
		tcontrol2->type=CTButton;
		tcontrol2->x1=tx[2];//10+tcontrol2->font->max_bounds.width*28;
	//	tcontrol2->y1=tcontrol->y1;
	//	tcontrol2->y2=tcontrol->y1+fh;
		//tcontrol2->y1=tcontrol->y1-fh;
		tcontrol2->y1=tcontrol->y1-(int)(1.5*fh);
		tcontrol2->y2=tcontrol->y1;
		tcontrol2->z=-1; //put above fileopen control

	//	if (tcontrol->y1-fh>0) {
	//		tcontrol2->y1=tcontrol->y1-fh;//46;
	//	} else {
	//		fprintf(stderr,"Warning: not enough space for control buttons above fileopen control.\n");
	//	tcontrol2->y1=0;
	//	}
		tcontrol2->x2=tcontrol->x2;//10+tcontrol2->font->max_bounds.width*50;
		//tcontrol2->y2=tcontrol->y1;//46+(tcontrol2->font->max_bounds.ascent+tcontrol2->font->max_bounds.descent+4); 
		tcontrol2->x3=tcontrol2->x2-tcontrol2->x1;  //hold on to original size
		tcontrol2->z=-100; //above other controls
		strcpy(tcontrol2->text,"Date");
		tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)btnfoDate_DownUp;
		tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)btnfoDate_DownUp;
		//tcontrol2->Keypress=(FTControlfuncwk *)foOpenFile2_Keypress;
		//tcontrol2->OnDraw=(FTControlfunc *)foOpenFile2_OnDraw;
		tcontrol2->OnResize=(FTControlfuncwcxy *)btnfoDate_OnResize;
		tcontrol2->flags=CResizeX1|CGrowX1|CGrowX2|CNoTab;
		tcontrol2->sx1=(float)(tcontrol2->x2-tcontrol2->x1)/(float)cw;//.3; //need? - change to correct proportion then for resize
		tcontrol2->sx2=(float)(tcontrol2->x2-tcontrol2->x1)/(float)cw;//.3;
	//	fprintf(stderr,"c sx2=%f\n",tcontrol2->sx2);
		//tcontrol2->gx2=-40;
		//tcontrol2->gy2=-70;
		tcontrol2->color[0]=gray;
		tcontrol2->color[1]=ltgray;
		tcontrol2->color[2]=dkgray;
		//tcontrol2->win=twin->num;
		AddFTControl(twin,tcontrol2);
	}  //CFODate

	free(tcontrol2);
//	fprintf(stderr,"before free\n");
	//free(tcontrol2);
} //tcontrol!=0

if (PStatus.flags&PInfo) 
	fprintf(stderr,"End AddFileOpenFTControls\n");

}  //AddFileOpenFTControls


void AddScrollFTControls(FTWindow *twin, FTControl *tcontrol)
{
FTControl *tcontrol2;
char tstr[FTMedStr],path[FTMedStr];
//FTWindow *twin;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"AddScrollFTControls\n");
}

//these controls are outside of their parent control

tcontrol2=(FTControl *)malloc(sizeof(FTControl));

if (tcontrol->flags&CVScroll) { // || tcontrol->flags&CHScroll)
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"Add vertical scroll\n");
	}

	//flag CVScroll would auto make a new scroll bar (4 FTControls)
	//A scrollbar is simply 4 FTControls
	//vertical:
	//_scr_up_controlname=(top) scroll up button
	//_scr_down_controlname=(bottom) scroll down button
	//_scr_vslide_controlname=middle slider button
	//_scr_vback_controlname=middle background button
	//horizontal:
	//_scr_left_controlname=(left) scroll left button
	//_scr_right_controlname=(right) scroll right button
	//_scr_hslide_controlname=middle slider button
	//_scr_hback_controlname=middle background button
	memset(tcontrol2,0,sizeof(FTControl));

	//scroll up arrow
	sprintf(tstr,"%s%s",PStatus.PrefixVScrollUp,tcontrol->name);
	//sprintf(tstr,"ScrollUp_%s",tcontrol->name);
	//strcpy(tcontrol2->name,"s1_foOpenFile2");  
	strcpy(tcontrol2->name,tstr);  
#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x2;
	tcontrol2->y1=tcontrol->y1;
	tcontrol2->x2=tcontrol2->x1+16;
	tcontrol2->y2=tcontrol2->y1+16;
	tcontrol2->z=tcontrol->z-1; //above other controls
	//do away with callback and put inline to free for user to use
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)Scroll_UpButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)Scroll_UpButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)Scroll_UpButton;  //double click can also scroll
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)s1_foOpenFile2_DownUp;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)s1_foOpenFile2_DownUp;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)s1_foOpenFile2_DownUp;  //double click can also scroll
	//this click has some internal processing (getting filename and putting filename in filename field)
	//and can have human (user/external) processing too
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)btnGetFile_Click;
	//ButtonDown up will select a file
	//tcontrol2->tab=-1;
	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//50ms 1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//300ms 1000=1ms
	//tcontrol2->flags=CGrowX1|CGrowX2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollUp;
	//inherit Grow controls of parent control?
	//tcontrol2->flags|=(tcontrol->flags&(CGrowX1|CGrowY1|CGrowX2|CGrowX2));
	//tcontrol2->flags=CGrowX1|CGrowX2|CNoColorChangeButton(0)|CVScrollUp|CNoTab;
	tcontrol2->flags=CGrowX1|CGrowX2|CNoColorChangeButton(0)|CVScrollUp|CNoTab;
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}
	//tcontrol2->gx1=-40;//tcontrol2->x1-twin->w;
	//tcontrol2->gx2=-24;//tcontrol2->x2-twin->w;
	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=black;
	//tcontrol2->win=twin->num;
	strcpy(path,PStatus.ProjectPath);  //may not need
#if Linux
	strcat(path,"images/arrowup.bmp");
#endif
#if WIN32
	strcat(path,"images\\arrowup.bmp");
#endif

	//strcat(path,"images/folder.bmp");
	tcontrol2->image[0]=FT_LoadBitmapFileAndScale(path,tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	//tcontrol2->image[0]=FT_LoadBitmap(path);
	tcontrol2->bmpinfo[0]=(BITMAPINFOHEADER *)(tcontrol2->image[0]+sizeof(BITMAPFILEHEADER));
	tcontrol2->bmpdata[0]=((unsigned char *)tcontrol2->bmpinfo[0]+sizeof(BITMAPINFOHEADER));

#if Linux
	if (tcontrol2->image[0]!=0) {
		tcontrol2->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol2->bmpdata[0],tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,32,(tcontrol2->x2-tcontrol2->x1)*4);
		//tcontrol2->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol2->bmpdata[0],tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,32,(tcontrol2->x2-tcontrol2->x1)*4);
		if (tcontrol2->ximage[0]<=0) {
			fprintf(stderr,"XCreateImage failed for image[0] %s on control %s\n",path,tcontrol2->name);
		} 
	} else {
		fprintf(stderr,"could not load %s for control %s\n",path,tcontrol2->name);
	}
#endif
#if WIN32
	//tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->bmpinfo[0]->biWidth,tcontrol2->bmpinfo[0]->biHeight,1,32,tcontrol2->bmpdata[0]);
	tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,1,32,tcontrol2->bmpdata[0]);
#endif

	//tcontrol2->image[0]=FT_LoadBitmap("images/gray.bmp",tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_VScrollUp=AddFTControl(twin,tcontrol2);

	//scroll down arrow
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixVScrollDown,tcontrol->name);
	//sprintf(tstr,"ScrollDown_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
#endif
	tcontrol2->type=CTButton; 
	tcontrol2->x1=tcontrol->x2;
	tcontrol2->y1=tcontrol->y2-16;
	tcontrol2->x2=tcontrol2->x1+16;
	tcontrol2->y2=tcontrol->y2;
	tcontrol2->z=tcontrol->z-1; //above other controls
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)Scroll_DownButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)Scroll_DownButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)Scroll_DownButton;  //double click can also scroll
	strcpy(path,PStatus.ProjectPath);
#if Linux
	strcat(path,"images/arrowdown.bmp");
#endif
#if WIN32
	strcat(path,"images\\arrowdown.bmp");
#endif

	//strcat(path,"images/folder.bmp");
	tcontrol2->image[0]=FT_LoadBitmapFileAndScale(path,tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	tcontrol2->bmpinfo[0]=(BITMAPINFOHEADER *)(tcontrol2->image[0]+sizeof(BITMAPFILEHEADER));
	tcontrol2->bmpdata[0]=((unsigned char *)tcontrol2->bmpinfo[0]+sizeof(BITMAPINFOHEADER));

#if Linux
	if (tcontrol2->image[0]!=0) {
		tcontrol2->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol2->bmpdata[0],tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,32,(tcontrol2->x2-tcontrol2->x1)*4);
	}
#endif
#if WIN32
	//tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->bmpinfo[0]->biWidth,tcontrol2->bmpinfo[0]->biHeight,1,32,tcontrol2->bmpdata[0]);
	tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,1,32,tcontrol2->bmpdata[0]);
#endif


	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//50000;//1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//100000;//1000=1ms
	//tcontrol2->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollDown;
	//tcontrol2->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CNoColorChangeButton(0)|CVScrollDown|CNoTab;
	tcontrol2->flags=CNoColorChangeButton(0)|CVScrollDown|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}
	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=black;
	//tcontrol2->win=twin->num;
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_VScrollDown=AddFTControl(twin,tcontrol2);


	//vertical back button
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
	//sprintf(tstr,"ScrollBack_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x2;
	tcontrol2->y1=tcontrol->y1+16;  //should get from scroll up
	tcontrol2->x2=tcontrol2->x1+16;
	tcontrol2->y2=tcontrol->y2-16; //should get from scroll down
	tcontrol2->z=tcontrol->z; //scrollbar back button is below slide and directional buttons
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)VScroll_BackButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)VScroll_BackButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)VScroll_BackButton;  //double click can also scroll
	//tcontrol2->tab=-1;
	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//1000=1ms
	//tcontrol2->flags=C2D|CGrowX1|CGrowX2|CGrowY2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScroll;
	//tcontrol2->flags=C2D|CGrowX1|CGrowX2|CGrowY2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollBack|CNoTab;
	tcontrol2->flags=C2D|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollBack|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}
	//tcontrol2->gx1=-40;
	//tcontrol2->gx2=-24;
	//tcontrol2->gy2=-86;
	tcontrol2->color[0]=gray;
	tcontrol2->color[1]=dkgray;
	//tcontrol2->win=twin->num;
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_VScrollBack=AddFTControl(twin,tcontrol2);


	//vertical slide button
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
	//sprintf(tstr,"ScrollSlide_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x2;
	tcontrol2->y1=tcontrol->y1+16;  //should get from scroll 
	tcontrol2->x2=tcontrol2->x1+16;
	tcontrol2->y2=tcontrol->y2-16;  //should get from scroll 
	tcontrol2->z=tcontrol->z-1; //above other controls
	//tcontrol2->MouseMove=(FTControlfuncwcxy *)VScrollSlide_MouseMove;//this click has some internal processing 
	//tcontrol2->flags=CGrowX1|CGrowX2|CGrowY2|CNoColorChangeButton(0)|CVScrollSlide|CNoTab;  
	tcontrol2->flags=CNoColorChangeButton(0)|CVScrollSlide|CNoTab;  
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}
	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=ltgray2; //buttondown
	tcontrol2->color[2]=ltgray2; //mouseover
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_VScrollSlide=AddFTControl(twin,tcontrol2);
} //CVScroll

if (tcontrol->flags&CHScroll) { 


	if (PStatus.flags&PInfo) {
			fprintf(stderr,"Add horizontal scroll\n");
		}

	//bit CHScroll will auto make a new scroll bar (4 FTControls)
	//A scrollbar is simply 4 FTControls
	//horizontal:
	//_scr_left_controlname=(left) scroll left button
	//_scr_right_controlname=(right) scroll right button
	//_scr_hslide_controlname=middle slider button
	//_scr_hback_controlname=middle background button

	//(left) scroll left arrow
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixHScrollLeft,tcontrol->name);
	//sprintf(tstr,"ScrollUp_%s",tcontrol->name);
	//strcpy(tcontrol2->name,"s1_foOpenFile2");  
	strcpy(tcontrol2->name,tstr);  
#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x1;
	tcontrol2->y1=tcontrol->y2;
	tcontrol2->x2=tcontrol2->x1+16;
	tcontrol2->y2=tcontrol2->y1+16;
	tcontrol2->z=tcontrol->z-1; //above other controls
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)Scroll_LeftButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)Scroll_LeftButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)Scroll_LeftButton;  //double click can also scroll
	//tcontrol2->tab=-1;
	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//50ms 1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//300ms 1000=1ms
	//tcontrol2->flags=CGrowX1|CGrowX2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollUp;
	tcontrol2->flags=CNoChangeOnMouseOver|CNoColorChangeButton(0)|CHScrollLeft|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=black;
	strcpy(path,PStatus.ProjectPath);  //may not need
	#if Linux
	strcat(path,"images/arrowleft.bmp");
	#endif
	#if WIN32
	strcat(path,"images\\arrowleft.bmp");
	#endif

	tcontrol2->image[0]=FT_LoadBitmapFileAndScale(path,tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	tcontrol2->bmpinfo[0]=(BITMAPINFOHEADER *)(tcontrol2->image[0]+sizeof(BITMAPFILEHEADER));
	tcontrol2->bmpdata[0]=((unsigned char *)tcontrol2->bmpinfo[0]+sizeof(BITMAPINFOHEADER));

	#if Linux
	if (tcontrol2->image[0]!=0) {
		tcontrol2->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol2->image[0]+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,32,(tcontrol2->x2-tcontrol2->x1)*4);
	}
	#endif
	#if WIN32
	//tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->bmpinfo[0]->biWidth,tcontrol2->bmpinfo[0]->biHeight,1,32,tcontrol2->bmpdata[0]);
	tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,1,32,tcontrol2->bmpdata[0]);
	#endif

	//tcontrol2->image[0]=FT_LoadBitmap("images/gray.bmp",tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_HScrollLeft=AddFTControl(twin,tcontrol2);

	//(right) scroll right arrow
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixHScrollRight,tcontrol->name);
	//sprintf(tstr,"ScrollDown_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
	#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
	#endif
	tcontrol2->type=CTButton; 
	tcontrol2->x1=tcontrol->x2-16;
	tcontrol2->y1=tcontrol->y2;
	tcontrol2->x2=tcontrol->x2;
	tcontrol2->y2=tcontrol->y2+16;
	tcontrol2->z=tcontrol->z-1; //above other controls
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)Scroll_RightButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)Scroll_RightButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)Scroll_RightButton;  //double click can also scroll
	//tcontrol2->tab=-1;
	strcpy(path,PStatus.ProjectPath);
	#if Linux
	strcat(path,"images/arrowright.bmp");
	#endif
	#if WIN32
	strcat(path,"images\\arrowright.bmp");
	#endif
	tcontrol2->image[0]=FT_LoadBitmapFileAndScale(path,tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1);
	tcontrol2->bmpinfo[0]=(BITMAPINFOHEADER *)(tcontrol2->image[0]+sizeof(BITMAPFILEHEADER));
	tcontrol2->bmpdata[0]=((unsigned char *)tcontrol2->bmpinfo[0]+sizeof(BITMAPINFOHEADER));
	#if Linux
	if (tcontrol2->image[0]!=0) {
		tcontrol2->ximage[0]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)tcontrol2->image[0]+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,32,(tcontrol2->x2-tcontrol2->x1)*4);
	}
	#endif
	#if WIN32
	//tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->bmpinfo[0]->biWidth,tcontrol2->bmpinfo[0]->biHeight,1,32,tcontrol2->bmpdata[0]);
	tcontrol2->hbmp[0]= CreateBitmap(tcontrol2->x2-tcontrol2->x1,tcontrol2->y2-tcontrol2->y1,1,32,tcontrol2->bmpdata[0]);
	#endif

	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//50000;//1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//100000;//1000=1ms
	//tcontrol2->flags=CGrowX1|CGrowY1|CGrowX2|CGrowY2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CVScrollDown;
	tcontrol2->flags=CNoColorChangeButton(0)|CHScrollRight|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}

	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=black;
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_HScrollRight=AddFTControl(twin,tcontrol2);

	//scroll background button
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixHScrollBack,tcontrol->name);
	//sprintf(tstr,"ScrollBack_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
	#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
	#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x1+16;
	tcontrol2->y1=tcontrol->y2;  //should get from scroll up
	tcontrol2->x2=tcontrol->x2-16;
	tcontrol2->y2=tcontrol2->y1+16; //should get from scroll down
	tcontrol2->z=tcontrol->z; //above scrollbar back button
	//tcontrol2->ButtonDownHold[0]=(FTControlfuncwcxy *)HScroll_BackButton;
	//tcontrol2->ButtonDownUp[0]=(FTControlfuncwcxy *)HScroll_BackButton;
	//tcontrol2->ButtonDoubleClick[0]=(FTControlfuncwcxy *)HScroll_BackButton;  //double click can also scroll
	//tcontrol2->tab=-1;
	tcontrol2->interval[0].tv_sec=0;
	tcontrol2->interval[0].tv_usec=50000;//1000=1ms
	tcontrol2->delay[0].tv_sec=0;
	tcontrol2->delay[0].tv_usec=500000;//1000=1ms
	//tcontrol2->flags=C2D|CGrowX2|CGrowY1|CGrowY2|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CHScrollBack|CNoTab;
	tcontrol2->flags=C2D|CNoChangeOnMouseOver|CNoColorChangeButton(0)|CHScrollBack|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX2;
	}

	tcontrol2->color[0]=gray;
	tcontrol2->color[1]=dkgray;
	//tcontrol2->win=twin->num;
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_HScrollBack=AddFTControl(twin,tcontrol2);

	//scroll slide button
	memset(tcontrol2,0,sizeof(FTControl));
	sprintf(tstr,"%s%s",PStatus.PrefixHScrollSlide,tcontrol->name);
	//sprintf(tstr,"ScrollSlide_%s",tcontrol->name);
	strcpy(tcontrol2->name,tstr);  
	#if Linux
	tcontrol2->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol2->font==0) fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol2->name);
	#endif
	tcontrol2->type=CTButton;
	tcontrol2->x1=tcontrol->x1+16;
	tcontrol2->y1=tcontrol->y2;  //should get from scroll 
	tcontrol2->x2=tcontrol->x2-16;
	tcontrol2->y2=tcontrol2->y1+16;  //should get from scroll 
	tcontrol2->z=tcontrol->z-1; //above other controls
	//tcontrol2->MouseMove=(FTControlfuncwcxy *)HScrollSlide_MouseMove;//this click has some internal processing 
	//tcontrol2->flags=CGrowY1|CGrowX2|CGrowY2|CNoColorChangeButton(0)|CHScrollSlide;  
	tcontrol2->flags=CNoColorChangeButton(0)|CHScrollSlide;  
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX2;
	}
	tcontrol2->color[0]=ltgray;
	tcontrol2->color[1]=ltgray2; //buttondown
	tcontrol2->color[2]=ltgray2; //mouseover
	tcontrol2->parent=tcontrol;
	//copy a pointer to this scrollbar button to the frame control
	tcontrol->FTC_HScrollSlide=AddFTControl(twin,tcontrol2);
} //CHScrollbar


if (tcontrol2!=0) {
	free(tcontrol2);
}	

if (PStatus.flags&PInfo) {
	fprintf(stderr,"End AddScrollFTControls\n");
}

}  //AddScrollFTControls


//x is added/subtracted in resize code?
//this just adjusts/boundary checks
void btnfoName_OnResize(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2,*tcontrol3,*tcontrolf;
//FTItem *tFTItem;
int bw;
char tstr[FTMedStr];

//clean up these funcitons=
//go thru each FTItem in a while
//make a loop for number of FTItems listed
if (PStatus.flags&PInfo) 
	fprintf(stderr,"Resize Name\n");


tcontrolf=tcontrol->parent;  //fileopen
if (tcontrolf!=0) {
  if (tcontrol!=0) { //has name
    tcontrol->x3=tcontrol->x2-tcontrol->x1; //this FTControl is being resized, store new size 
    if (tcontrol->x1>tcontrolf->x2) tcontrol->x1=tcontrolf->x2;  //if resize goes over fileopen FTControl
    if (tcontrol->x2>tcontrolf->x2) tcontrol->x2=tcontrolf->x2;
    if (tcontrol->x1<tcontrolf->x1) tcontrol->x1=tcontrolf->x1;
    if (tcontrol->x2<tcontrolf->x1) tcontrol->x2=tcontrolf->x1;
  }  //has name

//  if (tcontrolf->flags&CFOSize) { //has Size
    sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrolf->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
      bw=tcontrol2->x3;  //original size
      tcontrol2->x1=tcontrol->x2; //adjust button 2
      if (tcontrol2->x1+bw<tcontrolf->x2) tcontrol2->x2=tcontrol2->x1+bw;
      else tcontrol2->x2=tcontrolf->x2;

//  if (tcontrolf->flags&CFODate) { //has Date
    sprintf(tstr,"%s%s",PStatus.PrefixDate,tcontrolf->name);
    tcontrol3=GetFTControl(tstr);
    if (tcontrol3!=0) {
      bw=tcontrol3->x3; //original size
      //bw=tcontrol4->x2-tcontrol4->x1;  //original size
      tcontrol3->x1=tcontrol2->x2; //adjust button 3
      if (tcontrol3->x1+bw<tcontrolf->x2) tcontrol3->x2=tcontrol3->x1+bw;
      else tcontrol3->x2=tcontrolf->x2;
      if (tcontrol3->x2<tcontrolf->x2) tcontrol3->x2=tcontrolf->x2;
    }  //tcontrol3!=0

  }  //tcontrol2!=0

DrawFTControl(tcontrolf);  //fileopen will draw title/size/date FTControls
}  //tcontrolf!=0 //fileopen exists

}//end btnfoName_OnResize

void btnfoSize_OnResize(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2,*tcontrol3,*tcontrolf;
//FTItem *tFTItem;
int fw;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) 
	fprintf(stderr,"btnfoSize_OnResize\n");

if (tcontrol!=0) {
  tcontrolf=tcontrol->parent;
#if Linux
  fw=tcontrol->font->max_bounds.width; //was ascent
#endif
#if WIN32
   fw=tcontrol->fontwidth;
#endif

  
  //sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrolf->name);
	sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrolf->name);
  tcontrol2=GetFTControl(tstr);  //get button 1 (name)  


  sprintf(tstr,"%s%s",PStatus.PrefixDate,tcontrolf->name);
  tcontrol3=GetFTControl(tstr);  //get button 3 (date)


  if (tcontrol2!=0) { 
    if (tcontrol->x1>tcontrolf->x2) tcontrol->x1=tcontrolf->x2;  
    if (tcontrol->x2>tcontrolf->x2) tcontrol->x2=tcontrolf->x2;
    if (tcontrol->x1<tcontrol2->x1+fw) tcontrol->x1=tcontrol2->x1+fw;  
    if (tcontrol->x2<tcontrolf->x1) tcontrol->x2=tcontrolf->x1;
    tcontrol->x3=tcontrol->x2-tcontrol->x1; //this FTControl is being resized, store new size 
    //bw=tcontrol->x3;  //original size

    if (PStatus.flags&EResizeX1) { //resizing left
//              fprintf(stderr,"2=%s\n",tcontrol3->name); 
       tcontrol2->x2=tcontrol->x1; //adjust button 1
//       if (tcontrol->x1+bw<tcontrolf->x2) tcontrol->x2=tcontrol->x1+bw;
       if (tcontrol->x1<tcontrol2->x1+fw) tcontrol->x1=tcontrol2->x2+fw;
//       else tcontrol->x2=tcontrolf->x2;
       if (tcontrol->x1>tcontrol->x2-fw) tcontrol->x1=tcontrol->x2-fw;
       } 
    else { //resizing right
      if (tcontrol->x2<tcontrol->x1+fw) tcontrol->x2=tcontrol->x1+fw;
      if (tcontrol->x2>tcontrolf->x2-fw) tcontrol->x2=tcontrolf->x2-fw;
    }



//              fprintf(stderr,"3=%s\n",tcontrol4->name); 
    if (tcontrol3!=0) {
      //bw=tcontrol3->x3; //original size
      tcontrol3->x1=tcontrol->x2; //adjust button 3
//      if (tcontrol3->x1+bw<tcontrolf->x2) tcontrol3->x2=tcontrol3->x1+bw;
      if (tcontrol3->x1+fw<tcontrolf->x2) tcontrol3->x2=tcontrol3->x1+fw;
      else tcontrol3->x2=tcontrolf->x2;
      if (tcontrol3->x2<tcontrolf->x2) tcontrol3->x2=tcontrolf->x2;
    }  //tcontrol3!=0
  }  //tcontrol2!=0
DrawFTControl(tcontrolf);
}  //tcontrol!=0



}//end btnfoSize_OnResize


void btnfoDate_OnResize(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol2,*tcontrolf;
//FTItem *tFTItem;
int fw;
char tstr[FTMedStr];


if (tcontrol!=0) {
  tcontrolf=tcontrol->parent;
#if Linux
  fw=tcontrol->font->max_bounds.width;//was ascent;
#endif
#if WIN32
  fw=tcontrol->fontwidth;
#endif

  
//  sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrolf->name);
//  tcontrol2=GetFTControl(tstr);  //get button 1 (name)  


  sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrolf->name);
  tcontrol2=GetFTControl(tstr);  //get button 2 (size)


  if (tcontrol2!=0) { 
    if (tcontrol->x1>tcontrolf->x2-fw) tcontrol->x1=tcontrolf->x2-fw;  
    if (tcontrol->x1<tcontrol2->x1+fw) tcontrol->x1=tcontrol2->x1+fw;  

    tcontrol->x3=tcontrol->x2-tcontrol->x1; //this FTControl is being resized, store new size 
    //bw=tcontrol->x3;  //original size

//can only be resizing left

       tcontrol2->x2=tcontrol->x1; //adjust button 2

//       if (tcontrol->x1+bw<tcontrolf->x2) tcontrol->x2=tcontrol->x1+bw;
//       else tcontrol->x2=tcontrolf->x2;
//       if (tcontrol->x1>tcontrol->x2-fw) tcontrol->x1=tcontrol->x2-fw;


  }  //tcontrol2!=0
DrawFTControl(tcontrolf);
}  //tcontrol!=0



}//end btnfoDate_OnResize


void btnfoName_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
//[1]=field to sort by, [2]=direction
#if PINFO
fprintf(stderr,"in name sort\n");
#endif
//tcontrol=GetFTControl("foOpenFile2");
tcontrol=tcontrol->parent;
if (tcontrol->StartSelectedTextInChars&0x1) tcontrol->StartSelectedTextInChars&=~0x1;  //change direction of sort
else tcontrol->StartSelectedTextInChars|=0x1;
tcontrol->FirstCharacterShowing=0; //Sort by Name
if (tcontrol->ilist!=0)  tcontrol->ilist->flags|=FTIL_REREAD;
DrawFTControl(tcontrol);
#if PINFO
fprintf(stderr,"after name sort %d\n",tcontrol->StartSelectedTextInChars);
#endif
} //btnfoName_DownUp

void btnfoSize_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
//tcontrol=GetFTControl("foOpenFile2");
tcontrol=tcontrol->parent;
if (tcontrol->StartSelectedTextInChars&0x2) tcontrol->StartSelectedTextInChars&=~0x2;  //change direction of sort
else tcontrol->StartSelectedTextInChars|=0x2;
tcontrol->FirstCharacterShowing=1;  //Sort by Size
if (tcontrol->ilist!=0)  tcontrol->ilist->flags|=FTIL_REREAD;
DrawFTControl(tcontrol);
} //btnfoName_DownUp

void btnfoDate_DownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
//tcontrol=GetFTControl("foOpenFile2");
tcontrol=tcontrol->parent;
if (tcontrol->StartSelectedTextInChars&0x4) tcontrol->StartSelectedTextInChars&=~0x4;  //change direction of sort
else tcontrol->StartSelectedTextInChars|=0x4;
tcontrol->FirstCharacterShowing=2;  //Sort by Date
if (tcontrol->ilist!=0)  tcontrol->ilist->flags|=FTIL_REREAD;
DrawFTControl(tcontrol);
} //btnfoName_DownUp



//TODO: Now that ->last is implemented (controls are double-linked lists) clean up and make more simple - no need to pass as pointer? - may still want to - to make sure control=0 when returned
//Called from DestroyFTWindowsN
//void DelFTControlAll(FTControl **tcontrol)
//DelFTControls
void DelFTControlAll(FTWindow *twin)
{
//FTControl **nFTControl; 
FTControl *tcontrol;//,*ncontrol; 
//FTWindow *twin2;

	if (PStatus.flags&PInfo) {
		fprintf(stderr,"Start DelFTControlAll\n");
	}

//tcontrol is the icontrol (first FTControl) for a window

//have to delete this way because list can be changed by sub FTControls like scroll bar FTControls
//they get deleted with the fileopen FTControl 
//and simply going down the icontrol list would contain broken links
//this way the FTControl list is constantly shifted up to icontrol
//so scroll FTControls may get deleted, but then when delFTControl looks for subFTControls they 
//will already be deleted

if (twin!=0) {
	//go through all windows controls and delete them
	while(twin->icontrol!=0) {
		tcontrol=twin->icontrol;
//start fresh for each control - because an individual control may delete others
		DelFTControl(tcontrol);
	} //while
} //twin!=0

	if (PStatus.flags&PInfo)
		fprintf(stderr,"End DelFTControlAll\n");

return;
}  //end DelFTControlAll

void DelFTControl(FTControl *tcontrol)
{
FTWindow *twin;//,*twin2;
FTControl *lcontrol,*last,*dcontrol,*tc;
//int datainuse;

if (tcontrol!=0) {

	if (PStatus.flags&PInfo) {
		fprintf(stderr,"DelFTControl %s\n",tcontrol->name);
	}

	//  fprintf(stderr,"tcontrol %p\n",tcontrol);
	//  fprintf(stderr,"*tcontrol %p\n",*tcontrol);
	//  fprintf(stderr,"(*tcontrol)->next=%p\n",(*tcontrol)->next);

	//scroll bar FTControls attached to fileopen/Dropdown 
	//are automatically deleted when window is closed
	//potentially a fileopen FTControl could be deleted without the window being deleted

	dcontrol=tcontrol;  //icontrol=twin->icontrol

	if (dcontrol!=0) {

		if (PStatus.MouseOverFTControl==dcontrol) {
			PStatus.MouseOverFTControl=0;
		}

		if (PStatus.ExclusiveFTControl==dcontrol) {
			PStatus.ExclusiveFTControl=0;
		}

	//if this control has a vertical or horizontal scroll bar control attached - delete those controls too
//if only deleting one control
		if ((dcontrol->flags&CVScroll) || (dcontrol->flags&CHScroll)) {

			if (PStatus.flags&PInfo) {
				fprintf(stderr,"Deleting Scroll controls for %s\n",tcontrol->name);
			}

		 DelScrollFTControls(tcontrol);  //it's ok that this function calls FTDelControl because the scroll control does not have CVScroll or CHScroll set.
		}

	//if this control has a a filename, size, or date button attached - delete those controls too
		if ((dcontrol->flags&CFOName) || (dcontrol->flags&CFOSize) || (dcontrol->flags&CFODate)) {
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"Deleting File Open controls for %s\n",tcontrol->name);
			}

		  DelFileOpenFTControls(tcontrol);
		}


	//#if 0 
	//have to go through all FTControls and connect the previous FTControl to the next FTControl
	//if FTControls were double linked list (next and last) then this would not be necessary
	//but if this is the first control - we would have to get the window to set icontrol=0

	//twin=GetFTWindowN(dcontrol->win);
	twin=dcontrol->window;

	if (twin==0) {
		return;
	}

	//fprintf(stderr,"twin->icontrol=%p\n",twin->icontrol);
	lcontrol=twin->icontrol;
	last=lcontrol;
	while(lcontrol!=0) {
	//fprintf(stderr,"FTControl %s\n",lFTControl->name);
	if (lcontrol==dcontrol) {  //this is the control we need to delete

//		if (PStatus.flags&PInfo)
//		  fprintf(stderr,"Deleting FTControl %s\n",dcontrol->name);

	//    fprintf(stderr,"twin->icontrol=%p\n",twin->icontrol);
	//    fprintf(stderr,"lFTControl=%p\n",lFTControl);
	//    fprintf(stderr,"dFTControl=%p\n",dFTControl);

//		fprintf(stderr,"found control to delete\n");
//		fprintf(stderr,"last is %s\n",last->name);
//		fprintf(stderr,"Moving last %s next pointer\n",last->name);
//		fprintf(stderr,"Moving last %s next pointer=%p\n",last->name,lcontrol->next);
	//if this is the first control - set window first control to next control
		if (twin->icontrol==lcontrol) {
			twin->icontrol=lcontrol->next;
			lcontrol->last=0;		
		}	else {
			if (last->next!=0){
				last->next=lcontrol->next;  //remove tcontrol from FTControl list  
			}
			tc=lcontrol->next;
			if (tc!=0) {
				if (tc->last!=0 && last!=0) {
					tc->last=last->last;
				}
			} //tc!=0
		}


	//    fprintf(stderr,"Deleting2 FTControl %s\n",dFTControl->name);
	//    fprintf(stderr,"ilist=%p\n",dFTControl->ilist);
	//    fprintf(stderr,"ilist2=%p\n",dFTControl->ilist2);

		//free dynamically allocated FTItem lists
	//  if (tcontrol->ilist) FreeFTItemList(&dFTControl->ilist);
	//  if (tcontrol->ilist) FreeFTItemList(tcontrol->ilist);
		//Notice that DelFTItemList frees the filemenu and Dropdown control itemlist data structure
		//CloseFTWindow does not free the FTWindow structure for itemlist windows -
		//because they need to be preserved so people do not need to additems to Dropdown controls
		//each time they are clicked on and opened.
		if (dcontrol->ilist) {
			DelFTItemList(dcontrol->ilist);      
			dcontrol->ilist=0;
		}
	//tcontrol is being deleted - so don't worry about setting tcontrol->ilist=0 
	//  if (dFTControl->ilist2) FreeFTItemList(&dFTControl->ilist2);
		//fprintf(stderr,"tcontrol->ilist=%p\n",tcontrol->ilist);

	//    fprintf(stderr,"2twin->icontrol=%p\n",twin->icontrol);
	//    fprintf(stderr,"2lFTControl=%p\n",lFTControl);

		if (dcontrol->data!=0)
		  {
		  //fprintf(stderr,"tcontrol=%p tcontrol->data=%p tcontrol->data=%p\n",tcontrol,tcontrol->data,tcontrol->data);
		  //other data FTControls may be using this data
		  //need to search thru all other FTControls to see if this data is shared
		  //because a number of FTControls may use the same data
		  //data should be read by filename (and position pointer) off disk dynamically, not stored in RAM
		  //some data files are Gigabytes in size
		  //rewrite this

		  //for now only go thru window 0 
		 	//go thru each FTControl and see if data pointer is the same
	//    if (tcontrol->win==0) {//this is a delete of a special "track" FTControl, 
		                        //that may have the same data as other FTControls
		                        
	//      tcontrol2=PStatus.iwindow->icontrol;
		    //for now only check FTControls on this window for FTControls using the same data

//data that other controls use can be freed on this control
//each control needs to malloc its own data
//infact, probably users should free their own malloc'd data
#if 0 
		    datainuse=0;
		    twin2=PStatus.iwindow;
		    while (twin2!=0) {
		      tcontrol2=twin2->icontrol;
		     	while (tcontrol2!=0)  {
		     	  if (tcontrol2->data!=0 && tcontrol2->data==dcontrol->data) {
		         //fprintf(stderr,"Data still used by FTControl %s\n",tcontrol2->name);
		         datainuse=1;  //do not delete data a different FTControl is using the address/pointer of
		        }
		        tcontrol2=tcontrol2->next;
		       }  //end while tcontrol2!=0
		      twin2=twin2->next;
		    }  //end while twin2!=0
#endif
		     //can free data no other FTControls are using it
//			   if (!datainuse) {

//removed - user shuold be responsible for mallocing and freeing data on a data control
#if 0 				
		         //fprintf(stderr,"free tcontrol->data %p",tcontrol->data);
							if (dcontrol->data!=0) {
			         free(dcontrol->data);
			         dcontrol->data=0;
							}
#endif
//perhaps we should not even close data file handle but leave that for the user to do
							//and close the data file handle
							if (dcontrol->flags2&CDataFileOpen) {
								if (dcontrol->datafptr!=0) {
									fclose(dcontrol->datafptr);
									dcontrol->datafptr=0;
								}
								dcontrol->flags2&=~CDataFileOpen;
							} //CDataFileOpen
//		     }//		   if (!datainuse) {
		//		fprintf(stderr,"About to delete DataLabelFTControl\n");
				//Free any connected DataLabel control
				//and only deleting one control - otherwise will be deleted using ->next
				if (dcontrol->DataLabelFTControl!=0) {
					//may need to find control with next pointing to dcontrol->DataLabelFTControl and set = 0
					DelFTControl(dcontrol->DataLabelFTControl);
					dcontrol->DataLabelFTControl=0;
				} //DataLabelFTControl

		     }  //end if tcontrol->data!=0 (FTControl has data)

		 
	//    fprintf(stderr,"3twin->icontrol=%p\n",twin->icontrol);
	//    fprintf(stderr,"3lFTControl=%p\n",lFTControl);


		      if(dcontrol->image[0]!=0) {  //when something is freed it may not == 0 after
							//doesn't matter because control is getting deleted anyway
//#if WIN32
				  //free(dcontrol->image[0]);
					//dcontrol->image[0]=0;
//#endif
#if WIN32
				  if (dcontrol->hbmp[0]!=0) {
					  DeleteObject(dcontrol->hbmp[0]);
				  }
#endif
		      }

		      if(dcontrol->image[1]!=0) {  //when something is freed it may not == 0 after
				  //free(dcontrol->image[1]);
					//dcontrol->image[1]=0;
#if WIN32
				  if (dcontrol->hbmp[1]!=0) {
					  DeleteObject(dcontrol->hbmp[1]);
				  }
#endif

		      }

		      if(dcontrol->image[2]!=0) {  //when something is freed it may not == 0 after
				  //free(dcontrol->image[1]);
					//dcontrol->image[1]=0;
#if WIN32
				  if (dcontrol->hbmp[2]!=0) {
					  DeleteObject(dcontrol->hbmp[2]);
				  }
#endif

		      }


		      if(dcontrol->erase!=0) {  //when something is freed it may not == 0 after
//#if WIN32
				  //free(dcontrol->erase);
					//dcontrol->erase=0;
//#endif
		         //(*tcontrol)->erase=0;
		      }

#if Linux
		      if(dcontrol->ximage[0]!=0) {  //when something is freed it may not == 0 after
//for now		         XDestroyImage(dcontrol->ximage[0]);
						dcontrol->ximage[0]=0;
					} //if(dcontrol->ximage[0]!=0)
		      if(dcontrol->ximage[1]!=0) {  //when something is freed it may not == 0 after
//for now	         XDestroyImage(dcontrol->ximage[1]);
						dcontrol->ximage[1]=0;
					} //if(dcontrol->ximage[1]!=0)
		      if(dcontrol->xerase!=0) {  //when something is freed it may not == 0 after
//	for now	         XDestroyImage(dcontrol->xerase);
						dcontrol->xerase=0;
					} //if(dcontrol->xerase!=0)
#endif
			
			  //bmpinfo[0] is not freed- it's just a pointer not mallocd the image has the mallocd pointer, and then to the entire bmp structure
		      //if(dcontrol->bmpinfo[0]!=0) {  
		         //free(dcontrol->bmpinfo[0]);
		         //dcontrol->bmpinfo[0]=0;
		      //}

						//If this control had the window focus, remove it
					if (twin->FocusFTControl==dcontrol) {
						twin->FocusFTControl=0;
					} //FocusFTControl

	//        if (lFTControl==tcontrol && tcontrol->next==0) 
	//set the pointer from the list FTItem before to the next FTItem
	//        fprintf(stderr,"lFTControl=%p tcontrol=%p\n",lFTControl,tcontrol);
	//        fprintf(stderr,"last->name=%s\n",last->name);

	//twin->icontrol
		      
		     //get next pointer before delete FTControl
	//        last->next=dFTControl->next;  

	//        fprintf(stderr,"last->next=%p\n",last->next);

//	        fprintf(stderr,"free dcontrol %p %s\n",dcontrol,dcontrol->name);
		      free(dcontrol);  //no tcontrol was mallocd in addFTControl
		      dcontrol=0; //doesn't matter - but calling program will have to know that control was deleted and address not valid anymore even if not 0 - just less confusing not to have to pass pointer
		      if (PStatus.flags&PInfo) {
		        fprintf(stderr,"FTControl deleted\n");
					}

		      return;
	} //found match

	last=lcontrol;
//	fprintf(stderr,"Going to next control to delete: lcontrol=%p\n",lcontrol);

	if (lcontrol!=0) {  //lcontrol could =0 if it was a deleted scroll bar control
		lcontrol=lcontrol->next;
//	fprintf(stderr,"Going to next control to delete: lcontrol->next=%p %s\n",lcontrol,lcontrol->name);
	} //lcontrol!=0
	} //end while

	fprintf(stderr,"Could not find FTControl to delete\n");
	} //if tcontrol!=0
} //*tcontrol!=0

//#endif
return;
}  //end DelFTControl



//not used anymore because it caused an infinite recursion- delFTControl calls this function
void DelFileOpenFTControls(FTControl *tcontrol)
{
char tstr[FTMedStr];
FTControl *tcontrol2;


if (tcontrol!=0) {

  if (tcontrol->flags&CFOName) {
    sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
			DelFTControl(tcontrol2);
			//find the control that pointed to it, and set to next
			//better to have a double-linked list
		}
    tcontrol->flags&=~CFOName; //remove name flag from this control
  }

  if (tcontrol->flags&CFOSize) {
    sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) DelFTControl(tcontrol2);
    tcontrol->flags&=~CFOSize;
  }

  if (tcontrol->flags&CFODate) {
    sprintf(tstr,"%s%s",PStatus.PrefixDate,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
			DelFTControl(tcontrol2);
		}
    tcontrol->flags&=~CFODate;
  }

}  //tcontrol!=0

}  //DelFileOpenFTControls



//tph- this function is no longer used- it created an infinite recursion
void DelScrollFTControls(FTControl *tcontrol)
{
//char tstr[FTMedStr];
//FTControl *tcontrol2;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"DelScrollFTControls\n");
} 

if (tcontrol!=0) {
  if (tcontrol->flags&CVScroll) {
    
    if (tcontrol->FTC_VScrollUp!=0) {
				DelFTControl(tcontrol->FTC_VScrollUp);
				tcontrol->FTC_VScrollUp=0;
		}

    if (tcontrol->FTC_VScrollDown!=0) {
				DelFTControl(tcontrol->FTC_VScrollDown);
				tcontrol->FTC_VScrollDown=0;
		}

    if (tcontrol->FTC_VScrollBack!=0) {
				DelFTControl(tcontrol->FTC_VScrollBack);
				tcontrol->FTC_VScrollBack=0;
		}

    if (tcontrol->FTC_VScrollSlide!=0) {
				DelFTControl(tcontrol->FTC_VScrollSlide);
				tcontrol->FTC_VScrollSlide=0;
		}

		/*
		//sprintf(tstr,"%s%s",PStatus.PrefixVScrollUp,tcontrol->name);
    //tcontrol2=GetFTControl(tstr);
    //if (tcontrol2!=0) {
	//		DelFTControl(tcontrol2);
	//	}

    sprintf(tstr,"%s%s",PStatus.PrefixVScrollDown,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
			DelFTControl(tcontrol2);
		}

    sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
			DelFTControl(tcontrol2);
		}

    sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
    tcontrol2=GetFTControl(tstr);
    if (tcontrol2!=0) {
			DelFTControl(tcontrol2);
		}
*/

   tcontrol->flags&=~CVScroll;
   }  //VScroll
}  //tcontrol!=0

if (PStatus.flags&PInfo) {
	fprintf(stderr,"End DelScrollFTControls\n");
} 


}  //DelScrollFTControls

int SetDefaultFontForFTControl(FTControl *tcontrol)
{
#if WIN32
int result;
FTWindow *twin;
#endif

#if Linux
	tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol->font==0) {
		fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
	}

	tcontrol->fontwidth=tcontrol->font->max_bounds.width;
	tcontrol->fontheight=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
		//Choice:
		//1) Create any font here when adding control (need to create hwindow until opening)
		//2) Don't create any font until drawing control (don't need to create hwindow until opening)
		//I am currently choosing 1) - opening the hwindow with CreateFTWindow and then showing with OpenFTWindow - 
		//this choice is apparently not available - or I can't find it - for XWindows but this is not an issue 
		//because Font can be defined in XWindows without the Window needing to be created.
//LOGFONT logf;
		//tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,TEXT("Times New Roman"));
		tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		//tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,0,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,NULL);
		//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
		//memset(&logf,0,sizeof(LOGFONT));
		//ChooseFont()
		//tcontrol->font=CreateFontIndirect(&logf);
		if (tcontrol->font==0) {
			fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
			return(0);
		}
		twin=tcontrol->window;
		if (twin==0) {
			fprintf(stderr,"AddFTControl: FTControl %s doesn't have an FTWindow using the default window's font.\n",tcontrol->name);
			//this may occur if trying to add an itemlist to a control that hasn't been added yet
			//AddFTControl(twin,tcontrol) connects the FTWindow to the FTControl
			//could make AddFTItem(tcontrol,titem) to AddFTItem(twin,tcontrol,titem)- but that seems excessive since tcontrol should already be part of a twin
			//currently the control must be added before an itemlist can be added to it
			//return(0);
		}
		//the problem is that twin->hdcWin has not been defined yet since the window has not been created yet
		if (twin==0) {
			SelectObject(GetDC(0),tcontrol->font); 
			result=GetTextMetrics(GetDC(0),&tcontrol->textmet);
		} else {
			SelectObject(twin->hdcWin,tcontrol->font); 
		//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
		//tmAveCharWidth-     The average width of characters in the font (generally defined as the width of the letter x ). This value does not include the overhang required for bold or italic characters.
		//tmMaxCharWidth-    The width of the widest character in the font.
		//tph: but with fixed pitch (monospace), unless bold is selected it will always be regular
		
			result=GetTextMetrics(twin->hdcWin,&tcontrol->textmet);
		} //twin==0
		//tcontrol->fontwidth=tcontrol->textmet.tmMaxCharWidth;
		tcontrol->fontwidth=tcontrol->textmet.tmAveCharWidth; //using tmMaxCharWidth makes the text too small for labels- there is a large space at the end
		tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
return(1);		
} //int SetDefaultFontForFTControl(FTControl tcontrol)


int SetDefaultFontForFTWindow(FTWindow *twin)
{
#if WIN32
	int result;
#endif
#if Linux
	twin->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (twin->font==0) {
		fprintf(stderr,"Could not find font for FTWindow %s.\n",twin->name);
		return(0);
	}

	twin->fontwidth=twin->font->max_bounds.width;
	twin->fontheight=twin->font->max_bounds.ascent+twin->font->max_bounds.descent;
#endif //Linux
#if WIN32
			twin->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
			if (twin->font==0) {
				fprintf(stderr,"CreateFont failed\n");
				return(0);
			}
			//the problem is that twin->hdcWin has not been defined yet since the window has not been created yet
			SelectObject(twin->hdcWin,twin->font); 
			//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
			result=GetTextMetrics(twin->hdcWin,&twin->textmet);
			twin->fontwidth=twin->textmet.tmAveCharWidth;
			twin->fontheight=twin->textmet.tmHeight;

			SelectObject(twin->hdcWin,twin->font); 
			//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
			result=GetTextMetrics(twin->hdcWin,&twin->textmet);
			twin->fontwidth=twin->textmet.tmAveCharWidth;
			twin->fontheight=twin->textmet.tmHeight;
#endif //WIN32
return(1);
} //int SetDefaultFontForFTWindow(FTControl *twin)


//Unselect a FolderTab Control- sets CNotShowing, for all controls under it
int UnselectFolderTab(FTControl *tcontrol) 
{
	FTControl *tc;
	FTWindow *twin;

	if (tcontrol->type!=CTFolderTab) {
		fprintf(stderr,"Tried to UnselectFolderTab on control %s that is not type CTFolderTab\n",tcontrol->name);
		return(0);
	} else {
		//set all controls belonging to this FolderTab to CNotShowing
		twin=tcontrol->window;
		if (twin!=0) {
			tc=twin->icontrol;
			while(tc!=0) {
				if (tc->FolderTab!=0 && tc->FolderTab==tcontrol && tc!=tcontrol) { //control belongs to a Foldertab and belongs to the same FolderTab- don't unselect this control because the top level CTFolderTab doesn't get CnotShowing
					tc->flags|=CNotShowing; 
					if (tc->type==CTFolderTabGroup) {
						UnselectFolderTabGroup(tc); //will recursively set CNotShowing on any control on it's current FolderTab
					}
				} //if (tc->FolderTabGroup==tcontrol->FolderTabGroup) {
				tc=(FTControl *)tc->next;
			}//while 
		} else {
			fprintf(stderr,"SelectFolderTab window for FolderTab control %s ==0\n",tcontrol->name);
			return(0);
		}//twin!=0
		return(1);
	} //if (tcontrol->type!=CTFolderTab) {
} //int UnselectFolderTab(FTControl *tcontrol) {



//Select a FolderTab Control- sets/unsets CNotShowing, for all controls under it
int SelectFolderTab(FTControl *tcontrol) {
	FTControl *tc;
	FTWindow *twin;

	if (tcontrol->type!=CTFolderTab) {
		fprintf(stderr,"Tried to SelectFolderTab on control %s that is not type CTFolderTab\n",tcontrol->name);
		return(0);
	} else {
		if (tcontrol->FolderTabGroup!=0) {
			if (tcontrol->FolderTabGroup->CurrentFolderTab!=tcontrol) { //CurrentFolderTab is not this one
				//since we have to go through all the controls on this window anyway, there is no time saving only unselecting those currently showing
				tcontrol->FolderTabGroup->CurrentFolderTab=tcontrol;

				//set all controls belonging to this FolderTabGroup to CNotShowing except those on this FolderTab
				twin=tcontrol->window;
				if (twin!=0) {
					tc=twin->icontrol;
					while(tc!=0) {
						if (tc->FolderTabGroup==tcontrol->FolderTabGroup) { //control belongs to the same FolderTabGroup
							if (tc->FolderTab!=tcontrol) { //and a different FolderTab
								if (tc->type!=CTFolderTab) { //the actual foldertab buttons show
									if (tc->type==CTFolderTabGroup) {
										UnselectFolderTabGroup(tc); //will recursively set CNotShowing on any control on it's current FolderTab
									}
									tc->flags|=CNotShowing; //set any not belonging to this FolderTab
								} //if (tc->type!=CTFolderTab) { //the actual foldertab buttons show
							} else { //if (tc->FolderTab!=tcontrol->FolderTab) { 
								//same FolderTabGroup and the same FolderTab
								if (tc->type==CTFolderTabGroup) {
									SelectFolderTabGroup(tc); //recursively unset CNotShowing on any control on it's current FolderTab
								}
								tc->flags&=~CNotShowing; //clear any belonging to this FolderTab
							} //if (tc->FolderTab!=tcontrol->FolderTab) { //and a different FolderTab
						} //if (tc->FolderTabGroup==tcontrol->FolderTabGroup) {
						tc=(FTControl *)tc->next;
					}//while 
				} else {
					fprintf(stderr,"SelectFolderTab window for FolderTab control %s ==0\n",tcontrol->name);
					return(0);
				}//twin!=0
				return(1);
			} //if (tcontrol->FolderTabGroup->CurrentFolderTab!=tcontrol) { //CurrentFolderTab is not this one
		} else { //		if (tcontrol->FolderTabGroup!=0) {
			fprintf(stderr,"Tried to SelectFolderTab on FolderTab %s with no FolderTabGroup\n",tcontrol->name);
			return(0);
		} //		if (tcontrol->FolderTabGroup!=0) {
		return(1);
	} //if (tcontrol->type!=CTFolderTab) {
	
} //int SelectFolderTab(FTControl *tcontrol) {

int UnselectFolderTabGroup(FTControl *tcontrol)
{
	FTWindow *twin;
	FTControl *tc;

	if (tcontrol->type!=CTFolderTabGroup) {
		fprintf(stderr,"UnselectFolderTabGroup control %s is not a FolderTabGroup\n",tcontrol->name);
		return(0);
	} else {
		twin=tcontrol->window;
		if (twin!=0) {
			//unselect all controls on and under this FolderTabGroup
			tc=twin->icontrol;
			while(tc!=0) {
				if (tc->FolderTabGroup==tcontrol) {
					tc->flags|=CNotShowing;
					if (tc->type==CTFolderTabGroup) {
						UnselectFolderTabGroup(tc);
					}
				}
				tc=(FTControl *)tc->next;
			} //while
		} else {
			fprintf(stderr,"FTControl %s has no window\n",tcontrol->name);
			return(0);
		}
	} //	if (tcontrol->type!=CTFolderTabGroup) {
	return(1);
}//int UnselectFolderTabGroup(FTControl *tcontrol)

int SelectFolderTabGroup(FTControl *tcontrol)
{
	FTWindow *twin;
	FTControl *tc;

	if (tcontrol->type!=CTFolderTabGroup) {
		fprintf(stderr,"SelectFolderTabGroup control %s is not a FolderTabGroup\n",tcontrol->name);
		return(0);
	} else {
		twin=tcontrol->window;
		if (twin!=0) {
			//Select all controls on and under this FolderTabGroup, but only in the current FolderTabs
			tc=twin->icontrol;
			while(tc!=0) {
				if (tc->FolderTabGroup==tcontrol && (tc->FolderTab==tcontrol->CurrentFolderTab || tcontrol->CurrentFolderTab==0 || tc->type==CTFolderTab)) {					
					//control belongs to this FolderTabGroup, and: is on the current folder, or this FolderTabGroup has no current FolderTab yet, or is a FolderTab control (in which case the button shows)
					if (tcontrol->CurrentFolderTab==0) {
						//set the CurrentFolderTab to this control's FolderTab
						tcontrol->CurrentFolderTab=tc->FolderTab;
					}
					tc->flags&=~CNotShowing;
					if (tc->type==CTFolderTabGroup) {
						SelectFolderTabGroup(tc);
					}
				}
				tc=(FTControl *)tc->next;
			} //while
		} else {
			fprintf(stderr,"FTControl %s has no window\n",tcontrol->name);
			return(0);
		}
	} //	if (tcontrol->type!=CTFolderTabGroup) {
	return(1);
}//int SelectFolderTabGroup(FTControl *tcontrol)


//=======ITEMS


//AddFTItem(control, FTItem) and FTItem contains click functions, FTItem number, etc.
void AddFTItem(FTControl *tcontrol,FTItem *titem)
{
FTItem *litem,*last;
int maxwidth,fw,fh;

if (PStatus.flags&PInfo)
  fprintf(stderr,"AddFTItem %s\n",titem->name);


maxwidth=FTstrlen(titem->name); //get length of new item name

if (tcontrol->ilist==0) { //no FTItems yet
	//control itemlist has no items yet
	//fprintf(stderr,"no FTItems in %s yet\n",tcontrol->name);
	tcontrol->ilist=(FTItemList *)malloc(sizeof(FTItemList));
	memset(tcontrol->ilist,0,sizeof(FTItemList));
	tcontrol->ilist->control=tcontrol;
	tcontrol->ilist->cur=0; //no item selected yet
	tcontrol->ilist->orig=0; 
	tcontrol->FirstItemShowing=1; //scrollbar changes
	//if control has no font (use the default)
	if (tcontrol->font==0) {
		tcontrol->ilist->flags|=FTIL_USE_CONTROL_FONT; //tell AddFTControl to copy any new FTControl font to the itemlist
		SetDefaultFontForFTControl(tcontrol);
	} 
#if WIN32
		//the problem is that twin->hdcWin has not been defined yet since the window has not been created yet
	if (tcontrol->window==0) {
			SelectObject(GetDC(0),tcontrol->font); 
			GetTextMetrics(GetDC(0),&tcontrol->textmet);
		} else {
			SelectObject(tcontrol->window->hdcWin,tcontrol->font); 
			GetTextMetrics(tcontrol->window->hdcWin,&tcontrol->textmet);
		} //twin==0
#endif

	tcontrol->ilist->font=tcontrol->font;
	tcontrol->ilist->fontwidth=tcontrol->fontwidth;
	tcontrol->ilist->fontheight=tcontrol->fontheight;
#if WIN32
	tcontrol->ilist->textmet=tcontrol->textmet;
	memcpy(&tcontrol->ilist->textmet,&tcontrol->textmet,sizeof(TEXTMETRIC));
#endif
	//tcontrol->ilist->window=tcontrol->window;

	if (tcontrol->x2<=tcontrol->x1) { //x2 left to be default
		tcontrol->flags|=CSetDefaultSize; //sets x2 and x3 for dropdown controls
		//+2 because +1 is added to the text in DrawFTControl and +1 more to give a space after the text
		tcontrol->x2=tcontrol->x1+(maxwidth+(FT_DEFAULT_DROPDOWN_WIDTH+2)*(tcontrol->type==CTDropdown))*tcontrol->fontwidth;
		if (tcontrol->type==CTDropdown) {
			tcontrol->x3=tcontrol->x1+(maxwidth+1)*tcontrol->fontwidth;
		}
	} //if (tcontrol->x2<=tcontrol->x1) { //x2 left to be default
	 
	if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList || tcontrol->type==CTFileOpen) {
		//for dropdown controls, unlike other itemlist windows - the width is only the size of the control
//actually I don't know - the window could be bigger, or their could be a scroll bar
		//tcontrol->ilist->width=tcontrol->x2-tcontrol->x1;  //ilist->width is total width not visible width
		tcontrol->ilist->flags|=FTIL_HAS_NO_HOT_KEYS;  //so can draw entire string, not one character at a time
		if (tcontrol->type==CTItemList || tcontrol->type==CTFileOpen) {
			tcontrol->ilist->flags|=FTIL_IS_IN_A_CONTROL; //itemlist is in a control not a window
			tcontrol->ilist->window=tcontrol->window;  //add the control's window as the itemlist window- important because DelFTItemList will free any associated itemlist window
		} //if (tcontrol->type==CTItemList) {
	} //if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList) {

	if (tcontrol->type==CTFileOpen) {
		//FileOpen control also needs control's window for DrawSelectedFTItem and DrawUnselectedFTItem
		tcontrol->ilist->window=tcontrol->window;  //add the control's window as the itemlist window
	}
	
} //tcontrol->ilist==0

//by now there should definitely be a font for the parent control
fw=tcontrol->ilist->fontwidth;
fh=tcontrol->ilist->fontheight;

//go to last position in list
litem=tcontrol->ilist->item;

last=0;
while(litem!=0) {
	if (FTstrlen(litem->name)>maxwidth) { //this item name length is longer than any other
		maxwidth=FTstrlen(litem->name);
		//fprintf(stderr,"new maxwidth=%d\n",maxwidth);
	} //strlen
	last=litem;
	litem=(FTItem *)litem->next;
} //litem!=0


//set itemlist width-
//itemlist width is total width in pixels without clipping
//for itemlist windows: if this width or height is larger than the largest possible itemlist window, OpenFTWindow needs to clip the window and add a horiz and/or vertical scroll bar
//for itemlist controls: if this width or height is larger than the itemlist control it is in then a scrollbar(s) needs to be added to the control
//if the width is in a dropdown and is smaller than the control, OpenFTWindow will enlarge it
if (tcontrol->type==CTDropdown) {
	//possibly there should be a CAdjustControlToFitItemList flag- because with a dropdown it can't be presumed that
	//the control should change size to accomodate the itemlist in it
	//tcontrol->ilist->width=(maxwidth+1)*tcontrol->ilist->font->max_bounds.width;//+PStatus.PadX);
	tcontrol->ilist->width=(maxwidth+FT_DEFAULT_DROPDOWN_WIDTH+2)*fw;//+PStatus.PadX);
	//fprintf(stderr,"ilist width=%d\n",tcontrol->ilist->width);
} 

if (tcontrol->type==CTItemList) {
	//tcontrol->ilist->width=(maxwidth+PStatus.PadX*2)*fw;//+PStatus.PadX);
	tcontrol->ilist->width=maxwidth*fw;//+PStatus.PadX);
	//if larger than control, add horizontal scrollbar to control
	if (tcontrol->ilist->width>tcontrol->x2-tcontrol->x1) {
		tcontrol->flags|=CHScroll;
	}
	//if the itemlist height is too large for the control add a vertical scroll bar
	if (tcontrol->ilist->height>tcontrol->y2-tcontrol->y1) {
		tcontrol->flags|=CVScroll;
	}

}

if (tcontrol->type==CTFileMenu) {
	//tcontrol->ilist->width=(maxwidth+PStatus.PadX)*fw;
	tcontrol->ilist->width=(maxwidth+1)*fw;
}

if (tcontrol->flags&CSetDefaultSize) { //sets x2 and x3 for dropdown controls
	//adjust the itemlist control's width if CSetDefaultSize is set, which gets set if an item is added with a control that has an x2<=x1
	tcontrol->x2=tcontrol->x1+(maxwidth+3*(tcontrol->type==CTDropdown))*fw;
	if (tcontrol->type==CTDropdown) {
		tcontrol->x3=tcontrol->x1+(maxwidth+1)*fw;
	}
} //if (tcontrol->flags&CSetDefaultSize) { //sets x2 and x3 for dropdown controls


//add new FTItem
litem=(FTItem *)malloc(sizeof(FTItem));
memcpy(litem,titem,sizeof(FTItem));
if (tcontrol->ilist->num==0) {//first item in itemlist
	tcontrol->ilist->item=litem;
} else {  //connect last item
	last->next=litem;
}
tcontrol->ilist->num++;
//set item's parent ilist
litem->parent=tcontrol->ilist;
//set itemlist window new dimensions 
tcontrol->ilist->height=tcontrol->ilist->num*(fh+PStatus.PadY); //one extra PadY for bottom area
//if (tcontrol->type==CTItemList) {  //the dimensions of the itemlist are to the dimensions of the control
//note that width and height are the full width and height and are clipped to the control in DrawFTItemList
//	tcontrol->ilist->width=tcontrol->x2-tcontrol->x1;
//	tcontrol->ilist->height=tcontrol->y2-tcontrol->y1;
//}
litem->num=tcontrol->ilist->num; //set item number



//if ItemList control with scroll bars, they need to be adjusted here, because DrawControl doesn't adjust them
//this also save time in redrawing them
if (tcontrol->type==CTItemList) {
	tcontrol->numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->fontheight+PStatus.PadY);

	if (tcontrol->FTC_VScrollSlide!=0) {
		CalcVScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
	}
	if (tcontrol->FTC_HScrollSlide!=0) {
		CalcHScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
	}
}

//fprintf(stderr,"added FTItem %s with i=%d\n",litem->name,litem->i);
if (PStatus.flags&PInfo)
  fprintf(stderr,"AddFTItem Done\n");

//return litemList;
}  //end AddFTItem


//AddFTSubItem - add a subitem to an item
//AddFTItem adds an item to a control (like adding item "New" to a Filemenu "File")
//AddFTSubItem adds an item to an item (like adding "Window" to "New" in a Filemenu)
//In this way people can get an item from a control, and then get its subitem
//and get a subitem from that subitem, etc.
//There is no simple way of getting a subitem from the control
//Perhaps just one AddFTItem(tlist,item,num)- and doing away with AddFTSubItem(), but then the problem of choosing which item will have the subitemlist - this "SubItem" function is the most logical method I can think of for now.
void AddFTSubItem(FTItem *titem,FTItem *titem2)
{
FTItem *litem,*last;
FTItemList *titemlist;
int maxwidth,x,y;

if (PStatus.flags&PInfo)
  fprintf(stderr,"AddFTSubItem\n");


if (titem==0) return;
if (titem->ilist==0) {  //no subitems on this item yet
	titem->ilist=(FTItemList *)malloc(sizeof(FTItemList));
	memset(titem->ilist,0,sizeof(FTItemList));
//set new itemlist's parent to titem's itemlist
	titemlist=titem->ilist;
	titemlist->parent=titem->parent;
	titemlist->control=titem->parent->control; //set control to parent control for LostFocus to close
	//set new itemlist's font to parent itemlist font
	titemlist->font=titemlist->parent->font;
	titemlist->fontwidth=titemlist->parent->fontwidth;
	titemlist->fontheight=titemlist->parent->fontheight;
#if WIN32
	//titemlist->textmet=titemlist->parent->textmet;
	memcpy(&titemlist->textmet,&titemlist->parent->textmet,sizeof(TEXTMETRIC));
#endif
	titemlist->cur=0; //no item selected yet
	titemlist->orig=0;
}
litem=titem->ilist->item;
//go to last item
last=0;
maxwidth=FTstrlen(titem2->name); //length of name of item being added
//fprintf(stderr,"while\n");
while(litem!=0) {
	last=litem;
	if (FTstrlen(litem->name)>maxwidth) {  //this item name is longer than all others
		maxwidth=FTstrlen(litem->name);
	} //strlen
	litem=litem->next;
}  //while
//fprintf(stderr,"maxwidth=%d\n",maxwidth);
//fprintf(stderr,"after while\n");
litem=(FTItem *)(FTItem *)malloc(sizeof(FTItem));
memcpy(litem,titem2,sizeof(FTItem));
if (titem->ilist->num==0) {//first item in itemlist
	titem->ilist->item=litem;
} else {  //set last item next to this new item
	last->next=litem;
}
titem->ilist->num++;

//set item's parent to itemlist
litem->parent=titem->ilist;
//set item number
litem->num=titem->ilist->num; 
//set itemlist window new dimensions
titemlist=litem->parent;
#if Linux
x=titemlist->fontwidth;
y=titemlist->fontheight;
#endif
#if WIN32
if (titemlist->font==0 || titemlist->fontwidth<=0 || titemlist->fontheight<=0) {
	//set font to parent window font
	//titemlist->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
	//if this is a subitem of a control use the control font info
	if (titemlist->window==0) { //item is descended from control-has no window
		titemlist->font=titemlist->control->font;
		titemlist->fontwidth=titemlist->control->fontwidth;
		titemlist->fontheight=titemlist->control->fontheight;
		//titemlist->textmet=titemlist->control->textmet;
		memcpy(&titemlist->textmet,&titemlist->control->textmet,sizeof(TEXTMETRIC));
	} else {
		titemlist->font=titemlist->window->font;
		titemlist->fontwidth=titemlist->window->fontwidth;
		titemlist->fontheight=titemlist->window->fontheight;
		//titemlist->textmet=titemlist->window->textmet;
		memcpy(&titemlist->textmet,&titemlist->window->textmet,sizeof(TEXTMETRIC));
	}  //titemlist->window==0
} //titemlist->font==0

x=titemlist->fontwidth;
y=titemlist->fontheight;
#endif

//titemlist->width=(maxwidth+1)*titemlist->font->max_bounds.width;//+PStatus.PadX*2);
//titemlist->height=titemlist->num*(titemlist->font->max_bounds.ascent+titemlist->font->max_bounds.descent+PStatus.PadY);
titemlist->width=(maxwidth+1)*x;//+PStatus.PadX*2);
titemlist->height=titemlist->num*(y+PStatus.PadY);


if (PStatus.flags&PInfo)
  fprintf(stderr,"AddFTSubItem Done\n");

//return(1);
} //AddFTSubItem

//FTstrlen - strlen that excludes &
int FTstrlen(char *tstr) 
{
int i,j;

i=0;
j=0;
while (tstr[i]!=0) {
	if (tstr[i]!='&') j++;
i++;
} //while

return(j);
} //FTstrlen


//num=0 first FTItem (perhaps should be 1= first FTItem)
//FTItem * GetFTItemFromFTControl(FTItemList *litemList,char *name,int num)
//possibly make GetFTItemFromFTControlByName
FTItem * GetFTItemFromFTControl(FTControl *tcontrol, char *name)
{
FTItem *titem;

if (PStatus.flags&PInfo)
  fprintf(stderr,"GetFTItemFromFTControl %s\n",name);


//maintain tcontrol->num here? or each time AddFTItem?

if (tcontrol==0) {
  fprintf(stderr,"Error: Tried to get an FTItem from a nonexisting FTControl\n");
//  fprintf(stderr,"num=%d\n",num);
//  fprintf(stderr,"name=%s\n",name);
  return(0);
  }

  //fprintf(stderr,"tcontrol->ilist=%p\n",tcontrol->ilist);
  //fprintf(stderr,"tcontrol->ilist->item=%p\n",tcontrol->ilist->item);
  //fprintf(stderr,"tcontrol->ilist->item->name=%s\n",tcontrol->ilist->item->name);

titem=tcontrol->ilist->item;
while(titem!=0) {

	if (strcmp(titem->name,name)==0) {
		//fprintf(stderr,"Found item %s\n",name);
		return(titem);
	}

titem=titem->next;
} //while

//if (litem==0) fprintf(stderr,"Tried to get non existant FTItem number %d.\n",num);

fprintf(stderr,"Could not find item %s\n",name);
return(0);



//if (PStatus.flags&PInfo)
//  fprintf(stderr,"End GetFTItemFromFTControl\n");

//return litem;
} //end GetFTItemFromFTControl

//It was a tough decision to make item numbers in an itemlist start with 1
//Currently the first item has item number = 1, because:
//1) it seems more logical that the first item would be item #1
//2) SelectedItem would then have to = -1 when no item is selected
//but there are cons:
//1) programmatically, it is easier to presume that FirstItemShowing=0, etc.- now they need to be set (although currentselected would have to be set=-1 this way)
FTItem * GetFTItemFromFTControlByNum(FTControl *tcontrol,int num)
{
FTItem *titem;
int i;

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"GetFTItemFromFTControlByNum\n");

if (num<1) {
	fprintf(stderr,"Error: GetFTItemFromFTControlByNum with item number <1 (%d)\n",num);
	return(0);
} //num<1

if (tcontrol==0) {
  fprintf(stderr,"Error: Tried to get an FTItem from a nonexisting FTControl\n");
  return(0);
  }

titem=tcontrol->ilist->item;
i=1;
while(titem!=0) {

	if (i==num) {
		return(titem);
	}

i++;
titem=titem->next;
} //while

//if (litem==0) fprintf(stderr,"Tried to get non existant FTItem number %d.\n",num);
//item not found
return(0);
//if (PStatus.flags&PInfo)
//  fprintf(stderr,"End GetFTItemFromFTControl\n");

//return litem;
} //end GetFTItemFromFTControlByNum

FTItem * GetFTItemFromFTItemList(FTItemList *tlist, char *name)
{
FTItem *titem;

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"GetFTItemFromFTItemList\n");

if (tlist==0) {
  fprintf(stderr,"Error: Tried to get an FTItem from a nonexisting FTItemList\n");
  return 0;
  }

titem=tlist->item;

while(titem!=0) {

	if (strcmp(titem->name,name)==0) {
		return(titem);
	}

titem=titem->next;
} //while

//if (litem==0) fprintf(stderr,"Tried to get non existant FTItem number %d.\n",num);

return(0);
//if (PStatus.flags&PInfo)
//  fprintf(stderr,"End GetFTItemFromFTItemList\n");

} //end GetFTItemFromFTItemList

FTItem * GetFTItemFromFTItemListByNum(FTItemList *tlist,int num)
{
FTItem *titem;
int i;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"GetFTItemFromFTItemListByNum\n");
}

if (num<1) {
	//fprintf(stderr,"Error: GetFTItemFromFTItemListByNum with item number <1 (%d)\n",num);
	return(0);
} //num<1


if (tlist<=0) {
  fprintf(stderr,"Error: Tried to get an FTItem from a nonexistant FTItemList\n");
  return(0);
  }

titem=tlist->item;
i=1;
while(titem!=0) {

	if (i==num) {
		return(titem);
	}

i++;
titem=titem->next;
} //while

//if (litem==0) fprintf(stderr,"Tried to get non existant FTItem number %d.\n",num);

return(0);
//if (PStatus.flags&PInfo)
//  fprintf(stderr,"End GetFTItemFromFTItemListByNum\n");

} //end GetFTItemFromFTItemListByNum


//int SelectFTItemByName(FTControl *tcontrol,char *name) 
int SelectFTItemByName(FTItemList *tlist,char *name,unsigned int flags) 
{
FTItem *titem;

if (tlist!=0 && strlen(name)!=0) {
	titem=GetFTItemFromFTItemList(tlist,name);
	if (titem!=0) {
		//select this item
		SelectFTItem(titem,flags);
		return(1);
	} //titem!=0
} else {	
	if (tlist==0) {	
		fprintf(stderr,"SelectFTItemByName with FTItemList=0\n");
	} else {
		fprintf(stderr,"SelectFTItemByName with name=0\n");
	}
	return(0);
}//tcontrol!=0

return(0);
} //SelectFTItemByName

int UnselectFTItemByName(FTItemList *tlist,char *name) 
{
FTItem *titem;

if (tlist!=0 && strlen(name)!=0) {
	titem=GetFTItemFromFTItemList(tlist,name);
	if (titem!=0) {
		//select this item
		UnselectFTItem(titem);
		return(1);
	} //titem!=0
} else {	
	if (tlist==0) {	
		fprintf(stderr,"UnselectFTItemByName with FTItemList=0\n");
	} else {
		fprintf(stderr,"UnselectFTItemByName with name=0\n");
	}
	return(0);
}//tcontrol!=0

return(0);
} //SelectFTItemByName


//I think DrawFTControls does most of this, and I just need to set the itemlist->cur=num, and itemlist->control->val=num
int SelectFTItemByNum(FTItemList *tlist,int num,unsigned int flags) 
{
FTItem *titem;

if (tlist!=0 && num>0) {
	if (tlist->cur!=num) {
		//selected item is changing
		//unless shift or control is down unselect current item
		if (!(PStatus.flags&(PShift|PControl))) {
			if (tlist->cur!=0) {  //no item selected yet
				titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
				if (titem!=0) {
					UnselectFTItem(titem);
				}
			} 
		} //if (!(PStatus.flags&(PShift|PControl)) {
		tlist->cur=num;
		titem=GetFTItemFromFTItemListByNum(tlist,num);
		if (titem!=0) {
			SelectFTItem(titem,flags);
			return(1);
		} //titem!=0
	} //if (tlist!=0 && num>=0) {
} //if (tlist!=0) {
return(0);
} //SelectFTItemByNum

int UnselectFTItemByNum(FTItemList *tlist,int num) 
{
FTItem *titem;

if (tlist!=0 && num>0) {
	if (tlist->cur!=num) {
		//selected item is changing
		tlist->cur=num;
		titem=GetFTItemFromFTItemListByNum(tlist,num);
		if (titem!=0) {
			UnselectFTItem(titem);
			return(1);
		} //titem!=0
	} //if (tlist!=0 && num>=0) {
} //if (tlist!=0) {
return(0);
} //UnselectFTItemByNum


int SelectFTItem(FTItem *titem, unsigned int flags)
{
FTControl *tcontrol;
FTItemList *tlist;
FTItem *litem,*litem2,*litem3;
char  *tstr;
int FoundSelected;

	if (titem!=0) {
		if (titem->parent!=0) { //get this item's itemlist 
			tlist=titem->parent;				

			if (flags&FTIL_SELECT_MULTIPLE_WITH_CONTROL) { //select with control key mode
				if (titem->flags&FTI_SELECTED) { //item is already selected
					UnselectFTItem(titem);
					return(1);
				} 
			} //if (flags&FTIL_SELECT_MULTIPLE_WITH_CONTROL) { //select with control key mode

			//if a select with shift key mode, select all items between last selected item and this item
			if (flags&FTIL_SELECT_MULTIPLE_WITH_SHIFT) { //select with shift key mode
				//there are a few options here- but I chose:
				// from cur (the last selected or unselected item) select all in between
				//-presumes that the itemlist is always in order- for example if sorted by name, date, etc. that the itemlist is recreated (the alternative is to just change an index)
				//there is an intersting thing I never noticed and that is that shift selection always selects from the last select (at least in MSVC)- so if you shift in between two selected items, only the items in between the current and last selected or unselected (which is surprising- unselecting an item with control still selects it in a MSVC file open window) all other items are unselected
				//(note that cur cannot be used because it is the last selected or unselected and so is not the nearest selected item)
				if (tlist->cur>0 && tlist->cur!=titem->num) { //there is at least 1 selected item (can use NumSelectedItems too)
					//determine if we are selecting up or down
					if (tlist->cur>titem->num) {
						litem=titem;
						litem2=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
					} else {
						litem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
						litem2=titem;
					} //if (tlist->cur>titem->num) {
					//now select all items in between
					//litem=litem->next; //advance one item - no because first item could have been unselected
					//first unselect all others in the list
					//go from item 1 to litem and unselect all items above
					litem3=tlist->item;
					while(litem3!=0) {
						if (litem3->flags&FTI_SELECTED) {
							//item is not already selected
							litem3->flags&=~FTI_SELECTED;
							tlist->NumSelectedItems--;
						} //if (litem3->flags&FTI_SELECTED) {
						if (litem3==litem) { //last one
							litem3=0;
						} else {
							litem3=litem3->next;
						}
					} //while(litem3!=0) {

					//go from litem2 to last item and unselect all items below
					litem3=litem2->next;
					while(litem3!=0) {
						if (litem3->flags&FTI_SELECTED) {
							//item is not already selected
							litem3->flags&=~FTI_SELECTED;
							tlist->NumSelectedItems--;
						} //if (litem3->flags&FTI_SELECTED) {
						litem3=litem3->next;
					} //while(litem3!=0) {

					//now select all items between current and last selected or unselected item
					while(litem!=0) {
						if (!(litem->flags&FTI_SELECTED)) {
							//item is not already selected
							litem->flags|=FTI_SELECTED;
							tlist->NumSelectedItems++;
						} //if (!(litem->flags&FTI_SELECTED)) {
						if (litem==litem2) {
							litem=0;
						} else {
							litem=litem->next;
						} //if (litem==litem2) {
					} //while(litem!=litem2) {
				} //if (tlist->cur>0) {
			} //if (flags&FTIL_SELECT_MULTIPLE_WITH_SHIFT) { //select with shift key mode

			//if there are already selected items determine if any need to be unselected, depending on the flags
			if (!flags) {  //flags=0- unselect any selected items
				UnselectAllFTItems(tlist);
			} //if (tcontrol->ilist->flags&FTIL_SELECTED_ITEMS) {
			//item is selected (not toggled off)
			titem->flags|=FTI_SELECTED;
			tlist->cur=titem->num;
			tlist->flags|=FTIL_HAS_SELECTED_ITEMS;
			tlist->NumSelectedItems++;
			tcontrol=tlist->control; //get control with this itemlist
			if (tcontrol!=0) {
				tcontrol->value=titem->num; //set the control value to the selected item (for multiply selected items this value is the most recently selected)
				//todo: go through itemlist and add each item text to a comma delimited string in ->text ?
				//alternatively people could use a function like GetSelectedFTItem to get the currently selected text string
				//I thought I could just exist after finding an unselected and a selected- but not if the control key is pressed and there are selected items after unselected items
				litem=tlist->item;
				FoundSelected=0;
				tstr=tcontrol->text;
				strcpy(tstr,"");
				while(litem!=0) {
					if (litem->flags&FTI_SELECTED) {
						FoundSelected++;
						if (FoundSelected>1) {
							strcat(tstr,",");
						} 
						strcat(tstr,litem->name);						
					} //if (litem->flags&FTI_SELECTED) {
					litem=litem->next;
				} //while(litem!=0) {
				if (tcontrol->type==CTFileOpen) {
					//selecting an item in a FileOpen control
					if (!(titem->flags&FTI_FOLDER)) {
						//copy filename to ->filename ->text or both?
						//copy all filenames to a string, and then to both the control and any linked textbox
						//this could probably be done more efficiently by just checking for a match and removing or inserting some item name

						//strcpy(tcontrol->filename,titem->name);
						//strcpy(tcontrol->text,titem->name);
						strcpy(tcontrol->filename,tcontrol->text);
						if (tcontrol->TextBoxFile!=0) {
							strcpy(tcontrol->TextBoxFile->text,tcontrol->filename);
							DrawFTControl(tcontrol->TextBoxFile); //redraw the textbox with new text
						} //if (tcontrol->TextBoxFile!=0) {
					} else {
						//currently I don't clear any linked filename textbox text when a folder is selected
						/*
						//selected item is a folder
						strcpy(tcontrol->filename,"");  
						strcpy(tcontrol->text,"");  
						if (tcontrol->TextBoxFile!=0) { //if linked to a textbox control, remove any existing filename
							if (tcontrol->TextBoxFile->text!=0 && strlen(tcontrol->TextBoxFile->text)>0) { //if there is any text
								strcpy(tcontrol->TextBoxFile->filename,""); //erase any filenames
								strcpy(tcontrol->TextBoxFile->text,""); //erase any text
								DrawFTControl(tcontrol->TextBoxFile); //redraw the linked textbox 
							} //if (tcontrol->TextBoxFile->text!=0 && strlen(tcontrol->TextBoxFile->text)>0) ) {
						} //if (tcontrol->TextBoxFile!=0) {
						*/
					} //if (!(titem->flags&FTI_FOLDER)) {
					//tcontrol->ilist->cur=tcontrol->FirstItemShowing+i;  //set index for text to draw selected    
				} //if (tcontrol->type==CTFileOpen) {
				//tcontrol->value=titem->num; //probably should remove: the last selected or unselected item should come from tcontrol->ilist->cur, in addition, in the future a control could have multiple lists
				strcpy(tcontrol->text,titem->name);
				DrawFTControl(tcontrol); //refresh newly selected item
				return(1);
			} else {
				return(0);
			} //tcontrol!=0
		} else { //titem->parent!=0
			//item has no itemlist parent
			fprintf(stderr,"SelectFTItem - item %s has no itemlist parent.",titem->name);
		}//titem->parent!=0
	} //titem!=0
return(0);
} //SelectFTItem


int UnselectFTItem(FTItem *titem)
{
	FTControl *tcontrol;
	FTItemList *tlist;
	FTItem *litem;
	int FoundSelected;
	char *tstr;

	if (titem!=0) {
		//unselect this item
		titem->flags&=~FTI_SELECTED;
		if (titem->parent!=0) { //get this item's itemlist 
			tlist=titem->parent;				
			//if no other selected items unset FTIL_HAS_SELECTED_ITEMS;
			tlist->NumSelectedItems--;
			if (tlist->NumSelectedItems==0) {
				tlist->flags&=~FTIL_HAS_SELECTED_ITEMS;  //this itemlist has no more selected items
				tlist->cur=0;
			} //			if (tlist->NumSelectedItems==0) {
			tcontrol=tlist->control; //get control with this itemlist
			if (tcontrol!=0) {
				//tcontrol->value=0; 
				//go through itemlist and add each item text to a comma delimited string in ->text
				//perhaps using a linked list and just removing one item would be faster
				litem=tlist->item;
				FoundSelected=0;
				tstr=tcontrol->text;
				strcpy(tstr,"");
				while(litem!=0) {
					if (litem->flags&FTI_SELECTED) {
						FoundSelected++;
						if (FoundSelected>1) {
							strcat(tstr,",");
						} 
						strcat(tstr,litem->name);						
					} //if (litem->flags&FTI_SELECTED) {
					litem=litem->next;
				} //while(litem!=0) {

				if (tcontrol->type==CTFileOpen) {
					//unselecting an item in a FileOpen control
					if (!(titem->flags&FTI_FOLDER)) {
						//unselected item is a file - if a folder there is no need to update the FileOpen control text and filename fields
						strcpy(tcontrol->filename,tcontrol->text);
						if (tcontrol->TextBoxFile!=0) {
							strcpy(tcontrol->TextBoxFile->text,tcontrol->filename);
							DrawFTControl(tcontrol->TextBoxFile); //redraw the textbox with new text
						} //if (tcontrol->TextBoxFile!=0) {
					} //if (!(titem->flags&FTI_FOLDER)) {
					//tcontrol->ilist->cur=tcontrol->FirstItemShowing+i;  //set index for text to draw selected - currently done is ButtonUp function   
				} //if (tcontrol->type==CTFileOpen) {
				//probably this needs to be depricated (->value = selected item number) because of multiple selecting
				//tlist->cur should be used to get last selected/unselected, if even that
				//if (tlist->cur==0) {  
				//	tcontrol->value=0;
				//}
				DrawFTControl(tcontrol); //refresh newly selected item
				return(1);
			} else {
				return(0);
			} //tcontrol!=0
		} else {
			//item has no parent itemlist
			fprintf(stderr,"UnselectFTItem - item %s has no itemlist parent.",titem->name);
		}//titem->parent!=0
	} else {
		return(0);
	} //titem!=0
return(0);
} //UnselectFTItem

int UnselectAllFTItems(FTItemList *tlist)
{
	FTControl *tcontrol2;
	FTItem *titem;
	int redrawcontrol;

	if (tlist!=0) {
		titem=tlist->item;
		redrawcontrol=0;
		tlist->cur=0;  //last selected or unselected becomes 0
		if (tlist->NumSelectedItems>0) {
			//at least 1 item is selected
			tlist->NumSelectedItems=0;
			tlist->flags&=~FTIL_HAS_SELECTED_ITEMS;
			while(titem!=0) {
				//if this item is selected
				if (titem->flags&FTI_SELECTED) {
					titem->flags&=~FTI_SELECTED; //unselect this item
					redrawcontrol=1;
				} //if (titem->flags&FTI_SELECTED) {
				titem=titem->next;
			} //while(titem!=0) {

			if (redrawcontrol) {  //a selected item became unselected so need to redraw control with this itemlist
				tcontrol2=tlist->control; //get control with this itemlist
				if (tcontrol2!=0) {
					tcontrol2->value=0;
					strcpy(tcontrol2->text,""); //clear text in the control with the itemlist
					DrawFTControl(tcontrol2); //refresh control with newly unselected items
				} //tcontrol2!=0
			} //		if (redrawcontrol) {
		} //if (tlist->NumSelectedItems>0) {
			return(1);
	} //if (tlist!=0) {
return(0);
} //UnselectAllFTItems

int DelFTItem(FTControl *tcontrol, char *name)
{
FTItem *litem,*last,*litem2;
FTItemList *tlist;
int maxwidth,x,y;
int found; //renumber all items after deleted

litem=tcontrol->ilist->item;
if (litem==0) {
  fprintf(stderr,"Tried to delete FTItem from FTControl without any list.\n");
  return(0);
}

found=0;
last=0;
while(litem!=0) {

	if (found) {
		//renumber any items after
		litem->num--;
	} else { //found

	//if items have same name - just delete first

		if (strcmp(litem->name,name)==0) {
			found=1;
			if (last==0) {  //is first item
				tcontrol->ilist->item=litem->next;
			} else {  //is not first item
				last->next=litem->next;
			} //last ==0
			tcontrol->ilist->num--; //decrease number of items in this control item list
			if (litem->ilist!=0) {  //there are subitems which need to be deleted 
				DelFTItemList(litem->ilist);
				litem->ilist=0;
				//dont worry about setting litem->ilist=0 since this itemlist is being deleted anyway
			} //litem->item 
			free(litem);
			if (tcontrol->ilist->num==0) {
				free(tcontrol->ilist);  //no more items in this control item list, free the list head
				tcontrol->ilist=0;
			} else { //there are still items in this itemlist
				//adjust ItemList Window dimensions
				tlist=tcontrol->ilist;
#if Linux
				y=tlist->font->max_bounds.ascent+tlist->font->max_bounds.descent;
#endif
#if WIN32
				y=tlist->fontheight;
#endif
				tlist->height=tlist->num*(y+PStatus.PadY);
				maxwidth=0;
				litem2=tlist->item;
				while(litem2!=0) { //go thru entire list again to determine max width of largest item
					if (strlen(litem2->name)>(unsigned int)maxwidth) {
						maxwidth=strlen(litem->name);
					}
					litem2=litem2->next;
				} //while litem2
#if Linux
				x=tlist->font->max_bounds.width;//+PStatus.PadX*2);
#endif
#if WIN32
				x=tlist->fontwidth;
#endif
				tlist->width=maxwidth*x;//+PStatus.PadX*2);
			} //tcontrol->ilist->num==0					
			//renumber all below
	//		return(1);
		} //strcmp
	} //found

	if (found) {
		//if ItemList control with scroll bars, they need to be adjusted here, because DrawControl doesn't adjust them
		//this also save time in redrawing them
		if (tcontrol->type==CTItemList) {
			if (tcontrol->FTC_VScrollSlide!=0) {
				CalcVScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
			}
			if (tcontrol->FTC_HScrollSlide!=0) {
				CalcHScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
			}
		}

	}
litem=litem->next;
} //while

//return(0); //not found
return(found); //not found
}  //end DelFTItem

int DelFTItemByNum(FTControl *tcontrol,int num)
{
FTItem *litem,*last,*litem2;
FTItemList *tlist;
int i,found,x,y;
unsigned int maxwidth;

litem=tcontrol->ilist->item;
if (litem==0) {
  fprintf(stderr,"Tried to delete FTItem from FTControl without any list.\n");
  return(0);
}

found=0;
last=0;
i=0;
while(litem!=0) {

	if (found) {
		litem->num--;
	} else { //found

		if (i==num) {
			found=1;
			if (last==0) {  //is first item
				tcontrol->ilist->item=litem->next;
			} else {  //is not first item
				last->next=litem->next;
			} //last ==0
			tcontrol->ilist->num--; //decrease number of items in this control item list
			if (litem->ilist!=0) {  //there are subitems which need to be deleted 
				DelFTItemList(litem->ilist);
				litem->ilist=0;
			} //litem->item 
			free(litem);
			if (tcontrol->ilist->num==0) {
				free(tcontrol->ilist);  //no more items in this control item list, free the list head
				tcontrol->ilist=0;
			} else { //there are still items in this itemlist
				//adjust ItemList Window dimensions
				tlist=tcontrol->ilist;
#if Linux
				y=tlist->font->max_bounds.ascent+tlist->font->max_bounds.descent;
#endif
#if WIN32
				y=tlist->fontheight;
#endif
				tlist->height=tlist->num*(y+PStatus.PadY);
				maxwidth=0;
				litem2=tlist->item;
				while(litem2!=0) {
					if (strlen(litem2->name)>maxwidth) {
						maxwidth=strlen(litem->name);
					}
					litem2=litem2->next;
				} //while litem2
#if Linux
				x=tlist->font->max_bounds.width;
#endif
#if WIN32
				x=tlist->fontwidth;
#endif

				tlist->width=maxwidth*x;//+PStatus.PadX*2);
			} //tcontrol->ilist->num==0					

			return(1);
		} //i==num
	} //found

i++;
litem=litem->next;
} //while

return(found); //0=not found, 1=found
}  //end DelFTItemByNum

//DelAllFTItems - delete all items from a control recursively
int DelAllFTItems(FTControl *tcontrol) 
{

DelFTItemList(tcontrol->ilist);
tcontrol->ilist=0;
tcontrol->value=0;


return(1);
} //DelAllFTItems



//DelFTItemList - delete all items in a list recursively
//could be problem because deleting tlist in a control will not set control->ilist=0
int DelFTItemList(FTItemList *tlist) 
{
FTItem *litem,*delitem,*titem;
FTWindow *twin;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"DelFTItemList\n");
}

if (tlist!=0) {

	//delete any subitemlists
	litem=tlist->item;
	while(litem!=0) {

		if (litem->ilist!=0) { //this FTItem has a subitem that needs to be deleted
			DelFTItemList(litem->ilist);
			litem->ilist=0;
		} 
		delitem=litem;	
		litem=litem->next;
		//we have to update the parent now because other items in this list may try to set their parent item pointer to 0 after they are freed
		if (tlist->item==delitem) {
			tlist->item=litem; //update the first item in the itemlist
		}
		free(delitem); //free this item
	} //while litem!=0


	//Now delete actual itemlist
	//Close and delete any window associated with this ItemList
	if (tlist->window!=0 && !(tlist->flags&FTIL_IS_IN_A_CONTROL)) {  //there is a window associated with this itemlist and the itemlist is not in a control (like CTFileOpen or CTItemList)
		twin=tlist->window;
//no need for these two calls (close+destroy) since CloseFTWindow calls DestroyFTWindow		if (twin->flags&WOpen) {
			CloseFTWindow(twin); //closes ItemList Xwindow (but does not destroy twin structure)
			//we need to explicitly destroy the FTWindow structure here - 
			//because deleting an itemlist only happens when a window with a filemenu or dropdown control
			//is being destroyed or when a user removes an itemlist and then expects it to be empty
			//just CloseFTItemList does not destroy the FTWindow structure - 
			//since added items must be preserved
			DestroyFTWindow(twin); //free FTWindow malloc'd structure
			tlist->window=0;
//		} //twin->flags&WOpen
//		fprintf(stderr,"DestroyFTWindow in DelFTItemList %s\n",twin->name);
//		DestroyFTWindow(twin); //free FTWindow malloc'd structure
	} //tlist->window!=0


//We need to set any pointer to this itemlist to 0, this is not done automatically
if (tlist->parent==0) {
	if (tlist->control!=0) {
		tlist->control->ilist=0; //this ilist will now be freed
		tlist->control->value=0;
	} //tlist->control!=0
} else {//tlist->parent==0
	//parent is an itemlist
//perhaps tlist->parent should be FTItem descended from
//otherwise we have to go through each item to see which points to this ilist
	titem=tlist->parent->item;
	while(titem!=0) {
		if (titem->ilist==tlist) {
			titem->ilist=0; //freeing this ilist
		}
		titem=titem->next;
	} //titem!=0
//	tlist->parent->ilist=0; //this sub ilist will now be freed
}  //tlist->parent==0

free(tlist); //free itemlist
} //tlist!=0


if (PStatus.flags&PInfo)
      fprintf(stderr,"After DelFTItemList\n");   

return(1);
} //DelFTItemList


//Delete a subitem from an item (by name)
int DelFTSubItem(FTItem *titem, char *name)
{
FTItem *litem,*last,*litem2;
FTItemList *tlist;
int found,x,y;
unsigned int maxwidth;


if (titem->ilist==0) {
  fprintf(stderr,"Tried to delete an FTItem from an FTItem without any list of subitems.\n");
	return(0);
}

litem=titem->ilist->item;
if (litem==0) {
  fprintf(stderr,"Tried to delete FTItem from FTControl without any subitems.\n");
  return(0);
}

found=0;
last=0;
while(litem!=0) {

	if (found) {
		//lower the index number of any item after the deleted item
		litem->num--;
	} else { //found

		if (strcmp(litem->name,name)==0) {
			found=1;
			if (last==0) {  //is first item
				titem->ilist->item=litem->next;
			} else {  //is not first item
				last->next=litem->next;
			} //last ==0
			titem->ilist->num--; //decrease number of items in this control item list
			if (litem->ilist!=0) {  //there are subitems which need to be deleted 
				DelFTItemList(litem->ilist);
				litem->ilist=0;
			} //litem->item 
			free(litem);
			if (titem->ilist->num==0) {
				free(titem->ilist);  //no more items in this control item list, free the list head
				titem->ilist=0;
			} else { //there are still items in this itemlist
				//adjust ItemList Window dimensions
				tlist=titem->ilist;
#if Linux
				y=tlist->font->max_bounds.ascent+tlist->font->max_bounds.descent;
#endif
#if WIN32
				y=tlist->fontheight;
#endif

				tlist->height=tlist->num*(y+PStatus.PadY);
				maxwidth=0;
				litem2=tlist->item;
				while(litem2!=0) {
					if (strlen(litem2->name)>maxwidth) {
						maxwidth=strlen(litem->name);
					}
					litem2=litem2->next;
				} //while litem2
#if Linux
				x=tlist->font->max_bounds.width;//+PStatus.PadX*2);
#endif
#if WIN32
				x=tlist->fontwidth;
#endif

				tlist->width=maxwidth*x;//+PStatus.PadX*2);
			} //titem->ilist->num==0					

			return(1);
		} //strcmp
	} //found
litem=litem->next;
} //while

return(found); 
}  //end DelFTSubItem

//Delete a subitem from an item (by index number)
int DelFTSubItemByNum(FTItem *titem,int num)
{
FTItem *litem,*last,*litem2;
FTItemList *tlist;
int i,found,x,y;
unsigned int maxwidth;

if (titem->ilist==0) {
  fprintf(stderr,"Tried to delete an FTItem from an FTItem without any list of subitems.\n");
	return(0);
}

litem=titem->ilist->item;
if (litem==0) {
  fprintf(stderr,"Tried to delete FTItem from FTControl without any subitems.\n");
  return(0);
}

found=0;
last=0;
i=1;
while(litem!=0) {

	if (found) {
		//lower item number index for all items after deleted item
		litem->num--;
	} else {

		if (i==num) {
			found=1;
			if (last==0) {  //is first item
				titem->ilist->item=litem->next;
			} else {  //is not first item
				last->next=litem->next;
			} //last ==0
			titem->ilist->num--; //decrease number of items in this control item list
			if (litem->ilist!=0) {  //there are subitems which need to be deleted 
				DelFTItemList(litem->ilist);
				litem->ilist=0;
			} //litem->item 
			free(litem);
			if (titem->ilist->num==0) {
				free(titem->ilist);  //no more items in this control item list, free the list head
				titem->ilist=0;
			} else { //there are still items in this itemlist
				//adjust ItemList Window dimensions
				tlist=titem->ilist;
#if Linux
				y=tlist->font->max_bounds.ascent+tlist->font->max_bounds.descent;
#endif
#if WIN32
				y=tlist->fontheight;
#endif

				tlist->height=tlist->num*(y+PStatus.PadY);
				maxwidth=0;
				litem2=tlist->item;
				while(litem2!=0) { //go thru entire itemlist again for max width
					if (strlen(litem2->name)>maxwidth) {
						maxwidth=strlen(litem->name);
					}
					litem2=litem2->next;
				} //while litem2
#if Linux
				x=tlist->font->max_bounds.width;//+PStatus.PadX*2);
#endif
#if WIN32
				x=tlist->fontwidth;
#endif

				tlist->width=x;//+PStatus.PadX*2);
			} //titem->ilist->num==0					

			return(1);
		} //strcmp
	} //found
i++;
litem=litem->next;
} //while

return(0); //not found
}  //end DelFTSubItembyNum


void SortFTItemList(FTItemList *tlist,int order,int direct)
{
FTItem *litem,*litem2,titem;
int inorder,swap,td;
char tstr1[100],tstr2[100];  //for date+time

#if PINFO
fprintf(stderr,"Sort order=%d direct=%d\n",order,direct);
#endif

//order 0=name,1=size,2=date
//direct 0=ascend,1=descend (0xdsn  current order of date, size, name)

td=direct&(int)(pow(2.0,(double)order));  //pow(2.0,order) is 2^order

if (tlist==0) {
  fprintf(stderr,"Error: SortFTItemList with null FTItemList.\n");
  return;
}

//go thru list and compare each FTItem swapping if out of order
//until list is in order
inorder=0;

while(inorder==0) {
  litem=tlist->item;

  inorder=1;
  while (litem!=0) {
    if (litem->next!=0) {
      litem2=(FTItem *)litem->next;
//folders always go up and files down
      swap=0;
      //a=file b=folder, swap
      if (!(litem->flags&FTI_FOLDER) && litem2->flags&FTI_FOLDER) swap=1;
      if (!swap && (litem->flags&FTI_FOLDER)==(litem2->flags&FTI_FOLDER)) {
        //here a and b can only be both folders or files
        switch(order) {
          case 0:  //name
            if (strcmp(litem->name,litem2->name)>0 && !td) swap=1;
            else {
              if (strcmp(litem->name,litem2->name)<0 && td) swap=1;
            }
          break;
          case 1: //size
            if (litem->size>litem2->size && !td) swap=1;
            else {
              if (litem->size<litem2->size && td) swap=1;
            }
          break;
          case 2: //date
#if Linux
              sprintf(tstr1,"%02d%02d%02d%02d%02d%02d",litem->idate.tm_year-100,litem->idate.tm_mon+1,litem->idate.tm_mday,litem->idate.tm_hour,litem->idate.tm_min,litem->idate.tm_sec);
              sprintf(tstr2,"%02d%02d%02d%02d%02d%02d",litem2->idate.tm_year-100,litem2->idate.tm_mon+1,litem2->idate.tm_mday,litem2->idate.tm_hour,litem2->idate.tm_min,litem2->idate.tm_sec);
#endif
#if WIN32
              sprintf(tstr1,"%02d%02d%02d%02d%02d%02d",litem->idate.wYear,litem->idate.wMonth,litem->idate.wDay,litem->idate.wHour,litem->idate.wMinute,litem->idate.wSecond);
              sprintf(tstr2,"%02d%02d%02d%02d%02d%02d",litem2->idate.wYear,litem2->idate.wMonth,litem2->idate.wDay,litem2->idate.wHour,litem2->idate.wMinute,litem2->idate.wSecond);
#endif
			  if (strcmp(tstr1,tstr2)>0 && !td) swap=1;
            else {
              if (strcmp(tstr1,tstr2)<0 && td) swap=1;
            }
          break;
        }  //end switch order
      }  //end if !swap
//if a>b and both files or folders, or if a=file and b=folder
//      if ((strcmp(litem->name,litem2->name)>0 && (litem->flags&FTI_FOLDER)==(litem2->flags&FTI_FOLDER)) || (!(litem->flags&FTI_FOLDER) && litem2->flags&FTI_FOLDER)) {
        if (swap) {
     //     fprintf(stderr,"swap %s %s\n",litem->name,litem2->name);
//         fprintf(stderr,"swap %d %d\n",litem->size,litem2->size);
 //         fprintf(stderr,"swap %s %s\n",tstr1,tstr2);
          memcpy(&titem,litem,sizeof(FTItem));  //t<-a
          memcpy(litem,litem2,sizeof(FTItem));  //a<-b
          memcpy(litem2,&titem,sizeof(FTItem));  //b<-t
          //put back next pointers (and index number) so list structure is in original order, 
          //string pointers need to go with moved FTItem
          litem2->next=litem->next;  
          litem2->num=litem->num;  
          litem->next=(FTItem *)titem.next;
          litem->num=titem.num;
          inorder=0;
        }  //if swap
    }  //next!=0
    litem=litem->next;
  }  //end while litem!=0
  
}  //end while inorder==0

#if PINFO
fprintf(stderr,"Done SortFTItemList\n");
#endif
}  //SortFTItemList


//FTItemList *GetCurrentFTItemList(FTControl *tcontrol)
//returns currently selected itemlist - although titem=GetSelectedFTItem, then titem->parent can be used - infact make GetCurrentFTItemList GetSelectedFTItem(tcontrol->ilist) then return titem->parent;

/*
//possibly FTItem_keypress(FTItemList *tlist,int key) to handle enter and hotkeys
//to handle enter and possibly hotkeys
void FTItem_Keypress(FTItemList *tlist,int key) {

switch(key) {
	case ENTER:
		//call any Click(0) function
		if (titem->click(0)!=0) {
			(*titem->click(0)) (titem->window,titem->control,0,0);
		}
	break;
	default:
	break;
} //switch(key)
}  //FTItem_Keypress
*/


//OpenNextFileControl - if there is a next tabbed file control, close this itemlist of this control and open the itemlist of the next one
void OpenNextFileControl(FTControl *tcontrol) {
FTWindow *twin;//,*twin2;
FTControl *tcontrol2;

//twin=GetFTWindowN(tcontrol->win);
twin=tcontrol->window;  
if (twin!=0) {
	tcontrol2=GetNextTabFTControl(twin,1);  //current window focus,1=only filemenus
	//filemenus are ignored in tabbing - although perhaps could be included too
	if (tcontrol!=tcontrol2) { //if the same control - don't bother to close and open same itemlist
	#if PINFO
		fprintf(stderr,"next tab is %s\n",tcontrol2->name);
	#endif
		//twin->FocusFTControl
		//twin2=GetFTWindowN(tcontrol2->win); //could presume control is on same window
		FT_LoseFocus(twin,tcontrol);
		FT_SetFocus(twin,tcontrol2);
//		fprintf(stderr,"Close itemlist for %s\n",tcontrol->name);
		CloseFTItemList(tcontrol->ilist); //close current itemlist
		OpenFTItemList(tcontrol2->ilist); //open next tab itemlist
	} //tcontrol2!=tcontrol2
} //twin!=0

} //OpenNextFileControl


//OpenPrevFileControl - if there is a previous tabbed file control, close this itemlist of this control and open the itemlist of the previous one
void OpenPrevFileControl(FTControl *tcontrol) {
FTWindow *twin;
FTControl *tcontrol2;

//twin=GetFTWindowN(tcontrol->win);
twin=tcontrol->window;  
if (twin!=0) {
	tcontrol2=GetPrevTabFTControl(twin,1);  //current window focus,1=only filemenus
//	if (tcontrol==tcontrol2) {
//		fprintf(stderr,"Getprev returned same control\n");
//	}
	//filemenus are ignored in tabbing - although perhaps could be included too
	if (tcontrol!=tcontrol2) { //if the same control - don't bother to close and open same itemlist
	#if PINFO
		fprintf(stderr,"previous tab is %s\n",tcontrol2->name);
	#endif
		//twin2=GetFTWindowN(tcontrol2->win); //could presume control is on same window
	//	twin2=tcontrol2->window; //could presume control is on same window
		//twin2->focus=tcontrol2->tab;
	//	twin2->FocusFTControl=tcontrol2;
		FT_LoseFocus(twin,tcontrol);
		FT_SetFocus(twin,tcontrol2);
		CloseFTItemList(tcontrol->ilist); //close current itemlist
		OpenFTItemList(tcontrol2->ilist); //open next tab itemlist
	} //tcontrol2!=tcontrol2
} //twin!=0

} //OpenPrevFileControl


int GetFreeFTWindowNum(void)
{
FTWindow *twin;
int i,found,ex;


if (PStatus.iwindow==0) {
	return(1); //first window
}

i=PStatus.iwindow->num;
ex=0;
while(!ex) {
  //test each possible window number
  twin=PStatus.iwindow;
	found=0;
	//go thru all windows to see if this number is free
  while(twin!=0) {
    if (twin->num==i) {
      found=1;
      twin=0;  //exit while
    } else {
      twin=twin->next;
		} //twin->num==i
  }  //while(twin!=0)

  if (found==0) {
		ex=1;
		return i;
	} else {
		i++;
	} //if (found==0)
}  //end while ex==0

return(0);

}  //end GetFreeFTWindowNum


//was void CreateFTWindow(FTWindow *w)
//Creates window, but does not draw
//CreateFTWindow - mallocs the FTWindow memory, adds controls
FTWindow *CreateFTWindow(FTWindow *w)
{
int winnum;
FTWindow *twin;
//XWindowChanges wc;
//Atom *proto;
//int numproto,i;
//XEvent xevent;


	//CreateFTWindow mallocs the memory for all windows
	//make the same procedure for all windows
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"CreateFTWindow %s\n",w->name);
	}
	//   fprintf(stderr,"win x=%d y=%d\n",w->x,w->y);
	//fprintf(stderr,"In CreateFTWindow &w=%p w=%p\n",&w,w);

	winnum=GetFreeFTWindowNum();

	if (PStatus.iwindow==0) {
		PStatus.iwindow=(FTWindow *)malloc(sizeof(FTWindow));
		twin=PStatus.iwindow;
	} else {  
		//not first window
		twin=PStatus.iwindow;
	//go to last window
		while (twin->next!=0) {
		  twin=(FTWindow *)twin->next;
		}

		twin->next=(FTWindow *)malloc(sizeof(FTWindow));
		//fprintf(stderr,"malloc twin->next %d %p\n",sizeof(FTWindow),twin->next);
	//    fprintf(stderr,"after malloc %p\n",twin->next);
		// fprintf(stderr,"after malloc2\n");
		twin=(FTWindow *)twin->next;
	} //if (PStatus.iwindow==0) {


	memcpy(twin,w,sizeof(FTWindow));
	twin->icontrol=0;
	twin->FocusFTControl=0;
	twin->num=winnum;
	twin->next=0;
	if (twin->color==0) {
		twin->color=wingray;
	}

#if WIN32
	twin->hinst=PStatus.hInstance;
#endif

#if WIN32	
//For now I am creating the hWindow for Windows - so the user can get the font metrics of the window for adding controls
		CreatehWindow(twin);
#endif


//add a default font if none is set
	if (twin->font==0) {
		SetDefaultFontForFTWindow(twin);
	} //if (twin->font==0) 

	//add controls here so they can access the font
	if (twin->AddFTControls!=0) {
	(*twin->AddFTControls) (twin);
	}


	 //DrawFTWindow(twin);
		//Dropdown needs or Window clips it

#if WIN32
	//in Windows show any itemlist or splash window now (was in CreateHWindow)
	if (twin->flags&(WItemList|WSplash)) {
		ShowWindow(twin->hwindow,SW_SHOWNORMAL);
		UpdateWindow(twin->hwindow);
		SetForegroundWindow(twin->hwindow);
	}
#endif //WIN32

	//if (twin->icontrol!=0) twin->focus=1; //for now set FTControl that has tab=1 as FTControl w init focus
	  if (PStatus.flags&PInfo) {
	     fprintf(stderr,"added window FTControls\n");
		}

	//set global exclusive window flag and pointer - was only in CreateHWindow
	if (twin->flags&WExclusive) {  //if this window has exclusive FTControl
		PStatus.flags|=PExclusive;  //WItemLists are always exclusive, others may be set exclusive by users
		PStatus.ExclusiveFTWindow=twin;  //this would cause problems on autoclosing windows on focusout
	}


	if (PStatus.flags&PInfo) {
		fprintf(stderr,"End CreateFTWindow %s\n",twin->name);
	}

return(twin);
} //CreateFTWindow


FTWindow * GetFTWindowN(int num)
{
int ex;
FTWindow *twin;

ex=0;
twin=PStatus.iwindow;
while(ex==0)
{
if (twin->num==num)
  {
  ex=1;
  //fprintf(stderr,"got here return %d\n",twin->num);
  return(twin);
  }

  if (twin->next==0) ex=1;
  else  twin=(FTWindow *)twin->next;
  //fprintf(stderr,"got here2 %p\n",twin);
}  //end while

if (PStatus.flags&PInfo)
  fprintf(stderr,"Window %d not found.\n",num);
return(0);
}  //end GetFTWindowN

FTWindow * GetFTWindow(char *name)
{
FTWindow *twin;

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"GetFTWindow %s\n",name);

twin=GetFTWindowI(name,0);
return(twin);
}  //end GetFTWindow


//possibly if WClose bit is set - call DestroyFTWindow first
//GetFTWindowI returns a pointer to one instance of an FTWindow
FTWindow * GetFTWindowI(char *name,int inst)
{
//int ex;
FTWindow *twin;

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"GetFTWindowI (Instance) %s inst=%d\n",name,inst);

//ex=0;
twin=PStatus.iwindow;
while(twin!=0)
{
if (twin->inst==inst && strcmp(twin->name,name)==0)
  {
  //fprintf(stderr,"got here return %d\n",twin->num);
	//this is a temporary thing: if window is not open pretend it does not exist
	//until I can figure out why CloseFTWindow cannot deallocate the FTWindow
	//there are problems when a user destroys an FTWindow and then FT tries to finish operations on it - so for now they are not unallocated until the main window is closed.
	//an alternative approach is to force users to check WOpen to see if a window is actually closed
	//I have pretty much decided that windows should be freed on Close - but have not implemented yet
//	if (twin->flags&WOpen) {
	  return(twin);
//	} else {
//		return(0);
//	} //WOpen
  }

 twin=(FTWindow *)twin->next;
  //fprintf(stderr,"got here2 %p\n",twin);
}  //end while

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"FTWindow %s not found.\n",name);
return(0);
}  //end GetFTWindowI


#if WIN32
//possibly if WClose bit is set - call DestroyFTWindow first
//GetFTWindowI returns a pointer to one instance of an FTWindow
//FTWindow * GetFTWindowI(char *name,int inst)
FTWindow * GetFTWindowFromhWnd(HWND hwindow)
{
int ex;
FTWindow *twin;

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"GetFTWindowI (Instance) %s inst=%d\n",name,inst);

ex=0;
twin=PStatus.iwindow;
while(twin!=0)
{
if (twin->hwindow==hwindow)
  {
  //fprintf(stderr,"got here return %d\n",twin->num);
	//this is a temporary thing: if window is not open pretend it does not exist
	//until I can figure out why CloseFTWindow cannot deallocate the FTWindow
	//there are problems when a user destroys an FTWindow and then FT tries to finish operations on it - so for now they are not unallocated until the main window is closed.
	//an alternative approach is to force users to check WOpen to see if a window is actually closed
	//I have pretty much decided that windows should be freed on Close - but have not implemented yet
//	if (twin->flags&WOpen) {
	  return(twin);
//	} else {
//		return(0);
//	} //WOpen
  }

 twin=(FTWindow *)twin->next;
  //fprintf(stderr,"got here2 %p\n",twin);
}  //end while

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"FTWindow %s not found.\n",name);
return(0);
}  //end GetFTWindowFromhWin
#endif //WIN32


#if WIN32
void CreatehWindow(FTWindow *twin) 
{
wchar_t utf8_classname[FTMedStr],utf8_windowname[FTMedStr];
//int result;
FTWindow *twin2;
//int WinYOffset; //offset of window due to titlebar, etc.


	if (twin->flags&(WItemList|WSplash)) {
	//SPLASH MENU WINDOW

	twin->flags|=WExclusive; //just in case not already set

  //WinAttr.save_under=1;
if (PStatus.flags&PInfo) {
	fprintf(stderr,"Before CreateWindow\n");
}

//InputOutput - can be drawn to
//InputOnly- cannot be drawn to


  //create and register window class
	//note there is also WNDCLASSEX
	//twin->wclass.cbSize = sizeof(WNDCLASS);

	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)twin->name,-1,(LPWSTR)utf8_classname,FTMedStr);
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)twin->title,-1,(LPWSTR)utf8_windowname,FTMedStr);
		      
	memset(&twin->wclass,0,sizeof(WNDCLASS));
	twin->wclass.style			= 0;//CS_HREDRAW | CS_VREDRAW; //setting to 0 removes register class failing
	twin->wclass.lpfnWndProc	= ProcessEvents;
	twin->wclass.cbClsExtra		= 0;
	twin->wclass.cbWndExtra		= 0;
	//twin->wclass.hInstance		= twin->hinst;
	//twin->wclass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	//twin->wclass.hIcon			= LoadIcon(twin->hinst,IDI_APPLICATION);
	twin->wclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	//twin->wclass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//if a window has a background brush - DefWindowProc() will erase the window contents
	//twin->wclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	twin->wclass.lpszMenuName	= (LPCWSTR)utf8_classname;
	twin->wclass.lpszClassName	= (LPCWSTR)utf8_classname;
	//twin->wclass.hIconSm		= LoadIcon(twin->wclass.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (RegisterClass(&twin->wclass)==0) {
		fprintf(stderr,"RegisterClass for window %s failed.\n",twin->name);
	}


	//create a bitmap (as a frame buffer for the image of all the controls inside the window)
	//twin->bmpdata=(unsigned char *)malloc(twin->w*twin->h*4);
	//memset(twin->bmpdata,0x3f,twin->w*twin->h*4);
	//twin->hbmp[0] = CreateBitmap(twin->w,twin->h,1,32,twin->bmpdata);

	//for Windows, to have a window without a titlebar can only be a child window apparently.
	//So for the child window to open we need to: 1) know the parent and 2) change the coords to be relative to parent

	//It seems that possibly a child window cannot extend past a parent window - verify

	//go back to itemlist control's window - 
	//twin2=PStatus.FocusFTWindow;
	if (PStatus.ExclusiveFTControl!=0) {
		twin2=PStatus.ExclusiveFTControl->window; 
	} else {
		twin2=PStatus.FocusFTWindow; 
	}


	//as a child window, coordinates of window are relative to parent window, not screen - for the first control window only
	//In Windows even though the sub window has focus I don't set it in the Windows messages because 
	//a WS_CHILD window apparently cannot be the parent of another WD_CHILD window- the CreateWindow command below fails
	//I could set FocusFTWindow to the child window and create another variable called FocusMenuFTWindow to track the top level parent window
/*	if (!(PStatus.FocusFTWindow->flags&(WItemList|WSplash))) {
		if (twin->x>0) {
			twin->x-=twin2->x;
		}
		if (twin->y>0) {
			twin->y-=twin2->y;
		}
	}
*/

/*
		if (twin->ilist->parent==0) {

		//came from control
		if (twin->ilist->control!=0) {
			twin2=twin->ilist->control->window;
		} else {
			twin2=PStatus.iwindow; //just use main window as parent
		}

			//twin2=twin->ilist->control->window;

	//as a child window, coordinates of window are relative to parent window, not screen - for the first control window only
				if (twin->x>0) {
					twin->x-=twin2->x;
				}
				if (twin->y>0) {
					twin->y-=twin2->y;
				}

		}

		*/
	/*
		if (twin->ilist->control!=0) {
			twin2=twin->ilist->control->window;
		} else {
			twin2=PStatus.iwindow; //just use main window as parent
		}
*/
/*
*/


	//the PStatus.FocusFTWindow must be the parent window
/*
if (twin->ilist->parent==0) {
		//came from control
//		if (twin->ilist->control!=0) {
//			twin2=twin->ilist->control->window;
//		} else {
			twin2=PStatus.iwindow; //just use main window as parent
//		}

		//as a child window, coordinates of window are relative to parent window, not screen
		if (twin->x>0) {
			twin->x-=twin2->x;
		}
		if (twin->y>0) {
			twin->y-=twin2->y;
		}


	} //else { //came from window (has parent window)

		*/
	//in this case just use the PStatus.FocusFTWindow which twin2 is set to above

		//twin2=twin->ilist->parent->window;  //can't use itemlist parent window because in window child window is clipped to parent window apparently
		//twin2=PStatus.iwindow; //just use main window as parent
		//we need to go all the way back to the itemlist control
		

		//problem: a child window can only appear within a parent window- or else it is clipped
		//twin2=twin->ilist->parent->window;
		//solution: so use the parent window of the control with focus
		//it's important that twin2 be the correct parent or else the window will not appear visible
		//twin2=PStatus.ExclusiveFTControl->window; 
		//Note that ExclusiveFTControl->window only gets changed when an itemlist is open,
		//but an itemlist can open another window so get the Window with the current focus which is in PStatus.FocusFTWindow
		//twin2=PStatus.FocusFTWindow; 
		//we could also use GetForegroundWindow(); and match the HWND
		//if (twin->ilist->control!=0) {
			//twin2=twin->ilist->control->window; //use the child window as parent
		//} else {
		//	twin2=PStatus.iwindow; //just use main window as parent
	//	}

		/*
	if (twin->x>0) {
			twin->x-=PStatus.ExclusiveFTControl->window->x;
		}
		if (twin->y>0) {
			twin->y-=PStatus.ExclusiveFTControl->window->y;
		}
		*/

		//twin2=twin->ilist->window;
	//} //if (twin->ilist->parent==0) {


	
	//WS_EX_APPWINDOW
  //twin->hwindow= CreateWindowEx(0,(LPCWSTR)utf8_title,(LPCWSTR)utf8_title,
	//twin->hwindow= CreateWindow((LPCWSTR)utf8_title,NULL,
	//twin->hinst=PStatus.hInstance;
   //When creating a child window the parent has to be on top or else it can't be seen

		//ShowWindow(twin->2->hwindow,SW_SHOWNORMAL);
		//UpdateWindow(twin2->hwindow);
		//SetForegroundWindow(twin2->hwindow);

	//twin->hwindow= CreateWindow((LPCWSTR)utf8_classname,(LPCWSTR)utf8_windowname,
	//WS_CHILD|WS_VISIBLE,
	//twin->x,twin->y,twin->w,twin->h,twin2->hwindow,NULL,twin->hinst,NULL);

if (PStatus.ExclusiveFTControl && PStatus.ExclusiveFTControl->type==CTFileMenu) {
	twin->hwindow= CreateWindow((LPCWSTR)utf8_classname,(LPCWSTR)utf8_windowname,
	WS_CHILD|WS_VISIBLE,
	twin->x,twin->y,twin->w,twin->h,twin2->hwindow,NULL,twin->hinst,NULL);

} else {
	//DropDown must extend beyond window (apparenly only 1 WS_POPUP can be open at any time- because createwindow undraws the earlier popup- with filemenus the parent filemenu window disappears)
	twin->hwindow= CreateWindow((LPCWSTR)utf8_classname,(LPCWSTR)utf8_windowname,
	WS_POPUP|WS_VISIBLE,
	twin->x,twin->y,twin->w,twin->h,0,NULL,twin->hinst,NULL);
}
//if (twin2!=0) {
//		twin->hwindow= CreateWindow((LPCWSTR)utf8_classname,(LPCWSTR)utf8_windowname,
//	WS_POPUP|WS_VISIBLE,
//	twin->x,twin->y,twin->w,twin->h,twin2->hwindow,NULL,twin->hinst,NULL);
//} else {
//}
	//twin->x,twin->y,twin->w,twin->h,twin2->hwindow,NULL,twin->hinst,NULL);
	//
	//



//	SetForegroundWindow(twin2->hwindow);

	//	twin->hwindow= CreateWindow((LPCWSTR)utf8_title,(LPCWSTR)utf8_title,
	//WS_POPUP|WS_VISIBLE,
	//twin->x,twin->y,twin->w,twin->h,twin2->hwindow,NULL,twin->hinst,NULL);

	//give this new window the focus
	//if we don't do this- dropdown itemlists open on the wrong window
	//it's possible that a person may want to open a new window without giving it the focus
	//but I think it should be presumed that any newly created window gets the focus
	//the user can always change focus to some other window
	//the alternative is that the user would have to set the focus for every window created
	//and when a control on the window is clicked it then gets the focus
	//but I think default focus is better 
//	PStatus.FocusFTWindow=twin; 
	

	if (!twin->hwindow)	{
     MessageBox(0,(LPCWSTR)_T("Could not create hWindow"),(LPCWSTR)_T("Error"),0);
	} 

	if (twin->hdcWin==0) {
		twin->hdcWin=GetDC(twin->hwindow);
	} else {  //free the DC and get a new one - or else we can't draw on the window
		ReleaseDC(twin->hwindow,twin->hdcWin);
		twin->hdcWin=GetDC(twin->hwindow);
	}
	if (twin->hdcMem==0) {
		twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
	}	else {  //free the DC and get a new one - or else we can't draw on the window
		ReleaseDC(twin->hwindow,twin->hdcMem);
		twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
	}

#if 0 //now in CreateFTWindow
	if (twin->font==0) {
		twin->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		if (twin->font==0) {
		}
		//the problem is that twin->hdcWin has not been defined yet since the window has not been created yet
	} //twin->font==0
		SelectObject(twin->hdcWin,twin->font); 
		//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
		result=GetTextMetrics(twin->hdcWin,&twin->textmet);
		twin->fontwidth=twin->textmet.tmAveCharWidth;
		twin->fontheight=twin->textmet.tmHeight;


		//add controls here so they can access the font
		if (twin->AddFTControls!=0) {
		(*twin->AddFTControls) (twin);
		}


	 //DrawFTWindow(twin);
		//Dropdown needs or Window clips it


	ShowWindow(twin->hwindow,SW_SHOWNORMAL);
	UpdateWindow(twin->hwindow);
	SetForegroundWindow(twin->hwindow);
#endif	

if (PStatus.flags&PInfo) {
	fprintf(stderr,"After CreatehWindow\n");
}

} else { //if (twin->flags&(WItemList|WSplash)) {

	//IS NOT SPLASH OR ITEMLIST WINDOW
	//convert 1 byte to 2-byte character code (string)
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)twin->name,-1,(LPWSTR)utf8_classname,FTMedStr);
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)twin->title,-1,(LPWSTR)utf8_windowname,FTMedStr);

  //create and register window class
	//note there is also WNDCLASSEX
	//twin->wclass.cbSize = sizeof(WNDCLASS);
	memset(&twin->wclass,0,sizeof(WNDCLASS));
	twin->wclass.style			= CS_HREDRAW | CS_VREDRAW;
	twin->wclass.lpfnWndProc	= ProcessEvents;
	twin->wclass.cbClsExtra		= 0;
	twin->wclass.cbWndExtra		= 0;
	twin->wclass.hInstance		= twin->hinst; 
	//twin->wclass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	twin->wclass.hIcon			= LoadIcon(NULL,IDI_APPLICATION); //first param=A handle to an instance of the module whose executable file contains the icon to be loaded. This parameter must be NULL when a standard icon is being loaded. 
	twin->wclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	//twin->wclass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//twin->wclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	//twin->wclass.lpszMenuName	= (LPCWSTR)_T("test");//twin->name;
	twin->wclass.lpszMenuName	= (LPCWSTR)utf8_classname;//(LPCWSTR)twin->title;//"test";//twin->name;
	//twin->wclass.lpszClassName	= (LPCWSTR)_T("test");//twin->name;
	twin->wclass.lpszClassName	= (LPCWSTR)utf8_classname;//(LPCWSTR)twin->title;//"test";//twin->name;
	//twin->wclass.hIconSm		= LoadIcon(twin->wclass.hInstance, MAKEINTRESOURCE(IDI_SMALL));


	if (RegisterClass(&twin->wclass)==0) {
		fprintf(stderr,"RegisterClass for window %s failed.\n",twin->name);
	}



//     twin->hwindow= CreateWindow((LPCWSTR)twin->title,(LPCWSTR)twin->title,
//		 WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
//     twin->x,twin->y,twin->w,twin->h,NULL,NULL,PStatus.hInstance,NULL);


	//CreateWindow includes the titlebar height (and scrollbar width and height), so this needs to be added
	twin->rect.top=twin->y;
	twin->rect.left=twin->x;
	twin->rect.bottom=twin->y+twin->h;
	twin->rect.right=twin->x+twin->w;
	AdjustWindowRect(&twin->rect,WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0); //0=window has no menu
	//AdjustWindowRect(&twin->rect,WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,0); //0=window has no menu
	twin->WinYOffset=twin->y-twin->rect.top; //store offset for DrawItemList
	twin->WinXOffset=twin->x-twin->rect.left; //store offset for DrawItemList
	//WinYOffset=twin->y-twin->rect.top; //store offset for DrawItemList
	//The AdjustWindowRect function does not take the WS_VSCROLL or WS_HSCROLL styles into account. 
	//To account for the scroll bars, call the GetSystemMetrics function with SM_CXVSCROLL or SM_CYHSCROLL. 

	//twin->hwindow= CreateWindowEx(WS_EX_APPWINDOW,(LPCWSTR)_T("test"),(LPCWSTR)_T("test"),
	//WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN|WS_VISIBLE,  //FTMessageBox twin->hWindow gets overwritten with 0 if WS_VISIBLE is not here
		 twin->hwindow= CreateWindow((LPCWSTR)utf8_classname,(LPCWSTR)utf8_windowname,
		 WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,  //FTMessageBox twin->hWindow gets overwritten with 0 if WS_VISIBLE is not here
		 twin->x,twin->y,twin->rect.right-twin->rect.left,twin->rect.bottom-twin->rect.top,NULL,NULL,twin->hinst,NULL);
		 //twin->rect.left,twin->rect.top,twin->rect.right-twin->x,twin->rect.bottom-twin->y,NULL,NULL,twin->hinst,NULL);
		 //twin->x,twin->y,twin->rect.right,twin->rect.bottom,NULL,NULL,twin->hinst,NULL);
		 
  //    twin->x,twin->y,twin->w,twin->h,NULL,NULL,twin->hinst,NULL);

		 if (!twin->hwindow) {
			  MessageBox(0,(LPCWSTR)L"Could not open hWindow",(LPCWSTR)L"Error",0);
					twin->flags&=~WOpen;
					return;
			} 


		 if (twin->hdcWin==0) {
			twin->hdcWin=GetDC(twin->hwindow);
		 }	else {  //free the DC and get a new one - or else we can't draw on the window
			ReleaseDC(twin->hwindow,twin->hdcWin);
			twin->hdcWin=GetDC(twin->hwindow);
		}

		 if (twin->hdcMem==0) {
			twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
		 }	else {  //free the DC and get a new one - or else we can't draw on the window
			ReleaseDC(twin->hwindow,twin->hdcMem);
			twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
		}

#if 0 //now in CreateFTWindow
		 if (twin->font==0) {
			twin->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
			if (twin->font==0) {
			}
			//the problem is that twin->hdcWin has not been definited yet since the window has not been created yet
		 } //twin->font==0

 		SelectObject(twin->hdcWin,twin->font); 
		//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
		result=GetTextMetrics(twin->hdcWin,&twin->textmet);
		twin->fontwidth=twin->textmet.tmAveCharWidth;
		twin->fontheight=twin->textmet.tmHeight;


		//add controls here so they can access the font
		if (twin->AddFTControls!=0) {
		(*twin->AddFTControls) (twin);
		}

/*
		 //DrawFTWindow(twin);  //also opens window is not open
		ShowWindow(twin->hwindow,SW_SHOWNORMAL);
		UpdateWindow(twin->hwindow);
		SetForegroundWindow(twin->hwindow);
 */
#endif //now in CreateFTWindow

} //WItemList or WSplash //if (twin->flags&(WItemList|WSplash)) {


#if 0 //moved to CreateFTWindow
	if (twin->flags&WExclusive) {  //if this window has exclusive FTControl
		PStatus.flags|=PExclusive;  //WItemLists are always exclusive, others may be set exclusive by users
		PStatus.ExclusiveFTWindow=twin;  //this would cause problems on autoclosing windows on focusout
	}
#endif

//call any OnCreate/OnInit functions after actual draw

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End CreatehWindow\n");
	if (twin!=0) {
	  fprintf(stderr,"End CreatehWindow %s (%d)\n",twin->name,twin->num);
	}
}//PInfo


} //CreatehWindow
#endif //WIN32

//Open create FTWindow
//This also draws inside of window
//CreateFTWindow creates the FTWindow data structure, Open creates the XWindow in Linux 
//(note: hWindow in Windows is created in CreateFTWindow) - and draws the window
//DrawFTWindow only draws the window inside (background, and FTControls) 
void OpenFTWindow(FTWindow *twin) 
{
#if Linux
XSetWindowAttributes WinAttr;
//XWindowChanges wc;
//int rtr;//resize;
//Window xwin;
XSizeHints hints;
#endif
char winname[FTMedStr];
#define BORDER_WIDTH 2



if (twin==0) {
	fprintf(stderr,"Error: OpenFTWindow with Null pointer\n");
	return;
}

if (PStatus.flags&PInfo) {
  fprintf(stderr,"OpenFTWindow %s (%d)\n",twin->name,twin->num);
}


if (twin->flags&WOpen) {
	fprintf(stderr,"Error: OpenFTWindow %s already open or did not close properly\n",twin->name);
//see if open in XWindows if no reopen
//reopen anyway?
	return;
}



if (twin->w==0) {
if (PStatus.flags&PInfo)	{
	fprintf(stderr,"Window %s width=0 setting to 100\n",twin->name);
	}
	twin->w=100;
}

if (twin->h==0) {
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"Window %s height=0 setting to 100\n",twin->name);
	}
	twin->h=100;
}


if (PStatus.flags&PInfo) {
  fprintf(stderr,"Creating XWindow %d %d\n",twin->w,twin->h);
}


//This needs to be done here because once window is open, windows starts producing messages
//one message is WM_PAINT which will call DrawFTWindow which would find WOpen not set and calls OpenFTWindow again
//perhaps a better way is to look for the hwindow or xwindow handle
//twin->flags|=WOpen; //important to make sure this is set before calling DrawFTWindow - or infinite windows are opened


//fprintf(stderr,"twin->flags=%x\n",twin->flags);
if (twin->flags&(WItemList|WSplash)) {
//if ((twin->flags&WItemList) || (twin->flags&WSplash)) {

	//SPLASH MENU WINDOW

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Window is Menu or Splash window\n");
}


	twin->flags|=WExclusive; //just in case not already set

//WinMask=0;

#if Linux
//Setting the attributes
  WinAttr.border_pixel=BlackPixel(PStatus.xdisplay,PStatus.xscreen);
  WinAttr.background_pixel=WhitePixel(PStatus.xdisplay,PStatus.xscreen);
  WinAttr.override_redirect=1;//True;
//WinAttr.event_mask =(ButtonPressMask|ButtonReleaseMask|KeyPressMask|ButtonMotionMask|ExposureMask|PointerMotionMask);
  WinAttr.event_mask = xeventmask;//WinMask;
// (ButtonPressMask|ButtonReleaseMask|KeyPressMask|ButtonMotionMask|ExposureMask|PointerMotionMask|FocusChangeMask|StructureNotifyMask|PropertyChangeMask|VisibilityChangeMask)//|SubstructureRedirectMask)
#endif

  //WinAttr.save_under=1;
if (PStatus.flags&PInfo) {
	fprintf(stderr,"Before CreateWindow\n");
}

//InputOutput - can be drawn to
//InputOnly- cannot be drawn to



#if Linux
//  fprintf(stderr,"x=%d y=%d w=%d h=%d\n",twin->x,twin->y,twin->w,twin->h);
//this is an itemlist (menu) window - with no titlebar
  twin->xwindow = XCreateWindow(PStatus.xdisplay,
      RootWindow(PStatus.xdisplay,PStatus.xscreen),twin->x,twin->y,twin->w,twin->h,1,
    PStatus.depth,InputOutput,CopyFromParent,CWEventMask|CWOverrideRedirect,&WinAttr);
#endif

#if WIN32

	if (!twin->hwindow) {
		//for itemlist windows need to recreate in Windows
		CreatehWindow(twin);
		//twindow->flags|=WOpen;
	}

	//if that failed and there is still no hwindow print error
	if (!twin->hwindow) 	{
		MessageBox(0,(LPCWSTR)_T("Could not open window: hWindow did not get created."),(LPCWSTR)_T("Error"),0);
		 	twin->flags&=~WOpen;
			return;
	} 
	
//	 DrawFTWindow(twin);  //draw inside of window
	ShowWindow(twin->hwindow,SW_SHOWNORMAL);
	UpdateWindow(twin->hwindow);
//	twin->flags|=WOpen; //important to make sure this is set before calling DrawFTWindow - or infinite windows are opened
//	SetForegroundWindow(twin->hwindow);

//		 DrawFTWindow(twin);  //draw inside of window

	 	//set RECT structure for drawing on Window
	//GetClientRect(twin->hwindow,&twin->rect);
	//twin->rect.left=twin->x;
	//twin->rect.right=twin->w;
	//twin->rect.top=twin->y;
	//twin->rect.bottom=twin->h;


	
	//twin->hdcMem = CreateCompatibleDC(twin->hdcWin);

#endif  //WIN32
	twin->flags|=WOpen; //important to make sure this is set before calling DrawFTWindow - or infinite windows are opened

#if WIN32
	SetForegroundWindow(twin->hwindow);
#endif //WIN32

if (PStatus.flags&PInfo) {
	fprintf(stderr,"After CreateWindow\n");
}

} else { //if (twin->flags&(WItemList|WSplash)) {
//apparently XCreateSimpleWindow does not place window in correct position
//this window has a titlebar

	//REGULAR WINDOW (NOT SPLASH WINDOW) - has titlebar




/*
  twin->xwindow = XCreateSimpleWindow(PStatus.xdisplay,
                RootWindow(PStatus.xdisplay,PStatus.xscreen),twin->x,twin->y,twin->w,twin->h,BORDER_WIDTH,
                BlackPixel(PStatus.xdisplay,0),BlackPixel(PStatus.xdisplay,0));

*/

//possibly each window should have an option to be a subwindow - in which case there will be no taskbar icon
#if Linux
  WinAttr.border_pixel=BlackPixel(PStatus.xdisplay,PStatus.xscreen);
  WinAttr.background_pixel=WhitePixel(PStatus.xdisplay,PStatus.xscreen);

  WinAttr.override_redirect=0;//True;
  WinAttr.event_mask = xeventmask;

	//fprintf(stderr,"CreateWindow at %d,%d\n",twin->x,twin->y);


  twin->xwindow = XCreateWindow(PStatus.xdisplay,
      RootWindow(PStatus.xdisplay,PStatus.xscreen),twin->x,twin->y,twin->w,twin->h,1,
    PStatus.depth,InputOutput,CopyFromParent,CWEventMask|CWOverrideRedirect,&WinAttr);

//InputOutput or CopyFromParent

//  XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_NET_WM_WINDOW_TYPE,XA_ATOM,32,
//                      PropModeReplace,(unsigned char *)&XA_NET_WM_WINDOW_TYPE_DIALOG,1); 
#endif //Linux
#if WIN32


	 if (!twin->hwindow) {
		//Messagebox windows need an hwindow created
		CreatehWindow(twin);
	 } //if (!twin->hwindow) {

	 if (!twin->hwindow) {
      MessageBox(0,(LPCWSTR)L"Could not open window: hWindow did not get created.",(LPCWSTR)L"Error",0);
			twin->flags&=~WOpen;
			return;
 	} //if (!twin->hwindow) {


	ShowWindow(twin->hwindow,SW_SHOWNORMAL);
	UpdateWindow(twin->hwindow);
//	twin->flags|=WOpen; //important to make sure this is set before calling DrawFTWindow - or infinite windows are opened
//	SetForegroundWindow(twin->hwindow);

	
#endif  //WIN32

	twin->flags|=WOpen; //important to make sure this is set before calling DrawFTWindow - or infinite windows are opened

#if WIN32
	SetForegroundWindow(twin->hwindow);
#endif


} //WItemList or WSplash //if (twin->flags&(WItemList|WSplash)) {



#if Linux

if (twin->xwindow == 0)
 {
 fprintf (stderr, "Could not open window %d\n",twin->num);
 exit(EXIT_FAILURE);
 }


if (!(twin->flags&(WItemList|WSplash))) {
	XReparentWindow(PStatus.xdisplay,twin->xwindow,RootWindow(PStatus.xdisplay,PStatus.xscreen),twin->x,twin->y); //make all windows have main root window as parent - otherwise wm changes position relative to initial window
}

 //  fprintf(stderr,"win x=%d y=%d\n",twin->x,twin->y);
 //unbelievable ->title<10 chars????
XSetStandardProperties(PStatus.xdisplay,twin->xwindow,twin->title,twin->ititle,None,NULL,0,NULL);


XSelectInput(PStatus.xdisplay,twin->xwindow,xeventmask);


hints.flags=USPosition;
hints.x=twin->x; //obsolete - but what replaces?
hints.y=twin->y;
XSetWMNormalHints(PStatus.xdisplay,twin->xwindow,&hints);


XMapRaised(PStatus.xdisplay,twin->xwindow);

//somehow window location is not in createwindow, move window to initial location
//XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);

//This still appears to redraw window in new location

/*
if (PStatus.flags&PInfo) fprintf(stderr,"before XConfigure\n");
   wc.x = twin->x;
   wc.y = twin->y;
   XConfigureWindow(PStatus.xdisplay,twin->xwindow,CWX|CWY,&wc);
*/

//reparent to rootwindow



if (PStatus.flags&PInfo) fprintf(stderr,"before XCreateGC\n");
twin->xgc= XCreateGC(PStatus.xdisplay, twin->xwindow, 0, 0);
//  XSetFont(PStatus.xdisplay,twin->xgc,tcontrol->font->fid);
// Construct a rectangle to allow drawing outside of window
//this iwl only clip within a window
//	rect.x      = -40;
//	rect.y      = -40;
//	rect.width  = 640;
//	rect.height = 480;
//	XSetClipRectangles(PStatus.xdisplay,twin->xgc,0,0,&rect,1,Unsorted) ;
if (PStatus.flags&PInfo) fprintf(stderr,"before XSetWMProtocols\n");
XSetWMProtocols(PStatus.xdisplay,twin->xwindow,&XA_WM_DELETE_WINDOW,1);
//XSetWMProtocols(PStatus.xdisplay,twin->xwindow,&XA_WM_TAKE_FOCUS,1);


//    XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_WM_PROTOCOLS, XA_ATOM, 32,
//                    PropModeAppend, (unsigned char * ) &XA_WM_DELETE_WINDOW, 1);
if (PStatus.flags&PInfo) {
	fprintf(stderr,"before XClearWindow\n");
}
XClearWindow(PStatus.xdisplay,twin->xwindow);

#endif //Linux





//    XSync(PStatus.xdisplay,1);  


//   fprintf(stderr,"DrawFTWindow\n");
//   DrawFTWindow(twin->num);

//XFlush(PStatus.xdisplay);  //XFlush may draw things out of order?




//if (!(twin->flags&WItemList)) {
//shouldn't itemlist windows have exclusive control of input?

	if (twin->flags&WExclusive) {  //if this window has exclusive FTControl
		PStatus.flags|=PExclusive;  //WItemLists are always exclusive, others may be set exclusive by users
		PStatus.ExclusiveFTWindow=twin;  //this would cause problems on autoclosing windows on focusout
	}

	//if (twin->flags&WItemList) {  //if this window is an itemlist window
//icontrol is the control list - not the control with exlusive control
		//PStatus.ExclusiveFTControl=twin->icontrol;  //even though itemlists are not controls, this was an easy way of connecting an itemlist window with its parent control for closing on LostFocus
	//}


//because commands may be called on the window later
//wait until window has focus (for example setting focus to a different window 
//(while this window is still being drawn) would be lost
//because this window would take focus back when drawing was complete
//twin=GetFTWindow("FTControl");
		//wait until this window has focus
//	fprintf(stderr,"before while\n");


//	fprintf(stderr,"after while\n");
//}  //!WItemList

//   fprintf(stderr,"after draw window\n");

//} //if twin->xwindow ==0

#if Linux
XSync(PStatus.xdisplay,0); //wait for XServer to process all events in queue
#endif
#if WIN32
	  GdiFlush();
#endif

//XSync doesn't work - it doesn't wait for a window to be done drawing - possibly I have an older version
if (PStatus.flags&PInfo) {
	fprintf(stderr,"before DrawFTWindow in OpenFTWindow\n");
}

DrawFTWindow(twin); //Draw FTWindow inside (background, FTControls)

if (PStatus.flags&PInfo) {
	fprintf(stderr,"after DrawFTWindow in OpenFTWindow\n");
}

//XSync(PStatus.xdisplay,0); //wait for XServer to process all events in queue
//Window is not getting fully drawn before calling OnOpen
//XFlush(PStatus.xdisplay);  //XFlush may draw things out of order?

//wait for window to be completely drawn before moving on
//add timeout?
//wait for expose with 0
/*
twin->flags|=WWaitUntilDoneDrawing;
while(twin->flags&WWaitUntilDoneDrawing) {
	ProcessEvents();
} //while
*/

/*
//wait until window gets focus
if (PStatus.flags&PInfo) fprintf(stderr,"before XGetInputFocus\n");
if (!(twin->flags&(WItemList|WSplash))) {
  xwin=0;
  while (twin->xwindow!=xwin) { //was while but hanged on itemlist
//  fprintf(stderr,"FT_GetInputfocus\n");
		ProcessEvents(); //blocks
    XGetInputFocus(PStatus.xdisplay,&xwin,&rtr);

    //XGetInputFocus(PStatus.xdisplay,&xwin,&rtr);
  }
}
*/

#if Linux
XFlush(PStatus.xdisplay);
XSync(PStatus.xdisplay,0); //wait for XServer to process all events in queue
#endif
#if WIN32
	  GdiFlush();
#endif

//call any OnOpen functions after actual draw
//OnOpen only is called if this window has not been drawn yet
//the big problem is finding someway to define the window size before opening
//was before drawing window and FTControls -
//but I thought - a window can be modified before being Drawn
//the problem is that the window frame appears before the FTControls are drawn
//and if the OnOpen function takes time, this frame looks like the program is hanging

  if (twin->OnOpen!=0) {
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"Calling user OnOpen function.\n");
		}
		//fprintf(stderr,"twin=%p\n",twin);
		//fprintf(stderr,"twin->name=%s\n",twin->name);
		//fprintf(stderr,"twin->OnOpen=%p\n",twin->OnOpen);
		//fprintf(stderr,"*twin->OnOpen=%p\n",*twin->OnOpen);
    (*twin->OnOpen) (twin);
		//fprintf(stderr,"after OnOpen\n");
		twin=GetFTWindow(winname); //check to see if window was deleted
		if (twin==0) {
			//twin->flags&=~WOpen;
			return; //user deleted window
		}
  }

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End OpenFTWindow\n");
	if (twin!=0) {
	  fprintf(stderr,"End OpenFTWindow %s (%d)\n",twin->name,twin->num);
	}
}//PInfo

} //openFTWindow



//add width, height parameters?
//This needs a timer, so when the button is still down - only redraw window and controls every second - otherwise the drawing needs to catch up to the resizing
void ResizeFTWindow(FTWindow *twin) {

//XSetWindowAttributes WinAttr;
//XSizeHints hints;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"ResizeFTWindow\n");
}//PInfo

if (twin!=0) {

#if Linux
	if (twin->xwindow!=0) {
	
		XResizeWindow(PStatus.xdisplay,twin->xwindow,twin->w,twin->h);

//resizing window does not adjust the dimensions of data controls

/*
		hints.flags=USPosition;
		hints.x=twin->x; //obsolete - but what replaces?
		hints.y=twin->y;
		XSetWMNormalHints(PStatus.xdisplay,twin->xwindow,&hints);
*/

	//	XMapRaised(PStatus.xdisplay,twin->xwindow);

		//somehow window location is not in createwindow, move window to initial location
		//XMoveWindow(PStatus.xdisplay,twin->xwindow,twin->x,twin->y);

		//This still appears to redraw window in new location

		//another method:
		/*
		if (PStatus.flags&PInfo) fprintf(stderr,"before XConfigure\n");
			 wc.x = twin->x;
			 wc.y = twin->y;
			 XConfigureWindow(PStatus.xdisplay,twin->xwindow,CWX|CWY,&wc);
		*/
		} //twin->xwindow!=0
#endif //Linux
} //twin!=0

} //ResizeFTWindow


//Draw FTWindow by number
void DrawFTWindowN(int num)
{
FTWindow *twin;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"DrawFTWindowN\n");
}//PInfo


twin=GetFTWindowN(num);
DrawFTWindow(twin);

}  //DrawFTWindowN




//Draws created FTWindow inside (background and FTControls)
void DrawFTWindow(FTWindow *twin)
{  //this also does redraw
char winname[FTMedStr];

//one idea is to just draw to a screen buffer/bitmap and then simply draw the bitmap to the window 
//this might be the most portable method


if (PStatus.flags&PInfo) {
  fprintf(stderr,"DrawFTWindow\n");
}//PInfo


if (twin==0) {
	fprintf(stderr,"Error: DrawFTWindow passed null window\n");
	return;
}

strcpy(winname,twin->name);



if (PStatus.flags&PInfo) {
  fprintf(stderr,"FTWindow= %s (window num=%d) w=%d h=%d\n",twin->name,twin->num,twin->w,twin->h);
}

if (twin->w<=0 || twin->h<=0) {
  fprintf(stderr,"Error: DrawFTWindow called with <=0 width or height\n");
} //twin->w

//if (!(twin->flags&WOpen) || (twin->flags&WNotVisible)) {

if (!(twin->flags&WOpen)) {

if (PStatus.flags&PInfo) {
  fprintf(stderr,"DrawFTWindow %s needs to be opened\n",twin->name);
}

	OpenFTWindow(twin); //Window has not been opened yet - or was hidden and XWindow needs to be recreated
	return;
}

//fprintf("DrawFTWindow %s has WOpen\n",twin->name);

if (twin!=0) {
//#if PINFO
//  fprintf(stderr,"twin %s twin->flags&WNotVisible %d \n",twin->name,twin->flags&WNotVisible);
//#endif

//perhaps DrawFTWindow should erase twin->flags&WNotVisible bit
//twin->flags&=~WNotVisible; //window not visible when added

#if Linux
//Draws the gray background and any image on the window
  if (PStatus.depth==16) {
	  XSetForeground(PStatus.xdisplay,twin->xgc,wingray16);
	} else {
	  XSetForeground(PStatus.xdisplay,twin->xgc,wingray);
	}

//for some reason this is not getting drawn immediately
XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,0,twin->w,twin->h);
#endif
#if WIN32

//PAINTSTRUCT lpPaint;
//HDC hdcWin;
//hdcWin=BeginPaint(twin->hwindow,&lpPaint);
//winbrush=CreateSolidBrush(wingray); 
//SelectObject(hdcWin,winbrush);
//SelectObject(twin->hdcWin,winbrush);
SetBkMode(twin->hdcWin,OPAQUE);
twin->brush=CreateSolidBrush(wingray); 
SelectObject(twin->hdcWin,twin->brush);
GetClientRect(twin->hwindow,&twin->rect);
FillRect(twin->hdcWin,&twin->rect,twin->brush);
DeleteObject(twin->brush);
//InvalidateRect(twin->hwindow,NULL,TRUE);
//TextOut(hdcWin,10,10,L"Hello World",11);
//TextOut(twin->hdcWin,10,10,L"Hello World",11);

//EndPaint(twin->hwindow,&lpPaint);
//InvalidateRect(twin->hwindow,NULL,TRUE); //refresh window
//ReleaseDC(twin->hwindow,twin->hdcWin);
//DeleteObject(winbrush);
//SetBkColor(hdcMem,RGB(0,0,0));
//SetBkColor(twin->hdcWin,wingray);

//SelectObject(twin->hdcMem,twin->hbmp);
//StretchBlt(twin->hdcWin,0,0,twin->w,twin->h,twin->hdcMem,0,0,twin->w,twin->h,SRCCOPY);
//DeleteObject(twin->hbmp);

  //UpdateWindow(twin->hwindow);
#endif

#if Linux
//draw any image on the window
//image actually stretched to window
//note that currently a window can only have 1 image
  if (twin->ximage !=0) {//there is an image on the window 
//    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,twin->image[0],0,0,twin->x1,twin->y1,twin->w,twin->h);
    XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,twin->ximage,0,0,0,0,twin->w,twin->h);
  }
#endif

//fprintf(stderr,"Drawing window's controls\n");

	//Draw Window Controls before any open itemlist
// DrawFTControls(twin->num,0);
 DrawFTControls(twin,0);



//Need to draw items in window. Drawing a filemenu control redraws all windows
//but DrawWindow only draws one window with controls inside. ItemList windows have no controls
//so items in window are drawn here. In addition, many time only a single subitemlist window is drawn or redrawn
 //XSync(PStatus.xdisplay,0);  

	if (twin->flags&WItemList) {  //this is an ItemList Window
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"Window is an itemlist window, drawing itemlist\n");
		}//PInfo

		//Draw Items if any
		if (twin->ilist!=0) {
			DrawFTItemList(twin->ilist);		
		}
	}

//fprintf(stderr,"before xflush\n");


#if Linux
 XFlush(PStatus.xdisplay);  
// XSync(PStatus.xdisplay,0);  
#endif
#if WIN32
	  GdiFlush();
#endif

//fprintf(stderr,"before ondraw\n");

//call any window onDraw functions
  if (twin->OnDraw!=0) {
		(*twin->OnDraw) (twin);
		twin=GetFTWindow(winname); //check to see if window was deleted
		if (twin==0) return; //user deleted window
	} //OnDraw

}  //else twin!=0


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End DrawFTWindow\n");
}

}  //end DrawFTWindow


int GetFTWindowInstance(char *name)
{
FTWindow *twin;
int inst;

twin=PStatus.iwindow;
inst=0;
while (twin!=0) {
  if (!(strcmp(twin->name,name) && twin->inst>inst)) inst=twin->inst;
twin=twin->next;
}  //end while

return(inst);
}  //GetFTWindowInstance



//Possibly call DestroyFTWindow too - no need to keep FTWindow (twin) structure
//CloseFTWindowNum - DestroyFTWindow for a window by number
//CloseFTWindow is the same as DestroyFTWindow
//perhaps make DestroyFTWindowByNum, DestroyFTWindowByName, etc?
void CloseFTWindowNum(int num)
{
FTWindow *twin;

twin=GetFTWindowN(num);

CloseFTWindow(twin);
//if (twin!=0) {
//}
//else
//  fprintf(stderr,"CloseFTWindowNum FTWindow number %d not found.\n",num);
}  //end CloseFTWindowNum

void CloseFTWindowByName(char *name)
{
CloseFTWindowI(name,0);
}  //CloseFTWindowByName


//CloseFTWindow with instance (when a window with one name is opened more than once)
void CloseFTWindowI(char *name,int inst)
{
FTWindow *twin;

twin=GetFTWindowI(name,inst);
#if PINFO
fprintf(stderr,"GetFTWindowI %p\n",twin);
#endif
CloseFTWindow(twin);
} //CloseFTWindowI


//CloseFTWindow - closes FTWindow and destroys XWindow, FTWindow, and FTWindow's FTControls
void CloseFTWindow(FTWindow *twin)
{ 
//FTWindow *twin2;//,*last;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"CloseFTWindow\n");
}

if (twin!=0) {

			//call any OnClose functions - needs to be done here because by the time of message
			//twin is already 0
			if (twin->OnClose!=0) {
				(*twin->OnClose) (twin);
			}


      //should be DelFTControlAll(FTWindow *lwin)
      //free all FTControls for a deleted window
			//if this FTWindows were double-linked lists we would not have to go down list
			//possibly implement ->last link back to previous FTWindow - then remember to set PStatus->iwindow=0 if first FTWindow

//			twin2=PStatus.iwindow;
//			last=twin2;
//			while(twin2!=0) {
//				if (twin2==twin) { //this is the window to close
/*
					if (last==0) {
						PStatus.iwindow=twin->next;
					} else { 
						last->next=twin->next;
					} //last==0
*/
/*
				  if (twin->image[0]!=0) {
				    XDestroyImage(twin->image[0]);
				    //twin->image[0]=0; //dont bother - window is destroyed anyway
				  }
*/

				  //if (twin->bmpinfo!=0) {
				   // free(twin->bmpinfo);
				  //}

#if Linux
				  if (twin->xwindow!=0) {
				    XFreeGC(PStatus.xdisplay,twin->xgc);
				    XUnmapWindow(PStatus.xdisplay,twin->xwindow);
				    XDestroyWindow(PStatus.xdisplay,twin->xwindow);
						twin->xwindow=0;
						twin->flags&=~WOpen;
				  }
#endif //Linux
#if WIN32
				  if (twin->hwindow!=0) {
						ReleaseDC(twin->hwindow,twin->hdcWin);
						ReleaseDC(twin->hwindow,twin->hdcMem);
						  //CloseWindow(twin->hwindow);  //minimizes
						twin->flags&=~WOpen; //or else endless loop because DestroyWindow calls CloseFTWindow if WOpen
						DestroyWindow(twin->hwindow);
						twin->hwindow=0;
						
				  }
#endif //WIN32


/*
//In theory a main window might be closed but not destroyed and the program still running.

				  if (twin==PStatus.iwindow) {
				    PStatus.iwindow=0;  //this is the root window
						//root window may not be first window opened - for example splash or some other window
				    PStatus.flags|=PExit;
				  }
*/

//		#if PINFO
//				  fprintf(stderr,"going to free twin=%p\n",twin);
//		#endif
//				  free(twin);
//				  twin=0;
		//      fprintf(stderr,"freed win\n");


					//if any window is closed, exclusive control of input events by any window or control is ended too
					//If this window has exclusive control, delete exclusive control
					if (PStatus.flags&PExclusive && PStatus.ExclusiveFTWindow==twin) {
						if (twin->ilist!=0) { //window has an attached itemlist
							//if this window has a parent exclusive control passes to the parent
							if (twin->ilist->parent==0) { //this window is the first descendant of a filemenu or dropdown - so close PItemListOpen bit
								PStatus.flags&=~PItemListOpen;  //I probably don't need this bit anymore - WOpen is enough and WExclusive- it's still used in CheckHotKeys
								PStatus.ExclusiveFTControl=0;
								PStatus.ExclusiveFTWindow=0;
								PStatus.flags&=~PExclusive; //no windows have exclusive control anymore
							} else { //if (twin->ilist->parent==0) {
								//this window itemlist has a parent, so exclusive control is passed to the parent
								PStatus.ExclusiveFTWindow=twin->ilist->parent->window;
							} //if (twin->ilist->parent==0) {
						} else { //if (twin->ilist!=0) {
							//this usually doesn't happen- that a window with exclusive control would have no itemlist
							//if it does, just release exclusive control
							PStatus.ExclusiveFTControl=0;
							PStatus.ExclusiveFTWindow=0;
							PStatus.flags&=~PExclusive; //no windows have exclusive control anymore
						} ////if (twin->ilist!=0) {
					} //PStatus.flags&PExclusive

					//This could probably be done better
					//if (PStatus.flags&PItemListOpen) { //an itemlist is open
					//} //WOpenItemList


				  if (PStatus.flags&PInfo) {
				    fprintf(stderr,"Closed FTWindow '%s'.\n",twin->name);
					}

//one method: no FTWindow is actually destroyed until the last window closes - just WClose is changed to WOpen (this protects code running after user closes window like on exit button)
//second method: put in checks to make sure after any user function that the twin and tcontrol still exist
//possibly only set destroy bit - because exit button can get buttonup - 
//and more processing happens on windows and control - like call buttonup function





//					if (!(twin->flags&(WItemList|WSplash))) {
					if (!(twin->flags&WItemList)) {
						//Important Note: WItemList FTWindow structures must not be destroyed until the parent window is destroyed because otherwise, for a dropdown box the items would need to be added, each time the control is clicked on
						//Allow Splash Window to be destroyed

						//CloseFTWindow does not destroy ItemList windows because they belong to controls on other window - itemlist windows are created and destroyed with the window that has their parent control.
//there is some error when an FTWindow is destroyed
//for now I am keeping FTWindows around until the end of the program
//					twin->flags|=WClose; //Close when done processing buttonup, etc - in ProcessEvents

						//this line will free the FTWindow (twin) structure
						DestroyFTWindow(twin); //for now destroy FTWindow too - its too confusing for users to have to CloseWindow and the DestroyWindow too

						if (PStatus.flags&PInfo) {
							fprintf(stderr,"Destroyed FTWindow\n");
						} //PInfo
						return;

//					} //!WItemList|WSplash
					} //!WItemList
					
//possibly after button up and user function calls - check to see if twin==0 or return if it does

//				} //twin2==twin
//				last=twin2;
//				twin2=twin2->next;
//			} //while twin2!=0


      //twin->xwindow=0;
      //fprintf(stderr,"after xfree,unmap, and destroy\n");
      //fprintf(stderr,"after free twin\n");
}  //twin!=0 


return;
}  //end CloseFTWindow


//DestroyFTWindow - Frees the FTWindow malloc'd structure, and deletes the FTWindow from the linked list descending from PStatus
void DestroyFTWindow(FTWindow *twin) 
{
FTWindow *last,*twin2;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"DestroyFTWindow\n");
}

if (twin!=0) {
if (PStatus.flags&PInfo) {
	fprintf(stderr,"DestroyFTWindow %s\n",twin->name);
}

  //CloseFTWindow now calls DestroyFTWindow
	if (twin->flags&WOpen) {
		CloseFTWindow(twin);
	} //twin->flags&WOpen


	//other windows might be linked to this window
	//and so repair the missing link by connecting the previous ftwindow to the ftwindow after the destroyed ftwindow
	twin2=PStatus.iwindow;
	last=twin2;
	//double-linked list would be faster - would not have to go through each window
	while(twin2!=0) {

		if (twin2==twin) {  //found the window to destroy


			//Delete this FTWindow's controls
			if (PStatus.flags&PInfo) {
				  fprintf(stderr,"Before DelFTControlAll for %s\n",twin->name);   
			}

			DelFTControlAll(twin);

			if (PStatus.flags&PInfo) {
				  fprintf(stderr,"After DelFTControlAll\n");   
			}

			//free this window and link the last
			//if this window is the first window, relink PStatus.iwindow
			if (twin==PStatus.iwindow) {
				PStatus.iwindow=twin->next; //set the first window to the next
			} else {
				//otherwise set the last window to the next window
				last->next=twin->next;
			} 

//			fprintf(stderr,"here\n");
#if Linux
			//free twin->image[0]?
			if (twin->ximage!=0) {
			    XDestroyImage(twin->ximage);
				twin->ximage=0;
		    //twin->ximage=0; //dont bother - window is destroyed anyway
		  }
#endif

		  //if (twin->bmpinfo!=0) {
		   // free(twin->bmpinfo);
		  //}

			//if (twin->bmpdata!=0) {
			//	free(twin->bmpdata); //free any previous mallc'd bitmap data
			//}

			if (twin==PStatus.ExclusiveFTWindow) {
				PStatus.ExclusiveFTWindow=0;
				PStatus.flags&=~PExclusive;
			}

		  if (twin==PStatus.iwindow) {
		    PStatus.iwindow=0;  //this is the root window
				//root window may not be first window opened - for example splash or some other window
		    //PStatus.flags|=PExit; //user may want to keep program alive with no windows
		  }



			//fprintf(stderr,"Before free(twin) %s\n",twin->name);
			if (twin!=0) {
				//free(twin);  //causes crash in Windows
				twin=0;
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"After DestroyFTWindow\n");   
				}
				return;
			}
			//fprintf(stderr,"after free (twin)\n");
		} //twin2==twin
	last=twin2;
	twin2=twin2->next;
	} //while twin2!=0
} //twin!=0

if (PStatus.flags&PInfo) {
      fprintf(stderr,"After DestroyFTWindow\n");   
}
} //DestroyFTWindow


/*
void FreeFTWindows(void)
{
#if 0
int a;
FTWindow *twin,*lwindow;

twin=iwindow;
for(a=0;a<numwindows;a++)
{
lwindow=twin;
twin=(FTWindow *)twin->next;
free(lwindow);
}  //end for a
#endif
}  //end FreeFTWindows
*/


/*
//from a window, return the filemenu FTControl that is open
FTControl *GetOpenFileMenu(FTWindow *twin)
{
FTControl *tcontrol;

tcontrol=twin->icontrol;

while(tcontrol!=0) {
  if (tcontrol->flags&CItemListOpen) {
    return(tcontrol);
  }

  tcontrol=tcontrol->next;
}  //while tcontrol!=0

return(0);
} //GetOpenFileMenu
*/


//void CheckHotKeys(FTWindow *twin,unsigned char key)
void CheckHotKeys(FTWindow *twin,KeySym key)
{
FTControl *tcontrol;//,*tcontrol2;
FTItemList *tlist;
FTItem *titem;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"CheckHotKeys\n");
}

//hotkeys
//if key==FTControl->hotkey, or FTControl->FTItem->hotkey, move focus to that FTControl
//check hotkey1 and hotkey2

//if menu open check hotkeys against current submenu
//if (twin->flags&WItemListOpen) {
//Use PStatus Exclusive?
//if (twin->flags&WExclusive) {
if (PStatus.flags&PItemListOpen) {
//this program has an itemlist open 

	//get currently selected ItemList
	tlist=PStatus.SelectedFTItemList;

	if (tlist!=0) {
		//see if key matches any item in this list
		titem=tlist->item;
		while(titem!=0) {
			if (CheckHotKeyFTItem(titem,key)) { //hotkey matches
				//open any subitemlist and call click function, or else close

			//if item has an ilist - open - or else close all itemlist windows
				if (titem->ilist!=0) { //item has subitemlist
					if (titem->ilist->window!=0) { //subitemlist has window
						twin=titem->ilist->window;
						if (twin->flags&WOpen) {
							CloseFTItemList(titem->ilist); //close opened subitemlist
						} else {  //subitemlist window is not open
							tlist->cur=titem->num;
							//SelectFTItem(titem);
							titem->flags|=FTI_SELECTED; //for now
							OpenFTItemList(titem->ilist); //open closed subitemlist
							DrawSelectedFTItem(tlist,titem->num);	//select item	- sets tlist->cur
						}
					} else { //subitem has no window yet
						tlist->cur=titem->num;
						//SelectFTItem(titem);
						titem->flags|=FTI_SELECTED; //for now
						OpenFTItemList(titem->ilist); //open closed subitemlist
						DrawSelectedFTItem(tlist,titem->num);  //select item 

					}//titem->ilist->window
				} else { //titem->ilist!=0
					//no subitemlist - close all itemlists
					tcontrol=PStatus.ExclusiveFTControl;
					if (tcontrol!=0) {
					  CloseFTItemList(tcontrol->ilist);
					} //tcontrol!=0
				} //titem->ilist!=0
				//call any function related to selecting this item (even if not the final selection)
				if (titem->click[0]!=0) {
					(*titem->click[0]) (tlist->window,tlist->control,0,0);
					//if (tlist->control==0 || tlist->window==0) return; //user deleted control or window
				} //titem->click(button)
		    return;
			} //CheckHotKeyFTItem
			titem=titem->next;
		} //while
	} //tlist!=0

} else { //PItemListOpen //WExclusive //WItemListOpen 
  //no ItemList open
  //check hotkeys against all FTControls of this window

	//control does not need to have focus to have a hotkey recognized - this is certainly true for filemenu controls
  tcontrol=twin->icontrol;
  while (tcontrol!=0) {
  //if (!(twin->flags&WItemListOpen) || (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu))) {

//		fprintf(stderr,"Check HotKey for %s key=%d\n",tcontrol->name,(int)key);
    if (CheckHotKeyFTControl(tcontrol,key)) {
//  fprintf(stderr,"%d %d\n",PStatus.flags&PAlt,tcontrol->hotkey&PAlt || tcontrol->hotkey2&PAlt);
    //pretend a click down-up on the FTControl
 //     fprintf(stderr,"match\n");
		//Clear the control that has focus
			if (twin->FocusFTControl!=tcontrol && twin->FocusFTControl!=0) {
				//tcontrol2=twin->FocusFTControl;
				FT_LoseFocus(twin,twin->FocusFTControl);		
				//twin->FocusFTControl=tcontrol;
				FT_SetFocus(twin,tcontrol);

				
				//simulate a button click on the control 
				//tcontrol->ButtonClick[0]
			}

			//if (tcontrol->type==CTButton) {
					SimulateButtonClick(twin,tcontrol->x1+1,tcontrol->y1+1,0);
			//	}
			//ButtonDownUp(tcontrol);
//OpenItemList sets twin->focus=tcontrol->tab
			//open any item list if this is a filemenu or dropdown control 
			//OpenFTItemList(tcontrol->ilist);
     return;
    }  //if CheckHotKeyFTControl
 

  tcontrol=tcontrol->next;
}  //end while


}  //WExclusive //end if WItemListOpen else


//clear alt, cntl -but not if 2 shift keys or 2 alt keys, 2 cntl keys
//if (key!=-30 && key!=-30 && key!=-23 && key!=126 && key!=-28 && key!=-29) PStatus.flags&=~(PAlt|PControl);


if (PStatus.flags&PInfo) {
	fprintf(stderr,"End CheckHotKeys\n");
}

}  //end CheckHotKeys



//keyt does not need to have focus to match a hotkey at this level
//int CheckHotKeyFTControl(FTControl *tcontrol,unsigned char key)
int CheckHotKeyFTControl(FTControl *tcontrol,KeySym key)
{
int i;

if (key!=0) {

	//currently only 4 possibly hotkeys
	for(i=0;i<4;i++) {

		if ((tcontrol->hotkey[i]&0xff)==key) { //key portion matches
			//fprintf(stderr,"hotkey %d matches %d\n",tcontrol->hotkey[i],(int)key);
			if (!(tcontrol->hotkey[i]&PControl) && !(tcontrol->hotkey[i]&PAlt) && !(tcontrol->hotkey[i]&PShift)) return(1);//match=1;  //no control, alt or shift

			if (PStatus.flags&PAlt && tcontrol->hotkey[i]&PAlt) { //test Alt key 
				//and combination with Cntl and Shift
				if ((PStatus.flags&PControl && tcontrol->hotkey[i]&PControl) || 
						(~(PStatus.flags&PControl) && ~(tcontrol->hotkey[i]&PControl))) {
					if ((PStatus.flags&PShift && tcontrol->hotkey[i]&PShift) || 
						(~(PStatus.flags&PShift) && ~(tcontrol->hotkey[i]&PShift))) {
						return(1); 
					} //PShift
				} //PControl
			} //PAlt

			if (PStatus.flags&PControl && tcontrol->hotkey[i]&PControl) { //test Control key 
				//and combination with Alt and Shift
				if ((PStatus.flags&PAlt && tcontrol->hotkey[i]&PAlt) || 
						(~(PStatus.flags&PAlt) && ~(tcontrol->hotkey[i]&PAlt))) {
					if ((PStatus.flags&PShift && tcontrol->hotkey[i]&PShift) || 
						(~(PStatus.flags&PShift) && ~(tcontrol->hotkey[i]&PShift))) {
						return(1); 
					} //PShift
				} //PAlt
			} //PControl

			if (PStatus.flags&PShift && tcontrol->hotkey[i]&PShift) { //test Shift key 
				//and combination with Alt and Control
				if ((PStatus.flags&PAlt && tcontrol->hotkey[i]&PAlt) || 
						(~(PStatus.flags&PAlt) && ~(tcontrol->hotkey[i]&PAlt))) {
					if ((PStatus.flags&PControl && tcontrol->hotkey[i]&PControl) || 
						(~(PStatus.flags&PControl) && ~(tcontrol->hotkey[i]&PControl))) {
						return(1); 
					} //Control
				} //PAlt
			} //PShift
		} //key==hotkey
	}//for i

} //key!=0
return(0);

/*		
	if (PStatus.flags&PAlt && PStatus.flags&PControl) {  //both alt and cntl
		if (tcontrol->hotkey[i]&PAlt && tcontrol->hotkey[i]&PControl) return(1);//match=1;
	}
	if (PStatus.flags&PAlt && !(PStatus.flags&PControl)) {  //alt not cntl
	//fprintf(stderr,"here2 \n");
		if (tcontrol->hotkey[i]&PAlt && !(tcontrol->hotkey[i]&PControl)) return(1);//match=1;
	}
	if (!(PStatus.flags&PAlt) && PStatus.flags&PControl) {  //cntl not alt
		if (!(tcontrol->hotkey[i]&PAlt) && tcontrol->hotkey[i]&PControl) return(1);//match=1;
	}
} //for i
*/


/*
//check Alt+key
if ((PStatus.flags&PAlt) && (tcontrol->altkey==key || tcontrol->altkey2==key)) {
	return(1);
}
if ((PStatus.flags&PControl) && (tcontrol->ctlkey==key || tcontrol->ctlkey2==key)) {
	return(1);
}
*/

/*
}  //hotkey=key
if ((tcontrol->hotkey2&0xff)==key) {
//if (!(tcontrol->hotkey2&(PControl|PAlt))) match=1;  //no alt or FTControl
if (!(tcontrol->hotkey2&PControl || tcontrol->hotkey2&PAlt)) return(1);//match=1;  //no alt or FTControl
if (PStatus.flags&PAlt && PStatus.flags&PControl) {  //both alt and cntl
  if (tcontrol->hotkey2&PAlt && tcontrol->hotkey2&PControl) return(1);//match=1;
}
if (PStatus.flags&PAlt && !(PStatus.flags&PControl)) {  //alt not FTControl
  if (tcontrol->hotkey2&PAlt && !(tcontrol->hotkey2&PControl)) return(1);//match=1;
}
if (!(PStatus.flags&PAlt) && PStatus.flags&PControl) {  //FTControl not alt
  if (!(tcontrol->hotkey2&PAlt) && tcontrol->hotkey2&PControl) return(1);//match=1;
}

} //hotkey2=key


return(0);
*/

}  //CheckHotKeyFTControl 




//int CheckHotKeyFTItem(FTItem *tFTItem,unsigned char key)
int CheckHotKeyFTItem(FTItem *tFTItem,KeySym key)
{

if (tFTItem!=0) {


if (tFTItem->hotkey[0]==key || tFTItem->hotkey[1]==key) {
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"Got item hotkey %c\n",(int)key);
	}
	return(1);
} 

} //tFTItem!=0

return(0);

/*
//the key matches - do the alt,cntl,none match?
if ((tFTItem->hotkey&0xff)==key) {

if (!(tFTItem->hotkey&(PControl|PAlt))) return(1);//match=1;  //no alt or FTControl
if (PStatus.flags&PAlt && key&PControl) {  //both alt and cntl
  if (tFTItem->hotkey&PAlt && tFTItem->hotkey&PControl) return(1);//match=1;
}
if (PStatus.flags&PAlt && !(key&PControl)) {  //alt not FTControl
  if (tFTItem->hotkey&PAlt && !(tFTItem->hotkey&PControl)) return(1);//match=1;
}
if (!(PStatus.flags&PAlt) && PStatus.flags&PControl) {  //FTControl not alt
  if (!(tFTItem->hotkey&PAlt) && tFTItem->hotkey&PControl) return(1);//match=1;
}

}  //hotkey=key


if ((tFTItem->hotkey2&0xff)==key) {
if (!(tFTItem->hotkey2&(PControl|PAlt))) return(1);//match=1;  //no alt or FTControl
if (PStatus.flags&PAlt && PStatus.flags&PControl) {  //both alt and cntl
  if (tFTItem->hotkey2&PAlt && tFTItem->hotkey2&PControl) return(1);//match=1;
}
if (PStatus.flags&PAlt && !(PStatus.flags&PControl)) {  //alt not FTControl
  if (tFTItem->hotkey2&PAlt && !(tFTItem->hotkey2&PControl)) return(1);//match=1;
}
if (!(PStatus.flags&PAlt) && PStatus.flags&PControl) {  //FTControl not alt
  if (!(tFTItem->hotkey2&PAlt) && tFTItem->hotkey2&PControl) return(1);//match=1;
}

} //hotkey2=key
} //tFTItem!=0


return(0);
*/
} //CheckHotKeyFTItem


#if 0 
void ButtonDownUpFTItem(FTControl *tcontrol,FTItem *tFTItem)
{
FTWindow *twin;
int ty;

//for now this function only handles button 0
//I think this is from hotkey keypress only
//also dropdown and filemenu enter key (and click?)

fprintf(stderr,"In ButtonDownUpFTItem\n");

      twin=GetFTWindowN(tcontrol->win);

      fprintf(stderr,"window=%s FTControl=%s\n",twin->name,tcontrol->name);

      PStatus.ButtonFTWindow[0]=twin;  
//      fprintf(stderr,"FT_SetFocus1\n");
      FT_SetFocus(twin,tcontrol); //give this FTControl the window focus
      tcontrol->flags|=CMouseOver;
//      ty=tcontrol->y1+(float)(tcontrol->y2-tcontrol->y1)*((float)tFTItem->i/(float)tcontrol->ilist->num);
      if (tcontrol->ilist->num>0) {
        //fprintf(stderr,"tcontrol y1=%d y2=%d\n",tcontrol->y1,tcontrol->y2);
//        ty=(float)(tcontrol->y2-tcontrol->y1)*((float)(tFTItem->i)/(float)tcontrol->ilist->num);
        ty=(float)tcontrol->y2*((float)(tFTItem->i)/(float)tcontrol->ilist->num);
        ty+=PStatus.PadY;  //tph
       //to put click in closer to center
      //dropdown has extra uncounted FTItem
//      ty+=(((tcontrol->type==CTDropdown)+1)*(tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.ascent)/2);  //to put click in closer to center
        ty+=tcontrol->font->max_bounds.ascent;  
        //fprintf(stderr,"tcontrol=%s tFTItem->i=%d ty=%d\n",tcontrol->name,tFTItem->i,ty);
      //should prbably be PadX
        CheckFTWindowButtonDown(twin,tcontrol->x1+tcontrol->font->max_bounds.width+1,ty,0);  //is only button 0 for now
        tcontrol->flags&=~CMouseOver;
        DrawFTControl(tcontrol); //this will also close a file menu
        CheckFTWindowButtonUp(twin,tcontrol->x1+tcontrol->font->max_bounds.width+1,ty,0);
      }  //nums>0
}  //end ButtonDownUpFTItem
#endif


//ButonDownUp is for hotkeys, and arrow keys when a FTControl needs a simulated click downup on
void ButtonDownUp(FTControl *tcontrol)
{
FTWindow *twin;
char winname[FTMedStr];
char conname[FTMedStr];

//for now only button 0 is handled
#if PINFO
fprintf(stderr,"In ButtonDownUp\n");
#endif

//twin=GetFTWindowN(tcontrol->win);
twin=tcontrol->window;
strcpy(winname,twin->name);
strcpy(conname,tcontrol->name);

PStatus.ButtonFTWindow[0]=twin;
//fprintf(stderr,"FT_SetFocus2\n");
FT_SetFocus(twin,tcontrol); //give this FTControl the window focus
//twin->focus=tcontrol->tab;
//CheckFTWindowMouseMove(tcontrol->win,tcontrol->x1+1,tcontrol->y1+1);
tcontrol->flags|=CMouseOver;
CheckFTWindowButtonDown(twin,tcontrol->x1+1,tcontrol->y1+1,0);  //button 0 only for now
twin=GetFTWindow(winname); //check to see if window was deleted
tcontrol=GetFTControl(conname); //check to see if control was deleted
if (twin==0 || tcontrol==0) {  //user could have deleted window in CheckFTButtonUp
	return;
} //twin==0
tcontrol->flags&=~CMouseOver;
DrawFTControl(tcontrol);
CheckFTWindowButtonUp(twin,tcontrol->x1+1,tcontrol->y1+1,0);  //button 0 for now
twin=GetFTWindow(winname); //check to see if window was deleted
tcontrol=GetFTControl(conname); //check to see if control was deleted
if (twin==0 || tcontrol==0) {  //user could have deleted window in CheckFTButtonUp
	return;
} //twin==0

} //end ButtonDownUp


/*
//Get the FileMenu FTControl that is open and is not a submenu
FTControl *GetTopMenu(FTWindow *twin)
{
FTControl *tcontrol;

if (twin->flags&WItemListOpen) {

  tcontrol=twin->icontrol;
  while(tcontrol!=0) {
    if (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu) && tcontrol->flags&CItemListOpen) return  (tcontrol);
    tcontrol=tcontrol->next;
  }  //end while
} 
else {
 fprintf(stderr,"No File Menu Windows open, but called GetTopMenu\n");
 return(0);
}
return(0);
} //end GetTopMenu
*/



//Open FTItemList for (dropdown or) filemenu (and submenu) FTControls
//Creates a new window for the itemlist
//and determines placement (based on if descended from control or itemlist)
//DrawFTItemListWindow() - Draws strings inside
//int OpenFTItemList(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
int OpenFTItemList(FTItemList *tlist)
{
FTWindow *twin,*twin2;
FTControl *tcontrol;
//int txw,tyh,tx,ty,cx,ex2;
//int x,y,width,height,fw,fh;
//FTItem *litem;//,*litem2;
FTWindow *nwin;
FTItemList *tlist2;
//#if WIN32
FTItem *titem;
//#endif
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
	fprintf(stderr,"in OpenFTItemList\n");
}

nwin=0;

//Note don't deal with setting/unsetting focus to/from a control here- that needs to be done earlier

//see if itemlist already has a window attached
if (tlist!=0) {

	//fprintf(stderr,"tlist!=0\n");
	PStatus.flags|=PItemListOpen;
	
	//PStatus.SelectedFTItemList must have a selection on it?
	PStatus.SelectedFTItemList=tlist;

	if (tlist->window==0) { //no FTWindow attached to this itemlist yet

		if (PStatus.flags&PInfo) {
			fprintf(stderr,"No window is attached to this itemlist yet\n");
		}
	//Create new window for ItemList
	//Dimensions of ItemList window are already in ItemList
			//needs name?
		nwin=(FTWindow *)malloc(sizeof(FTWindow));
		memset(nwin,0,sizeof(FTWindow));
		nwin->flags=WItemList;//|WExclusive;
		nwin->ilist=tlist;
		sprintf(tstr,"%s_s",tlist->control->name); //"FTControl"_s = name of window with items
		strcpy(nwin->name,tstr);
		nwin->icontrol=tlist->control; //save for LostFocus to close control itemlist windows
	//Determine position of new ItemList window
	//if from control - will start just under control
	//if from item - will start left or right of selected item
		//determine if this itemlist is descended from a control or another itemlist
//		if (tlist->control!=0) { //descended from control
//possibly needs to be better way of determining if itemlist parent is control
//similar thing exists with AddSubItem - perhaps should only be one AddItem(ItemList,item) 

//common to itemlist and subitemlist
			//fprintf(stderr,"Setting ExclusiveFTControl to %p\n",tcontrol);



		if (tlist->parent==0) { //descended from control
			tcontrol=tlist->control;
			twin=tcontrol->window;
			//twin->focus=tcontrol->tab;
			//Focus is set/unset in CheckFTButtonDown or wherever the selection is initiated before calling OpenFTItemList
			//if (twin->FocusFTControl!=0) { //wait to redraw the control that had focus until later
			//	tcontrol2=twin->FocusFTControl;
			//}
			twin->FocusFTControl=tcontrol;
			tcontrol->flags|=CItemListOpen;
			//fprintf(stderr,"Setting ExclusiveFTControl to %p\n",tcontrol);
			PStatus.ExclusiveFTWindow=twin;
			PStatus.ExclusiveFTControl=tcontrol;
			//PStatus.FocusFTWindow=twin;  //done with Windows message
//			PStatus.flags|=PItemListOpen;
			//control x,y is relative to control parent window x,y
#if Linux 
			nwin->x=tcontrol->x1+twin->x;
			nwin->y=tcontrol->y2+twin->y;		
#endif
#if WIN32
			if (tcontrol->type!=CTFileMenu) {
				nwin->x=tcontrol->x1+twin->x;
				nwin->y=tcontrol->y2+twin->y;		
				//other ItemList windows are WS_POPUP 
				//(because WS_CHILD windows are clipped to the parent window and this is a problem for low positioned drop down windows)
				//and so need to have their x,y adjusted
				//In Windows we need to add the Height and Width of the border
				nwin->x+=twin->WinXOffset;
				nwin->y+=twin->WinYOffset;
			} else {
				//FileMenus are currently WS_CHILD type and so don't need the windows offset added
				nwin->x=tcontrol->x1;
				nwin->y=tcontrol->y2;		
			}
#endif
	
			//verify width and height of new window are >0
			if (tlist->width==0 || tlist->height==0) {
				//Determine file menu list size
//#if WIN32
				if (tlist->font==0 || tlist->fontwidth==0 || tlist->fontheight==0) {
					//tlist->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
					tlist->font=tcontrol->font;
					tlist->fontwidth=tcontrol->fontwidth;
					tlist->fontheight=tcontrol->fontheight;
#if WIN32
					//tlist->textmet=tcontrol->textmet;
					memcpy(&tlist->textmet,&tcontrol->textmet,sizeof(TEXTMETRIC));
#endif				
				} //tlist->font=0
				//go through list to determine maximum width
				titem=tlist->item;
				tlist->width=0;
				tlist->height=0;
				while(titem!=0) {
					if ((int)strlen(titem->name)>tlist->width) {
						tlist->width=FTstrlen(titem->name);  //excludes &
					}
					tlist->height+=tlist->fontheight+PStatus.PadY;
					titem=titem->next;
				}//while(titem!=0) {
				//tlist->width+=2;
				//tlist->height+=PStatus.PadY;
				tlist->width*=tlist->fontwidth+PStatus.PadX;
				if (tlist->width<tcontrol->x2-tcontrol->x1) {
					tlist->width=tcontrol->x2-tcontrol->x1;
				}
//#endif				
			} //if (tlist->width==0 || tlist->height==0) {

//			fprintf(stderr,"ItemList descended from control %s at (%d,%d)\n",tcontrol->name,nwin.x,nwin.y);
//			nwin.z=0; 
			nwin->z=twin->z-1;
//			nwin.focus=tcontrol->tab; //set window focus to this control
			DrawFTControl(tcontrol); //to show control as selected
			//if control is a file menu indicate to parent window of filemenu control that a filemenu is open
/*
			if (tcontrol->type==CTFileMenu) {
				//set WFileMenuOpen bit so that CheckFTWindowMouseMove will know that there is a filemenu open
//				twin->flags|=WFileMenuOpen;
				nwin.flags|=WFileMenuOpen;			
			}
*/
		} else { //if (tlist->parent==0) { //if descended from control
			
			//this itemlist descended from another itemlist (is a submenu)
			//get x from window of parent itemlist - determine to open right or left to keep on screen
			//get y from selected item y 
			//? PStatus.ExclusiveFTWindow=twin;
			tlist2=tlist->parent;
			if (tlist2==0) {
				fprintf(stderr,"Error: ItemList has no control or itemlist parent.\n");
			}	else {
				//get x position from window of parent
				twin2=tlist2->window;
				if (twin2==0) {
					//fprintf(stderr,"Parent ItemList for this ItemList has no window.\n");
					nwin->x=0;
					nwin->y=0;
				} else {
					sprintf(tstr,"%s_%d",twin2->name,tlist2->cur); //"FTControl_s"_# = name of window with subitems
					strcpy(nwin->name,tstr);
#if Linux
					nwin->x=twin2->x+twin2->w-PStatus.PadX;
					nwin->y=twin2->y+(tlist2->cur>1); //I don't know why we need to add 1 pixel here - perhaps PadY
#endif
#if WIN32
			if (tlist->control!=0 && tlist->control->type!=CTFileMenu) {
				//other ItemList windows are WS_POPUP (because WS_CHILD windows are clipped by the parent window) and so need to have their x,y adjusted
				nwin->x=tlist->window->x+twin2->x+twin2->w-PStatus.PadX;
				nwin->y=tlist->window->y+twin2->y+(tlist2->cur>1);		
				//In Windows we need to add the Height and Width of the border
				nwin->x+=twin2->WinXOffset; 
				nwin->y+=twin2->WinYOffset;
			} else {
				//FileMenus are currently WS_CHILD type and so don't need the windows offset added
				nwin->x=twin2->x+twin2->w-PStatus.PadX;
				nwin->y=twin2->y+(tlist2->cur>1);		
			}
#endif
					nwin->z=twin2->z-1;  //the lower the number, the higher the window 
				}
				//get y position from current selected item in itemlist
				nwin->y+=(tlist2->cur-1)*(tlist->fontheight+PStatus.PadY);										
			} //titem==0
		} //titemlist->parent==0 //titemlist->control!=0 


		nwin->w=tlist->width;
		nwin->h=tlist->height;

		//if a dropdown control itemlist window, adjust the window width and height if the itemlist is too small or large
		tcontrol=tlist->control;
		if (tcontrol!=0) {
			if (tcontrol->type==CTDropdown) {
				if (tlist->width<tcontrol->x2-tcontrol->x1) {
					nwin->w=tcontrol->x2-tcontrol->x1;
				}
				//adjust for height to
			} //if (tcontrol->type==CTDropdown) {
		} //if (tcontrol!=0) {

			//fprintf(stderr,"ItemList width=%d height=%d\n",tlist->width,tlist->height);


		nwin->font=tlist->font; //new window inherits font of itemlist (which inherits font from control)
		nwin->fontwidth=tlist->fontwidth;
		nwin->fontheight=tlist->fontheight;
#if WIN32
		//nwin->textmet=tlist->textmet;
		memcpy(&nwin->textmet,&tlist->textmet,sizeof(TEXTMETRIC));
#endif

	//Create Window for ItemList
		CreateFTWindow(nwin);	

	//Get pointer to newly created ItemList window
		twin=GetFTWindow(tstr);
		//connect this window to the itemlist
		//fprintf(stderr,"Got itemlist window %s= %p\n",twin->name,twin);
		//fprintf(stderr,"Got itemlist %p\n",twin);
		tlist->window=twin;

		if (twin!=0) { 
			DrawFTWindow(twin);  //(Open and) Draw ItemList Window
		}

	} else { //tlist->window==0
		//itemlist already has window (was created earlier- but may not be open/visible)
		//if window is not open, open
		//fprintf(stderr,"Itemlist has window\n");
		//PStatus.FocusFTWindow=tlist->window; //done with Windows message
		if (!(tlist->window->flags&WOpen)) {

			//fprintf(stderr,"but window is not yet open\n");
//PStatus.ExclusiveFTWindow=twin;?

			if (tlist->parent==0) { //descended from control
				//fprintf(stderr,"tlist has no parent\n");
				tcontrol=tlist->control;
				twin=tcontrol->window;
				//twin->focus=tcontrol->tab; //set window focus to filemenu/dropdown control
				//note focus is already set in CheckFTButtonDown
				//twin->FocusFTControl=tcontrol;
//				twin->flags|=WFileMenuOpen;
				//fprintf(stderr,"Setting ExclusiveFTControl to %p\n",tcontrol);
				PStatus.ExclusiveFTWindow=twin;
				PStatus.ExclusiveFTControl=tcontrol;
				PStatus.flags|=PItemListOpen;
				tcontrol->flags|=CItemListOpen;
				//adjust to window if window has moved
				twin2=tlist->window;
#if Linux
				twin2->x=twin->x+tcontrol->x1;
				twin2->y=twin->y+tcontrol->y2;		
#endif
#if WIN32
			if (tcontrol->type!=CTFileMenu) {
				twin2->x=tcontrol->x1+twin->x;
				twin2->y=tcontrol->y2+twin->y;		
				//other ItemList windows are WS_POPUP 
				//(because WS_CHILD windows are clipped to the parent window and this is a problem for low positioned drop down windows)
				//and so need to have their x,y adjusted
				//In Windows we need to add the Height and Width of the border
				twin2->x+=twin->WinXOffset;
				twin2->y+=twin->WinYOffset;
			} else {
				//FileMenus are currently WS_CHILD type and so don't need the windows offset added
				twin2->x=tcontrol->x1;
				twin2->y=tcontrol->y2;		
			}
#endif

				//				twin2->z=0;
				twin2->z=twin->z-1;  
	//			fprintf(stderr,"ItemList descended from control %s at (%d,%d)\n",tcontrol->name,nwin.x,nwin.y);
				DrawFTControl(tcontrol); //to show control as selected
			} else { //parent==0 - descended from control

				//itemlist with window already descended from another itemlist (is a submenu)
				//fprintf(stderr,"tlist does have parent\n");
				tlist2=tlist->parent;								
				twin2=tlist2->window;
				twin=tlist->window;
#if Linux
				twin->x=twin2->x+twin2->w;
				twin->y=twin2->y+1*(tlist2->cur>1);			
#endif
#if WIN32
			if (tlist->control!=0 && tlist->control->type!=CTFileMenu) {
				//other ItemList windows are WS_POPUP (because WS_CHILD windows are clipped by the parent window) and so need to have their x,y adjusted
				twin->x=tlist2->window->x+twin2->x+twin2->w-PStatus.PadX;
				twin->y=tlist2->window->y+twin2->y+(tlist2->cur>1);		
				//In Windows we need to add the Height and Width of the border
				twin->x+=twin2->WinXOffset;
				twin->y+=twin2->WinYOffset;
			} else {
				//FileMenus are currently WS_CHILD type and so don't need the windows offset added
				twin->x=twin2->x+twin2->w-PStatus.PadX;
				twin->y=twin2->y+(tlist2->cur>1);		
			}
#endif
				//add the correct height offset depending on which item is selected
				twin->y+=(tlist2->cur-1)*(tlist->fontheight+PStatus.PadY);																	
			} //parent==0
			DrawFTWindow(tlist->window);
		} else { //WOpen
			//fprintf(stderr,"WOpen still set on tlist->window\n");
		} //tlist->window->flags&WOpen

		if (nwin!=0) {
			free(nwin);
		}

	} //tlist->window==0


} //tlist!=0


if (PStatus.flags&PInfo) {
	fprintf(stderr,"Exit OpenFTItemList\n");
}

return 1;  //no FTItem clicked on
}  //end OpenFTItemList



//DrawFTItemList: is called from DrawWindow (or DrawControls if the item list is a control) and:
// Draws background and fills in the ItemList text
// DrawFTItemList only draws a single ItemList window - it does not recursively redraws the text in all open subitemlist windows too - this is done by DrawFTItemListAll()
// Because when mouse moves over new item - we only want to redraw the individual subitemlist
//OpenItemList creates a window and determines the position of the window 
//(the size of the ItemList window is calculated when each item is added or deleted)
//DrawFTItemList just fills in text
//perhaps all itemlists should just be viewed as controls, and a control added to any itemlist window
void DrawFTItemList(FTItemList *tlist) 
{
FTWindow *twin;
int i,cy,fh;
FTItem *titem;
FTControl *tcontrol;
#if WIN32
RECT ilrect;
#endif

if (PStatus.flags&PInfo) {
	fprintf(stderr,"DrawFTItemList\n");
}



if (tlist!=0) { //there is a tlist

	//Get Window that this Item List is attached to
//	fprintf(stderr,"getting tlist->window\n");
	twin=tlist->window;
//	fprintf(stderr,"got tlist->window\n");
	if (twin==0) {
		//if a control try to get the window from the control
		//note that a FileMenu won't open - it needs to not have a window
		
		if (tlist->control!=0 && tlist->control->type==CTItemList) { //if itemlist control just add control's window
			tlist->window=tlist->control->window;
			twin=tlist->window;
		} else {
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"No window attached to ItemList yet\n");
			}
			return;
		} //if (tlist->control!=0 && tlist->control->window!=0) {
			//fprintf(stderr,"Error: No window attached to ItemList yet\n");		
	} //twin==0


	//fill in text items
//for now each item takes the font of the itemlist
	titem=tlist->item;
	

	i=1;
	fh=tlist->fontheight;
	cy=fh-PStatus.PadY;
#if WIN32
	cy=0;
#endif

	tcontrol=0; //to avoid compiler warning about tcontrol being possibly used uninitialized in second if (tlist->flags&FTIL_IS_IN_A_CONTROL) below
	if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //itemlist is a control
		tcontrol=tlist->control;
		cy+=tlist->control->y1;
		titem=GetFTItemFromFTItemListByNum(tlist,tcontrol->FirstItemShowing);  //scrollbar changes
		i=tcontrol->FirstItemShowing; //scrollbar changes
#if WIN32
			//cy+=twin->WinYOffset;  //Windows titlebar, border, etc. are added to Window size on button clicks, and drawing routines like FillRect, etc.
#endif
	}


	//draw background because sometimes there are not enough items to fill
#if Linux
	//fill in background
	//XSetForeground(PStatus.xdisplay, twin->xgc,tcontrol->textcolor[1]);
	XSetForeground(PStatus.xdisplay,twin->xgc,white);
	if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //only fill control
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-tcontrol->x1,tcontrol->y2-tcontrol->y1);
	} else {  //fill entire window
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,0,twin->w,twin->h);
	}
	XSetForeground(PStatus.xdisplay,twin->xgc,black);
#endif //Linux
#if WIN32
	//twin->hdcWin=GetWindowDC(twin->hwindow);  //for some reason twin->hdcWin becomes an invalid handle occassionally
	//GetWindowDC gets a DC that includes the windows titlebar, etc. and so causes this drawing to be too high
	
	if (twin->hdcWin==0) {
		twin->hdcWin=GetDC(twin->hwindow);
	} else {  //free the DC and get a new one - or else we can't draw on the window
		ReleaseDC(twin->hwindow,twin->hdcWin);
		twin->hdcWin=GetDC(twin->hwindow);
	}
	

	SelectObject(twin->hdcWin,tlist->font); //apparently important to select a font- otherwise it might be selected to something else
	SelectObject(twin->hdcWin,GetStockObject(WHITE_BRUSH));
	GetClientRect(twin->hwindow,&ilrect);
	if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //only fill control
		ilrect.left=tcontrol->x1;
		ilrect.top=tcontrol->y1;
		ilrect.right=tcontrol->x2;
		ilrect.bottom=tcontrol->y2;		
		//ilrect.top+=twin->WinYOffset;
		//ilrect.bottom+=twin->WinYOffset;
	}//if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //only fill control
	FillRect(twin->hdcWin,&ilrect,(HBRUSH)GetStockObject(WHITE_BRUSH));
#endif

	//tlist->flags|=FTIL_NO_BACKGROUND_FILL; //since we already filled the white box above, don't refill in drawing items
	//	fprintf(stderr,"before loop\n");
	while(titem!=0) {
		//if (tlist->cur==i) {  //this item is selected
		if (titem->flags&FTI_SELECTED) {
			DrawSelectedFTItem(tlist,i);
		} else {
			DrawUnselectedFTItem(tlist,i);
		}

		i++;
		cy+=fh;
		cy+=PStatus.PadY;
		titem=titem->next;
#if Linux
		if (tlist->flags&FTIL_IS_IN_A_CONTROL && cy>=tlist->control->y2) { //itemlist is a control and so may be clipped
#endif
#if WIN32
		if (tlist->flags&FTIL_IS_IN_A_CONTROL && cy>=(tlist->control->y2-fh-PStatus.PadY)) { //itemlist is a control and so may be clipped
#endif
			tlist->control->numvisible=i-tlist->control->FirstItemShowing; //for vertical scroll bar
			titem=0; //exit while
		}
	} //while(titem!=0)

	//tlist->flags&=~FTIL_NO_BACKGROUND_FILL; //reset background fill flag for other functions that DrawSelectedFTItem without drawing the complete itemlist

} //tlist!=0

if (PStatus.flags&PInfo) {
	fprintf(stderr,"Exit DrawFTItemList\n");
}

}  //DrawFTItemList


//Looks better to draw only selected and unselected item - otherwise flicker is seen
//This function also sets PStatus.SelectedFTItem= to the current FTItem
//DrawSelectedItem
void DrawSelectedFTItem(FTItemList *tlist,int num) 
{
FTWindow *twin;
FTControl *tcontrol;
FTItem *titem;
int cx,cy,fw,fh;
unsigned int ci;
#if WIN32
POINT tp[2];
RECT ilrect; //rectangle for each item
#endif
int maxchar,numchars;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"DrawSelectedFTItem\n");
}

if (tlist!=0 && num>0) { //there is a tlist
	//Get Window that this Item List is attached to
	twin=tlist->window;
	if (twin==0) {
		fprintf(stderr,"Error: No window attached to ItemList\n");
	} //twin==0


//get item
	titem=GetFTItemFromFTItemListByNum(tlist,num);
	if (titem!=0) {  //an item is returned
//draw selected item text
//get dimensions of item
		PStatus.SelectedFTItem=titem;
		PStatus.SelectedFTItemList=tlist;
		fw=tlist->fontwidth;
		fh=tlist->fontheight;

		//cx=0;
		cx=PStatus.PadX;
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
			cx+=tlist->control->x1;
			if (tlist->control->type==CTFileOpen) {  //adjust to folder and file image
				cx=tlist->control->x1+tlist->control->fontheight*2;//+PStatus.PadX; //because icons are square based on height 2 characters of padding (no PadX)
			}
		}
#if Linux
		cy=fh*num+PStatus.PadY*(num-1);
#endif
#if WIN32
		cy=(fh+PStatus.PadY)*(num-1);
#endif
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //itemlist is a control
			cy+=tlist->control->y1;
			cy-=(tlist->control->FirstItemShowing-1)*(fh+PStatus.PadY);  //if scrolled
			//cx=tlist->control->x1;
			if (tlist->control->type==CTFileOpen) {  //todo: adding tlist->padding so this can be changed for any itemlist
				cy-=(num-1)*(PStatus.PadY);  //FileOpen doesn't add PadY
			}
		}


		//set control text to selected item, call OnChange function if any
		tcontrol=tlist->control;//PStatus.ExclusiveFTControl;
		//fprintf(stderr,"tcontrol=%p\n",tcontrol);
		if (tcontrol!=0) {
			//fprintf(stderr,"titem->name=%s\n",titem->name);
			if (tcontrol->type==CTFileOpen ||  tcontrol->type==CTItemList) { //tcontrol->type==CTDropdown 
				//not until the item is clicked on for dropdown- otherwise text appears in a dropdown that hasn't been clicked on but was only moused over
				//should be possibly SelectedText and SelectedItem
				strcpy(tcontrol->text,titem->name); //tcontrol->text gets both filename and folder name (for CTFileOpen)
				//but  not CTFileMenu
			}
			if (tcontrol->type==CTFileOpen) {
				if (!(titem->flags&FTI_FOLDER)) {
					//fprintf(stderr,"filename titem->name=%s\n",titem->name);
					strcpy(tcontrol->filename,titem->name); //filename gets only files (for CTFileOpen)
					//if a textbox in the same window is linked to this FileOpen control's selected filename update it's file name text
					if (tcontrol->TextBoxFile!=0) {
						strcpy(tcontrol->TextBoxFile->text,tcontrol->filename);
						DrawFTControl(tcontrol->TextBoxFile); //redraw with new text
					} //if (tcontrol->TextBoxFile!=0) {
				} //				if (!(titem->flags&FTI_FOLDER)) {
			} //if (tcontrol->type==CTFileOpen) {
			if (tcontrol->type!=CTDropdown) {
				if (tcontrol->OnChange!=0) {  //needs to happen only after the itemlist has closed for a dropdown control
					(*tcontrol->OnChange) (twin,tcontrol);
				} //tcontrol->OnChange
			} //if (tcontrol->type!=CTItemList) {
		} //tcontrol!=0



	//	if (!(tlist->flags&FTIL_NO_BACKGROUND_FILL)) {  //if this is called from DrawFTItemList then we don't need to redraw the white background
			//draw selected background
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,blue);  
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,tcontrol->x2-tcontrol->x1,fh+PStatus.PadY);			
			} else {	
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			}
			XSetForeground(PStatus.xdisplay,twin->xgc,white);  
#endif
#if WIN32
			//twin->hdcWin=GetWindowDC(twin->hwindow);  //for some reason twin->hdcWin becomes an invalid handle occassionally
			if (twin->hdcWin==0) {
				twin->hdcWin=GetDC(twin->hwindow);
			} else {  //free the DC and get a new one - or else we can't draw on the window
				ReleaseDC(twin->hwindow,twin->hdcWin);
				twin->hdcWin=GetDC(twin->hwindow);
			}

			SelectObject(twin->hdcWin,tlist->font); //important to select a font or else a different one might be selected
			SetDCBrushColor(twin->hdcWin,blue);
			SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
			//GetClientRect(twin->hwindow,&twin->rect);
			ilrect.top=cy;
			ilrect.bottom=cy+fh+PStatus.PadY;
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				ilrect.left=cx;//tlist->control->x1;
				ilrect.right=tlist->control->x2-1;//ilrect.left+tlist->width;
//				ilrect.top+=twin->WinYOffset;
//				ilrect.bottom+=twin->WinYOffset;
				if (tlist->control->type==CTFileOpen) {
					//ilrect.top+=PStatus.PadY;
					//ilrect.left-=PStatus.PadX; //needs to be to the left more
					ilrect.bottom-=PStatus.PadY; //fileopen items have no PadY because they can't have hot keys					
				}				
			} else {
				ilrect.left=0;
				ilrect.right=ilrect.left+tlist->window->w;//ilrect.left+tlist->width;
			}			
			FillRect(twin->hdcWin,&ilrect,(HBRUSH)GetStockObject(DC_BRUSH));
			//FillRect(twin->hdcWin,&twin->rect,twin->brush);
			//DeleteObject(twin->brush);
			//twin->brush=CreateSolidBrush(white); 
			//SelectObject(twin->hdcWin,twin->brush);
			SelectObject(twin->hdcWin,GetStockObject(WHITE_BRUSH));
		
			SetBkMode(twin->hdcWin,TRANSPARENT); //so no text background color
			//twin->pen=CreatePen(PS_SOLID,1,black);
			//SelectObject(twin->hdcWin,twin->pen);
			SelectObject(twin->hdcWin,GetStockObject(WHITE_PEN)); //so line will be white too
			SetTextColor(twin->hdcWin,white);
			//SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);	
			//SetBkColor(twin->hdcWin,tcontrol->color[0]);
#endif
	//	} //if (!(tlist->flags&FTIL_NO_BACKGROUND_FILL)) {

		ci=0;
		maxchar=(tlist->width-PStatus.PadX)/fw;		//PadX*2?
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
			ci=tcontrol->FirstItemCharacterShowing;
			maxchar=(tlist->control->x2-tlist->control->x1-PStatus.PadX)/fw;
		}
		//cx=PStatus.PadX;

//		fprintf(stderr,"item name = %s\n",titem->name);
//		fprintf(stderr,"item name = %s\n",titem->name);
		//probably drawing each character should only be done for a FileMenu control

		if (tlist->flags&FTIL_HAS_NO_HOT_KEYS) {
			if (strlen(titem->name)>maxchar) {
				numchars=maxchar;
			} else {
				numchars=strlen(titem->name);
			}
#if Linux
			//XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,titem->name,numchars);
			XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,&titem->name[ci],numchars);
#endif
#if WIN32
			MultiByteToWideChar(CP_UTF8,0,(LPCSTR)titem->name,numchars,(LPWSTR)titem->name_16,FTMedStr);
			TextOut(twin->hdcWin,cx,cy,(LPWSTR)&titem->name_16[ci],numchars);
#endif
		} else { //have to draw each character in case of an underline

			if (FTstrlen(titem->name)>maxchar) {
				numchars=maxchar;
			} else {
				numchars=strlen(titem->name);
			}


			//check for maximum character unecessary - is done in addFTItem- update: check if item text is trimmed in addFTItem- because should not be
			//while(ci<strlen(titem->name)) { //for each character
			while(ci<numchars) { //for each character

			  if (titem->name[ci]==0x26) { //& underline
#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy+1,cx+fw,cy+1);
#endif
#if WIN32
					tp[0].x=cx;	tp[0].y=cy+fh;	tp[1].x=cx+fw;	tp[1].y=cy+fh; //+fh because Win text origin is top -left, Xwin bottom-left
					Polyline(twin->hdcWin,tp,2);
#endif
				} else {  //do not draw & symbol
#if Linux
					XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,&titem->name[ci],1);
#endif
#if WIN32
					MultiByteToWideChar(CP_UTF8,0,(LPCSTR)&titem->name[ci],1,(LPWSTR)titem->name_16,FTMedStr);
					TextOut(twin->hdcWin,cx,cy,(LPWSTR)titem->name_16,1);
#endif
					cx+=fw;
				}
			ci++;
			} //while ci<strlen - for each character		
		} //		if (tlist->flags&FTIL_HAS_NO_HOT_KEYS) {
	} //titem!=0
//reset color
//			XSetForeground(PStatus.xdisplay,twin->xgc,black);  
//			XSetBackground(PStatus.xdisplay,twin->xgc,white);
#if WIN32
		//DeleteObject(twin->brush);
		//DeleteObject(twin->pen);
				//in Windows we need to refresh the window
		//SendMessage(twin->hwindow,WM_PAINT,0,0L); 
	GdiFlush();
#endif


} //tlist!=0

if (PStatus.flags&PInfo) {
	fprintf(stderr,"Exit DrawSelectedFTItem\n");
}
}  //DrawSelectedFTItem


//DrawUnSelectedItem
//Looks better to draw only selected and unselected item - otherwise flicker is seen
void DrawUnselectedFTItem(FTItemList *tlist,int num) 
{
FTWindow *twin;
FTItem *titem;
int cx,cy,fw,fh;
unsigned int ci;
int maxchar,numchars;
#if WIN32
//LPVOID lpMsgBuf;
DWORD dw; 
char tstr[FTMedStr];
POINT tp[2];
RECT ilrect; //item list rectangle
#endif


if (PStatus.flags&PInfo) {
	fprintf(stderr,"DrawSelectedFTItem\n");
}

if (tlist!=0 && num>0) { //there is a tlist
	//Get Window that this Item List is attached to
	twin=tlist->window;
	if (twin==0) {
		fprintf(stderr,"Error: No window attached to ItemList\n");
	} //twin==0


//get item
	titem=GetFTItemFromFTItemListByNum(tlist,num);
	if (titem!=0) {  //an item is returned
//draw selected item text
//get dimensions of item
		fw=tlist->fontwidth;
		fh=tlist->fontheight;

		//cx=0;
		cx=PStatus.PadX; //add padding for text
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
			cx+=tlist->control->x1;
			if (tlist->control->type==CTFileOpen) {  //adjust to folder and file image
				cx=tlist->control->x1+tlist->control->fontheight*2;//+PStatus.PadX; //because icons are square based on height 2 characters of padding (no PadX)
			}
		}
#if Linux
		cy=fh*num+PStatus.PadY*(num-1);
#endif
#if WIN32
		cy=(fh+PStatus.PadY)*(num-1);
#endif
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //itemlist is a control
			cy+=tlist->control->y1;
			cy-=(tlist->control->FirstItemShowing-1)*(fh+PStatus.PadY);  //if scrolled
			if (tlist->control->type==CTFileOpen) {  //todo: adding tlist->padding so this can be changed for any itemlist
				cy-=(num-1)*(PStatus.PadY);  //FileOpen doesn't add PadY
			}
			//cx=tlist->control->x1;
		}


		//if (!(tlist->flags&FTIL_NO_BACKGROUND_FILL)) {  //if this is called from DrawFTItemList then we don't need to redraw the white background
			//draw unselected background
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,white);  
			//XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy-fh,tlist->width,fh+PStatus.PadY);  //width may be twin->w or tcontrol width
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,tlist->control->x2-tlist->control->x1,fh+PStatus.PadY);			
			} else {	
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			}

			//XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			XSetForeground(PStatus.xdisplay,twin->xgc,black);  
#endif
#if WIN32
			//for some reason the hdcWin occassionally becomes an invalid handle
			//possibly because this is not in an WM_PAINT interval
			//possibly calling this comman brings this windows DC back as the default DC?
			//this was a nasty error that took a couple days to solve, and I still don't understand it
			//twin->hdcWin=GetWindowDC(twin->hwindow);
			//GetWindowDC gets a DC that includes the windows titlebar, etc. and so causes this drawing to be too high
			//I had to resort to the below GetDC because GetWindowDC causes the drawing to be too high because it includes windows decorations (titlebar, etc)
			//The GetWindowDC function description states: "After painting is complete, the ReleaseDC function must be called to release the device context. Not releasing the window device context has serious effects on painting requested by applications."
			if (twin->hdcWin==0) {
				twin->hdcWin=GetDC(twin->hwindow);
			} else {  //free the DC and get a new one - or else we can't draw on the window
				ReleaseDC(twin->hwindow,twin->hdcWin);
				twin->hdcWin=GetDC(twin->hwindow);
			}


			SetBkMode(twin->hdcWin,TRANSPARENT); //so no text background color
			//SetBkColor(twin->hdcWin,tcontrol->color[0]);
			//twin->brush=CreateSolidBrush(white); //fillrect is done with brush
			//SelectObject(twin->hdcWin,twin->brush);
			//SetDCBrushColor(twin->hdcWin,white);
			//SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
			SelectObject(twin->hdcWin,tlist->font); //important to select a font or a different one might be selected
			SelectObject(twin->hdcWin,GetStockObject(WHITE_BRUSH));
			GetClientRect(twin->hwindow,&twin->rect);
			ilrect.top=cy;
			ilrect.bottom=cy+fh+PStatus.PadY;
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				ilrect.left=cx;//tlist->control->x1;
				ilrect.right=tlist->control->x2-1;//ilrect.left+tlist->width;
//				ilrect.top+=twin->WinYOffset;
//				ilrect.bottom+=twin->WinYOffset;
				if (tlist->control->type==CTFileOpen) {
					//ilrect.top+=PStatus.PadY;
					//ilrect.left-=PStatus.PadX; //needs to be to the left more
					ilrect.bottom-=PStatus.PadY; //fileopen items have no PadY because they can't have hot keys					
				}				
			} else {
				ilrect.left=0;
				ilrect.right=ilrect.left+tlist->window->w;//ilrect.left+tlist->width;
			}			


			//	FillRect(twin->hdcWin,&twin->rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
			//if (!FillRect(twin->hdcWin,&twin->rect,twin->brush)) {
			if (!FillRect(twin->hdcWin,&ilrect,(HBRUSH)GetStockObject(WHITE_BRUSH))) {
			//This is a really unusual error- for some reason the Windows DC that I initially create becomes an invalid handle
			//-when I use GetWindowDC, at the occurrance of the usual invalid handle error, is now a change in font
			//possibly this was because I didn't SelectObject font above- now that I do there is no occassional error
			//if (!FillRect(GetWindowDC(twin->hwindow),&twin->rect,GetStockObject(WHITE_BRUSH))) {
				dw=GetLastError(); 
				sprintf(tstr,"FillRect failed GetLastError=%d\n",dw);
				FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);
			}
	
			//DeleteObject(twin->brush);
	//		twin->brush=CreateSolidBrush(black); 
	//		SelectObject(twin->hdcWin,twin->brush);
			//twin->pen=CreatePen(PS_SOLID,1,black);  //polyline is done with pen
			//SelectObject(twin->hdcWin,twin->pen);
			SelectObject(twin->hdcWin,GetStockObject(BLACK_BRUSH));  
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			SetTextColor(twin->hdcWin,black);
			//SetBkColor(twin->hdcWin,white);	
			//In Windows the font origin is top left, in XWindows bottom left
			//cy-=fh;
	//		SetBkMode(twin->hdcWin,TRANSPARENT); //so no text background color
			//SetBkColor(twin->hdcWin,tcontrol->color[0]);
#endif
//		} //if (!(tlist->flags&FTIL_NO_BACKGROUND_FILL)) {  //if this is called from DrawFTItemList then we don't need to redraw the white background
//sometimes an item is unselected but not selected like when a top level filemenu control changes with the arrow keys
//			if (tcontrol->OnChange!=0) {
//				(*tcontrol->OnChange) (twin,tcontrol);
//			} //tcontrol->OnChange

		ci=0;

		maxchar=(tlist->width-PStatus.PadX)/fw;		
		if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
			maxchar=(tlist->control->x2-tlist->control->x1-PStatus.PadX)/fw;
			ci=tlist->control->FirstItemCharacterShowing;
		}
		//cx+=PStatus.PadX; //add padding for text

//		fprintf(stderr,"item name = %s\n",titem->name);
		if (strlen(titem->name)>maxchar) {
			numchars=maxchar;
		} else {
			numchars=strlen(titem->name);
		}

		if (tlist->flags&FTIL_HAS_NO_HOT_KEYS) {
#if Linux
			XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,&titem->name[ci],numchars);
#endif
#if WIN32
			MultiByteToWideChar(CP_UTF8,0,(LPCSTR)&titem->name[ci],numchars,(LPWSTR)titem->name_16,FTMedStr);
			TextOut(twin->hdcWin,cx,cy,(LPWSTR)titem->name_16,numchars);
#endif
		} else { //have to draw each character in case of an underline

			//check for maximum character unecessary - is done in addFTItem
			while(ci<strlen(titem->name)) { //for each character

			  if (titem->name[ci]==0x26) { //& underline
	#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy+1,cx+fw,cy+1);
	#endif
	#if WIN32
					//tp[0].x=cx;	tp[0].y=cy+fh;	tp[1].x=cx+fw;	tp[1].y=cy+fh; //+fh because Win text origin is top -left, Xwin bottom-left
					tp[0].x=cx;	tp[0].y=cy+fh;	tp[1].x=cx+fw;	tp[1].y=cy+fh; //+fh because Win text origin is top -left, Xwin bottom-left
					Polyline(twin->hdcWin,tp,2);
	#endif
				} else {  //do not draw & symbol
	#if Linux
					XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,&titem->name[ci],1);
	#endif
	#if WIN32
					MultiByteToWideChar(CP_UTF8,0,(LPCSTR)&titem->name[ci],1,(LPWSTR)titem->name_16,FTMedStr);
					TextOut(twin->hdcWin,cx,cy,(LPWSTR)titem->name_16,1);
	#endif
					cx+=fw;
				}
			ci++;
			} //while ci<strlen - for each character		
		} //if (tlist->flags&FTIL_HAS_NO_HOT_KEYS) {
	} //titem!=0
//reset color
//			XSetForeground(PStatus.xdisplay,twin->xgc,black);  
//			XSetBackground(PStatus.xdisplay,twin->xgc,white);


#if WIN32
		//DeleteObject(twin->brush);
		//DeleteObject(twin->pen);
		//in Windows we need to refresh the window
		//SendMessage(twin->hwindow,WM_PAINT,0,0L); //causes submenu item to appear selected again
		GdiFlush();
#endif

	} //tlist!=0

if (PStatus.flags&PInfo) {
	fprintf(stderr,"Exit DrawSelectedFTItem\n");
}
}  //DrawUnselectedFTItem


//HideFTItemList
//HideFTItemListAll
//CloseFTItemList
//CloseFTItemListAll
//CloseFTItemList replaces EraseMenu
//Closes and destroys the windows of this itemlist and all subitemlists
//but does not destroy the itemlists themselves, that is done in DelItem (or DestroyFTWindow and DelFTControl)
void CloseFTItemList(FTItemList *tlist) 
{
FTWindow *twin;//,*twin2;
FTControl *tcontrol;
FTItem *titem;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"CloseFTItemList\n");
}

//make sure this tlist exists, has a window, and that the window is open before closing it
if (tlist!=0 && tlist->window!=0 && (tlist->window->flags&WOpen)) {
		//PStatus.flags&=~PCloseItemListOnUp; //just to make sure this bit gets cleared

/*
	if (tlist->parent==0) {
		//this is a top level filemenu or dropdown control itemlist
		tcontrol=tlist->control; //get parent control
		if (tcontrol->type==CTFileMenu) {
			twin=tcontrol->window;
			twin->flags&=~WFileMenuOpen; //erase filemenuopen bit in window control belongs to- so that CheckFTWindowMouseMove will know that there is no filemenu open
		}
	} //tlist->parent==0
*/

	//see if any items have subitemlists that have open windows
	//and close them recursively
	titem=tlist->item;
	while(titem!=0) {
		if (titem->ilist!=0) {  //item has subitemlist
			CloseFTItemList(titem->ilist); //recursively close subitemlist windows
		} //titem->ilist!=0	
		titem->flags&=~FTI_SELECTED; //unselect any selected item
		titem=titem->next;
	} //titem!=0
 
//			fprintf(stderr,"Close itemlist tlist->window=%p\n",tlist->window);
	if (tlist->window!=0) {	//there is an FTWindow attached to this tlist
		twin=tlist->window;		
//			fprintf(stderr,"Close itemlist window0 %s\n",twin->name);
		if (twin->flags&WOpen) {
			//fprintf(stderr,"Close itemlist window %s\n",twin->name);
			CloseFTWindow(twin); //erases WOpen bit
			//the twin is closed but not deleted
			//important - if using model of CloseFTWindow does not call DestroyFTWindow - 
			//tlist->window=0 needs to be removed:
			//tlist->window=0; //need to pass address to make this stored after leaving CloseFTItemList?
			//of have calling program set window=0?

#if WIN32
				//in Windows we need to refresh the parent window
				//twin=tlist->parent->window;
				//GetUpdateRect(twin,&twin->rect,0);
				//InvalidateRect(twin,&twin->rect,0);
				//SendMessage(twin->hwindow,WM_PAINT,0,0L); 

#endif


			//if this itemlist window was connected to another itemlist or control
			//set their flags to indicate that the window was closed
			if (tlist->parent==0) { //parent is control (not itemlist)
				tcontrol=tlist->control;
				tcontrol->flags&=~CItemListOpen;
				//set parent window flags to show 
//				twin2=tcontrol->window;
/*
				if (twin2!=0) {
					twin2->flags&=~WFileMenuOpen; //erase filemenuopen bit in window control belongs to- so that CheckFTWindowMouseMove will know that there is no filemenu open
				}
*/
				PStatus.flags&=~PExclusive; //this control itemlist does not have exclusive input control anymore
				PStatus.flags&=~PItemListOpen;
				PStatus.SelectedFTItem=0; //erase any selected item
				PStatus.SelectedFTItemList=0; //and itemlist
				PStatus.ExclusiveFTWindow=0;
				PStatus.ExclusiveFTControl=0;				

				//note closing itemlist may or may not lose focus of filemenu control
				//if clicked with mouse or lose focus yes, but if keyboard=no
				//redraw the Control to remove selected color
				DrawFTControl(tcontrol);
				if (tcontrol->type==CTDropdown && strcmp(tcontrol->text,tcontrol->text2)) {
					if (tcontrol->OnChange!=0) {  //needs to happen only after the itemlist has closed for dropdown control
						(*tcontrol->OnChange) (twin,tcontrol);
					} //tcontrol->OnChange
					strcpy(tcontrol->text2,tcontrol->text); //this is the current method to detect changed selection in an itemlist control
			} //if (tcontrol->type!=CTDropdown) {

			} else { //parent is another itemlist window
				//get the current selected item from the parent itemlist window
				PStatus.SelectedFTItem=GetFTItemFromFTItemListByNum(tlist->parent,tlist->parent->cur);
				PStatus.SelectedFTItemList=tlist->parent;
				//change exclusive window?
//				PStatus.ExclusiveFTWindow=0;
			} //tlist->parent==0
		} //twin->flags&WOpen
	} //tlist->window!=0
tlist->cur=0; //reset any selected item - depricated because of multiple selected items - now uses flag FTI_SELECTED
UnselectAllFTItems(tlist);
} //tlist!=0

//#if WIN32
//if (tlist->window!=0) {
//	tlist->window->flags|=FTW_NEEDS_WM_PAINT;
//}
//#endif

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Done CloseFTItemList\n");
}

} //CloseFTItemList

//CloseFTItemListControl - for convenience and clarity - is the same as CloseFTItemList
//but so people know that the control flags are reset too
void CloseFTItemListControl(FTControl *tcontrol) 
{
	CloseFTItemList(tcontrol->ilist);
} //CloseFTItemListControl

//CloseAllItemLists - When a window loses focus, all itemlists (filemenus) are closed
void CloseAllItemLists(void) 
{

if (PStatus.flags&PExclusive) {
  //if this is a filemenu or dropdown control with exclusive control, close it
	if (PStatus.ExclusiveFTControl!=0) {
		CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
	} 
	if (PStatus.ExclusiveFTWindow!=0 && (PStatus.ExclusiveFTWindow->flags&WItemList)) { //only filemenu,dropdown,not messagebox or splash window
		CloseFTWindow(PStatus.ExclusiveFTWindow);
		PStatus.flags&=~PExclusive;
	} 

} //PStatus.flags&PExclusive - a window has exclusive control

PStatus.flags&=~PItemListOpen;


} //CloseAllitemLists


#if 0 
//replaced by GetSelectedFTItemList
FTControl *GetOpenMenu(FTControl *tcontrol,int last) 
{
FTWindow *twin,*twin2;
FTControl *tcontrol2;
FTItem *tFTItem;
int ex;

//tcontrol must be main filemenu
//twin=GetFTWindowN(tcontrol->win);
twin=GetFTWindowN(tcontrol->win);
if (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu) && twin->flags&WItemListOpen) {
  if (strlen(tcontrol->submenu)>0) {
    //
    twin=GetFTWindow(tcontrol->submenu);
    if (twin!=0) {
      tcontrol2=twin->icontrol;
    ex=0;
    while(!ex) {

      if (tcontrol2->ilist!=0) {
        if (tcontrol2->ilist->cur==0) {  //no FTItem selected
          if (last) return(tcontrol2);  //this FTControl
          else return(tcontrol);  //previous FTControl
        }
        else {  //the submenu has a selected FTItem
          //it may be linked to an open menu, recursive part
//          tFTItem=GetFTItemFromFTControl(tcontrol2->ilist,0,tcontrol2->ilist->cur-1);
          tFTItem=GetFTItemFromFTControlByNum(tcontrol2->ilist,tcontrol2->ilist->cur-1);
#if PINFO
          fprintf(stderr,"FTItem selected = %s\n",tFTItem->name);
#endif
          if (tFTItem!=0) {
            if (tFTItem->submenu!=0) {  //FTItem has submenu, submenu should be open if cur is selected
              twin2=GetFTWindow(tFTItem->submenu);
#if PINFO
              fprintf(stderr,"submenu of selected FTItem = %s\n",twin2->name); 
#endif
             // if (twin2->flags&WNotVisible) {  //window not open, return last menu
						 if (!(twin2->flags&WOpen)) {  //window not open, return last menu
               //ButtonDownonFTItem();
              //OpenFTItemList(twin2,0,0,0);
//              twin2->ilist;
//              cur=0;
//              OpenMenu(twin2);
//              ex=1;  //probably don't need
              return(tcontrol2); 
              } 
              else {  //window open, search 
                tcontrol=tcontrol2;
                tcontrol2=twin2->icontrol;  //go to next FTControl
                //go back to top of loop
              } //WNotVis
            }  //tFTItem->submenu!=0
            else {  //tFTItem->submenu==0
//              ex=1;   //probably no need
              return(tcontrol2); //submenu does not exist, just return this FTControl
            }  //submenu!=0
          } //tFTItem!=0
          else {  //submenu has no FTItem list
//              ex=1;   //probably no need
              return(tcontrol2); //submenu has no FTItem list FTItems return this FTControl
          } //else tFTItem!=0
        } //else tcontrol->cur==0
      }  //tcontrol2->ilist!=0 
      else {
        return(tcontrol2); //submenu has no FTItem list return this FTControl
      } //else tcontrol2->ilist
 
    }  //while !ex

    }  //twin!=0

   
  }
//  else return(tcontrol); //no submenu return main menu


} //CTFileMenu !CSubMenu

return(tcontrol);
} //GetOpenMenu

#endif


//void ScrollFTItemList(FTWindow *twin,FTControl *tcontrol,unsigned char key,int flags)
void ScrollFTItemList(FTWindow *twin,FTControl *tcontrol, unsigned int flags)
{
	FTItemList *tlist;
	FTItem *litem;
	char winname[FTMedStr];
	char conname[FTMedStr];
	int ScrollInc;


	strcpy(winname,twin->name);
	strcpy(conname,tcontrol->name);


	//it's important to make clear that scrolling down is viewed here as scrolling the viewable itemlist entries down (although the actual items appear to scroll up- so it can be confusing)
	//so, for example pressing the down arrow key is scrolling down, the up arrow key is scrolling up

	//todo: FirstItemShowing should =1 when the first item is showing
	//tcontrol=CTFileopen, or CTItemList

	if (flags&FT_SCROLL_FTITEMLIST_DOWN) {  //for example the down arrow key, or mouse wheel
		if (tcontrol->ilist!=0) {
			if (flags&FT_SCROLL_FTITEMLIST_MOUSEWHEEL) {
				ScrollInc = 3; //for mousewheel scroll 3 items at a time
			}
			else {
				ScrollInc = 1; //otherwise only scroll 1 item at a time
			}
			tlist=tcontrol->ilist;
			if (!(flags&FT_SCROLL_FTITEMLIST_SELECTION)) { //only scrolling itemlist- not selected item(s)
				if (tcontrol->FirstItemShowing+tcontrol->numvisible<tlist->num+1) { //there are more items then are showing
				  tcontrol->FirstItemShowing+=ScrollInc;  //so increment first item showing
				  if (tcontrol->FirstItemShowing + tcontrol->numvisible>tlist->num+1) {
					  tcontrol->FirstItemShowing = tlist->num +1 - tcontrol->numvisible;  //because ScrollInc can be > 1
				  }
				  if (tcontrol->flags&CVScroll) {
					UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
				  }
				  DrawFTControl(tcontrol); //DrawSelectedFTItem for FileOpen doesn't currently work well
				}
			} else {
			   if (tcontrol->ilist->cur<tcontrol->ilist->num) {  
				//DrawUnselectedFTItem(tcontrol->ilist,tcontrol->ilist->cur);
			//  fprintf(stderr,"Keypress func up top=%d cur=%d num=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing,tcontrol->ilist->num);
				if (tcontrol->ilist->cur-tcontrol->FirstItemShowing+1==tcontrol->numvisible) {//(tcontrol->CursorLocationInChars+tcontrol->numvisible)) {
				  tcontrol->FirstItemShowing+=ScrollInc;  //at bottom of control, scroll items or we are just scrolling an itemlist
				  if (tcontrol->FirstItemShowing + tcontrol->numvisible>tlist->num+1) {
					  tcontrol->FirstItemShowing = tlist->num +1 - tcontrol->numvisible;  //because ScrollInc can be > 1
				  }
				}
				if (PStatus.flags&PShift) {
					SelectFTItemByNum(tlist,tlist->cur+1,FTIL_SELECT_MULTIPLE_WITH_SHIFT);
				} else {
					if (PStatus.flags&PControl) {
						SelectFTItemByNum(tlist,tlist->cur+1,FTIL_SELECT_MULTIPLE_WITH_CONTROL);
					} else {
						SelectFTItemByNum(tlist,tlist->cur+1,0);
					} //if (PStatus.flags&PControl) {
				} //if (PStatus.flags&PShift) {
				DrawFTControl(tcontrol); //DrawSelectedFTItem for FileOpen doesn't currently work well
			   } //if (!(flags&FT_SCROLL_FTITEMLIST_SELECTION)) { //only scrolling itemlist- not selected item(s)
			//probably this should be DrawSelectedFTItem- doesn't matter too much because DrawFTControls doesn't reload if not necessary
			//otherwise DrawSelectedFTItem, and DrawUnselectedFTItem need to have scrolling too

			//DrawSelectedFTItem(tcontrol->ilist,tcontrol->ilist->cur);

		  }  //tcontrol->ilist->cur<tcontrol->ilist->num-1
	  } //ilist!=0
	} //if (flags&FT_SCROLL_FTITEMLIST_DOWN) {

	if (flags&FT_SCROLL_FTITEMLIST_UP) { //for example the up arrow key, or mouse wheel
		if (tcontrol->ilist!=0) {
			if (flags&FT_SCROLL_FTITEMLIST_MOUSEWHEEL) {
				ScrollInc = 3; //for mousewhell scroll 3 items at a time
			}
			else {
				ScrollInc = 1; //otherwise only scroll 1 item at a time
			}
			tlist=tcontrol->ilist;
			if (!(flags&FT_SCROLL_FTITEMLIST_SELECTION)) {  //only scrolling itemlist- not selection(s)
				if (tcontrol->FirstItemShowing>1) { //there are items not shown above the items showing
				  tcontrol->FirstItemShowing-=ScrollInc;  //so decrement first item showing
				  if (tcontrol->FirstItemShowing < 1){
					  tcontrol->FirstItemShowing = 1; //because ScrollInc can be >1
				  }
		  		  if (tcontrol->flags&CVScroll) {
					UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
				  }
				  DrawFTControl(tcontrol);  //redraw FTControl
				}
			} else {
				if (tlist->cur>1) {
					//DrawUnselectedFTItem(tcontrol->ilist,tcontrol->ilist->cur);
					if (tlist->cur==tcontrol->FirstItemShowing) {
						tcontrol->FirstItemShowing-=ScrollInc;
						if (tcontrol->FirstItemShowing < 1){
							tcontrol->FirstItemShowing = 1; //because ScrollInc can be >1
						}
					}
					if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
						if (PStatus.flags&PShift) {
							SelectFTItemByNum(tlist,tlist->cur-1,FTIL_SELECT_MULTIPLE_WITH_SHIFT);
						} else {
							if (PStatus.flags&PControl) {
								SelectFTItemByNum(tlist,tlist->cur-1,FTIL_SELECT_MULTIPLE_WITH_CONTROL);
							} else {
								SelectFTItemByNum(tlist,tlist->cur-1,0);
							} //if (PStatus.flags&PControl) {
						} //if (PStatus.flags&PShift) {
					} //if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
					DrawFTControl(tcontrol);  //redraw FTControl
				} //tlist->cur>1
			} //if (!(flags&FT_SCROLL_FTITEMLIST_SELECTION)) {
		} //if tcontrol->ilist!=0
	} //if (flags&FT_SCROLL_FTITEMLIST_UP) {


	if (flags&FT_SCROLL_FTITEMLIST_PAGE_UP) { //for example the page up key
	  if (tcontrol->ilist!=0) {
	//    if (tcontrol->ilist->cur>0) {
		if (tcontrol->FirstItemShowing>1) {
		  if (tcontrol->FirstItemShowing-1-tcontrol->numvisible>0) {
			tcontrol->FirstItemShowing-=(tcontrol->numvisible-1);
			if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
			  tcontrol->ilist->cur-=(tcontrol->numvisible-1);
			}
		  } else  {
			tcontrol->FirstItemShowing=1;
			if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
			  tcontrol->ilist->cur=1;
			}
		  }
	//    fprintf(stderr,"Keypress func up i0=%d i1=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing);

		  if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {   
			//copy filename to fileopen FTControl 
			litem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->ilist->cur);//+1
			if (litem!=0) {
				if (tcontrol->type==CTFileOpen || tcontrol->type==CTDropdown || tcontrol->type==CTItemList) {
		  			strcpy(tcontrol->text,litem->name); //text gets folders and files
				}
				if (!(litem->flags&FTI_FOLDER)) {
					strcpy(tcontrol->filename,litem->name);
				} else {
					strcpy(tcontrol->filename,"");  
				}
			} //litem!=0
		  } //if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {   

		  if (tcontrol->flags&CVScroll) {
			UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
		  }
		  DrawFTControl(tcontrol);  //redraw fileopen FTControl, will redraw scroll FTControls

	//now done in DrawSelectedFTItem
	/*
		  if (tcontrol->OnChange!=0) {
					(*tcontrol->OnChange) (twin,tcontrol);
					twin=GetFTWindow(winname); //check to see if window was deleted
					tcontrol=GetFTControl(conname); //check to see if control was deleted
					if (tcontrol==0 || twin==0) return; //user deleted control or window
				} //OnChange
	*/

		  }  //i[0]>0 //cur>0  //if tcontrol->FirstCharacterShowing>0

	  }  //ilist!=0

	}  //if (flags&FT_SCROLL_FTITEMLIST_PAGE_UP) {


	if (flags&FT_SCROLL_FTITEMLIST_PAGE_DOWN) {//for example the page down key
		if (tcontrol->ilist!=0) {
		if (tcontrol->FirstItemShowing<tcontrol->ilist->num-1) {
	//  fprintf(stderr,"Keypress func up top=%d cur=%d num=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing,tcontrol->ilist->num);
	//    fprintf(stderr,"tcontrol->CursorLocationInChars=%d nums=%d\n",tcontrol->CursorLocationInChars,tcontrol->ilist->num);
		  if ((tcontrol->FirstItemShowing+2*tcontrol->numvisible)<tcontrol->ilist->num) {
			tcontrol->FirstItemShowing+=tcontrol->numvisible;
			if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
			  tcontrol->ilist->cur+=tcontrol->numvisible;
			}
		  }
		  else {
			//go to last FTItem
			if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {
			  tcontrol->ilist->cur=tcontrol->ilist->num-1;
			}
	//        tcontrol->CursorLocationInChars=tcontrol->ilist->cur-tcontrol->numvisible+1;
			tcontrol->FirstItemShowing=tcontrol->ilist->num-tcontrol->numvisible+1;
			//number of FTItems is less than number of FTItems allowed to be visible
	//        if (tcontrol->CursorLocationInChars<0) tcontrol->CursorLocationInChars=0;
	//        fprintf(stderr,"cur=%d nums=%d\n",tcontrol->ilist->cur,tcontrol->ilist->num);
		  }

		  if (flags&FT_SCROLL_FTITEMLIST_SELECTION) {   
	//        litem=GetFTItemFromFTControl(tcontrol->ilist,0,tcontrol->ilist->cur);
	//        litem=GetFTItemFromFTControlByNum(tcontrol->ilist,tcontrol->ilist->cur);
			litem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->ilist->cur+1);
					if (litem!=0) {
						if (tcontrol->type==CTFileOpen || tcontrol->type==CTDropdown || tcontrol->type==CTItemList) {
						strcpy(tcontrol->text,litem->name); //text gets folders and files
						}
				  if (!(litem->flags&FTI_FOLDER))
					strcpy(tcontrol->filename,litem->name);
				  else
					strcpy(tcontrol->filename,"");  
					} //litem!=0
		  }

	//      update_sb_foOpenFile2();  //update scroll button

		  if (tcontrol->flags&CVScroll) {
			//UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
			  CalcVScrollSlide(twin,tcontrol);  //need to update location of scroll slide button
		  }
		  DrawFTControl(tcontrol);  //redraw fileopen FTControl, will redraw scrollbar FTControls

	//now done in DrawSelectedFTItem
	/*
		  if (tcontrol->OnChange!=0) {
					(*tcontrol->OnChange) (twin,tcontrol);
					twin=GetFTWindow(winname); //check to see if window was deleted
					tcontrol=GetFTControl(conname); //check to see if control was deleted
					if (tcontrol==0 || twin==0) return; //user deleted control or window
				} //OnChange
	*/
		}  //less than nums
	  }  //ilist!=0

	}  //if (flags&FT_SCROLL_FTITEMLIST_PAGE_DOWN) {


}  //void ScrollFTItemList(FTWindow *twin,FTControl *tcontrol, unsigned int flags)

//GetFTItemFromFTItemListXY - return item which mouse x,y is over
//presumes x,y is over itemlist window - does no boundary checking
FTItem *GetFTItemFromFTItemListXY(FTItemList *tlist,int x,int y) 
{
int fh;
int titemnum;

if (tlist!=0) {
	fh=tlist->fontheight+PStatus.PadY;
	titemnum=y/fh+1;
	if (titemnum>0) {
		return(GetFTItemFromFTItemListByNum(tlist,titemnum));
	} 
} //tlist!=0
return(0);
} //GetFTItemFromFTItemListXY

//When a button is up on an itemlist it is to select the item
void FTItemButtonUp(FTItemList *tlist,int x,int y,int button) 
{
FTItem *titem;
FTWindow *twin;
FTControl *tcontrol;
//char conname[FTMedStr],winname[FTMedStr];

if (tlist!=0) {
	//determine which item is clicked on and call click(button) function if any
	titem=GetFTItemFromFTItemListXY(tlist,x,y);
	//if item has an ilist - open - or else close all itemlist windows
	if (titem!=0) {
		if (titem->ilist!=0) { //item has subitemlist
			if (titem->ilist->window!=0) {
				twin=titem->ilist->window;
				if (twin->flags&WOpen) {
					CloseFTItemList(titem->ilist); //close opened subitemlist
				} else {
					OpenFTItemList(titem->ilist); //open closed subitemlist
				}
			} else { 
				OpenFTItemList(titem->ilist); //open closed subitemlist
			}//titem->ilist->window
		} else { //titem->ilist==0
			//no subitemlist - close all itemlists
			tcontrol=PStatus.ExclusiveFTControl;
			if (tcontrol!=0) {
				if (tcontrol->type==CTDropdown) {
					//dropdown value could be selected itemlist index - but can't be used if subitemlists are allowed
					//for now dropdown controls can not have subitemlists
					tcontrol->value=tcontrol->ilist->cur;
					strcpy(tcontrol->text,titem->name); //copy text of item
					//could strcpy subitemlist items into control here and ignore value
					//strcpy(tcontrol->text,titem->name);
				}
				CloseFTItemList(tcontrol->ilist);
				//fprintf(stderr,"Draw dropdown again\n");
				if (tcontrol->type==CTDropdown) {
					DrawFTControl(tcontrol); //redraw with selected text
				} //CTDropdown
			} //tcontrol!=0
		} //titem->ilist!=0


//		fprintf(stderr,"before check for click and onchange\n");
		if (tlist->window!=0 && tlist->control!=0) {
			//call any function related to selecting this item (even if not the final selection)
//		fprintf(stderr,"call click\n");
			if (titem->click[button]!=0) {
				(*titem->click[button]) (tlist->window,tlist->control,x,y);
		//tlist windows are not destroyed until control window is destroyed
		//		tlist->control=GetFTControl(conname);
		//		tlist->window=GetFTWindow(winname);
		//		if (tlist->control==0 || tlist->window==0) return; //user deleted control or window
			} //titem->click(button)


//		fprintf(stderr,"call onchange\n");
			//call any OnChange function related to selecting this item (even if not the final selection)
			if (tlist->control->OnChange!=0) {
				(*tlist->control->OnChange) (tlist->window,tlist->control);
				//fprintf(stderr,"after onchange\n");
		//		tlist->control=GetFTControl(conname);
		//		tlist->window=GetFTWindow(winname);
		//		if (tlist->control==0 || tlist->window==0) return; //user deleted control or window
			} //titem->control->OnChange
		} //tlist->window!=0 && tlist->control!=0


	} //titem!=0
} //tlist!=0
//call OnChange functions?

if (PStatus.flags&PInfo)
  fprintf(stderr,"Done FTItemButtonUp\n");

} //FTItemButtonUp







//FT_SetFocus - set focus to a window and to any control (control=0 - sets window to focus only)
//Possibly SetWindowFocus - it might be nice to have a twin->SetFocus();
void FT_SetFocus(FTWindow *twin,FTControl *tcontrol) 
{
FTControl *tcontrol2,*tcLostFocus;
//int tfocus;
#if Linux
int rtr;//resize;
Window xwin;
XWindowAttributes attribute;
#endif
//XWindowChanges xchange;
//FTWindow *twin2;
//Atom XA_WM_TAKE_FOCUS;
//Atom xa_WM_CHANGE_STATE;
//XClientMessageEvent ev;
//XEvent ev;
char winname[FTMedStr];
char conname[FTMedStr];
char conname2[FTMedStr];

//if (tcontrol!=0)

if (PStatus.flags&PInfo) {
	fprintf(stderr,"FT_SetFocus to window\n");
}


//PStatus.FocusFTWindow=GetFTWindow("cont");
//if (PStatus.FocusFTWindow!=0 && PStatus.FocusFTWindow!=twin) {
	//XUnmapWindow(PStatus.xdisplay,PStatus.FocusFTWindow->xwindow);
//	XLowerWindow(PStatus.xdisplay,PStatus.FocusFTWindow->xwindow);
///}


//other FTControls lose focus. in particular textbox, will lose cursor
//if tcontrol==0, all FTControls will lose focus
if (twin!=0) {
if (PStatus.flags&PInfo) fprintf(stderr,"FT_SetFocus to window %s\n",twin->name);


#if Linux
	XRaiseWindow(PStatus.xdisplay,twin->xwindow);
#endif
#if WIN32
	SetForegroundWindow(twin->hwindow);
#endif

if (tcontrol<=0) {
	//focus window only
	return;
}

//tfocus=twin->focus;
//tfocus=0;
tcLostFocus=0;
//fprintf(stderr,"before twin->FocusFTControl\n");

	if (tcontrol!=0 && tcontrol!=twin->FocusFTControl) {
		if (twin->FocusFTControl!=0) {
		//possibly make sure FocusFTControl still exists
		//tfocus=twin->FocusFTControl->tab;
			tcLostFocus=twin->FocusFTControl;
			//fprintf(stderr,"%s lostfocus\n",tcLostFocus->name);
		} //twin->FocusFTControl!=0
	} else {
		return; //this control already has the focus
	} //tcontrol!=0

  
//fprintf(stderr,"after twin->FocusControl\n");
//tfocus=twin->focus;

//Set window focus
//PStatus.focus=twin->num;
//PStatus.FocusFTWindow=twin; //done with windows message
//twin->focus=0;
twin->FocusFTControl=0; //need?
//fprintf(stderr,"xdisplay=%p twin->xwindow=%p\n",PStatus.xdisplay,(unsigned int *)twin->xwindow);
//XFlush(PStatus.xdisplay);  //BadMatch if not flushed now
//delay some time for window to be drawn
//tns=PStatus.utime.tv_usec+100000;
//while(tns>PStatus.utime.tv_usec); //delay for 1 second
//fprintf(stderr,"before loop PStatus.ctime=%p\n",PStatus.ctime);
//tns=PStatus.ctime->tm_sec+1;
//while(tns>PStatus.ctime->tm_sec) {
//  GetTime();
//  FT_GetInput();
//} //delay for 1 second
//override
//fprintf(stderr,"trying to focus into %s\n",twin->name);
//The windows manager will not raise the window
//XMapWindow(PStatus.xdisplay,twin->xwindow);
//XLowerWindow(PStatus.xdisplay,RootWindow(PStatus.xdisplay,PStatus.xscreen));
//XRaiseWindow(PStatus.xdisplay,twin->xwindow);


//it is not enough to set the input focus, we need to select the focus window (the titlebar turns dark blue, etc) by doing a raisewindow apparently
//XRaiseWindow(PStatus.xdisplay,twin->xwindow);
//XMapRaised(PStatus.xdisplay,twin->xwindow);
//perhaps produce a fake buttonclick event? to the windows manager

//fprintf(stderr,"before raise\n");
//XMapLowerRaised(PStatus.xdisplay,twin->xwindow);
//XMapRaised(PStatus.xdisplay,twin->xwindow);
//XMapWindow(PStatus.xdisplay,twin->xwindow);
#if Linux
	XRaiseWindow(PStatus.xdisplay,twin->xwindow);
#endif
#if WIN32
	SetForegroundWindow(twin->hwindow);
#endif

/*
memset(&xchange,0,sizeof(XWindowChanges));
*/
/*
xchange.x=twin->x+20;
xchange.y=twin->y+20;

xchange.width=twin->w+20;
xchange.height=twin->h+20;

//xchange.border_width=?
//sibling=?
//xchange.sibling=RootWindow(PStatus.xdisplay,PStatus.xscreen);
twin2=GetFTWindow("cont");
xchange.sibling=twin2->xwindow;
xchange.stack_mode=Above;

if (XReconfigureWMWindow(PStatus.xdisplay,twin->xwindow,0,CWStackMode|CWSibling|CWWidth|CWHeight|CWX|CWY,&xchange)==0) {
fprintf(stderr,"Could not reconfigureWMWindow\n");
};
//XIconifyWindow(PStatus.xdisplay,twin->xwindow,0);
//XWithdrawWindow - but no XAssertWindow?
*/
//WM_CHANGE_STATE
//IconicState
/*

    xa_WM_CHANGE_STATE = XInternAtom (PStatus.xdisplay,
                            "WM_CHANGE_STATE", False);

    ev.type = ClientMessage;
	  ev.send_event = True;
    ev.display = PStatus.xdisplay;
		ev.requestor
*/
/*
    ev.message_type = xa_WM_CHANGE_STATE;
    ev.format = 32;
    ev.data.l[0] = IconicState;
    ev.window = twin->xwindow;
*/
/*
	  xev.type = SelectionNotify;
	  xev.send_event = True;
	  xev.display = PStatus.xdisplay;
	  xev.requestor = xevent.xselectionrequest.requestor;
	  xev.selection = xevent.xselectionrequest.selection;
	  xev.target = xevent.xselectionrequest.target;
	  xev.property = xevent.xselectionrequest.property;
	  xev.time = xevent.xselectionrequest.time;
*/
/*
    XSendEvent(PStatus.xdisplay,
            RootWindow(PStatus.xdisplay, DefaultScreen(PStatus.xdisplay)),
            True,
            (SubstructureRedirectMask | SubstructureNotifyMask),
            &ev);
    XFlush(PStatus.xdisplay);
*/

//XA_WM_TAKE_FOCUS=XInternAtom(PStatus.xdisplay,"WM_TAKE_FOCUS",0);

//XSetWMProtocols(PStatus.xdisplay,twin->xwindow,&XA_WM_TAKE_FOCUS,0);

//XFlush(PStatus.xdisplay);  //BadMatch if not flushed now
//fprintf(stderr,"twin=%p\n",twin);
//fprintf(stderr,"xdisplay=%p twin->xwindow=%p\n",PStatus.xdisplay,(unsigned int *)twin->xwindow);
#if Linux
	//need to make sure Window is visible before setting focus, see this link: http://stackoverflow.com/questions/16720961/many-xsetinputfocuss-and-xsync-causes-error
	XGetWindowAttributes(PStatus.xdisplay, twin->xwindow, &attribute);
	if (attribute.map_state == IsViewable) {
		XSetInputFocus(PStatus.xdisplay, twin->xwindow, RevertToNone, CurrentTime);
	}
//wait for input focus to be set
//probably not necessary
//XGetInputFocus(PStatus.xdisplay,twin->
	while(xwin!=twin->xwindow) {
    XGetInputFocus(PStatus.xdisplay,&xwin,&rtr);
		ProcessEvents();
	}
#endif


if (tcontrol!=0) {
	strcpy(winname,twin->name);
	strcpy(conname,tcontrol->name);

	if (PStatus.flags&PInfo) {
		fprintf(stderr,"FT_SetFocus to FTControl %s\n",tcontrol->name);
	}

  //twin->focus=tcontrol->tab;  //give this FTControl the window focus
	twin->FocusFTControl=tcontrol;  //give this FTControl the window focus
	//unselect control that lost focus
	if (tcLostFocus!=0) {
			strcpy(conname2,tcLostFocus->name);
			//tcLostFocus->flags&~=
     // if (tcontrol2->type==CTTextBox || tcontrol2->type==CTTextArea) {
       DrawFTControl(tcLostFocus);  //remove cursor, and for buttons undo selected color
      //}  //textbox
      if (tcLostFocus->LostFocus!=0) (*tcLostFocus->LostFocus) (twin,tcLostFocus);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol2=GetFTControl(conname2); //check to see if control was deleted
			if (tcontrol2==0 || twin==0) return; //user deleted control or window
    }
	} //tcLostFocus!=0


  DrawFTControl(tcontrol);  //redraw control - to add selected color

  if (tcontrol->GotFocus!=0) (*tcontrol->GotFocus) (twin,tcontrol);
	twin=GetFTWindow(winname); //check to see if window was deleted
	tcontrol=GetFTControl(conname); //check to see if control was deleted
	if (tcontrol==0 || twin==0) return; //user deleted control or window


/*
  if (tcontrol->tab!=tfocus && tfocus!=0) {

    tcontrol2=GetFTControlWithTab(twin,tfocus);
    if (tcontrol2!=0) {
			strcpy(conname2,tcontrol2->name);

     // if (tcontrol2->type==CTTextBox || tcontrol2->type==CTTextArea) {
       DrawFTControl(tcontrol2);  //remove cursor, and for buttons undo selected color
      //}  //textbox
      if (tcontrol2->LostFocus!=0) (*tcontrol2->LostFocus) (twin,tcontrol2);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol2=GetFTControl(conname2); //check to see if control was deleted
			if (tcontrol2==0 || twin==0) return; //user deleted control or window
    }
       DrawFTControl(tcontrol);  //redraw control - to add selected color

	  }  //tcontrol->tab!=tfocus
*/



//}  //if tcontrol!=0

//done in events
//if (twin->GotFocus!=0) (*twin->GotFocus) ();

}  //twin!=0

//fprintf(stderr,"End FT_SetFocus\n");
if (PStatus.flags&PInfo) fprintf(stderr,"End FT_SetFocus to window %s\n",twin->name);
//sleep(5);
}  //end FT_SetFocus


//FT_LoseFocus - remove control from window focus
void FT_LoseFocus(FTWindow *twin,FTControl *tcontrol)
{
int lostfocus;

if (tcontrol!=0) {
	lostfocus=0;
	if (twin->FocusFTControl==tcontrol) {
		twin->FocusFTControl=0; //for now no control has focus, since this control had window focus
		lostfocus=1;
	}
	//always redraw control just to make sure selected color is erased
	DrawFTControl(tcontrol);//resets selected color of control that just lost focus - call LostFocus

	if (lostfocus && tcontrol->LostFocus!=0) {
		(*tcontrol->LostFocus) (twin,tcontrol);
	} //lostfocus && tcontrol->LostFocus

} //tcontrol!=0
} //FT_LoseFocus


void CheckFTWindowButtonDownHold(FTWindow *twin,int x,int y,int button)
{
unsigned long long thtime;
int curz;
FTControl *tcontrol,*tcontrol2;
//char winname[FTMedStr];
//char conname[FTMedStr];


//if (PStatus.flags&PInfo)
//  fprintf(stderr,"CheckFTWindowButtonDownHold\n");

if (twin!=0)  //this window has at least 1 FTControl
{
//strcpy(winname,twin->name);


curz=200;
tcontrol2=twin->icontrol;
tcontrol=0;
//go thru complete list of FTControls for this window for the 1st time
while(tcontrol2!=0) {
  if (tcontrol2->flags&CButtonDown(0) && x>=tcontrol2->x1 && x<=tcontrol2->x2 && y>=tcontrol2->y1 && y<=tcontrol2->y2) {

  if (tcontrol2->z<curz) {//was <curz || curz==-1) {
    tcontrol=tcontrol2;
    curz=tcontrol->z;
  }

  } //x1,y1,x2,y2
  tcontrol2=tcontrol2->next;
}  //end while tcontrol!=0


if (tcontrol!=0) {
//			strcpy(conname,tcontrol->name);
 // fprintf(stderr,"ButtonDown being held over control %s with delay=%d sec %d usec\n",tcontrol->name,(int)tcontrol->interval[button].tv_sec,(int)tcontrol->interval[button].tv_usec);

	//in ms
	thtime=tcontrol->lasttime[button].tv_sec*1000 + tcontrol->lasttime[button].tv_usec/1000 + \
            (((tcontrol->flags&CButtonHoldStart(button))>0) * \
	    (tcontrol->interval[button].tv_sec*1000+tcontrol->interval[button].tv_usec/1000)) + \
	    (!((tcontrol->flags&CButtonHoldStart(button))>0) * \
	    (tcontrol->delay[button].tv_sec*1000+tcontrol->delay[button].tv_usec/1000));

        //fprintf(stderr,"PStatus.utime= %lli us > thtime=%lli us\n",(unsigned long long)(PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec),thtime);

      //if ((unsigned long long)(PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec)>thtime) {
	if ((unsigned long long)(PStatus.utime.tv_sec*1000+PStatus.utime.tv_usec/1000)>thtime) {  //in ms
        //fprintf(stderr,"button held down %li %lli\n",PStatus.utime.tv_sec,thtime);
				//indicate that the initial delay has passed
        tcontrol->flags|=CButtonHoldStart(button);
				//update the lasttime the buttondownhold callback function was called
        tcontrol->lasttime[button].tv_sec=PStatus.utime.tv_sec;
        tcontrol->lasttime[button].tv_usec=PStatus.utime.tv_usec;

				//call scroll button functions - I moved these inline as opposed to internal callback to free for user to use - make these type=CTVScrollUp and then call these functions and a callback
				if (tcontrol->flags&CVScrollUp) {
					VScrollUp_ButtonDown(twin,tcontrol,x,y,button);
				} //CVScrollUp

				if (tcontrol->flags&CVScrollDown) {
					VScrollDown_ButtonDown(twin,tcontrol,x,y,button);
				} //CVScrollDown

				if (tcontrol->flags&CVScrollBack) {
					//fprintf(stderr,"buttondownhold\n");
					VScrollBack_ButtonDown(twin,tcontrol,x,y,button);
				} //CVScrollBack


				if (tcontrol->flags&CHScrollLeft) {
					HScrollLeft_ButtonDown(twin,tcontrol,x,y,button);
				} //CHScrollLeft

				if (tcontrol->flags&CHScrollRight) {
					HScrollRight_ButtonDown(twin,tcontrol,x,y,button);
				} //CHScrollRight

				if (tcontrol->flags&CHScrollBack) {
					//the scroll bar in GTK is implemented to scroll when the button is down after an initial scroll, no matter where mouse x,y is - perhaps separate initial buttondown and later repeating
					HScrollBack_ButtonDown(twin,tcontrol,x,y,button);
				} //CHScrollBack

        if (tcontrol->ButtonDownHold[button]!=0) {  //call any functions connected to ButtonDown
//        fprintf(stderr,"calling button hold\n");
          (*tcontrol->ButtonDownHold[button]) (twin,tcontrol,x,y);
//					twin=GetFTWindow(winname); //check to see if window was deleted
//					tcontrol=GetFTControl(conname); //check to see if control was deleted
//					if (tcontrol==0 || twin==0) return; //user deleted control or window
        }
      }  //end if time has passed to do another button down event

    }  //tcontrol!=0
//}  //end if this FTControl is getting the ButtonDownhold

//next

}  //twin!=0;

} //end CheckFTWindowButtonDownHold


/*
void SimulateButtonDown(FTWindow *twin,int x, int y, int button)
{
CheckFTWindowButtonDown(twin,x,y,button);
//CheckFTWindowButtonUp(twin,x,y,button);
} //SimulateButtonDown

void SimulateButtonUp(FTWindow *twin,int x, int y, int button)
{
//CheckFTWindowButtonDown(twin,x,y,button);
CheckFTWindowButtonUp(twin,x,y,button);
} //SimulateButtonUp
*/

//This is only called once by CheckHotKeys
//SimulateButtonDown needs to be replaced with a similar function to CheckFTWindowButtonDown
//called "HotKeyActivate" that goes through each control type in a similar way.
//The two functions can then share all the activate a control code (like open an itemlist, change the focus, etc.)
//There could also be a function called "activate" which is different from "click" and "keypress"
//because the user should not have to analyze the hotkey keypress for a button activated- although they still can through keypress
void SimulateButtonClick(FTWindow *twin,int x, int y, int button)
{
twin->flags|=FTW_SIMULATE_BUTTON_CLICK;
CheckFTWindowButtonDown(twin,x,y,button);
CheckFTWindowButtonUp(twin,x,y,button);
twin->flags&=~FTW_SIMULATE_BUTTON_CLICK;
} //SimulateButtonClick

#if 0 
void SimulateButtonHold(FTWindow *twin,int x, int y, int button)
{
CheckFTWindowButtonDown(twin,x,y,button);
CheckFTWindowButtonDownHold(twin,x,y,button);
} //SimulateButtonHold
#endif

void CheckFTWindowButtonDown(FTWindow *twin,int x,int y,int button)
{
//int bw,bh;//,curz;//,skip;
FTControl *tcontrol,*tcontrol2,*tTimeLine;//,*tcontrol3;
FTItemList *tlist;
FTItem *titem;
unsigned int cx,cy,curpos,fw,fh,maxheight;
unsigned int maxwidth;
unsigned int mx,my,i,tlen;
int curz;
char winname[FTMedStr];
char conname[FTMedStr];
long double lx,lx2;
//char tstr[FTMedStr];


//A button down event occured in this window (twin) at x,y (button #=button)

if (PStatus.flags&PInfo) {
  fprintf(stderr,"CheckFTWindowButtonDown\n");
}
//Is there a time when CheckFTWindowButtonDown is called without an attached window?
//if (twin==0) return;
//possibly twin may = 0 if a button is pressed down outside any window
//in which case - we would only CloseAllitemLists(); in LostFocus
if (twin==0) {
	return;
}


strcpy(winname,twin->name);



	//set window buttondown bit
twin->flags|=WButtonDown(button);

//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
	if (twin->ButtonDown[button]!=0) {  //call any window functions connected to ButtonDown
	  (*twin->ButtonDown[button]) (twin,x,y);
		twin=GetFTWindow(winname); //check to see if window was deleted
		if (twin==0) return; //user deleted window
	}
//} ////if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
//if (twin!=0) {
//#if PINFO
//fprintf(stderr,"win %s exclusive=%x\n",twin->name,PStatus.flags&PExclusive);
//#endif
//}

//for now if button 4 or 5 - mouse wheel up or down in Linux
//just call any window function and exit

//if (button>2) {
//	return;
//}

//ItemList Windows have no controls, so check for a click in an itemlist window
// && (twin->flags&WOpen) - I think we can presume it is open if it is a buttondown on an itemlist window
if (twin->flags&WItemList) {

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"Button Down was on an ItemList Window\n");
	}

//buttondown is in itemlist window
	
//	FTItemButtonDown(twin->ilist,x,y,button);

//ignore buttondown on itemlist windows - only happens when itemlist window was already open - and then all that matters is location on buttonup

return;
} //twin->flags&WItemList


//button down when an ItemList is open can only relate to filemenu or dropdown itemlist window
if (PStatus.flags&PItemListOpen) {
	//if the button down is on any window other than the itemlist window
	//close the open itemlist
	if (PStatus.ExclusiveFTControl!=0) {
		CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
	}
	DrawFTControls(twin,0);

#if 0 
	//go through controls and determine which control has buttondown
	tcontrol=twin->icontrol;
//go thru complete list of FTControls for this window for the 1st time
//check z too
	while(tcontrol!=0) {

		strcpy(conname,tcontrol->name);


	//find which filemenu or dropdown control x,y is over
//test ->z?
		if ((tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown) && !(tcontrol->flags&(CNotVisible|CNotShowing)) && x>=tcontrol->x1 && x<=tcontrol->x2 && y>=tcontrol->y1 && y<=tcontrol->y2) {

			if (PStatus.flags&PInfo) {
				fprintf(stderr,"Button down on FTControl %s\n",tcontrol->name);
			}

			tcontrol->flags|=CButtonDown(button);  //set ButtonDown flag
			PStatus.ButtonDownFTControl[button]=tcontrol;
			
			if (tcontrol->flags&CItemListOpen) { //this is the open ItemList  - close on buttonup
				//fprintf(stderr,"Setting PCloseItemOnUp\n");
	    	PStatus.flags|=PCloseItemListOnUp;
			} 
//a buttondown on a different filemenu than the one that is open cannot happen because a mousemove over a filemenu control would open it
//else {  //ItemList is closed, open - some other itemlist must be open
				//OpenFTItemList(tcontrol->ilist);
			//} //CItemListOpen
		return;
		} //if tcontrol->type==CTFileMenu ||
	tcontrol=tcontrol->next;
	}//while tcontrol!=0
#endif

return; //buttondown was not on control or on a control that was not a filemenu or dropdown
} //PStatus.flags&PItemListOpen

//buttondown is not in itemlist window
//could be on open or unopen filemenu control

//possibly exit after found= can only click on 1 FTControl - need depth for scroll bar drag button
//if (twin!=0 && (!(PStatus.flags&PExclusive) || twin->num==PStatus.ExclWin || (twin->flags&WItemListOpen))) {
//is a filemenu submenu and visible
//if (twin!=0 && (!(PStatus.flags&PExclusive) || twin->num==PStatus.ExclWin || (twin->flags&WItemList && !(twin->flags&WNotVisible)))) {
//window has exclusive control or is an open itemlist window (filemenu or dropdown list)
//if ((!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow || ((twin->flags&WItemList) && (twin->flags&WOpen)))) {
//There is no button down in an ItemList window beyond this point
//if either no window has exclusive control, OR this window has exclusive control 
if (!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow) {

#if PINFO
//  fprintf(stderr,"win %s is exclusive window\n",twin->name);
#endif


	twin->flags|=WButtonDown(button);  //set window button down bit
//  fprintf(stderr,"twin menuopen %d\n",twin->flags&WItemListOpen);
  
	PStatus.flags|=EButtonHold(button); //initiate the buttonhold



//GO THRU THIS WINDOW'S CONTROLS 
//first check events with any file menus that cover other FTControls
//no other FTControls can be clicked on with a menu or dropdown open
//still if is a submenu, need to recognize that click before others
	tcontrol=twin->icontrol;
//go thru complete list of FTControls for this window for the 1st time
//check z too

	curz=200;
	tcontrol2=0;
	while(tcontrol!=0)
	{

	//find which control x,y is over
		//fprintf(stderr,"control %s CNotShowing=%d z=%d\n",tcontrol->name,tcontrol->flags&CNotShowing,tcontrol->z);
		if (!(tcontrol->flags&(CNotVisible|CNotShowing)) && x>=tcontrol->x1 && x<=tcontrol->x2 && y>=tcontrol->y1 && y<=tcontrol->y2 && tcontrol->z<curz) {

			curz=tcontrol->z;
			tcontrol2=tcontrol;
		} //CNotVisible - mouse x,y is over this control

	tcontrol=(FTControl *)tcontrol->next;
	}//end while tcontrol!=0


	if (tcontrol2!=0) {  //buttondown was on a control
		tcontrol=tcontrol2;

		if (PStatus.flags&PInfo) {
			fprintf(stderr,"Button down on FTControl %s\n",tcontrol->name);
		}

//      GetTime();  //done in main()
		tcontrol->lasttime[button].tv_sec=PStatus.utime.tv_sec;
		tcontrol->lasttime[button].tv_usec=PStatus.utime.tv_usec;



		//bw=tcontrol->x2-tcontrol->x1;
		//bh=tcontrol->y2-tcontrol->y1;


		//Check for resizing control
		if (tcontrol->flags&(CResize|CResizeX1|CResizeX2|CResizeY1|CResizeY2))
			{
			//already know that x,y is inside FTControl
			//if mouse click is on x1 or x2 of FTControl (and within y1 and y2)
			if (x<tcontrol->x1+PStatus.ResizePix)
			  PStatus.flags|=EResizeX1;
			if (x>tcontrol->x2-PStatus.ResizePix)
			  PStatus.flags|=EResizeX2;
			//if mouse click is on y1 or y2 of FTControl (and within x1 and x2)
			if (y<tcontrol->y1+PStatus.ResizePix)
			  PStatus.flags|=EResizeY1;
			if (y>tcontrol->y2-PStatus.ResizePix)
			  PStatus.flags|=EResizeY2;
			}

			PStatus.ButtonX[button]=x;
			PStatus.ButtonY[button]=y;

			PStatus.LastMouseX=x;  //make these equal or else in functions lastmouse is from b4
			PStatus.LastMouseY=y;

			tcontrol->flags|=CButtonDown(button);  //set ButtonDown flag
			PStatus.ButtonDownFTControl[button]=tcontrol;


			//call scroll button functions - I moved these inline as opposed to internal callback to free for user to use
			if (tcontrol->flags&CVScrollUp) {
				VScrollUp_ButtonDown(twin,tcontrol,x,y,button);
			} //CVScrollUp

			if (tcontrol->flags&CVScrollDown) {
				VScrollDown_ButtonDown(twin,tcontrol,x,y,button);
			} //CVScrollDown

			if (tcontrol->flags&CVScrollBack) {
				VScrollBack_ButtonDown(twin,tcontrol,x,y,button);
			} //CVScrollBack


			if (tcontrol->flags&CHScrollLeft) {
				HScrollLeft_ButtonDown(twin,tcontrol,x,y,button);
			} //CHScrollLeft

			if (tcontrol->flags&CHScrollRight) {
				HScrollRight_ButtonDown(twin,tcontrol,x,y,button);
			} //CHScrollRight

			if (tcontrol->flags&CHScrollBack) {
				HScrollBack_ButtonDown(twin,tcontrol,x,y,button);
			} //CHScrollBack



#if 0 
			//check for button down on scrollbar back
			if (tcontrol->flags&CVScroll) {

				//Also do up+down buttons here

				//scrollbar may be attached to fileopen or textarea	control
				//see if mouse is above or below scrollbar button
				//and Page_Up or pgdn
				sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name[strlen(PStatus.PrefixVScrollBack)]);
				tcontrol2=GetFTControl(tstr);
				if (tcontrol2!=0) {
					//and get main control scrollbar is attached to
					sprintf(tstr,"%s",tcontrol->name[strlen(PStatus.PrefixVScrollBack)]);
					tcontrol3=GetFTControl(tstr);						
					if (tcontrol3!=0) {
						if (tcontrol3->type==CTTextArea) {  //scrollbar connected to textarea
							if (y<tcontrol2->y1) {
								FT_TextAreaPgUp(tcontrol3,0); //0=do not move cursor
							} else {
								FT_TextAreaPgDown(tcontrol3,0);  //0=do not move cursor
							}//y<tcontrol2->y1
						if (tcontrol3->type==CTFrame) { //scrollbar connected to frame
								if (y<tcontrol2->y1) {
								//	FT_FrameScrollPgUp(tcontrol3);
								} else {
								//	FT_FrameScrollPgDown(tcontrol3);
								}//y<tcontrol2->y1
						} //CTFrame

							DrawFTControl(tcontrol3); //this also redraws ahny attached scrollbars
						} //tcontrol3->type==CTTextArea
					} //tcontrol3!=0 VScrollBack


					//and get main control scrollbar is attached to
					sprintf(tstr,"%s",tcontrol->name[strlen(PStatus.PrefixVScrollDown)]);
					tcontrol3=GetFTControl(tstr);						
					if (tcontrol3!=0) {
						if (tcontrol3->type==CTFrame) { //scrollbar connected to frame
							FT_FrameScrollDown(tcontrol3);
						} //CTFrame
					} //tcontrol3!=0 ScrollDown



				} //tcontrol2!=0 button down was on a control

			} //CVScroll
#endif



		//change color of button down (button 0 only?)
		if (!(tcontrol->flags&CNoColorChangeButton(button))) {
			DrawFTControl(tcontrol); 
		  }


		if (tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown) {
			//ButtonDown was on a FileMenu or dropdown control
			//toggle open or closed attached itemlist

			//set focus to this filemenu or dropdown control - and remove from others
			//if (!(tcontrol->flags&CNoTab)) {
			if (twin->FocusFTControl!=tcontrol && twin->FocusFTControl!=0) {
				FT_LoseFocus(twin,twin->FocusFTControl);		
			}
			FT_SetFocus(twin,tcontrol);
			//}



//Itemlist cannot be open in this part because of above code
//so open
				OpenFTItemList(tcontrol->ilist);
/*
			if (tcontrol->flags&CItemListOpen) { //ItemList is open - close on buttonup
				fprintf(stderr,"Setting PCloseItemOnUp\n");
	    	PStatus.flags|=PCloseItemListOnUp;
				
			} else {  //ItemList is closed, open
				OpenFTItemList(tcontrol->ilist);
			} //CItemListOpen
*/
		} //tcontrol->type==CTFileMenu



	  if ((tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) && button==0) {  
//button down is possibly selecting text
//changed i[2] and i[3] to character units (was pixels)

		  //set the PSelectingText flag for any button down in a textbox or textarea control
		  //this flag gets reset on button up
		  PStatus.flags|=PSelectingText;
		  //initialize the 100ms timer that checks to see if we need to control scrolling the selecting even if the mouse is not moving and generating mousemove events
  			PStatus.timertime2.tv_sec=PStatus.utime.tv_sec;
			PStatus.timertime2.tv_usec=PStatus.utime.tv_usec;


			//fprintf(stderr,"set focus on textbox or textarea (%s)\n",tcontrol->name);
//fprintf(stderr,"Set focus3\n");
			if (!(tcontrol->flags&CNoTab)) {
				FT_SetFocus(twin,tcontrol); //
			}
			fw=tcontrol->fontwidth;
			fh=tcontrol->fontheight;
			mx=(x-tcontrol->x1)/fw;  //is uint
			my=(y-tcontrol->y1)/fh; //mouse in characters
			cx=0;//tcontrol->x1+1;
			cy=0;//tcontrol->y1+fh-1;
			maxwidth=(tcontrol->x2-tcontrol->x1)/fw;
			maxheight=(tcontrol->y2-tcontrol->y1)/fh;
			//find out which character is closest on this line if any
			//go through each character starting with i[1]
			i=tcontrol->FirstCharacterShowing;
			tlen=strlen(tcontrol->text);
			curpos=tcontrol->CursorLocationInChars;
			//tcontrol->CursorX=mx;
			//tcontrol->CursorY=my;


			if (tcontrol->type==CTTextBox) {
				//set cursor to last char if mouse is to right of end of text
				if (tlen<mx+i) {
					curpos=tlen; //last char
					tcontrol->CursorX=tlen-i; //adjust CursorX
				} else {
					curpos=mx+i;
					tcontrol->CursorX=mx;
				}
				tcontrol->CursorY=0;
			} else { //CTTextArea
				while(i<=tlen && cy<maxheight) {	
	//					fprintf(stderr,"i=%d tlen=%d cy=%d maxheight=%d mx=%d my=%d\n",i,tlen,cy,maxheight,mx,my);
					if (cy>my) { //mouse is past text
						curpos=i-1;	
						//if (tcontrol->text[curpos]==10) {
						//	curpos--; //if this character is a carriage return (10) move the cursor before it
						//}
						i=tlen+1;//exit loop
					}

					if ((cy==my && cx>=mx) || i==tlen) {
						if (tlen==0) {
							curpos=0;
						} else {
							curpos=i;//curpos=i+1; //this is closest position to mouse
						}
						i=tlen+1; //exit loop
					} //cy<=my

					if (tcontrol->text[i]==10) {
						if (cy==my && mx>cx) { //mouse is on same line and X is to right of text)
							curpos=i;
							i=tlen+1; //exit loop
						} else { //if (cy==my && mx>cx) { //mouse is on same line and X is to right of text)
							cx=0;
							cy++;
						} //if (cy==my && mx>cx) { //mouse is on same line and X is to right of text)
					} else {
						cx++;
						if (cx==maxwidth-1) {
							cx=0; 
							cy++;
						} //cx x2
					} //text[i]==10

					i++;
				}	//while			
				tcontrol->CursorX=cx;
				tcontrol->CursorY=cy;
			} //if (tcontrol->type==CTTextBox) {

//			curpos=tcontrol->FirstCharacterShowing+cy*maxwidth+cx;						

//				 fprintf(stderr,"curpos=%d\n",curpos);

	    tcontrol->StartSelectedTextInChars=curpos;//x;   //button 0 only for now
	    tcontrol->EndSelectedTextInChars=curpos;//tcontrol->StartSelectedTextInChars;
	    tcontrol->CursorLocationInChars=curpos;// move cursor to button down position
		//store the current cursor position because it changes when the mouse is moving selecting text but needs to be remembered
		tcontrol->ButtonDownCursorLocationInChars=curpos;

//      tcontrol->i[4]=0;
//      tcontrol->i[5]=0;
//      tcontrol->i[4]=curpos;//tcontrol->FirstCharacterShowing+(tcontrol->StartSelectedTextInChars-tcontrol->x1)/fw;
//      tcontrol->i[5]=curpos;//tcontrol->FirstCharacterShowing+(tcontrol->StartSelectedTextInChars-tcontrol->x1)/fw;
//			fprintf(stderr,"curpos=%d\n",curpos);
	    //if the selected text location is past text length set to end
	    //if (tcontrol->i[4]>strlen(tcontrol->text)) tcontrol->i[4]=strlen(tcontrol->text);
	    //if (tcontrol->i[5]>strlen(tcontrol->text)) tcontrol->i[5]=strlen(tcontrol->text);
//#if PINFO
//	    fprintf(stderr,"strlen text=%d\n",strlen(tcontrol->text));
//#endif

	    //unselect text on other FTControls on this window
	    tcontrol2=twin->icontrol;
	    while(tcontrol2!=0) {
//        if ((tcontrol2->type==CTTextBox  || tcontrol2->type==CTTextArea) && !(strcmp(tcontrol2->name,tcontrol->name)==0)) {
	      if (tcontrol!=tcontrol2 && (tcontrol2->type==CTTextBox  || tcontrol2->type==CTTextArea) && tcontrol2->StartSelectedTextInChars!=tcontrol2->EndSelectedTextInChars) {
	        tcontrol2->StartSelectedTextInChars=0;
	        tcontrol2->EndSelectedTextInChars=0;
//          tcontrol2->i[4]=0;
//          tcontrol2->i[5]=0;
	        tcontrol2->text2[0]=0;  //empty selected string
					//could possibly just update selected and cursor parts
	        DrawFTControl(tcontrol2); //update image of FTControl
	      }  
	      tcontrol2=tcontrol2->next;
	    }  //end while
//?		  }


	 //draw FTControl to show button down
	if (!(tcontrol->flags&CNoChangeOnMouseOver) && button==0)  //button 0 only for now
	  {
	//	fprintf(stderr,"Draw FTControl %s\n",tcontrol->name);
	  DrawFTControl(tcontrol);
	  }



	}  //CTTextBox or CTTextArea & button=0

	if (tcontrol->type==CTData || tcontrol->type==CTTimeLine) {
		//set initial data selection time
		//translate mouse position to time relative to data
		if (tcontrol->type==CTTimeLine) {
			tTimeLine=tcontrol;
		} else {
			tTimeLine=tcontrol->TimeLineFTControl;
		}
		if (tTimeLine!=0) { //data has timeline connected to it
			lx=tcontrol->StartTimeVisible+((long double)(x-tcontrol->x1))*tTimeLine->TimeScale; //time at mouse
			//if mouse is not near boundary of start or end of existing selection
			lx2=(tTimeLine->TimeScale*(long double)tTimeLine->TimeSpace);
			if ((!(lx>tcontrol->EndTimeSelected-lx2 && lx<tcontrol->EndTimeSelected+lx2)) && (!(lx>tcontrol->StartTimeSelected-lx2 && lx<tcontrol->StartTimeSelected+lx2))) {
				tcontrol->StartTimeSelected=lx;
				tcontrol->EndTimeSelected=lx;
				tTimeLine->StartTimeCursor=lx; //just track cursor on timeline
				tcontrol->StartTimeCursor=lx;  //or on data too?				
				tTimeLine->StartTimeSelected=lx; //StartTimeSelected is stored on timeline too
				tTimeLine->EndTimeSelected=lx;  //as is EndTimeSelected

			//and update any connected start,end, and/or duration controls
			//update any StartTimeSelected controls connected to data 
			//possibly people should connect each data control to the time labels if they want selecting the data to change the labels, but for now, simply presume that all data selections change an associated timeline selected time labels
				if (tTimeLine->StartTimeSelectedFTControl!=0) {
					//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
					FT_FormatTime(tTimeLine->StartTimeSelectedFTControl->text,tcontrol->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
					//strcpy(,tstr);
					DrawFTControl(tTimeLine->StartTimeSelectedFTControl); //redraw SelectedTime control
				} //StartTimeSelectedFTControl
				//update any EndTimeSelected controls connected to data 
				if (tTimeLine->EndTimeSelectedFTControl!=0) {
					//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
					FT_FormatTime(tTimeLine->EndTimeSelectedFTControl->text,tcontrol->EndTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
					//strcpy(,tstr);
					DrawFTControl(tTimeLine->EndTimeSelectedFTControl); //redraw SelectedTime control
				} //EndTimeSelectedFTControl
				//update any StartTimeSelected controls connected to data 
				if (tTimeLine->DurationSelectedFTControl!=0) {
					//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
					FT_FormatTime(tTimeLine->DurationSelectedFTControl->text,tcontrol->EndTimeSelected-tcontrol->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
					//strcpy(,tstr);
					DrawFTControl(tTimeLine->DurationSelectedFTControl); //redraw SelectedTime control
				} //DurationSelectedFTControl

			} //lx>tcontrol
		} //tTimeLine!=0

	} //CTData, CTTimeLine


		//FolderTabs are interesting in that for whatever reason GTK selects on just buttondown, not buttondown and then up
	  if (tcontrol->type==CTFolderTab && button==0) {  

			//fprintf(stderr,"FolderTab control selected with buttondown");
			if (!(tcontrol->flags&CNoTab)) {
				FT_SetFocus(twin,tcontrol); //
			}

			tcontrol2=tcontrol->FolderTabGroup;
			if (tcontrol2!=0) {
				SelectFolderTab(tcontrol);
				DrawFTControl(tcontrol2); //and redraw FolderTabGroup
			}//tcontrol2!=0

	}  //CTFolderTab & button=0

	//For FileOpen:
	//buttonclick is internal, Keypresses are external (autogenerated human code)
	//if FileOpen, get filename clicked on
	if (tcontrol->type==CTFileOpen && button==0) {
	//highlight the selected FTItem


		fh=tcontrol->fontheight;
	//copy the filename in the FTControl to "filename" (could be done in humanfunc)
		tlist=tcontrol->ilist;
		if (tlist!=0) {
			i=(int)((y-tcontrol->y1)/fh); //item 1 is first item, cur=0 is no items selected
			titem=(FTItem *)GetFTItemFromFTControlByNum(tcontrol,tcontrol->FirstItemShowing+i);
			//SelectFTItem() - 
			if (titem!=0) {
				if (PStatus.flags&PShift) {
					SelectFTItem(titem,FTIL_SELECT_MULTIPLE_WITH_SHIFT);
				} else {
					if (PStatus.flags&PControl) {
						SelectFTItem(titem,FTIL_SELECT_MULTIPLE_WITH_CONTROL);
					} else {
						SelectFTItem(titem,0);
					} //if (PStatus.flags&PControl) {
				} //if (PStatus.flags&PShift) {
				tlist->cur=tcontrol->FirstItemShowing+i;  //set index for text to draw selected- deprecated because of multiple selected items
			}  // titem!=0
			DrawFTControl(tcontrol);  //redraw FTControl todo: probably DrawSelectedFTItem instead of entire control

			if (tcontrol->OnChange!=0) {
				(*tcontrol->OnChange) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			} //OnChange
			//this FTItem will be strcpy to FTControl filename when drawn
	//    fprintf(stderr,"%s\n",tcontrol->filename);
		}  //ilist!=0  end if there is an FTItem list

	}  //end if CTFileOpen



//	if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
		if (tcontrol->ButtonDown[button]!=0)
			{  //call any functions connected to ButtonDown
				//these include internal scrollbars-one problem is that the scroll bar slide button is already updated by the time of this call
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"calling control ButtonDown function\n");
				}
				(*tcontrol->ButtonDown[button]) (twin,tcontrol,x,y);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) {
					return; //user deleted control or window
				}
			}
//		} 	//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
//				fprintf(stderr,"tcontrol->next=%p\n",tcontrol->next);


	} else { //tcontrol2!=0 //buttondown was on a control
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"Button down was not on any FTControl\n");
		}
		//button down was not on a control and no itemwindow was open
		//lose focus of any control that had focus
		if (twin->FocusFTControl!=0) {
			FT_LoseFocus(twin,twin->FocusFTControl);
		}

	}//tcontrol2!=0 //buttondown was on a control


} //if no window is exclusive or this window is the exclusive window



if (PStatus.flags&PInfo) {
  fprintf(stderr,"End CheckFTWindowButtonDown\n");
}

}  //end CheckFTWindowButtonDown




void CheckFTWindowButtonUp(FTWindow *twin,int x,int y,int button)
{
int bw,bh,MovingControl;
FTControl *tcontrol,*tcontrol2;
//int i;
#if Linux
//long long i64;
#endif
//FTItemList *tlist;
//FTItem *titem;
//int downup,skip;
FTWindow *twin2;
char winname[FTMedStr];
char conname[FTMedStr];
int fh;
int rx1,ry1,match,curz;
//int CursorX,CursorY,MaxWidth,MaxHeight;
//int cx,cy,curpos,fw,fh,maxwidth;

if (PStatus.flags&PInfo)
  fprintf(stderr,"CheckFTWindowButtonUp\n");


//reset the selecting text flag for any button up
//note that we don't need to determine the final position because that is done in FTCheckFTWindowMouseMove
if (PStatus.flags&PSelectingText) {
	PStatus.flags&=~PSelectingText; 
} 

if (twin==0) {
	return;
}
 
strcpy(winname,twin->name);

if (PStatus.flags&PInfo) {
	fprintf(stderr,"CheckFTWindowButtonUp in Window '%s'\n",twin->name);
}


PStatus.flags&=~EButtonHold(button);  //end any buttonhold


//if an itemlist is open - buttonup can only relate to filemenus or dropdown itemlists
if (PStatus.flags&PItemListOpen || twin->flags&WItemList) {  //needed to add twin->flags&WItemList because sometimes the window loses focus and resets the PItemListOpen bit

//If this window is an itemlist call the item the buttonup is over
	if (!(twin->flags&WItemList)) { 
		//this window is not an itemlist, but an itemlist is open
//the buttonup window must have an open filemenu or dropdown control - and the button 0 must be down

		//Button 0 is being held down with an itemlist window open
		//x and y are relative to window with control on it
//		fprintf(stderr,"x=%d y=%d\n",x,y);

		//Button is down with ItemList Window open:
		//1) if exclusivecontrol is filemenu - other filemenus can open and close
		//2) itemlist items can be selected and unselected
		//twin2=PStatus.ExclusiveFTWindow; //this is itemlist window-  another way of getting window

		//find out which itemlist window ,if any, the mouse is over with the buttondown

		//mouse position relative to window with filemenu or dropdown control
#if Linux
		rx1=twin->x+x;
		ry1=twin->y+y;
#endif
#if WIN32
		rx1=x;
		ry1=y;
#endif

		twin2=PStatus.iwindow;
		match=0;
		while(twin2!=0) {
			if (twin2->flags&WItemList) {
				if (twin2->flags&WOpen) {
					if (rx1>twin2->x && rx1<twin2->x+twin2->w &&
							ry1>twin2->y && ry1<twin2->y+twin2->h && twin2->z<twin->z) {
							//mouse is over this itemlist window
							match=1;
							twin=twin2;
							x=rx1-twin->x; //x,y will be 0 to twin->w
							y=ry1-twin->y;
					} //rx1>twin2->x
				} //WOpen
			} //WItemList
			twin2=twin2->next;
		} //while	twin2!=0

		if (!match) {  //buttonup not over any open itemlist window
			//fprintf(stderr,"Buttonup not over any open itemlist window\n");
			//but might be over another filemenu control
			//see if the open itemlist is from a filemenu
			tcontrol=PStatus.ExclusiveFTControl; //this may be the FileMenu or dropdown
			if (tcontrol!=0) {
				//if button up is not on the same control button down was on, and control is a filemenu or dropdown control
				//then determine which control if any the button up is over, 
				//and close any open item list, and call any window and control button up functions
				if (tcontrol!=PStatus.ButtonDownFTControl[0] && (tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown)) {
					//see if mouse is over a different filemenu
					tcontrol2=twin->icontrol;
					while(tcontrol2!=0) {
						if (tcontrol2->type==CTFileMenu || tcontrol2->type==CTDropdown) {
							if (x>tcontrol2->x1 && x<tcontrol2->x2 && 
								y>tcontrol2->y1 && y<tcontrol2->y2) {

								//buttonup is over a filemenu control
								//if down on control - closeitemlist windows
								if (PStatus.flags&PCloseItemListOnUp) {
									CloseFTItemList(tcontrol->ilist);
								}
								//PStatus.SelectedFTItemList=0;////need?
								//PStatus.SelectedFTItem=0; //need?
								//call OnButtonUp callbacks
								//call window buttonup function if any
								//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
									if (twin->ButtonUp[button]!=0) {
										(*twin->ButtonUp[button]) (twin,x,y);
										twin=GetFTWindow(winname); //check to see if window was deleted - we could pass twin to user function
										if (twin==0) return; //user deleted window 
									}
									//(check for deleted control?)
									if (tcontrol->ButtonUp[button]!=0) {
										(*tcontrol->ButtonUp[button]) (twin,tcontrol,x,y);
									}
								} else {
									if (twin->HotKeyActivate!=0) {
										(*twin->HotKeyActivate) (twin,tcontrol);
										twin=GetFTWindow(winname); //check to see if window was deleted - we could pass twin to user function
										if (twin==0) return; //user deleted window 
									}
									//(check for deleted control?)
									if (tcontrol->HotKeyActivate!=0) {
										(*tcontrol->HotKeyActivate) (twin,tcontrol);
									}		
								//} //if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
								return;
//								} //PCloseItemListOnUp		

/*
								if (tcontrol2!=tcontrol) { //buttonup is over a different filemenu control
									//PStatus.MouseOverFTControl=tcontrol2;
									//tcontrol->flags&=~CMouseOver;
									//DrawFTControl(tcontrol); //draw unselected filemenu
									PStatus.ExclusiveFTControl=tcontrol2;
									//close open itemlist and open new itemlist
									CloseFTItemList(tcontrol->ilist);
									OpenFTItemList(tcontrol2->ilist);
									PStatus.SelectedFTItemList=tcontrol2->ilist; //need?
									PStatus.SelectedFTItem=0; //need?
									return;
								} //tcontrol2!=tcontrol
*/
							} //x>tcontrol2->x1
						} //CTFileMenu
					tcontrol2=tcontrol2->next;
					}//while tcontrol2				
				//did not find any control
					//fprintf(stderr,"Did not find any control closeitemlist\n");
					CloseFTItemList(tcontrol->ilist);
					tcontrol->flags&=~CMouseOver; //clear mouse over
					DrawFTControl(tcontrol);
					//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
						if (twin->ButtonUp[button]!=0) {
							(*twin->ButtonUp[button]) (twin,x,y);			 
						}
					//} //if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
					return;

				} //tcontrol->type==CTFileMenu

			} //tcontrol!=0
		} //!match


//after this process this buttonup with an openitemlist like the rest - 
//now that the twin, and x,y are adjusted to be relative to the itemlist window the mouse is over

	} //!(twin->flags&WItemList)  itemlist is open but window is not an itemlist


	//buttonup is over an itemlist window
		//
//		if (PStatus.flags&PInfo)	{
//			fprintf(stderr,"window is itemlist - call item\n");
//		}



		//Close open ItemList
		//CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
//		CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
		//clear PCloseItemListOnUp because buttonup is over itemlist
//		PStatus.flags&=~PCloseItemListOnUp;
		//Button up is in ItemList window
		//call any functions associated with item x,y is over
		FTItemButtonUp(twin->ilist,x,y,button);
		twin->flags&=~WButtonDown(button);  //clear window button down bit
		//callback buttonup in window
		//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
			if (twin->ButtonUp[button]!=0) {
				(*twin->ButtonUp[button]) (twin,x,y);
			}
	//	} //if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
	//	fprintf(stderr,"after FTItemButtonUp\n");
	return;

//} //twin->flags&WItemList
//	} 

//else {//twin->flags&WItemList

/*



//		fprintf(stderr,"window is not itemlist\n");
		if (twin->flags&WButtonDown(button)) {
//			fprintf(stderr,"window had same button down\n");
			//see if mouse is over an item
			//see if buttondown mouse is over openitemlist window
			twin2=PStatus.ExclusiveFTWindow;
			tcontrol=PStatus.ExclusiveFTControl;
			if (twin2!=0 && tcontrol!=0) {
//				fprintf(stderr,"Exclusive window=%s control=%s\n",twin2->name,tcontrol->name);
				tlist=twin2->ilist;
				
				if (x>tcontrol->x1 && x<(tcontrol->x1+twin2->w) && 
						y>tcontrol->y2 && y<(tcontrol->y2+twin2->h)) {
					//verify that the mouse is over the itemlist window
//					fprintf(stderr,"mouse with buttondown is over itemlist\n");
//					titem=GetFTItemFromFTItemListXY(tlist,x-tcontrol->x1,y-tcontrol->y2); //x,y is relative to window with control
					//call any functions associated with item x,y is over
					FTItemButtonUp(tlist,x-tcontrol->x1,y-tcontrol->y2,button);
//					fprintf(stderr,"titem=%p\n",titem);
				} else { //x>tcontrol->x1
//				  fprintf(stderr,"Close item list=%x\n",PStatus.flags&PCloseItemListOnUp);
					if (!(x>tcontrol->x1 && x<tcontrol->x2 && 
							 y>tcontrol->y1 && y<tcontrol->y2) || (PStatus.flags&PCloseItemListOnUp)) {
						//buttonup is not over filemenu control
						//and buttonup is not over itemlist - Close Control's ItemList Window
						CloseFTItemList(tcontrol->ilist);					
						PStatus.flags&=~PCloseItemListOnUp;
						tcontrol->flags&=~CButtonDown(0);
					} //(!(x>tcontrol->x1

				} //x>tcontrol->x1
			} //twin2!=0 tcontrol!=0


		} //twin->flags&WButtonDown(button)
*/

		
		//see if buttonup is over filemenu control

//		twin->flags&=~WButtonDown(button);  //clear window button down bit
//	return;
//	}//twin->flags&WItemList


/*
//If this window is not an itemlist window
//find which filemenu control the buttonup mouse is over
//if none=close the open filemenu control OR 
//if on same filemenu or dropdown control as buttondown and PCloseItemListOnUp bit set then close control
	tcontrol=twin->icontrol;
	while(tcontrol!=0) {
		

		strcpy(conname,tcontrol->name);


		if ((tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown) && !(tcontrol->flags&(CNotVisible|CNotShowing)) &&  x>=tcontrol->x1 && x<=tcontrol->x2 && y>=tcontrol->y1 && y<=tcontrol->y2) {


//				CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
//			fprintf(stderr,"Close filemenu that had button down while open PCloseItemListOnUp=%x\n",(PStatus.flags&PCloseItemListOnUp));
//button==0 && (tcontrol->flags&CButtonDown(0)) && 
			if (PStatus.flags&PCloseItemListOnUp) {
				CloseFTItemList(tcontrol->ilist);
				PStatus.flags&=~PCloseItemListOnUp;
				tcontrol->flags&=~CButtonDown(0);
				//call buttonup user callback
				return;
			} else return; //or else this is a button up on a control that had a buttondown but the itemlist was not open at the time of buttondown

		} //tcontrol->type==CTFileMenu || 


	tcontrol=tcontrol->next;
	}//while tcontrol!=0


//button up on window that is not itemlist - but not on filemenu control that had button 0 down=
//close ItemList windows
//	fprintf(stderr,"Close filemenu button bit=%d\n");
	CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
	PStatus.flags&=~PCloseItemListOnUp;
	twin->flags&=~WButtonDown(button);  //clear window button down bit

	return;

*/


} //PItemListOpen

//Below here no ItemList is open


strcpy(winname,twin->name); //save name to check if deleted by user

//no matter what control (if any) the buttonup is on, end any controls being resized
if (PStatus.flags&(EResizeX1|EResizeX2|EResizeY1|EResizeY2)) {
//  fprintf(stderr,"stopping resizing\n");
	//stop resizing any FTControl
	if (PStatus.flags&EResizeX1) PStatus.flags&=~(EResizeX1);
	if (PStatus.flags&EResizeX2) PStatus.flags&=~(EResizeX2);
	if (PStatus.flags&EResizeY1) PStatus.flags&=~(EResizeY1);
	if (PStatus.flags&EResizeY2) PStatus.flags&=~(EResizeY2);
}



//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {

	//moved to top because twin can be deleted on buttonup for example on Exit button
	//window loses focus - clear window buttondown bits
	//if a buttondown was on this window then 
	//call window buttondownup function if any
	if (twin->flags&WButtonDown(button)) {
		twin->flags&=~WButtonDown(button);
		if (twin->ButtonDownUp[button]!=0) {
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"calling window ButtonDownUp function\n");
			}
			(*twin->ButtonDownUp[button]) (twin,x,y);
			twin=GetFTWindow(winname); //check to see if window was deleted
			if (twin==0) return; //user deleted control or window
		}
		if (twin->ButtonClick[button]!=0) {
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"calling window ButtonClick function\n");
			}
			(*twin->ButtonClick[button]) (twin,x,y);
			twin=GetFTWindow(winname); //check to see if window was deleted
			if (twin==0) return; //user deleted control or window
		}

	} else { //clear all other window buttondown bits
		twin2=PStatus.iwindow;
		while(twin2!=0) {
			twin2->flags&=~WButtonDown(button);
			twin2=twin2->next;
		} //while twin2!=0
	} //twin->flags

	//call window buttonup function if any
	if (twin->ButtonUp[button]!=0) {
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"calling window ButtonDownUp function\n");
		}
		(*twin->ButtonUp[button]) (twin,x,y);
		twin=GetFTWindow(winname); //check to see if window was deleted - we could pass twin to user function
		if (twin==0) return; //user deleted control or window
	}
//}//if (!(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
//I don't call the above functions for HotKeyActivate because it is below




if (!(PStatus.flags&PExclusive || twin==PStatus.ExclusiveFTWindow) || (twin->flags&WMsgBox)) {

	//downup=0;  //clicked downup on a FTControl (setting the tab), button up on a FTControl will not set tab
	MovingControl=0; //need MovingControl anymore?


		//does the end of a drag and drop call a ButtonUp function?
	//done with any mouse move
	if (PStatus.flags&EMouseMove) { //need to keep track on FTControl structure if being moved
		  PStatus.flags&=~EMouseMove;
	//    fprintf(stderr,"dropped FTControl %s\n",tcontrol->name);
		  MovingControl=1;  //to stop any down-up click event when was drag-drop
	}






	//Go through each control to find which x,y mouse is over on button up
	//z is important to determine scrollbar clicks - although perhaps scroll bars should be next to control

	twin->flags&=~WButtonDown(button);  //clear window button down bit
	//curz=-1;
	tcontrol=twin->icontrol;
	//go thru complete list of FTControls for this window for the 1st time
	curz=200;
	tcontrol2=0;
	while(tcontrol!=0) {

	//Find control that the mouse is over on this buttonup
		if (!(tcontrol->flags&(CNotVisible|CNotShowing)) && x>=tcontrol->x1 && x<=tcontrol->x2 && y>=tcontrol->y1 && y<=tcontrol->y2 && tcontrol->z<curz) {

			curz=tcontrol->z;
			tcontrol2=tcontrol;
		} //if !(tcontrol->flags&(CNotVisible|CNotShowing)

	tcontrol=tcontrol->next;
	} //while (tcontrol!=0)


//reset any buttondown on any different control
//in theory there could be a combination of buttons down or up
//Really needs a PStatus variable instead of while loop
	if (PStatus.ButtonDownFTControl[button]!=0 && PStatus.ButtonDownFTControl[button]!=tcontrol2) {
		tcontrol=PStatus.ButtonDownFTControl[button];
		tcontrol->flags&=~(CButtonDown(button));
		PStatus.ButtonDownFTControl[button]=0;
		DrawFTControl(tcontrol); //update control with button down
	}

/*
	tcontrol=twin->icontrol;
	while(tcontrol!=0) {
		//redraw unbuttonpressed button color
		if (tcontrol!=tcontrol2 && tcontrol->flags&CButtonDown(button)) {
	//    fprintf(stderr,"draw button up\n");
	//the CButtonDown bit is not reset in part 1 or 2 above, not until here
	

			//hide filemenu and dropdown FTControls
		//    fprintf(stderr,"here %s\n",tcontrol2->name);
			DrawFTControl(tcontrol);
		} 
		tcontrol=tcontrol->next;
	} //while tcontrol
*/


	if (tcontrol2!=0) {  //found control that mouse is over

		tcontrol=tcontrol2;
		strcpy(conname,tcontrol->name);

	//BUTTON UP IS OVER THIS CONTROL

			bw=tcontrol->x2-tcontrol->x1;
			bh=tcontrol->y2-tcontrol->y1;
			fh=tcontrol->fontheight;

			PStatus.ButtonX[button]=x;
			PStatus.ButtonY[button]=y;

			//uncolor mouseover color - possibly do in MouseMove
		  if (tcontrol->flags&CMouseOver) {
		    tcontrol->flags&=~CMouseOver;
		    DrawFTControl(tcontrol);  //remove mouseover
		  }


			//clear CButtonHoldStart - this is set after a buttonhold interval has passed
	      tcontrol->flags&=~CButtonHoldStart(button);

			//buttondown was on this control 
			if (tcontrol->flags&CButtonDown(button)) {
//				tcontrol->flags&=~CButtonDown(button); //clear ButtonDown bit
				//BUTTON DOWN AND BUTTON UP ARE ON THIS CONTROL
				//ignore buttondown and up on filemenu and dropdown - because itemlist stays open from buttondown

			if (PStatus.flags&PInfo) {
				fprintf(stderr,"Button %d down and up on FTControl %s\n",button,tcontrol->name);
			}


			//this control gets focus, unless CNoTab (or perhaps CNoFocus) bit is set
			if (!(tcontrol->flags&CNoTab) && button==0) {
				//twin->focus=tcontrol->tab; //give this control the window focus - so keypresses when control has focus will call user callback function - like deleting a data track with delete key
				FT_SetFocus(twin,tcontrol); //needs to redraw other control
				//fprintf(stderr,"Set focus on %s\n",tcontrol->name);
			}
 
			tcontrol->flags&=~(CButtonDown(button));  //done in beginning- not needed here - verify
			PStatus.ButtonDownFTControl[button]=0;
			DrawFTControl(tcontrol); //probably don't want to redraw FileOpen control

/*
			//If this is a filemenu control - close all open itemlist windows
			if ((tcontrol->flags&CItemListOpenOnButtonDown) && (tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown)) {
				CloseFTItemList(tcontrol->ilist);
			  PStatus.flags&=~PCloseItemListOnUp;
				tcontrol->flags&=~CButtonDown(button); //clear button down bit
				tcontrol->flags&~=CItemListOpenOnButtonDown; //clear itemlist was open when button was down
				return;
			} //tcontrol2->type==CTFileMenu
*/
		



	//for controls being moved or being resized, ignore all function calls and color changes

	//if moving this control ignore all other processing
			if (tcontrol->flags&CIsMoving) {
				fprintf(stderr,"control is moving\n");
				tcontrol->flags&=~CIsMoving;
				tcontrol->flags|=CDoneMoving;
			} else {
//Probably a CIsResizing would be helpful
				if (PStatus.flags&(EResizeX1|EResizeX2|EResizeY1|EResizeY2)) {
				fprintf(stderr,"resizing\n");

					//stop resizing any FTControl
					if (PStatus.flags&EResizeX1) PStatus.flags&=~(EResizeX1);
					if (PStatus.flags&EResizeX2) PStatus.flags&=~(EResizeX2);
					if (PStatus.flags&EResizeY1) PStatus.flags&=~(EResizeY1);
					if (PStatus.flags&EResizeY2) PStatus.flags&=~(EResizeY2);
				} else { 
					//not resizing or moving

					if (tcontrol->flags&CToggle && button==0) {
						tcontrol->value=!tcontrol->value;
						//fprintf(stderr,"toggle control to value=%d\n",tcontrol->value);
						DrawFTControl(tcontrol); //redraw control with new toggle color
						if (tcontrol->OnChange!=0) { 
							(*tcontrol->OnChange) (twin,tcontrol);
							if (tcontrol==0 || twin==0) return; //user deleted control or window
						} //onChange
					}  //end if toggle


					//if checkbox, change value
					if (tcontrol->type==CTCheckBox && button==0) {
						tcontrol->value=!(tcontrol->value);
						DrawFTControl(tcontrol);
						if (tcontrol->OnChange!=0) {
							(*tcontrol->OnChange) (twin,tcontrol);
							twin=GetFTWindow(winname); //check to see if window was deleted
							tcontrol=GetFTControl(conname); //check to see if control was deleted
							if (tcontrol==0 || twin==0) return; //user deleted control or window
						} //OnChange
					} //CTCheckBox

					//if option, change value
					if (tcontrol->type==CTOption && button==0) {
						if (tcontrol->value==0) {
							tcontrol->value=1;
							DrawFTControl(tcontrol);
							if (tcontrol->name!=0) {
								//go through other radio controls and set their value to 0
								tcontrol2=twin->icontrol;
								while (tcontrol2!=0) {
									if (tcontrol2->type==CTOption) {
										if (tcontrol2->OptionGroup==tcontrol->OptionGroup) { //isnt it enough to be in the same group?
											if (tcontrol2->name!=0  && strcmp(tcontrol2->name,tcontrol->name)) {
												tcontrol2->value=0;
												DrawFTControl(tcontrol2);
												} //tcontrol2->name
											}  //if group==
										}  //if CTOption
									tcontrol2=tcontrol2->next;
								}  //end while tcontrol2!=0
							}  //if name!=0

							if (tcontrol->OnChange!=0) {
								(*tcontrol->OnChange) (twin,tcontrol);
								twin=GetFTWindow(winname); //check to see if window was deleted
								tcontrol=GetFTControl(conname); //check to see if control was deleted
								if (tcontrol==0 || twin==0) return; //user deleted control or window			 
							} //OnChange
						}  //end if value was 0
					}  //end if option button

//move CTFileOpen to CheckFTWindowButtonDown()


					//fprintf(stderr,"simulate click?\n");
					//if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {
						//this function call may by an Exit button which frees twin and all controls
						if (tcontrol->ButtonDownUp[button]!=0) {
							if (PStatus.flags&PInfo) {
								fprintf(stderr,"calling control ButtonDownUp function\n");
							}
							(*tcontrol->ButtonDownUp[button]) (twin,tcontrol,x,y);
							//fprintf(stderr,"After call ButtonDownUp user function\n");
							twin=GetFTWindow(winname); //check to see if window was deleted
							tcontrol=GetFTControl(conname); //check to see if control was deleted
							if (tcontrol==0 || twin==0) {
								return; //user deleted control or window
							}
							//fprintf(stderr,"twin!=0\n");
						} //if (tcontrol->ButtonDownUp[button]!=0) {

						//this function call may by an Exit button which frees twin and all controls
						//ButtonClick is identical to ButtonDownUp but both are kept for ease of use
						//fprintf(stderr,"call buttonclick function? button=%d ButtonClick=%x \n",button,tcontrol->ButtonClick[button]);
						if (tcontrol->ButtonClick[button]!=0) {
							if (PStatus.flags&PInfo) {
								fprintf(stderr,"calling control ButtonClick function\n");
							}
							(*tcontrol->ButtonClick[button]) (twin,tcontrol,x,y);
							//fprintf(stderr,"After call ButtonDownUp user function\n");
							twin=GetFTWindow(winname); //check to see if window was deleted
							tcontrol=GetFTControl(conname); //check to see if control was deleted
							if (tcontrol==0 || twin==0) return; //user deleted control or window
							//fprintf(stderr,"twin!=0\n");
						} //ButtonClick
					//} else {//if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {
						//a hotkey may have activated a control
#if 0 
						if (twin->HotKeyActivate!=0) {
							if (PStatus.flags&PInfo) {
								fprintf(stderr,"calling window HotKeyActivate function\n");
							}
							(*twin->HotKeyActivate) (twin,tcontrol);
							twin=GetFTWindow(winname); //check to see if window was deleted - we could pass twin to user function
							if (twin==0) return; //user deleted window 
						} //if (twin->HotKeyActivate!=0) {

						//(check for deleted control?)
						if (tcontrol->HotKeyActivate!=0) {
							if (PStatus.flags&PInfo) {
								fprintf(stderr,"calling control HotKeyActivate function\n");
							}
							(*tcontrol->HotKeyActivate) (twin,tcontrol);
						} //if (tcontrol->HotKeyActivate!=0) {									
#endif
					//} //if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {

				} //PStatus.flags&(EResizeX1|EResizeX2|EResizeY1|EResizeY2)) Control was not resizing
			} //tcontrol->flags&CIsMoving - control was moving




			//END BUTTON WAS DOWN AND UP ON THIS BUTTONUP CONTROL
			} else {//tcontrol->flags&CButton(button) - buttondown was on this control
			//BUTTON DOWN WAS NOT ON THIS BUTTONUP CONTROL
				//button down was not on this control
				//possibly PStatus.buttondowncontrol(3) to store buttondown control
				//go through all other controls:
				//  1) Filemenus and dropdowns with buttondown - close
				//	other?

			if (PStatus.flags&PInfo) {
				fprintf(stderr,"Button %d up on FTControl %s (but not down on this control)\n",button,tcontrol->name);
			}

/*
			//button up that was not down on same control, (up on anything other than a filemenu) closes all itemlist windows
			if (tcontrol->type!=CTFileMenu) {
				if (PStatus.flags&PItemListOpen) {
					if (PStatus.ExclusiveFTControl!=0) {
						CloseFTItemList(PStatus.ExclusiveFTControl->ilist);
					} //ExclusiveFTControl
				  PStatus.flags&=~PCloseItemListOnUp;
				} //PItemListOpen
			} else { //tcontrol->type!=CTFileMenu
				//this is a buttonup on a file menu but not the control that had the button up
				//if filemenus (itemlist windows) are open just open this filemenu
				//if filemenus are not open, open this filemenu
				//-ignore because mousemove should get it
			} //tcontrol->type!=CTFileMenu
*/


			//this might be faster with a PStatus.ButtonDownFTControl since buttondown can only be on one control at a time todo: repalce CTButtonDown(button) with PStatus.ButtonDownFTControl[button]=tcontrol
			//clear the CButtonDown(button) bit from any other controls
			//close any itemlists that might be open connected to that control
				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					if (tcontrol2->flags&CButtonDown(button)) {
						if (tcontrol2->type==CTFileMenu || tcontrol2->type==CTDropdown) {
							//button up occured on a different control, other than the filemenu or dropdownbox the button went down on.
							//close open itemlist windows
							//CloseFTItemListAll(tcontrol2);
							CloseFTItemList(tcontrol2->ilist);
						  PStatus.flags&=~PCloseItemListOnUp;
						} //tcontrol2->type==CTFileMenu
					tcontrol2->flags&=~CButtonDown(button); //clear button down bit
					PStatus.ButtonDownFTControl[button]=0;
					} //tcontrol2->flags&CButton
				tcontrol2=tcontrol2->next;
				}//tcontrol2!=0


			//END BUTTONDOWN WAS NOT ON THIS BUTTONUP CONTROL

			} //tcontrol->flags&CButton(button)




//			if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {
				//rarely if ever will a FTControl have a ButtonUp function
				//because most will have downup functions
				if (tcontrol->ButtonUp[button]!=0) {
				  //call any functions connected to ButtonUp
				(*tcontrol->ButtonUp[button]) (twin,tcontrol,x,y);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
				} //if (tcontrol->ButtonUp[button]!=0) {

//			} else {//if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {
				//a hotkey may have activated a control
				//check if hotkey was pressed- below code probably should not be here
#if 0 
				if (twin->HotKeyActivate!=0) {
					(*twin->HotKeyActivate) (twin,tcontrol);
					twin=GetFTWindow(winname); //check to see if window was deleted - we could pass twin to user function
					if (twin==0) return; //user deleted window 
				}
				//(check for deleted control?)
				if (tcontrol->HotKeyActivate!=0) {
					(*tcontrol->HotKeyActivate) (twin,tcontrol);
				}									
#endif
//			} //if (twin->flags&FTW_SIMULATE_BUTTON_CLICK) {

			//END BUTTONUP WAS ON THIS CONTROL
//		} 
	} //tcontrol2!=0 - buttonup is over control

//		} //!tcontrol->flags&(CNotVisible|CNotShowing) and buttonup is over this control


	//button down was not over this control, remove mouseover from any control with CMouseOver









	/*
	//for now I am not worrying about z dimension on controls - two controls should not overlap each other - change scroll bar and fileopen buttons to be outside of controls. They can be connected by name.

		  if (tcontrol2->z<curz) {
		    tcontrol=tcontrol2;
		    curz=tcontrol->z;
		  }
	*/





} else { // (!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow)  if there is no exclusive window or this window is the exclusive window
//itemlist windows can have exclusive control - although perhaps I should exclude itemlist windows from being PStatus.ExclusiveFTWindow
//fprintf(stderr,"This window (%s) does not have exclusive control\n",twin->name);
//if (PStatus.ExclusiveFTWindow!=0) {
//	fprintf(stderr,"%s does\n",PStatus.ExclusiveFTWindow->name);
//}
//a program window has exclusive input but this is not that window
//is this possible?

}// (!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow)  if there is no exclusive window or this window is the exclusive window


//moved windows functions because - window might be destroyed as result


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End CheckFTWindowButtonUp\n");
}

}  //end CheckFTWindowButtonUp



void CheckFTWindowMouseMove(FTWindow *twin, int x,int y)
{
int maxwidth,fw,fh,match; //ex2,match
//long long tnum,newst,newost,newdt,tst,tet;
FTControl *tcontrol,*tcontrol2;//,*tTimeLine;
#if Linux
FTWindow *twin2;
Cursor tcursor;
#endif
FTControl *tcontrol3;//,*tTimeLine;
int ti,tj;
//int cx,cx2,cy,cy2,bw,bh,i,
int i,maxheight,tlen;
int cx,cy,mx,my,curpos;
int cz;//,ignore;
int rx1,ry1,rx2,ry2;
//FTItem *litem;
//FTWindow *twin2;
FTItemList *tlist,*tlist2;
FTItem *titem,*titem2;
char winname[FTMedStr];
char conname[FTMedStr],*tmpstr;
int redraw_text;



if (PStatus.flags&PInfo) {
	fprintf(stderr,"CheckFTWindowMouseMove\n");
}

//mouse move function must complete before any new mousemove function
//otherwise there could be errors in human functions (and other ft funcs)
//possibly, I am not sure of this
//if (!(PStatus.flags&PMouseMoveFunc))  {

//PStatus.flags|=PMouseMoveFunc;
//possibly need bit to wait until last mousemove is done

//only if window has focus

//fprintf(stderr,"in mouse move %s %d %d\n",twin->name,x,y);

//check depth

if (twin==0) {
	fprintf(stderr,"Error: CheckFTWindowMouseMove called with null FTWindow\n");
	return;
} 

if (PStatus.flags&PInfo) {
	fprintf(stderr,"CheckFTWindowMouseMove in %s\n",twin->name);
}

strcpy(winname,twin->name);


//#if 0 
//this is when button 1 opens the filemenu and the button is held down
//as the mouse is moved over the file menu windows
//if the button is up no file menu can be open
//see if the mouse is over any open menu window
//search thru all popup windows, or find FTControl with open menu and search sub windows?
//option 2: find open filemenu FTControl and search thru open windows
//I think this is only a check for submenus which are on top and before main window
//#if 0 

//if (twin->flags&WItemList) {
	//fprintf(stderr,"here");
//}

//handle a mousemove when an itemlist is open differently than other mousemoves
//if (PStatus.flags&PItemListOpen) {

	//================================================
	//The mouse is moving and an itemlist window is open
	//================================================

	  //fprintf(stderr,"Mouse moving and itemlist window is open (%s)\n",twin->name);
	//if there was a buttondown on the control window, this window will not be the itemlist window

	//if (twin->flags&WItemList || (twin->flags&WButtonDown(0))) {
	if (twin->flags&WItemList) {
	//=======================================================================
	//The mouse move is over an open itemlist window (not a filemenu control)
	//=======================================================================

		//see if a new item is selected - if yes change current selection and redraw window
		tlist=twin->ilist;
		titem=GetFTItemFromFTItemListXY(tlist,x,y);

	//	tlist2=GetSelectedFTItemList(tlist,x,y);
		if (titem!=0) {  //the mouse is over an item- which it must be since this window is an itemlist window, but just to be on the safe side
			if (titem->num!=tlist->cur) { //mouse over item is not the currently selected item
				//unselect the last selected item - including all subitemlists
				//UnSelectFTItem(tlist,tlist->cur); //
				//Close any subitemlist
				titem2=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
				if (titem2!=0) {
					titem2->flags&=~FTI_SELECTED; //unselect item
					if (titem2->ilist!=0) {
						CloseFTItemList(titem2->ilist); 
					} //if (titem2->ilist!=0) {
				} //if (titem2!=0) {
				if (tlist->cur>0) {  //unselect any selected item
					DrawUnselectedFTItem(tlist,tlist->cur);
				}
				tlist->cur=titem->num;
				titem->flags|=FTI_SELECTED; //select item
				DrawSelectedFTItem(tlist,tlist->cur); 
				//Open any subitemlist for newly selected item
				if (titem->ilist!=0) {
					//don't bother if already open?
	//				fprintf(stderr,"Opening subitemlist\n");
					OpenFTItemList(titem->ilist);
				}

	//		SelectFTItem(tlist,titem->num);
	//do not redraw entire window - only draw unselected and selected items

	//		DrawFTWindow(twin); //redraw control (use DrawFTItemList)?
			//DrawFTWindow calls DrawFTItemList which will open any subitemlists?
				return; //don't bother processing rest
			} else { // titem->num!=tlist->cur
				//the mousemove is over the currently selected item

				//it is possible that the mouse has moved off a subitemlist
				//this should be done in the LostFocus Windows message
				//and so we need to unselect any selected item on the subitemlist
				//if this item has an open subitemlist with cur>0 drawunselected and set cur=0
				if (titem->ilist!=0) {  //item has subitemlist
					tlist2=titem->ilist;
					if (tlist2->window!=0) { //subitemlist has a window
						if (tlist2->window->flags&WOpen) { //subitemlist window is open
							if (tlist2->cur>0) {
								//note that this list cannot have any subitemlist open because it would have been closed when moving the mouse back onto the subitemlist
								DrawUnselectedFTItem(tlist2,tlist2->cur);
								tlist2->cur=0;
								PStatus.SelectedFTItem=titem; //currently selected item changes to parent menu
								PStatus.SelectedFTItemList=tlist;
							} //tlist2->cur>0
						} //WOpen
					} //if (tlist2->window!=0) { /
				}  //if (titem->ilist!=0) {
	
			return; //mouse is not over new item 
			} // titem->num!=tlist->cur
		} //titem!=0 - mouse is not over any item- an else is impossible (a mousemove on an itemlist window that is not on an item)
	return; //no item selected
	} //twin->flags&WItemList



//===================================================================================
//Below here an itemlist window is open but the mouse is not over the itemlist window
//===================================================================================

	//if the mouse is over a FileMenu control that is not the currently selected FileMenu control, open that FileMenu
	if (PStatus.ExclusiveFTControl!=0 && PStatus.ExclusiveFTControl->type==CTFileMenu) {
		tcontrol=0;
		tcontrol2=twin->icontrol;
		cz=200;
		while(tcontrol2!=0) {
			if ((tcontrol2->type==CTFileMenu) &&  !(tcontrol2->flags&CNotVisible) &&    
				x>=tcontrol2->x1 && x<=tcontrol2->x2 &&  y>=tcontrol2->y1 && y<=tcontrol2->y2) {
				if (tcontrol2->z<cz) { //depth of FTControl - only want top
				  cz=tcontrol2->z;
				  tcontrol=tcontrol2;
				} //if (tcontrol2->z<cz) { //depth of FTControl - only want top
			} //if ((tcontrol2->type==CTFileMenu) &&  !(tcontrol2->flags&CNotVisible) &&    
			tcontrol2=tcontrol2->next;
		} //while tcontrol2!=0
		if (tcontrol!=0 && tcontrol!=PStatus.ExclusiveFTControl) {
			//the mouse is over a new FileMenu 
			//Close the existing FileMenu
			CloseFTItemListControl(PStatus.ExclusiveFTControl);
			//set the global variable to the new filemenu control 
			PStatus.ExclusiveFTControl=tcontrol;
			//and open the new filemenu
			OpenFTItemList(tcontrol->ilist);
			//reset the last MouseOverFTControl
			if (PStatus.MouseOverFTControl!=0) {
				PStatus.MouseOverFTControl->flags&=~CMouseOver;
				DrawFTControl(PStatus.MouseOverFTControl);  //redraw control without mouseover
			} //if (PStatus.MouseOverFTControl!=0) {
			PStatus.MouseOverFTControl=tcontrol;
			tcontrol->flags|=CMouseOver;
		} //if (tcontrol!=0 && tcontrol!=PStatus.ExclusiveFTControl) {
	} //if (PStatus.ExclusiveFTControl->type==CTFileMenu) {  //the exclusive control is a filemenu

//} //if (PStatus.flags&PItemListOpen) {


//=====================================
//Below here no itemlist window is open
//=====================================


//  fprintf(stderr,"Mouse not over itemlist window but itemlist window open\n");

//if the user is selecting text- mouse events over other controls are not recognized
//so before checking if the mouse is over any control, check to see if the user is selecting text
	if (PStatus.flags&PSelectingText) {
		//user is selecting text
	
		//get the textbox or textarea control that the user is selecting text in
		//this could be retrieved from twin->FocusFTControl or PStatus.ButtonDownFTControl
		tcontrol=twin->FocusFTControl;
		//tcontrol=PStatus.ButtonDownFTControl;
		if (tcontrol!=0) {
	
			if ((tcontrol->type==CTTextBox  || tcontrol->type==CTTextArea)) {  //button down is possibly selecting text
				//if (PStatus.flags&PInfo) {
					//fprintf(stderr,"mouse move selecting text\n");
				//}
		//              fprintf(stderr,"button down=%lli\n",(tcontrol->flags&CButtonDown(0)));
		//          fprintf(stderr,"button down textbox part\n");
				//mouse position 2 in text box
							//i[2]=original mouse position when button went down (in characters)
							//i[3]=current mouse position with button still down (in characters)
				//translate mouse position to character position

				redraw_text=0;
				fw=tcontrol->fontwidth;				
				maxwidth=(unsigned int)((tcontrol->x2-tcontrol->x1)/fw)-1;
				if (tcontrol->type==CTTextArea) {
					fh=tcontrol->fontheight;
					maxheight=((tcontrol->y2-tcontrol->y1)/fh)-1;
				}
				cx=0;//tcontrol->CursorLocationInChars;
				cy=0;
				//curpos=cy*maxwidth+cx; //cursor position							
				curpos=tcontrol->StartSelectedTextInChars;//
				//curpos=tcontrol->ButtonDownCursorLocationInChars;
		//              tcontrol->EndSelectedTextInChars=curpos;
		//							fprintf(stderr,"current curpos=%d\n",curpos);

				mx=(x-tcontrol->x1)/fw;
				//todo: need to determine CursorX and CursorY
				tcontrol->CursorX=mx; //update the current Cursor X position
				//we already know that the mouse is over the FTControl- update: no- now the mouse can be selecting text outside of a control
				//actually - since mouse is left of x1 - perhaps button down was on this FTControl?
				//scrolling may only be in textbox for now
				if (tcontrol->type==CTTextBox) {
					//determine nearest character: CursorX
					i=tcontrol->FirstCharacterShowing;
					tmpstr=tcontrol->text;
					cx=0;
					while(cx<mx && tmpstr[i]!=0) {
						i++;
						cx++;
					}
					tcontrol->CursorX=cx;

					//if (tcontrol->EndSelectedTextInChars<tcontrol->x1) { //scroll left if possible
					if (x<tcontrol->x1) { //scroll left if possible textbox only
						if (tcontrol->FirstCharacterShowing>0) {
							tcontrol->FirstCharacterShowing--;
							//tcontrol->StartSelectedTextInChars+=fw;  //have to move original down click
							tcontrol->StartSelectedTextInChars++;//=fw;  //have to move original down click
							redraw_text=1;
						} //if (tcontrol->FirstCharacterShowing>0) {
						//tcontrol->EndSelectedTextInChars=tcontrol->x1; //why is this needed?			
					}//if (x<tcontrol->x1) { //scroll left if possible textbox only

					if ((x-tcontrol->x1)>(maxwidth*fw) && (int)(strlen(tcontrol->text)-tcontrol->FirstCharacterShowing)>maxwidth) { //scroll right if possible
						//fprintf(stderr,"%d > 0\n",strlen(tcontrol->text)-tcontrol->FirstCharacterShowing-maxwidth);          
						//if (strlen(tcontrol->text)-tcontrol->FirstCharacterShowing>0) 
						tcontrol->FirstCharacterShowing++;
						//tcontrol->StartSelectedTextInChars-=fw;  //have to move original down click
						tcontrol->StartSelectedTextInChars--;//=fw;  //have to move original down click
						redraw_text=1;
						//tcontrol->EndSelectedTextInChars=tcontrol->x2; //why is this needed?
					} //if ((x-tcontrol->x1)>(maxwidth*fw) && (int)(strlen(tcontrol->text)-tcontrol->FirstCharacterShowing)>maxwidth) { //scroll right if possible
				} //CTTextBox
				 
				if (tcontrol->type==CTTextArea) {
					my=(y-tcontrol->y1)/fh;  //note CursorY is adjusted below
					if (y<tcontrol->y1) { 
						if (tcontrol->FirstCharacterShowing>0) {
							//go up one line
							curpos=tcontrol->FirstCharacterShowing;
							while(curpos>0 && tcontrol->text[curpos]!=10) {
								curpos--;
							}
							tcontrol->FirstCharacterShowing=curpos;
							tcontrol->StartSelectedTextInChars=curpos; //have to move original down click
							redraw_text=1;
						}//if (tcontrol->FirstCharacterShowing>0) {
					} //if (y<tcontrol->y1) { 
				} //if (tcontrol->type==CTTextArea) {

	
				//determine the character that the mouse is now closest to
				//character positions start with 0
				//mouse can be to left or right of control
				//adjust mouse X position to be on the control
				if (x<=tcontrol->x1) { 
					mx=0;
				} else {
					if (x>tcontrol->x2) {
						mx=maxwidth;
					} else {
						mx=(int)((x-tcontrol->x1)/fw); //mouse position in characters
					}
				} //if (x<=tcontrol->x1) { 


				//find out which character is closest on this line if any
				//go through each character starting with i[1]
				i=tcontrol->FirstCharacterShowing;
				tlen=strlen(tcontrol->text);
				curpos=tcontrol->CursorLocationInChars;
				//curpos=tcontrol->ButtonDownCursorLocationInChars;
				//fprintf(stderr,"mx=%d my=%d\n",mx,my);
				//mx,my are mouse coordinates in characters
				if (tcontrol->type==CTTextBox) {
					if (mx>tlen) {  //button down was past text (but still in textbox)
						mx=tlen;
					}
					curpos=i+mx;
					if (i!=curpos) {										
						//the current cursor position is different from the last cursor position, so we need to redraw the control (for the new cursor)
						redraw_text=1;
					}
				} else { //CTTextArea
					fh=tcontrol->fontheight;  //I don't know why but without this, there is an warning about the second below fh
					maxheight=(tcontrol->y2-tcontrol->y1)/fh;

					//mouse can be above or below control
					//adjust mouse Y position to be on the control
					if (y<=tcontrol->y1) { 
						my=0;
					} else {
						if (y>tcontrol->y2) {
							my=maxheight-1;
						} else {
							my=(int)((y-tcontrol->y1)/fh); //mouse position in characters
						}
					} //if (y<=tcontrol->y1) { 


					while(i<=tlen && cy<maxheight) {	
						if ((cy==my && cx>=mx) || i==tlen || cy>my) {
							if (i!=curpos) {										
								redraw_text=1;
							}
							curpos=i; //this is closest position to mouse
							i=tlen+1; //exit loop
						} //cy<=my

						if (tcontrol->text[i]==10) {
							if (cy==my && cx<mx) {  //mouse is on same row but to right of text
								curpos=i;
								i=tlen+1; //exit loop
							} else { //mouse is on same row but to right of text
								cy++;
								cx=0;
							} //mouse is on same row but to right of text
						} else {
							cx++;
							if (cx>maxwidth-1) {
								cx=0;
								cy++;
							} //cx x2
						} //text[i]==10
						i++;
					}	//while				
					tcontrol->CursorX=cx;
					tcontrol->CursorY=cy;
				} //if (tcontrol->type==CTTextBox) {	

		//fprintf(stderr,"curpos=%d [2]=%d [3]=%d\n",curpos,tcontrol->StartSelectedTextInChars,tcontrol->EndSelectedTextInChars);

				
				//adjust selected text start and end character positions				
				if (curpos!=tcontrol->ButtonDownCursorLocationInChars) {
					if (curpos<tcontrol->ButtonDownCursorLocationInChars) {
						//mouse position now is left of original button press position
						if (tcontrol->StartSelectedTextInChars!=curpos || tcontrol->EndSelectedTextInChars!=tcontrol->ButtonDownCursorLocationInChars) {
							//the selected start or end have changed and we need to redraw
							redraw_text=1;
						}
						tcontrol->StartSelectedTextInChars=curpos;
						tcontrol->EndSelectedTextInChars=tcontrol->ButtonDownCursorLocationInChars;
					}  else {
						//mouse position now is right of original button press position
						if (tcontrol->StartSelectedTextInChars!=tcontrol->ButtonDownCursorLocationInChars || tcontrol->EndSelectedTextInChars!=curpos) {
							//the selected start or end have changed and we need to redraw
							redraw_text=1;
						}
						tcontrol->StartSelectedTextInChars=tcontrol->ButtonDownCursorLocationInChars;
						tcontrol->EndSelectedTextInChars=curpos;
					}
				} //if (curpos!=tcontrol->ButtonDownCursorLocationInChars) {
							//fprintf(stderr,"[2]=%d [3]=%d\n",tcontrol->StartSelectedTextInChars,tcontrol->EndSelectedTextInChars);

				//fprintf(stderr,"i[4]=%d %d\n",tcontrol->i[4],tcontrol->i[5]);

				//note: possibly cursor position should only change when button goes up
				//a control field ButtonDownCursorLocationInChars could be stored to hold onto the original position
				//alternatively a SelectedCursorLocationInChars control field could be created to store the current cursor position when selecting text
				//and then on buttonup the actual cursor location can be changed

				tcontrol->CursorLocationInChars=curpos;//tcontrol->FirstCharacterShowing+curpos;				
              
		//need to draw textbox or textarea? - this is any textbox or textarea
     			if (redraw_text)	{
		//			fprintf(stderr,"draw control in buttondown mousemove\n");
					DrawFTControl(tcontrol);
				} //redraw
			} //if ((tcontrol->type==CTTextBox  || tcontrol->type==CTTextArea)) {  //button down is possibly selecting text

		} //if (tcontrol!=0) //there is a control with focus

//		if (twin->MouseMove>0) {  //call any MouseMove window functions? or ignore for selecting text
//		(*twin->MouseMove) (twin,x,y);
//		}//if (twin->MouseMove>0) {  //call any MouseMove window functions 

		return; //done with mousemove event
	}

//Find the control that the mouse is over
tcontrol=0;
tcontrol2=twin->icontrol;
cz=200;
while(tcontrol2!=0) {

//PFileMenuOpen
//PDropdownOpen

	//if the open itemlist is a filemenu control, search for other filemenu controls mouse might be over
//	if (PStatus.ExclusiveFTControl!=0 && PStatus.ExclusiveFTControl->type==CTFileMenu) {
	//	if ((!(PStatus.flags&PItemListOpen) || (tcontrol2->type==CTFileMenu)) &&  
	if (!(tcontrol2->flags&CNotVisible) &&    
		    x>=tcontrol2->x1 && x<=tcontrol2->x2 &&  y>=tcontrol2->y1 && y<=tcontrol2->y2) {
		    //fprintf(stderr,"over FTControl %s\n",tcontrol2->name);
		if (tcontrol2->z<cz) { //depth of FTControl - only want top
		    cz=tcontrol2->z;
				//fprintf(stderr,"Mouse over %s\n",tcontrol2->name);
		    tcontrol=tcontrol2;
		} //if (tcontrol2->z<cz) { //depth of FTControl - only want top
	} //if (!(tcontrol2->flags&CNotVisible) && 
	//} //if (PStatus.ExclusiveFTControl!=0 && PStatus.ExclusiveFTControl->type==CTFileMenu) {


/* redo with a global flag FT_FTCONTROL_MOVING, and FT_FTCONTROL_RESIZING and skip trying to find which control the mouse is over if these bits are set

	//if any control is being resized or moved set tcontrol to it even if mouse is not over
		if ((tcontrol2->flags&CIsMoving)||(tcontrol2->flags&(CResize|CResizeX1|CResizeX2|CResizeY1|CResizeY2)) ) {
				//a control had a buttondown on it
				//is MouseMove w/ button 0 down, if off of FTControl, still move/resize/select text FTControl 
				if (tcontrol2->flags&CButtonDown(0) &&  
				    tcontrol2->type!=CTFileMenu &&
				    tcontrol2->type!=CTTextBox &&
				    tcontrol2->type!=CTTextArea) {  //do not ignore mousemove in textbox
				   tcontrol=tcontrol2;
		 //      ignore=1;  //exit
				  // fprintf(stderr,"ignore=1\n");
				}  //CButtonDown

		}  //if CIsMoving, CResize, etc.
*/


tcontrol2=tcontrol2->next;
} //while tcontrol2!=0


//mouse is not over any control
if (tcontrol==0) {

//reset any mouseover control
	if (PStatus.MouseOverFTControl!=0) { //the mouse was over some other control
		PStatus.MouseOverFTControl->flags&=~CMouseOver;
		//fprintf(stderr,"Redraw %s\n",PStatus.MouseOverFTControl->name);
		if (!(PStatus.MouseOverFTControl->flags&CNoChangeOnMouseOver)) { //only redraw control if color change on mouse over is enabled
			DrawFTControl(PStatus.MouseOverFTControl);  //redraw control without mouseover
		}
		PStatus.MouseOverFTControl=0;
	} //MouseOver!=0


	//todo: add call to moving and resizing controls where mouse over different control doesn't matter as much


  if (twin->MouseMove>0) {  //call any MouseMove window functions 
	(*twin->MouseMove) (twin,x,y);
	twin=GetFTWindow(winname); //check to see if window was deleted
	tcontrol=GetFTControl(conname); //check to see if control was deleted
	if (tcontrol==0 || twin==0) {
		return; //user deleted control or window
	}
  }//if (twin->MouseMove>0) {  //call any MouseMove window functions 

	//return; //mouse is not over any control and filemenu or dropdown itemlist is not open
}  //if (tcontrol==0) {



//If a button was down on a ScrollBar Slide button, even if the mouse is not over a control, the scroll control needs to be redrawn
//	for(i=0;i<FT_NUM_MOUSE_BUTTONS;i++) {
	if (PStatus.ButtonDownFTControl[0]!=0  && !(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {
		//button is down on a control that may be a scrollbar
		//(tcontrol->flags&(CVScrollSlide|CHScrollSlide)) && ) {
		//fprintf(stderr,"ButtonDownFTControl\n");
		tcontrol3=PStatus.ButtonDownFTControl[0];
		
		//for dragging a scroll bar button, need to redraw control
			//note that CVScrollSlide is the actual slide button
		if (tcontrol3->flags&CVScrollSlide) {
		//fprintf(stderr,"Adjusting vertical scroll bars\n");
			VScrollSlide_Button0DownMouseMove(twin,tcontrol3,x,y);
		}
		if (tcontrol3->flags&CHScrollSlide) {
//					fprintf(stderr,"Adjusting horizontal scroll bar\n");
			HScrollSlide_Button0DownMouseMove(twin,tcontrol3,x,y,0);
		}

//if control with button down is timeline, select and/or scroll the timeline and connected controls and labels, and adjust the selected time indicator controls
			if (tcontrol3->type==CTTimeLine) {
				//get frame timeline is connected to
				//currently this is not the best method, perhaps there needs to be ConnectedFrameFTControl
				//because we have to go through the window to find the frame with this timeline control
				//perhaps simply connect scroll control to timeline - possibly use ->parent - but may be set by user
				//timeline should perhaps be completely autocreated and maintained by FT
				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					if (tcontrol2->type==CTFrame && tcontrol2->TimeLineFTControl==tcontrol3) {
						//fprintf(stderr,"found frame with timeline\n");
							//adjust start, end time of selection on timeline, and all connected data controls
						FT_AdvanceTimeLine(twin,tcontrol3,x,y);
						if (tcontrol2->FTC_HScrollSlide!=0)  {	

							//scroll frame scroll bar
							if (x>tcontrol2->x2 && x>=PStatus.LastMouseX) { //scroll right
								//fprintf(stderr,"Adjusting horizontal scroll bar\n");								
								//x=x-tcontrol2->x2+tcontrol2->FTC_HScrollSlide->x2;
								HScrollSlide_Button0DownMouseMove(twin,tcontrol2->FTC_HScrollSlide,x,y,x-tcontrol2->x2);							
								//HScrollRight_ButtonDown(twin,tcontrol2,x,y,0);
								//DrawFTControl(tcontrol2); //redraw frame
							} else {//x>tcontrol->x2
								if (x<tcontrol2->x1 && x<=PStatus.LastMouseX) {  //scroll left
									//this uses PStatus.LastMouseX-x; 
									//x=tcontrol2->FTC_HScrollSlide->x1-(tcontrol2->x1-x);
									HScrollSlide_Button0DownMouseMove(twin,tcontrol2->FTC_HScrollSlide,x,y,x-tcontrol2->x1);							
									//HScrollLeft_ButtonDown(twin,tcontrol2,x,y,0);
									//DrawFTControl(tcontrol2); //redraw frame
								} //x<tcontrol2->x1
							} //x>tcontrol->x2

						} //tcontrol2->FTC_HScrollSlide!=0
					} //tcontrol2->type==CTFrame
					tcontrol2=tcontrol2->next;
				} //while(tcontrol2
			} //CTTimeLine

			//scroll data forward or back relative to mouse x,y
			if (tcontrol3->type==CTData) {

				//if this data control is part of a frame that has a horizontal scrollbar, it may need to be scrolled
				tcontrol2=tcontrol3->FrameFTControl;
				if (tcontrol2!=0) {
					if (tcontrol2->FTC_HScrollSlide!=0)  {
						//scroll frame scroll bar
//							if (x>tcontrol2->x2 || x<tcontrol2->x1) {
							//scroll frame scroll bar
							if (x>tcontrol2->x2 && x>=PStatus.LastMouseX) { //scroll right
								//fprintf(stderr,"Adjusting horizontal scroll bar\n");								
								//x=x-tcontrol2->x2+tcontrol2->FTC_HScrollSlide->x2;
								
								HScrollSlide_Button0DownMouseMove(twin,tcontrol2->FTC_HScrollSlide,x,y,x-tcontrol2->x2);							
								//FT_AdvanceTimeLine(twin,tcontrol2,x,y);
								//DrawFTControl(tcontrol2); //redraw frame
							} else {//x>tcontrol->x2
								if (x<tcontrol2->x1 && x<=PStatus.LastMouseX) {  //scroll left
									//this uses PStatus.LastMouseX-x; 
									//x=tcontrol2->FTC_HScrollSlide->x1-(tcontrol2->x1-x);
									HScrollSlide_Button0DownMouseMove(twin,tcontrol2->FTC_HScrollSlide,x,y,x-tcontrol2->x2);							
									//FT_AdvanceTimeLine(twin,tcontrol2,x,y);
									//DrawFTControl(tcontrol2); //redraw frame
								} //x<tcontrol2->x1
							} //x>tcontrol->x2

					} //tcontrol2->FTC_HScrollSlide!=0
				} //tcontrol2!=0			
				FT_SelectData(twin,tcontrol3,x,y);
			} //CTData

//				DrawFTControl(tcontrol);

	} //if (PStatus.ButtonDownFTControl[0]!=0  && !(twin->flags&FTW_SIMULATE_BUTTON_CLICK)) {


	//if mouse move is not over a control we are done
	if (tcontrol==0) {
		return;
	}

//==========================================
//From here down the mouse is over a control
//==========================================

strcpy(conname,tcontrol->name);



//fprintf(stderr,"MouseMove here\n");
//tcontrol may=0
if (tcontrol!=PStatus.MouseOverFTControl) { //different control has mouse over

//  fprintf(stderr,"x,y =%d,%d win=%d,%d %s\n",x,y,twin->w,twin->h,twin->name);
  //if mouse is outside of window, erase all FTControl mouse overs

//HScrollSlide_MouseMove maybe should be HScrollSlide_ButtonDownMouseMove


				//call user MouseMove function and return - technically this should be tcontrol->ButtonDownMouseMove()
		//if (tcontrol->MouseMove>0) {  //call any FTControl functions connected to MouseMove
		 //   (*tcontrol->MouseMove) (twin,tcontrol,x,y);
		//	return;
		//} //if (tcontrol->MouseMove>0) {

	//} //PStatus.ButtonDownFTControl[i]!=0

	} //if (tcontrol!=PStatus.MouseOverFTControl) { //different control has mouse over


#if 0 
	//perhaps replace with PStatus.CursorFTControl if only one can have cursor change
	//go through and reset any control that caused a cursor change
	tcontrol2=twin->icontrol;
	while (tcontrol2!=0) {

	//erase cursor and prepare for next cursor
		if ((tcontrol2->flags&CCursor) && !(tcontrol2->flags&CButtonDown(0))) { 
			//remove any resize cursor, unless button down (resizing)
			//fprintf(stderr,"restore cursor2 %s\n",tcontrol->name);
			tcontrol2->flags&=~CCursor; 
#if Linux
			XUndefineCursor(PStatus.xdisplay,twin->xwindow);
			XFlush(PStatus.xdisplay); ///need to update cursor
#endif
		  } //if ((tcontrol2->flags&CCursor) && !(tcontrol2->flags&CButtonDown(0))) { 

		 tcontrol2=(FTControl *)tcontrol2->next;
	}  //end while
#endif

	//need to look at selecting text - only redraw textarea when new character is selected
	//only need to remove mouseover for buttons
	//app has no itemlist open
	//perhaps need - mousechanges bit which is default for CTButton, CTDropdown, etc
	if (PStatus.MouseOverFTControl!=0 && tcontrol!=PStatus.MouseOverFTControl &&
		PStatus.MouseOverFTControl->type!=CTTextArea && PStatus.MouseOverFTControl->type!=CTTextBox && 
		PStatus.MouseOverFTControl->type!=CTFileOpen && PStatus.MouseOverFTControl->type!=CTItemList) {  
		//fprintf(stderr,"Mouse was over some control\n");
		//	fprintf(stderr,"resetting Mouse over %s\n",PStatus.MouseOverFTControl->name);
		PStatus.MouseOverFTControl->flags&=~CMouseOver;
		tcontrol2=PStatus.MouseOverFTControl;
		if (!(tcontrol2->flags&CNoChangeOnMouseOver)) { //only redraw control if color change on mouse over is enabled
			DrawFTControl(tcontrol2);  //redraw control without mouseover
		} 
			//PStatus.MouseOverFTControl=tcontrol;
	//		fprintf(stderr,"after draw control mouseover\n");
	}//	if (PStatus.MouseOverFTControl!=0 && tcontrol!=PStatus.MouseOverFTControl &&


	//      fprintf(stderr,"mouse move over %s\n",tcontrol->name);

	//if (PStatus.flags&PInfo) fprintf(stderr,"Mouse is over FTControl '%s'\n",tcontrol->name);


	//Finally set PStatus.MouseOverFTControl to the current control the mouse is over and redraw it
	//Change color of control since mouse is over
	tcontrol->flags|=CMouseOver;
	PStatus.MouseOverFTControl=tcontrol; //set new mouseover control
	if (!(tcontrol->flags&CNoChangeOnMouseOver) && tcontrol->type!=CTTextBox && tcontrol->type!=CTTextArea && tcontrol->type!=CTFileOpen) {
	//		fprintf(stderr,"draw control mouseover tcontrol=%p\n",tcontrol);
		DrawFTControl(tcontrol);  //CTFileOpen FTControls take time to draw, and causes too slow/
		//fprintf(stderr,"draw control mouseover\n");
	} //if (!(tcontrol->flags&CNoChangeOnMouseOver) && 


	//if mouse is over a filemenu control
	//and a file menu is open
	//if the filemenu control itemlist is not open - close the open itemlist window of the other filemenu control
	//and open this control's itemlist 

	//diagnal 
	//XC_bottom_left_corner
	//XC_bottom_right_corner
	//XC_top_left_corner
	//XC_top_right_corner
	//#if 0 //redo later when revisiting FTI


	//CONTROL IS MOVING
	//check for resize - mouse is within FTControl already
	if (!(tcontrol->flags&CIsMoving)||(tcontrol->flags&(CResize|CResizeX1|CResizeX2|CResizeY1|CResizeY2)) ) {
		match=0;
		rx1=0;
		rx2=0;
		ry1=0;
		ry2=0;
		if (x<tcontrol->x1+PStatus.ResizePix) {
			rx1=1;
		}
		if (x>tcontrol->x2-PStatus.ResizePix) {
			rx2=1;
		}
		if (y<tcontrol->y1+PStatus.ResizePix) {
			ry1=1;
		}
		if (y>tcontrol->y2-PStatus.ResizePix) {
			ry2=1;
		}


		//top left diag
		if (!match && (tcontrol->flags&CResize || 
			(tcontrol->flags&CResizeX1 && tcontrol->flags&CResizeY1)) && 
			rx1 && ry1) {  
			//resize top left
		//              fprintf(stderr,"add top left cursor %s\n",tcontrol->name);
			match=1;  //match here because may be resizing but not setting cursor
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_top_left_corner);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif
			} //if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		}//		if (!match && (tcontrol->flags&CResize || 

		if (!match && ((tcontrol->flags&CResize) || 
			((tcontrol->flags&CResizeX1) && (tcontrol->flags&CResizeY2))) && 
			rx1 && ry2) {  
			//resize bottom left
			match=1;  //match here because may be resizing but not setting cursor
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_bottom_left_corner);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				 GdiFlush();
#endif

			}  //CCursor
		} //		if (!match && ((tcontrol->flags&CResize) || 

		if (!match && (tcontrol->flags&CResize || 
			(tcontrol->flags&CResizeX2 && tcontrol->flags&CResizeY1)) && 
			rx2 && ry1) {  
				//resize top right
			match=1;  //match here because may be resizing but not setting cursor
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_top_right_corner);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			} //if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		} //if (!match && (tcontrol->flags&CResize || 

		if (!match && (tcontrol->flags&CResize || 
			(tcontrol->flags&CResizeX2 && tcontrol->flags&CResizeY2)) && 
			rx2 && ry2) {  
			//resize bottom right
			match=1;  //match here because may be resizing but not setting cursor
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_bottom_right_corner);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			} //if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		} //if (!match && (tcontrol->flags&CResize || 

		if (!match && tcontrol->flags&(CResize|CResizeX1) && rx1) {  
		//resize left
		//fprintf(stderr,"add cursor %llx %s\n",tcontrol->flags,tcontrol->name);
			match=1;
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
				//uncomment these 3 lines to see various (very limited) cursors
				//tcursor=XCreateFontCursor(PStatus.xdisplay,XC_center_ptr);
				//tcontrol->i[4]++;
				//fprintf(stderr,"cursor=%d",tcontrol->i[4]);
				//tcursor=XCreateFontCursor(PStatus.xdisplay,tcontrol->i[4]);
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_sb_h_double_arrow);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			} //if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		} //if (!match && tcontrol->flags&(CResize|CResizeX1) && rx1) {  

		if (!match && tcontrol->flags&(CResize|CResizeX2) && rx2) {  
			//resize right
			//fprintf(stderr,"add cursor %s\n",tcontrol->name);
			match=1;
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_sb_h_double_arrow);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			}//if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		}//if (!match && tcontrol->flags&(CResize|CResizeX2) && rx2) {  

		if (!match && tcontrol->flags&(CResize|CResizeY1) && ry1) {  
			//resize up
		//              fprintf(stderr,"add up cursor %s\n",tcontrol->name);
			match=1;
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_double_arrow);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			}//if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		}//if (!match && tcontrol->flags&(CResize|CResizeY1) && ry1) {  

		if (!match && tcontrol->flags&(CResize|CResizeY2) && ry2) {  
			//resize up
		//              fprintf(stderr,"add down cursor %s\n",tcontrol->name);
			match=1;
			if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
#if Linux
				tcursor=XCreateFontCursor(PStatus.xdisplay,XC_double_arrow);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,tcursor);
				tcontrol->flags|=CCursor;  //cursor to erase later
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

			} //if (!(tcontrol->flags&CCursor)) { //have not changed the cursor yet
		} //if (!match && tcontrol->flags&(CResize|CResizeY2) && ry2) {  


		if (!match && (tcontrol->flags&CCursor) && !(tcontrol->flags&CButtonDown(0))) {
			//CButtonDown=if this FTControl is being resized now leave cursor
			//fprintf(stderr,"restore cursor %llx %s\n",tcontrol->flags,tcontrol->name);
			tcontrol->flags&=~CCursor; 
			//fprintf(stderr,"restore cursor %llx %s\n",tcontrol->flags,tcontrol->name);
#if Linux
			XUndefineCursor(PStatus.xdisplay,twin->xwindow);
			XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
			GdiFlush();
#endif

		} //if (!match && (tcontrol->flags&CCursor) && !(tcontrol->flags&CButtonDown(0))) {
	}  //!(tcontrol->flags&CIsMoving)tcontrol->flags&(CResize|CResizeX1|CResizeX2|CResizeY1|CResizeY2) )  - control is moving or resizing
	//END CONTROL IS MOVING
//	#endif //if 0 redo later when revisiting FTI




//BUTTON IS DOWN ON THIS CONTROL	
	//There was a button down on this control
	if (tcontrol->flags&CButtonDown(0)) {

		if (PStatus.flags&PInfo) {
			fprintf(stderr,"Drag FTControl %s %d\n",tcontrol->name,PStatus.MouseX);
		}

//#if 0 //redo when revisiting FTI
	// fprintf(stderr,"button down part\n");
	//if(PStatus.LastMouseX>=tcontrol->x1 && PStatus.LastMouseX<=tcontrol->x2 && PStatus.LastMouseY>=tcontrol->y1 && PStatus.LastMouseY<=tcontrol->y2)
	//{  //mouse is over FTControl
	//problem:  change in mouse for small FTControls (mouse moves off/leads FTControl)
	//am using lastMouseMove
	//perhaps should use +-6 if moving


		// if (tcontrol->flags&(CMove|CResize|CResizeX1|CResizeX2))
		//control is movable, or needs to have resize cursor reset
		if (tcontrol->flags&(CMove|CCursor)) {
		//            if (tcontrol->flags&CMove) {
		//I use CCursor to see if this FTControl is being resized
		//move the FTControl
			if (!(PStatus.flags&EMouseMove)) { //this is the first mouse move
			//              fprintf(stderr,"x=%d,y=%d\t",x,y);
			//if (x>=tcontrol->x1 && x<=tcontrol->x2 && y>=tcontrol->y1 && y<=tcontrol->y2)
			//look at button down X and Y, not current mouse x and y
				if (PStatus.ButtonX[0]>=tcontrol->x1 && PStatus.ButtonX[0]<=tcontrol->x2 && PStatus.ButtonY[0]>=tcontrol->y1 && PStatus.ButtonY[0]<=tcontrol->y2) {  //if mouse is over this FTControl

					//only first FTControl in this location is dragged and dropped
					PStatus.flags|=EMouseMove;
					tcontrol->flags|=CIsMoving;
					tcontrol->flags&=~CDoneMoving;
					//PStatus.MouseX=x;
					//PStatus.MouseY=y;
				} //if (PStatus.ButtonX[0]>=tcontrol->x1 && PStatus.ButtonX[0]<=tcontrol->x2 && P
			} else {  //!PStatus.flags&EMouseMove
              
				//not first mouse move
				//fprintf(stderr,"not first mouse move\n");
				//fprintf(stderr,"EResizeX1=%d\n",PStatus.flags&EResizeX1);
				match=0;
				if (tcontrol->flags&(CResize|CResizeX1) && PStatus.flags&EResizeX1) {
					//fprintf(stderr,"Resize left\n");
				//                  if (tcontrol->x1+(x-PStatus.LastMouseX)>twin->x1) {
					if (tcontrol->x1+(x-PStatus.LastMouseX)>0) {
						match=1;
						EraseFTControl(tcontrol);  //either erase now or make newx1,newy1, ...
						tcontrol->x1+=(x-PStatus.LastMouseX);
						if (tcontrol->x1>=tcontrol->x2) {
							tcontrol->x1=tcontrol->x2-2;
						}
						if (tcontrol->type==CTOption) {
							tcontrol->x3+=(x-PStatus.LastMouseX);
							if (tcontrol->x3<=tcontrol->x1) {
								tcontrol->x3=tcontrol->x2-1;
							}
						} //if (tcontrol->type==CTOption) {

						GetFTControlBackground(tcontrol);
						DrawFTControl(tcontrol);
					} //if (tcontrol->x1+(x-PStatus.LastMouseX)>0) {
				}  //if (tcontrol->flags&(CResize|CResizeX1) && PStatus.flags&EResizeX1) {
				//end if resize left

				if (tcontrol->flags&(CResize|CResizeX2) && PStatus.flags&EResizeX2) {
					//fprintf(stderr,"Resize right\n");
					//fprintf(stderr,"%d %d\n",tcontrol->x2,x-PStatus.LastMouseX);
					//if (tcontrol->x2+(x-PStatus.LastMouseX)<twin->x1+twin->w) {
					if (tcontrol->x2+(x-PStatus.LastMouseX)<twin->w) {
						match=1;
						EraseFTControl(tcontrol);
						tcontrol->x2+=(x-PStatus.LastMouseX);
						if (tcontrol->x2<=tcontrol->x1) {
							tcontrol->x2=tcontrol->x1+2;
						}
						if (tcontrol->type==CTDropdown) {
							tcontrol->x3+=(x-PStatus.LastMouseX);
							if (tcontrol->x3<=tcontrol->x1) {
								tcontrol->x3=tcontrol->x1+1;
							}
						}
                    
						GetFTControlBackground(tcontrol);                    
						DrawFTControl(tcontrol);
					} //if (tcontrol->x2+(x-PStatus.LastMouseX)<twin->w) {
					//fprintf(stderr,"tcontrol->x2=%d\n",tcontrol->x2);
				}  //if (tcontrol->flags&(CResize|CResizeX2) && PStatus.flags&EResizeX2) {  
				//end if resize right


				if (tcontrol->flags&CResize && PStatus.flags&EResizeY1) {
					//fprintf(stderr,"Resize top\n");
				//                  if (tcontrol->y1+(y-PStatus.LastMouseY)<twin->y1+twin->h+5) {
					if (tcontrol->y1+(y-PStatus.LastMouseY)<twin->h+5) {
						match=1;
						EraseFTControl(tcontrol);
						tcontrol->y1+=(y-PStatus.LastMouseY);
						if (tcontrol->y1>=tcontrol->y2) {
							tcontrol->y1=tcontrol->y2-1;
						}
						GetFTControlBackground(tcontrol);
						DrawFTControl(tcontrol);
					}
				}  //end if resize top


				if (tcontrol->flags&CResize && PStatus.flags&EResizeY2) {
				//                  if (tcontrol->y2+(y-PStatus.LastMouseY)<twin->y1+twin->h+5) {
					if (tcontrol->y2+(y-PStatus.LastMouseY)<twin->h+5) {
						match=1;
						EraseFTControl(tcontrol);
						tcontrol->y2+=(y-PStatus.LastMouseY);
						if (tcontrol->y2<=tcontrol->y1) {
							tcontrol->y2=tcontrol->y1+1;
						}
						GetFTControlBackground(tcontrol);
						DrawFTControl(tcontrol);
					}
				}  //	if (tcontrol->flags&CResize && PStatus.flags&EResizeY2) {
				//end if resize bottom



				//if (0) { //tcontrol->flags&CIsMoving) {
				if (match==0 && tcontrol->flags&CIsMoving) {
					//needs boundary checking like tcontrol->boundx1,boundy1
				//                 fprintf(stderr,"1 y1=%i y2=%i\n",tcontrol->y1,tcontrol->y2);


					EraseFTControl(tcontrol);
					tcontrol->x1+=(x-PStatus.LastMouseX);
					tcontrol->y1+=(y-PStatus.LastMouseY);
					tcontrol->x2+=(x-PStatus.LastMouseX);
					tcontrol->y2+=(y-PStatus.LastMouseY);
					tcontrol->x3+=(x-PStatus.LastMouseX);
					tcontrol->y3+=(y-PStatus.LastMouseY);
				//                 fprintf(stderr,"2 y1=%i y2=%i\n",tcontrol->y1,tcontrol->y2);
				//                  if (tcontrol->x1<twin->x1) {
					if (tcontrol->x1<0) {
					ti=tcontrol->x2-tcontrol->x1;
					tj=tcontrol->x3-tcontrol->x1;
					tcontrol->x1=0;//twin->x1;
					tcontrol->x2=ti;//twin->x1+ti;
					tcontrol->x3=tj;//twin->x1+tj;
					}
				//                  if (tcontrol->x2>twin->x1+twin->w) {
					if (tcontrol->x2>twin->w) {
					ti=tcontrol->x2-tcontrol->x1;
					tj=tcontrol->x3-tcontrol->x1;
					//tcontrol->x1=twin->x1+twin->w-ti;
					tcontrol->x1=twin->w-ti;
				//                    tcontrol->x2=twin->x1+twin->w;
					tcontrol->x2=twin->w;
				//                    tcontrol->x3=twin->x1+twin->w-tj;
					tcontrol->x3=tcontrol->x1+tj;
					}
				//                  if (tcontrol->y1<twin->y1) {
					if (tcontrol->y1<0) {
					ti=tcontrol->y2-tcontrol->y1;
				//                    tcontrol->y1=twin->y1;
					tcontrol->y1=0;
				//                    tcontrol->y2=twin->y1+ti;
					tcontrol->y2=ti;
					//tcontrol->y3=twin->y1+ti;
					tcontrol->y3=ti;
					}
				//                  if (tcontrol->y2>twin->y1+twin->h) {
					if (tcontrol->y2>twin->h) {
					ti=tcontrol->y2-tcontrol->y1;
				//                    tcontrol->y1=twin->y1+twin->h-ti;
					tcontrol->y1=twin->h-ti;
				//                    tcontrol->y2=twin->y1+twin->h;
					tcontrol->y2=twin->h;
				//                    tcontrol->y3=twin->y1+twin->h;
					tcontrol->y3=twin->h;
					}
				//                  fprintf(stderr,"3 y1=%i y2=%i\n",tcontrol->y1,tcontrol->y2);
					GetFTControlBackground(tcontrol);
					DrawFTControl(tcontrol);

					if (tcontrol->OnMove!=0) {
						(*tcontrol->OnMove) (twin,tcontrol,x,y);
						twin=GetFTWindow(winname); //check to see if window was deleted
						tcontrol=GetFTControl(conname); //check to see if control was deleted
						if (tcontrol==0 || twin==0) {
							return; //user deleted control or window
						}
					} //if (tcontrol->OnMove!=0) {
				}  //if (match==0 && tcontrol->flags&CIsMoving) {



				if (match) {
					match=0;  //call any OnResize functions
					if (tcontrol->OnResize!=0) {
						(*tcontrol->OnResize) (twin,tcontrol,x,y);
						twin=GetFTWindow(winname); //check to see if window was deleted
						tcontrol=GetFTControl(conname); //check to see if control was deleted
						if (tcontrol==0 || twin==0) {
							return; //user deleted control or window
						}
					} //OnResize
				} //if (match) {


			//fprintf(stderr,"tnum=%lli StartTimeVisible=%lli\n",tnum,tcontrol->StartTimeVisible);

			//fprintf(stderr,"tcontrol= %s diff=%d x=%d ts=%d\t",tcontrol->name,PStatus.MouseX,x,PStatus.timespace);
			}  //if (!(PStatus.flags&EMouseMove)) //this is not the first mouse move

		} //if (tcontrol->flags&(CMove|CCursor)) 
//#endif //redo when revisiting FTI



//still in  tcontrol->flags&CButtonDown(0)


#if 0 
//mouse does not need to be over control to select and/or scroll forward - it just needed to be on the control when the button 0 went down, and the button 0 must still be down
    if (tcontrol->type==CTData || tcontrol->type==CTTimeLine) {  //button down is possibly selecting data
					if (PStatus.flags&PInfo) {
						fprintf(stderr,"mousemove with button 0 down is possibly selecting data\n");
					}
					//translate mouse position to time relative to data
					if (tcontrol->type==CTTimeLine) {
						tTimeLine=tcontrol;
					} else {
						tTimeLine=tcontrol->TimeLineFTControl;
					} //CTTimeLine
						FT_AdvanceTimeLine(FTControl *tTimeLine);

						//set tcontrol->StartTimeSelected and EndTimeSelected
				} //CTData - selecting data
#endif

    }  //          if (tcontrol->flags&CButtonDown(0)) {
    //end if a button was held down on this FTControl
//END BUTTON IS DOWN ON THIS CONTROL



//#if 0
    if (tcontrol->MouseMove>0) {  //call any FTControl functions connected to MouseMove
        (*tcontrol->MouseMove) (twin,tcontrol,x,y);
		twin=GetFTWindow(winname); //check to see if window was deleted
		tcontrol=GetFTControl(conname); //check to see if control was deleted
		if (tcontrol==0 || twin==0) {
			return; //user deleted control or window
		}
    } //if (tcontrol->MouseMove>0) {  //call any FTControl functions connected to MouseMove
//#endif



  if (twin->MouseMove>0) {  //call any MouseMove window functions 
    (*twin->MouseMove) (twin,x,y);
	twin=GetFTWindow(winname); //check to see if window was deleted
	tcontrol=GetFTControl(conname); //check to see if control was deleted
	if (tcontrol==0 || twin==0) {
		return; //user deleted control or window
	}
  } //if (twin->MouseMove>0) {  //call any MouseMove window functions 



//if (PStatus.flags&PInfo) {
//	fprintf(stderr,"Exit CheckFTWindowMouseMove\n");
//}

}  //end CheckFTWindowMouseMove

void CheckFTWindowMouseVWheelMove(FTWindow *twin,int x,int y,int Delta)
{
//FTItemList *tlist;
FTControl *tcontrol;
char winname[FTMedStr];


	//moving the mouse wheel, currently only scrolls a vertical scroll bar for any itemlist that has a vertical scroll bar


	if (PStatus.flags&PInfo) {
		fprintf(stderr,"CheckFTWindowMouseVWheelMove\n");
	}

	if (twin==0) {
		fprintf(stderr,"Error: CheckFTWindowMouseVWheelMove called with null FTWindow\n");
		return;
	} 

	if (PStatus.flags&PInfo) {
		fprintf(stderr,"CheckFTWindowMouseVWheelMove in %s\n",twin->name);
	}

	strcpy(winname,twin->name);

	//currently the mouse wheel only scrolls the scroll bar for any itemlist control that has a vertical scroll bar

	tcontrol=twin->FocusFTControl;

	if (tcontrol!=0) {
		switch(tcontrol->type) {
			case CTItemList:  // && tcontrol->flags&CVScroll) {
			case CTFileOpen:
				//scroll to the next item
				if (Delta>0) { //scroll up
					ScrollFTItemList(twin, tcontrol, FT_SCROLL_FTITEMLIST_UP | FT_SCROLL_FTITEMLIST_MOUSEWHEEL);  //scroll up, do not scroll selection too
				} else { 
					if (Delta<0) {//Delta <0 - scroll down
						ScrollFTItemList(twin, tcontrol, FT_SCROLL_FTITEMLIST_DOWN | FT_SCROLL_FTITEMLIST_MOUSEWHEEL);  //scroll down, do not scroll selection too
					} //if (Delta<0) {//Delta <0 
				} //if (Delta>0) { //scroll up
			break;
			case CTTextArea: 
				//scroll to the next item
				if (Delta>0) { //move selected item up
					FT_TextAreaPgUp(tcontrol,0);  //scroll down, scroll selection too, 0=don't move the cursor too- just page up
				} else { 
					if (Delta<0) {//Delta <0 - move selected item down
						FT_TextAreaPgDown(tcontrol,0);  //scroll down, scroll selection too, 0=don't move the cursor too- just page up
					} //if (Delta<0) {//Delta <0 - scroll down
				} //if (Delta>0) { //scroll up
			break;
		} //switch(tcontrol->type) {
	} //if (tcontrol!=0) {

//=====================================
//Below here no itemlist window is open
//=====================================


  if (twin->MouseVWheelMove>0) {  //call any MouseMove window functions 
    (*twin->MouseVWheelMove) (twin,x,y);
	twin=GetFTWindow(winname); //check to see if window was deleted
	if (twin==0) {
		return; //user deleted control or window
	}
  } //if (twin->MouseVWheelMove>0) {  //call any MouseMove window functions 



} //void CheckFTWindowMouseVWheelMove(FTWindow *twin,int x,int y,int Delta)

void CheckFTWindowButtonDoubleClick(FTWindow *twin,int x, int y,int button)
{
int bw,bh;
FTControl *tcontrol;
long long i;
FTItem *litem;
char winname[FTMedStr];
char conname[FTMedStr];


if (PStatus.flags&PInfo)
  fprintf(stderr,"CheckFTWindowButtonDoubleClick\n");

//possibly exit after found= can only click on 1 FTControl - need depth for scroll bar drag button
if (twin!=0) {
strcpy(winname,twin->name);


if (!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow) {

tcontrol=twin->icontrol;
while (tcontrol!=0)
{
strcpy(conname,tcontrol->name);

bw=tcontrol->x2-tcontrol->x1;
bh=tcontrol->y2-tcontrol->y1;

//double-clicked down on this FTControl (this is second click down)
if (PStatus.ButtonFTWindow[button]==twin && 
    PStatus.ButtonX[button]>=tcontrol->x1 && PStatus.ButtonX[button]<=tcontrol->x2 && 
    PStatus.ButtonY[button]>=tcontrol->y1 && PStatus.ButtonY[button]<=tcontrol->y2)
{
//downup FTControls must be up clicked over FTControl - human may not mean to click
//if the button was being held down on this FTControl then this up is meant for this FTControl
#if PINFO
fprintf(stderr,"FTControl %s double-click\n",tcontrol->name);
#endif
PStatus.ButtonX[button]=x;
PStatus.ButtonY[button]=y;

//tcontrol->flags&=(~CButtonDown);  //button is not being held on this FTControl any more

//if (PStatus.flags&EResizeX1) PStatus.flags&=~(EResizeX1);
//if (PStatus.flags&EResizeX2) PStatus.flags&=~(EResizeX2);

twin->flags|=WButtonDown(button);  //set window button down bit

   //double-click also can call ButtonDownhold functions
   PStatus.flags|=EButtonHold(button);
   tcontrol->lasttime[button].tv_sec=PStatus.utime.tv_sec;
   tcontrol->lasttime[button].tv_usec=PStatus.utime.tv_usec;




//if FileOpen, get filename clicked on
if (tcontrol->type==CTFileOpen && button==0) {
//  litem=GetFTItemFromFTControl(tcontrol->ilist,0,tcontrol->FirstCharacterShowing);
  if (tcontrol->ilist!=0) {
//    litem=GetFTItemFromFTControl(tcontrol->ilist,0,tcontrol->ilist->cur);
//    litem=GetFTItemFromFTControl(tcontrol->ilist,tcontrol->ilist->cur);
//since this is a double-click - the item may not yet be selected
//find which item click is over and select item

	  //determine which item was double-clicked on
	i=(int)((y-tcontrol->y1)/(tcontrol->fontheight));
	if (i<tcontrol->ilist->num) { //only if there is an item where double-click is
		litem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->ilist->cur);
		if (litem!=0) {
			//if this is a folder, go in to the folder
			if (litem>0 && litem->flags&FTI_FOLDER) {
			//SetFileMenuName(tcontrol);
			tcontrol->ilist->cur=1;  //0 reset cur index
			tcontrol->FirstItemShowing=1; //0
#if Linux
			chdir(litem->name);
#endif
#if WIN32
			_chdir(litem->name);
#endif
			tcontrol->ilist->flags|=FTIL_REREAD;
					//copy path to tcontrol->pathname, or tcontrol->text2
#if Linux				
			getcwd(tcontrol->text2,FTBigStr);  //was BiggestStr
#endif
#if WIN32
			_getcwd(tcontrol->text2,FTBigStr);  //was BiggestStr
#endif
			strcpy(tcontrol->filename,"");  
			if (tcontrol->TextBoxPath!=0) {
				strcpy(tcontrol->TextBoxPath->text,tcontrol->text2);
				DrawFTControl(tcontrol->TextBoxPath);
			}
			}
			else {  //is file
			//copy the filename in the FTControl to "filename" (could be done in humanfunc)
			strcpy(tcontrol->filename,litem->name); //filename gets only file, text gets file and folder name
			}
			DrawFTControl(tcontrol);  //redraw FTControl

				if (tcontrol->OnChange>0)
				{  
				(*tcontrol->OnChange) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
				}


		//now done in DrawSelectedFTItem
		/*
 
			if (tcontrol->OnChange!=0) {
						(*tcontrol->OnChange) (twin,tcontrol);
						twin=GetFTWindow(winname); //check to see if window was deleted
						tcontrol=GetFTControl(conname); //check to see if control was deleted
						if (tcontrol==0 || twin==0) return; //user deleted control or window
					}
		*/
		} //	if (i<tcontrol->ilist->num) { //only if there is an item where double-click is
  //    fprintf(stderr,"%s\n",tcontrol->filename);
      }  //end if FTItem != 0
   } //if ilist!=0
}  //end if CTFileOpen




if (tcontrol->ButtonDoubleClick[button]>0)
{  //call any functions connected to ButtonUp
(*tcontrol->ButtonDoubleClick[button]) (twin,tcontrol,x,y);
twin=GetFTWindow(winname); //check to see if window was deleted
tcontrol=GetFTControl(conname); //check to see if control was deleted
if (tcontrol==0 || twin==0) return; //user deleted control or window
}
  //ex=1;  //only 1 FTControl for any button up event

//this FTControl gets focus
//pwin[win].focus=a;

//twin->focus=tcontrol->num;

}  //if FTControl had double-click button down

tcontrol=(FTControl *)tcontrol->next;

} //while tcontrol!=0

}  //window has exclusive FTControl

}  //if twin!=0

//click downup was not in any FTControl - change focus to window

if (PStatus.flags&PInfo)
  fprintf(stderr,"End CheckFTWindowButtonDoubleClick\n");
} //end CheckFTWindowButtonDoubleClick



void DelSelect(FTControl *tcontrol)
{
int tx,i;
char tstr[FTMedStr];


//erase selected text  
tx=0;
for(i=0;i<(int)strlen(tcontrol->text);i++) {
  if (!(i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars)) { //not selected text, do not erase-ignore
    tstr[tx]=tcontrol->text[i];
    tx++;
  }
  if (i==tcontrol->StartSelectedTextInChars) 
    tcontrol->CursorLocationInChars=tx;  //is start of selected text, move cursor here         
} //end for i  

tstr[tx]=0; 
strcpy(tcontrol->text,tstr); 
tcontrol->text2[0]=0;
tcontrol->StartSelectedTextInChars=0;
tcontrol->EndSelectedTextInChars=0;
//tcontrol->i[4]=0; //4 and 5 used for?
//tcontrol->i[5]=0;
}  //end DelSelect



void GoToNextTab(FTWindow *twin)
{
FTControl *tcontrol,*tcontrol2;
char winname[FTMedStr];
//char conname[FTMedStr];
int starttab,nexttab;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"GoToNextTab\n");
}

	if (twin!=0) {
		strcpy(winname,twin->name);
	}//twin!=0


	starttab=0;
	if (twin->FocusFTControl!=0) {
		starttab=twin->FocusFTControl->tab;
	} //FocusFTControl
  if (GetNextTab(twin,starttab)!=starttab) {

    tcontrol=GetFTControlWithTab(twin,starttab);  //this is also the control with the focus and is  twin->FocusFTControl

   // twin->focus=GetNextTab(twin,twin->focus);
	nexttab=GetNextTab(twin,starttab);

    if (tcontrol!=0) {
 		   //fprintf(stderr,"%s had focus\n",tcontrol->name);
		//strcpy(conname,tcontrol->name);
		FT_LoseFocus(twin,twin->FocusFTControl);

      //if (tcontrol->type==CTDropdown && tcontrol->flags&CItemListOpen) EraseMenu(tcontrol);
		//if (tcontrol->type==CTDropdown && tcontrol->flags&CItemListOpen) {
		//	CloseFTItemList(tcontrol->ilist);
		//}
		//  if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
		//	  DrawFTControl(tcontrol);  //remove cursor
		 // }
		 // if (tcontrol->LostFocus!=0) {
		//	  (*tcontrol->LostFocus) (twin,tcontrol);
		 // }
    }

//    tcontrol2=GetFTControlWithTab(twin,twin->focus);
    tcontrol2=GetFTControlWithTab(twin,nexttab);
	FT_SetFocus(twin,tcontrol2);
	//twin->FocusFTControl=tcontrol2;
    if (tcontrol2!=0) {
      //fprintf(stderr,"%s now has focus\n",tcontrol2->name);
      //cursor needs to be at end of text for textbox
      if (tcontrol2->type==CTTextBox || tcontrol2->type==CTTextArea) {
        tcontrol2->CursorLocationInChars=strlen(tcontrol2->text);
       // DrawFTControl(tcontrol2);  //draw cursor
      }
	  //I think the below is done in FT_LoseFocus, FT_SetFocus
	//DrawFTControl(tcontrol); //redraw control that lost focus
	//DrawFTControl(tcontrol2); //redraw control with focus
	  /*
      if (tcontrol2->GotFocus!=0) {
				(*tcontrol2->GotFocus) (twin,tcontrol2);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol2=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol2==0 || twin==0) return; //user deleted control or window
		} //GotFocus
			*/
    } //tcontrol2!=0

  }  //nexttab not equal to current tab

}  //GoToNextTab


void GoToPrevTab(FTWindow *twin)
{
FTControl *tcontrol,*tcontrol2;
char winname[FTMedStr];
//char conname[FTMedStr];
int starttab,prevtab;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"GoToPrevTab\n");
}


if (twin!=0) {
strcpy(winname,twin->name);
} //twin!=0

	starttab=0;
	if (twin->FocusFTControl!=0) {
		starttab=twin->FocusFTControl->tab;
		//make 
	} //FocusFTControl

  if (GetPrevTab(twin,starttab)!=starttab) {

    tcontrol=GetFTControlWithTab(twin,starttab);  
    prevtab=GetPrevTab(twin,starttab);
    if (tcontrol!=0) {
		//strcpy(conname,tcontrol->name);
		FT_LoseFocus(twin,twin->FocusFTControl);

//      if (tcontrol->type==CTDropdown && tcontrol->flags&CItemListOpen) EraseMenu(tcontrol);
//      if (tcontrol->type==CTDropdown && tcontrol->flags&CItemListOpen) {
//		  CloseFTItemList(tcontrol->ilist);
//	  }
 //     if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
//		  DrawFTControl(tcontrol);  //remove cursor
//	  }
 //     if (tcontrol->LostFocus!=0) {
//		  (*tcontrol->LostFocus) (twin,tcontrol);
//	  }
    }

    tcontrol2=GetFTControlWithTab(twin,prevtab);
//	FT_LoseFocus(twin,twin->FocusFTControl);
	FT_SetFocus(twin,tcontrol2);
//	twin->FocusFTControl=tcontrol2;
    if (tcontrol2!=0) {
      //cursor needs to be at end of text for textbox
      if (tcontrol2->type==CTTextBox || tcontrol2->type==CTTextArea) {
        tcontrol2->CursorLocationInChars=strlen(tcontrol2->text);
        //DrawFTControl(tcontrol2);  //draw cursor
      }
	  //I think the below is done in FT_LoseFocus, FT_SetFocus
	//DrawFTControl(tcontrol); //redraw control that lost focus
	//DrawFTControl(tcontrol2); //redraw control with focus
	  /*
      if (tcontrol2->GotFocus!=0) {
				(*tcontrol2->GotFocus) (twin,tcontrol2);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol2=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol2==0 || twin==0) return; //user deleted control or window
			} //GotFocus
		*/
    } //tcontrol2!=0


  }  //prevtab not equal to current tab

}  //GoToPrevTab


//CheckFTWindowKeypress - handle keypress in an FTWindow
//void CheckFTWindowKeypress(FTWindow *twin,unsigned char key)
void CheckFTWindowKeypress(FTWindow *twin,KeySym key)  //could make KeySym unsigned short
{
int bh,tx,tlen,a,maxwidth,maxheight,fw,fh,cx,i,j;
FTControl *tcontrol;//,*tcontrol2;
#if Linux
Atom type;
#endif
unsigned long len;
unsigned char *data;
#if Linux
unsigned long bytes_left;
int format;
Window txwin;
unsigned int result;
#endif
char tstr[FTMedStr];
char istr[FTMedStr];

int ltim,newcur;
FTItem *litem,*titem;//,*titem2;
//FTWindow *twin2;
FTItemList *tlist;
char winname[FTMedStr];
char conname[FTMedStr];
//char mapkey[10];
#if WIN32
HGLOBAL hClipBuffer; //for clipboard data
HANDLE hData; //for clipboard data
#endif
char *tmpstr;


//This is a large function
//The structure of this function should be examined to see if it can be simplified
//In addition, perhaps a table of contents overview should be listed here
//I decided to separate this function into control types and then process keypresses for each control, as opposed to processing keypresses, and then different code depending on the control type


if (PStatus.flags&PInfo) {
  fprintf(stderr,"CheckFTWindowKeypress key=%x\n",(int)key);
	if (twin!=0) {
		fprintf(stderr,"in window %s\n",twin->name);
	}
}

//tcontrol=GetFTControlWithTab(
//if (twin->FocusFTControl!=0) {
//	fprintf(stderr,"Control with focus= %s\n",twin->FocusFTControl->name);
//}


//do window Keypress things and then
//process Keypress for FTControl with focus only - do not bother with other FTControls
if (twin!=0) {
	strcpy(winname,twin->name);
} else {
  strcpy(winname,"");
}//twin!=0




//Check Hotkeys
	CheckHotKeys(twin,key);


//if (PStatus.flags&PExclusive) {
//handle open ItemList keypresses differently
//If an itemlist is open - do not call any user window keypresses - for example, an escape key to close a window would close a window instead of closing a dropdown control itemlist
if (PStatus.flags&PItemListOpen) {

//	fprintf(stderr,"ItemList window open\n");

	//get the selected itemlist
	tlist=0;
	titem=0;
	tcontrol=PStatus.ExclusiveFTControl;
	if (tcontrol!=0) {
		strcpy(conname,tcontrol->name);

		//tlist=GetSelectedFTItemList(tcontrol->ilist);
		titem=PStatus.SelectedFTItem;
		if (titem!=0) {  //an item is selected
			tlist=titem->parent;
			if (tlist==0) {
				tlist=tcontrol->ilist; //no parent, use control ilist 
			} //tlist==0
		} else { //no selected item
			tlist=tcontrol->ilist; //the open itemlist can only be the filemenu or dropdown control's itemlist
		} //titem!=0
	} else {  //tcontrol==0
		fprintf(stderr,"Error: In CheckFTWindowKeypress - PItemListOpen but PStatus.ExclusiveControl=0\n");
	} //tcontrol!=0

  switch(key) {

		case XK_Escape:  
		//Different programs handle this differently - 
		//for some if a subitemlist window is open but has no selection
	//Close any open ItemList - but just the last one
	//possibly just GetSelectedItemList(); returns current ilist - there can only be ony itemlist open
	//but GetSelected could be used for an individual ilist
	//perhaps only GetCurrentSelectedFTItemlist()
//perhaps a nicer method is if this list has an open itemlist window close only that one
			if (tlist!=0) {
/*
				if (titem!=0) {
					if (titem->ilist!=0) { //item has subitemlist
							
					} //titem->ilist
				} //titem!=0
*/
//				fprintf(stderr,"Escape close ItemList\n");
				//clear itemlist selected item too
				//if a dropdown itemlist set cur to orig
				if (tlist->control!=0) {
					if (tlist->control->type==CTDropdown) {
						//UnselectFTItemByNum(tlist,tlist->orig);
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
						if (titem!=0) {
							titem->flags&=~FTI_SELECTED;
						}
						tlist->cur=tlist->orig; //set cur to original selected item
						tlist->control->value=tlist->cur;
						if (tlist->cur==0) {
							strcpy(tlist->control->text,"");  //perhaps track original text
						} //
//						fprintf(stderr,"Setting cur to %d\n",tlist->orig);
					}
				} //tlist->control!=0
				CloseFTItemList(tlist);
			}//tlist!=0

		//LoseFocus of any control and set window focus to 0
//		FT_LoseFocus(twin,twin->FocusFTControl);		
		//twin->FocusFTControl=0;
		break;

    case XK_Return:
#if PINFO
      fprintf(stderr,"enter in filemenu or dropdown\n");        
#endif
			//can only be choosing selected item if any 
			//item is chosen (clicked on) even if there is a subitemlist
			if (titem!=0) { //an item is selected
				if (tcontrol!=0) {
					if (tcontrol->type==CTDropdown) {
						tcontrol->value=titem->num;//tcontrol->ilist->cur;
					}
					CloseFTItemList(tcontrol->ilist);
					if (titem->click[0]!=0) {
						(*titem->click[0]) (twin,tcontrol,0,0);
						twin=GetFTWindow(winname); //check to see if window was deleted
						tcontrol=GetFTControl(conname); //check to see if control was deleted
						if (tcontrol==0 || twin==0) return; //user deleted control or window

						if (tcontrol->type==CTDropdown) {
							DrawFTControl(tcontrol); //redraw dropdown with selected text in textbox							
						}
					}
				} //tcontrol!=0
			} //titem!=0

    break;  //XK_Return
    case XK_Up:
#if PINFO
      fprintf(stderr,"CItemListOpen\n");        
#endif
			if (tlist!=0) {
//could be open without selected item
				if (tlist->cur==0) {  //no item selected
					tlist->cur=tlist->num;
					DrawSelectedFTItem(tlist,tlist->cur);
					
					//Open any subitemlist
					titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);						
					if (titem!=0) {
						if (titem->ilist!=0) { //this item has a subitemlist
								//with arrow keys the subitemlist cannot be open already
								//so open subitemlist window
								OpenFTItemList(titem->ilist); //open the subitemlist
						} //titem->ilist!=0
					} //titem!=0
					//selected item has changed so call any OnChange functions
					//done in DrawSelectedFTItem
/*
					if (tcontrol->OnChange!=0) {
						(*tcontrol->OnChange) (twin,tcontrol);
						//twin=GetFTWindow(winname); //check to see if window was deleted
						//tcontrol=GetFTControl(conname); //check to see if control was deleted
						//if (tcontrol==0 || twin==0) return; //user deleted control or window
					} //OnChange
*/
					//DrawFTItemList(tlist); //redraw newly selected item
				} else { //is a current selected item
					newcur=tlist->cur-1;
					if (newcur<1) {
						newcur=tlist->num;
					}
					if (newcur!=tlist->cur) {
						//fprintf(stderr,"new selection is different from current\n");
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
						//close any subitemlist windows
						CloseFTItemList(titem->ilist); //this does necessary checks for ilist==0, etc.
						//unselect last selected item
						DrawUnselectedFTItem(tlist,tlist->cur);
						//set new selected item
						tlist->cur=newcur;
						//draw newly selected item
						DrawSelectedFTItem(tlist,tlist->cur);						
						//get current item
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);						
						if (titem!=0) {
							if (titem->ilist!=0) { //this item has a subitemlist
									//with arrow keys the subitemlist cannot be open already
									//so open subitemlist window
									OpenFTItemList(titem->ilist); //open the subitemlist
							} //titem->ilist!=0
						} //titem!=0

						//selected item has changed so call any OnChange functions - done in DrawSelectedFTItem
//						if (tcontrol->OnChange!=0) {
//							(*tcontrol->OnChange) (twin,tcontrol);
							//twin=GetFTWindow(winname); //check to see if window was deleted
							//tcontrol=GetFTControl(conname); //check to see if control was deleted
							//if (tcontrol==0 || twin==0) return; //user deleted control or window
//						} //OnChange
					} //newcur!=tlist->cur
				} //tlist->cur==0
			} //tlist!=0

    break;
    case XK_Down:
//			if (PStatus.flags&PInfo) 
			  //fprintf(stderr,"down arrow 0 %s\n",twin->name);
			if (tlist!=0 ) {
				//fprintf(stderr,"tlist->cur=%d\n",tlist->cur);
				if (tlist->cur==0) {  //no item selected
					tlist->cur=1;
					//fprintf(stderr,"drawselected\n");
					DrawSelectedFTItem(tlist,1);
					//fprintf(stderr,"after drawselected\n");
					//Open any subitemlist
					titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);						
					//fprintf(stderr,"after GetFTItemFromFTItemList\n");
					if (titem!=0) {
						if (titem->ilist!=0) { //this item has a subitemlist
								//with arrow keys the subitemlist cannot be open already
								//so open subitemlist window
								OpenFTItemList(titem->ilist); //open the subitemlist
						} //titem->ilist!=0
					} //titem!=0
					//selected item has changed so call any OnChange functions
					//this is done in DrawSelectedFTItem
//					if (tcontrol->OnChange!=0) {
//						(*tcontrol->OnChange) (twin,tcontrol);
						//twin=GetFTWindow(winname); //check to see if window was deleted
						//tcontrol=GetFTControl(conname); //check to see if control was deleted
						//if (tcontrol==0 || twin==0) return; //user deleted control or window
//					} //onChange
				} else { //is a current selected item
					newcur=tlist->cur+1;
					if (newcur>tlist->num) {
						newcur=1;
					}
					if (newcur!=tlist->cur) {
						//fprintf(stderr,"new selection is different from current\n");
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
						//close any subitemlist windows that are open
						if (titem!=0) {
							CloseFTItemList(titem->ilist); //this does necessary checks for ilist==0, etc.
						}
						//unselect last selected item
						//fprintf(stderr,"drawunselected\n");
						DrawUnselectedFTItem(tlist,tlist->cur);
						//set new selected item
						tlist->cur=newcur;
						//draw newly selected item
						//fprintf(stderr,"draw newly selected\n");
						DrawSelectedFTItem(tlist,tlist->cur);						
						//fprintf(stderr,"after draw newly selected\n");
						//get current item
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);						
						//fprintf(stderr,"after get ftsubitem\n");
						if (titem!=0) {
							if (titem->ilist!=0) { //this item has a subitemlist
									//with arrow keys the subitemlist cannot be open already
									//so open subitemlist window
									OpenFTItemList(titem->ilist); //open the subitemlist
							} //titem->ilist!=0
						} //titem!=0
						//selected item has changed so call any OnChange functions
//						if (tcontrol->OnChange!=0) {
//							(*tcontrol->OnChange) (twin,tcontrol);
							//twin=GetFTWindow(winname); //check to see if window was deleted
							//tcontrol=GetFTControl(conname); //check to see if control was deleted
							//if (tcontrol==0 || twin==0) return; //user deleted control or window
//						}
					} //newcur!=tlist->cur
				} //tlist->cur==0


			} //tlist!=0
			//fprintf(stderr,"past down arrow\n");
    break;
    case XK_Right:
			if (tlist!=0) {	
				if (titem!=0) {  //some item is selected
					if (titem->ilist!=0) { //item has subitemlist
						//if this subitemlist window is not open yet, open
						if (titem->ilist->window!=0) {
							twin=titem->ilist->window;
							if (!(twin->flags&WOpen)) {
								OpenFTItemList(titem->ilist);
							}
						} //titem->ilist->window!=0
						//select first item in subitemlist
						tlist=titem->ilist;
						tlist->cur=1;
						DrawSelectedFTItem(tlist,tlist->cur);
						titem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);						
						//fprintf(stderr,"after get ftsubitem\n");
						if (titem!=0) {
							if (titem->ilist!=0) { //this item has a subitemlist
									//with arrow keys the subitemlist cannot be open already
									//so open subitemlist window
									OpenFTItemList(titem->ilist); //open the subitemlist
							} //titem->ilist!=0
						} //titem!=0

//						if (tcontrol->OnChange!=0) {
//							(*tcontrol->OnChange) (twin,tcontrol);
							//twin=GetFTWindow(winname); //check to see if window was deleted
							//tcontrol=GetFTControl(conname); //check to see if control was deleted
							//if (tcontrol==0 || twin==0) return; //user deleted control or window
//						} //OnChange
					} else { //titem->ilist!=0
						//there is no subitemlist - go to next filemenu control if any
						OpenNextFileControl(tcontrol);
					} //titem->ilist!=0
				} else { //titem!=0
					//no item is selected - go to next file control
					OpenNextFileControl(tcontrol);
				} //titem!=0
			} //tlist!=0
    break;
    case XK_Left:
			if (tlist!=0) {	
				if (tlist->parent==0) { //is descended from a control - is not subitemlist
//					fprintf(stderr,"openprevfilecontrol\n");
					OpenPrevFileControl(tcontrol);
				} else { //this is a subitemlist
					//some item must be selected - above tlist is from selected item but make sure anyway
					if (titem!=0) {  //some item is selected
						//unselect item
						DrawUnselectedFTItem(tlist,tlist->cur);
						//reset current selection
						tlist->cur=0;
						//set current selected item to parent item
						//important to distinguish between parent of item and parent of itemlist
						tlist=tlist->parent;
						//this needs to be done here, because there is no call to DrawSelectedFTItem
						PStatus.SelectedFTItem=GetFTItemFromFTItemListByNum(tlist,tlist->cur);
						PStatus.SelectedFTItemList=tlist;
					} //titem!=0
				} //tlist->parent==0
			} //tlist!=0
    break;

	} //switch(key)


	return;
}  //PStatus.flags&PItemListOpen - program has a control with exclusive control over input

//	fprintf(stderr,"Somehow no return after keypress with ItemList window open\n");
//No ItemList Open after here


//possibly exit after found= can only click on 1 FTControl - need depth for scroll bar drag button
//twin=GetFTWindowN(win);
if (twin!=0) {

//call any window keypress functions
if (twin->Keypress!=0) {
	(*twin->Keypress) (twin,key);
} //twin->Keypress


//This window has controls, and no exclusive window is open or this window is the exclusive window
if (twin->icontrol!=0 && (!(PStatus.flags&PExclusive) || twin==PStatus.ExclusiveFTWindow)) {

//fprintf(stderr,"after exclu\n");

//  tcontrol=twin->icontrol;
//  while (tcontrol!=0)
//  {

//get FTControl that has window focus
//note:  opening a window, currently does not set focus to the first FTControl
if (PStatus.flags&PInfo) {
	fprintf(stderr,"twin=%s\n",twin->name);
}

//tcontrol=GetFTControlWithTab(twin,twin->focus);
tcontrol=twin->FocusFTControl;







//fprintf(stderr,"FTControl tab=%d focus=%d\n",tcontrol->tab,twin->focus);
//if (tcontrol->win==win && pwin[win].focus==a)
//fprintf(stderr,"key=%d\n",key);
//if (tcontrol->win==twin->num && twin->focus==tcontrol->tab)
//{
if (tcontrol!=0) {

if (PStatus.flags&PInfo) {
	fprintf(stderr,"FTControl %s has focus\n",tcontrol->name);
}

	strcpy(conname,tcontrol->name);

//a FTControl can be deleted (like a .wav or .avi track)


//WINDOW KEYPRESS FUNCTIONS
//even if a different control is in focus, filemenu hotkeys are active
//  if (!(twin->flags&WItemListOpen) &&
//Check Hotkeys - for filemenu keys
//	CheckHotKeys(twin,key); //is done above


  if (!(twin->flags&WItemList) &&
      !(tcontrol->flags&CNoTab) &&
       tcontrol->type!=CTLabel && tcontrol->type!=CTImage && tcontrol->type!=CTTimer) { //tab may erase selected text=shouldn't
      //option FTControl? fileopen?
    switch(key) {
			case XK_Escape:
				FT_LoseFocus(twin,twin->FocusFTControl);		
			break;
      case XK_Down:
//        GoToNextTab(twin); 
      break;
      case XK_Up:
//        GoToPrevTab(twin); 
      break;
      case XK_Tab:
#if Linux
      case XK_ISO_Left_Tab:
#endif
//window tabs
//#if 0
        if (PStatus.flags&PShift) {
	        //fprintf(stderr,"shift tab\n"); 
					GoToPrevTab(twin); //shift is on
					//Redraw control with tab
				} else {
	        //fprintf(stderr,"tab\n"); 
					GoToNextTab(twin); 
					//fprintf(stderr,"after tab\n"); 
				} //PShift
      break;
      case XK_Right: //perhaps unless textbox or textarea
			//		GoToNextTab(twin); 
      break;
      case XK_Left:
		//			GoToPrevTab(twin); 
      break;

//#endif
    }  //end switch window keypress functions
  }  //end if not !(twin->flags&WItemListOpen), has no tab, or is Label




//START Keypress IN TEXTBOX CODE
//todo: be more selective about when to redraw control:
//1) ignore Page_Up/pgdn/arrows/home/end when CursorLocationInChars and i[1] have not changed
//2) only erase and redraw cursor for arrow keys - or just row
if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea)
{
strcpy(istr,tcontrol->text);//to see if need to call OnChange()

fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;

//POSSIBLY MAY WANT TO ADD #if Linux/Win32 here and remove -1 for Linux
//maxwidth=max number of character that can show in this textbox
maxwidth=(int)((tcontrol->x2-tcontrol->x1)/fw)-1;  //-2 for padding, maxwidth is number of chars in a row
maxheight=(int)((tcontrol->y2-tcontrol->y1)/fh)-1; //maxheight is the number of rows of text in the control
//if (PStatus.flags&PInfo) {
//	fprintf(stderr,"key=0x%x\n",key);
//}


//cntl-a select all text


//I think with cap lock cntl-V did not work
//cut-copy-paste
//FTControl KEY COMBINATIONS
if (PStatus.flags&PControl) {//FTControl key is down
  switch(key) {
//   case 0x41: 
//   case 0x61:  //a,A =select all text
   case XK_A: 
   case XK_a:  //a,A =select all text
			tcontrol->StartSelectedTextInChars=0;
			tcontrol->EndSelectedTextInChars=strlen(tcontrol->text);

		break; //a,A = select all text
//   case 0x43: 
//   case 0x63:  //c,C =copy
   case XK_C: 
   case XK_c:  //c,C =copy

#if PINFO
     //fprintf(stderr,"Copy %s\n",tcontrol->text2);
	fprintf(stderr,"Copy\n");
#endif

//to copy we have to own the XA_CLIPBOARD selection 
//to send windows that request this data the text from this window
//This text, for example, data can be stored anywhere, but I chose to store it in 
//the XA_PRIMARY selection property

//I guess in this way, any window that wants to write to the clipboard has to 
//own the XA_CLIPBOARD selection, and then handle SelectionRequest events to send
//any other client (xwindow program) that data, until some other xwindow needs to copy 
//data to the clipboard, and then must own XA_CLIPBOARD

//name=malloc(100);
//strcpy(name,"testname, This is only a test");

//copy string to PRIMARY property of twin
//in SelectionRequest event handler, 

		if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //only if there is selected text
#if Linux
			if (twin->xwindow!=0) {
			//this program will send XA_PRIMARY to any window that requests data from XA_CLIPBOARD

				memcpy(tstr,&tcontrol->text[tcontrol->StartSelectedTextInChars],tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars);
				tstr[tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars]=0;
			//fprintf(stderr,"Copy %s\n",tstr);
				//XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,XA_STRING,
			//            8, PropModeReplace, (unsigned char *)tcontrol->text2,strlen(tcontrol->text2));


			//XChangeProperty appears to have a text limit of 7 characters - I can't figure out why
			//it seemed that XSetStandardProperties(PStatus.xdisplay,twin->xwindow,twin->title,twin->ititle,None,NULL,0,NULL);
			//has a limit of 10 characters for window title
			//fprintf(stderr,"string %s length=%d and %d\n",tstr,(int)strlen(tstr),tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars);
			result=XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,XA_STRING,
						      8, PropModeReplace, (unsigned char *)tstr,strlen(tstr));

			//				fprintf(stderr,"result=%d\n",result);

							if (result == BadAlloc
						     || result == BadAtom || result == BadMatch
						     || result == BadValue || result == BadWindow)
						    {
								fprintf(stderr,"control-c XChangeProperty failed %d\n",result);
						    }


			//and set the secondary buffer
			result=XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_SECONDARY,XA_STRING,
						      8, PropModeReplace, (unsigned char *)tstr,strlen(tstr));

			//				fprintf(stderr,"result=%d\n",result);

							if (result == BadAlloc
						     || result == BadAtom || result == BadMatch
						     || result == BadValue || result == BadWindow)
						    {
								fprintf(stderr,"control-c XChangeProperty failed %d\n",result);
						    }

			//make this window own the clipboard selection
				XSetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD,twin->xwindow,CurrentTime);
				if (twin->xwindow!=XGetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD))
					fprintf(stderr,"Could not get CLIPBOARD selection.\n");


			//make this window own the clipboard selection - needs twice for some reason
				XSetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD,twin->xwindow,CurrentTime);
				if (twin->xwindow!=XGetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD))
					fprintf(stderr,"Could not get CLIPBOARD selection.\n");



			}  //xwindow!=0

			XFlush(PStatus.xdisplay);
#endif //Linux
#if WIN32
			memcpy(tstr,&tcontrol->text[tcontrol->StartSelectedTextInChars],tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars);
			tstr[tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars]=0;

			if (OpenClipboard(twin->hwindow)) {
				EmptyClipboard();
				hClipBuffer = GlobalAlloc(0, strlen(tstr)+1);
				data=(unsigned char *)GlobalLock(hClipBuffer);
				memcpy(data,tstr,strlen(tstr)+1);
				GlobalUnlock(hClipBuffer);
				SetClipboardData(CF_TEXT,hClipBuffer);
				CloseClipboard();
			} //if (OpenClipboard()) {
			GdiFlush();
#endif


} //		if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //only if there is selected text

   break;

//   case 0x56:
//   case 0x76:  //v,V =paste
   case XK_V:
   case XK_v:  //v,V =paste

#if Linux
//not if CReadOnly
     txwin=XGetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD);
     //txwin is the xwindow that owns the specified selection (XA_CLIPBOARD)
    //fprintf(stderr,"Current owner of XA_CLIPBOARD=%p\n",txwin);
    //to get data from XA_CLIPBOARD and put in XA_PRIMARY property
   //if no owner, this program will get SelectionRequest and send
   //whatever is in XA_PRIMARY, which will be an empty string
   if (txwin) {
  //   fprintf(stderr,"convert selection\n");
     result=XConvertSelection(PStatus.xdisplay,XA_CLIPBOARD,XA_STRING,XA_PRIMARY,twin->xwindow,CurrentTime);
	      if (result == BadAlloc
           || result == BadAtom || result == BadMatch
           || result == BadValue || result == BadWindow)
          {
		      fprintf(stderr,"control-v XChangeProperty failed %d\n",result);
          }

 //    if (result==BadWindow) fprintf(stderr,"convert selection error - BadWindow\n");
  //   if (result==BadAtom) fprintf(stderr,"convert selection error - BadAtom\n");
   }  //txwin
#endif //Linux
   data=0; 
//wait for SelectionRequest-SelectionNotify?
//wait for SelectionNotify if FreeThought is owner
//or timeout if no 
ltim=PStatus.ctime->tm_sec;  //PStatus.utime.tv_usec
while (!(PStatus.flags&PGotSelection) && PStatus.ctime->tm_sec<ltim+1) {
//  fprintf(stderr,"%d=%d\n",(unsigned int)PStatus.utime.tv_usec,ltim);
  GetTime();  //the while(1) takes priority over the periodical GetTime in main()
  FT_GetInput();
}  //end while 1 second
  PStatus.flags&=~PGotSelection;  //clear GotSelection bit
#if PINFO
  fprintf(stderr,"after convert selection\n");
#endif
#if Linux
   if (twin->xwindow!=0) {
//important that XGetWindowProperty alloc's the data and will not work with already allocated memory
     XGetWindowProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,0,10000000L,0,XA_STRING,&type,&format,&len,&bytes_left,&data);				
	 }
#endif
#if WIN32
   len=0; //current paste from the clipboard is not implemented in Windows
   if (OpenClipboard(twin->hwindow)) {
	   hData=GetClipboardData(CF_TEXT); //CF_UNICODE
	   if (hData!=0) {
		   data=(unsigned char *)GlobalLock(hData);
		   GlobalUnlock(hData);
		   CloseClipboard();
		   len=strlen((const char *)data);
	   } else {
		   CloseClipboard();
	   }
   } //if (OpenClipboard()) {
#endif
   //todo: needs Windows code to set len, bytes_left and data


//fprintf(stderr,"data %s length=%li bytes_left=%li\n",data,len,bytes_left);
   //0=cursor loc in string, 1=first showing character
   //paste over any selection, or insert in to text
   //assemble a new string

   tx=0;
   cx=tcontrol->CursorLocationInChars;
   //tstr[0]=0;
   for(i=0;i<=(int)strlen(tcontrol->text);i++) {
     if (i==tcontrol->CursorLocationInChars) { //this is where the cursor is, insert the pasted text
//       strcat(&tstr[tx],data);
//       tx+=len;
       //need to look at 1 character after text to paste to end of text
       for(a=0;a<(int)len;a++) {
         tstr[tx]=data[a];
//         fprintf(stderr,"here tx=%d tstr=%s\n",tx,tstr);
         tx++;
       } //end for a
			cx=tx; //new cursor position is at end of inserted text
     } //end if i==tcontrol->CursorLocationInChars, insert pasted text

     if (i<(int)strlen(tcontrol->text) && !(i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars)) { 
       //not selected text, do not erase-ignore (ignore selected text which will not be copied to new string)
       tstr[tx]=tcontrol->text[i];
       tx++;
     }
   } //end for i  
   //todo: may have to scroll text to the left too if paste goes beyond tc->x2
   //todo: set tcontrol->CursorX and CursorY
   //tcontrol->CursorX+=len;
   tcontrol->CursorLocationInChars=cx; //move cursor to end of pasted text
//pasting also resets any selected text
   tcontrol->StartSelectedTextInChars=cx;
   tcontrol->EndSelectedTextInChars=cx;
//   fprintf(stderr,"after i %s\n",data);
   tstr[tx]=0; 
   if (data!=0 && strlen((char *)data)>0 && strlen(tcontrol->text)==0) {
     strcpy(tstr,(char *)data);
     tcontrol->CursorLocationInChars=len; //move cursor to end of pasted text
   }
//   fprintf(stderr,"here1.5 i4=%d i5=%d\n",tcontrol->i[4],tcontrol->i[5]);
//   fprintf(stderr,"here2 tx=%d tstr=%s text=%s\n",tx,tstr,tcontrol->text);
   strcpy(tcontrol->text,tstr); 
//   fprintf(stderr,"here3 tx=%d tstr=%s text=%s\n",tx,tstr,tcontrol->text);
#if Linux
   if (data!=0) {
		 XFree(data);
		data=0;
	 }
#endif


   if (tcontrol->type==CTTextBox) {
		  //adjust the FirstCharacterShowing if the pasted text moves text past the textbox window
	   if (tcontrol->CursorLocationInChars>maxwidth) {
		 tcontrol->FirstCharacterShowing=strlen(tcontrol->text)-maxwidth;
		 tcontrol->CursorLocationInChars=strlen(tcontrol->text);
		 //tcontrol->CursorX=maxwidth/tcontrol->fontwidth;
	   } 
   }

   if (tcontrol->type==CTTextArea) {
		//adjust the FirstCharacterShowing showing to be as close to maxheight
		i=tcontrol->CursorLocationInChars;
		j=0;
		while(i>=0) {
			if (tcontrol->text[i]==10) {
				tcontrol->FirstCharacterShowing=i+1;
				j++;
				if (j==maxheight) {
					i=-1; //exit 
				} //j==maxheight
			} //==10
		i--;
		} //while i
   }


//   DrawFTControl(tcontrol);
  //   XFlush(PStatus.xdisplay);

//fprintf(stderr,"twin primary type=%d format=%d len=%d data=%s\n",type,format,len,data);
#if PINFO
     fprintf(stderr,"after paste\n");
#endif

   break;

//   case 0x58:
//   case 0x78:  //x,X =Cut (scoop)
   case XK_X:
   case XK_x:  //x,X =Cut (scoop)

#if PINFO
     //fprintf(stderr,"Scoop (Cut) %s\n",tcontrol->text2);
		fprintf(stderr,"Scoop (Cut) selected text");
#endif
		if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //only if there is selected text
#if Linux
     if (twin->xwindow!=0) {
//       XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,XA_STRING,
 //           8, PropModeReplace, (unsigned char *)tcontrol->text2,strlen(tcontrol->text2));

				strncpy(tstr,&tcontrol->text[tcontrol->StartSelectedTextInChars],tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars);
       XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,XA_STRING,
            8, PropModeReplace, (unsigned char *)tstr,strlen(tstr));


      //make this window own the clipboard selection
      XSetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD,twin->xwindow,CurrentTime);
      if (twin->xwindow!=XGetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD))
        fprintf(stderr,"Could not get CLIPBOARD selection.\n");


       XChangeProperty(PStatus.xdisplay,twin->xwindow,XA_SECONDARY,XA_STRING,
            8, PropModeReplace, (unsigned char *)tstr,strlen(tstr));


      //make this window own the clipboard selection
      XSetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD,twin->xwindow,CurrentTime);
      if (twin->xwindow!=XGetSelectionOwner(PStatus.xdisplay,XA_CLIPBOARD))
        fprintf(stderr,"Could not get CLIPBOARD selection.\n");

   
      }  //xwindow!=0 
#endif //Linux
#if WIN32
			memcpy(tstr,&tcontrol->text[tcontrol->StartSelectedTextInChars],tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars);
			tstr[tcontrol->EndSelectedTextInChars-tcontrol->StartSelectedTextInChars]=0;

			if (OpenClipboard(twin->hwindow)) {
				EmptyClipboard();
				hClipBuffer = GlobalAlloc(0, strlen(tstr)+1);
				data=(unsigned char *)GlobalLock(hClipBuffer);
				memcpy(data,tstr,strlen(tstr)+1);
				GlobalUnlock(hClipBuffer);
				SetClipboardData(CF_TEXT,hClipBuffer);
				CloseClipboard();
			} //if (OpenClipboard()) {
			GdiFlush();
#endif

     //erase selected text  
     tx=0;
     for(i=0;i<(int)strlen(tcontrol->text);i++) {
       if (!(i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars)) { //not selected text, do not erase-ignore
         tstr[tx]=tcontrol->text[i];
         tx++;
       }
       if (i==tcontrol->StartSelectedTextInChars && tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) {
         tcontrol->CursorLocationInChars=tx;  //is start of selected text, move cursor here         
		 //SetCursorXYFromLocationInChars(tcontrol);  //set new CursorX and CursorY
	   }

     } //end for i  
     tstr[tx]=0; 
     strcpy(tcontrol->text,tstr); 
     //tcontrol->text2[0]=0;
     tcontrol->StartSelectedTextInChars=0;
     tcontrol->EndSelectedTextInChars=0;
//     tcontrol->i[4]=0;
//     tcontrol->i[5]=0;
//     DrawFTControl(tcontrol);   
//     XFlush(PStatus.xdisplay);

		}		//if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //only if there is selected text

     break;



  }  //end switch key
} else { //end if FTControl
//END FTControl KEY COMBINATIONS


if (maxwidth<=0) {
  fprintf(stderr,"textbox %s is not wide enough for this font of text.\n",tcontrol->name);
  } else {

//fprintf(stderr,"FTControl %s focus=%d\n",tcontrol->name,twin->focus,tcontrol->tab);
//this window, FTControl has focus,
//bw=tcontrol->x2-tcontrol->x1;
bh=tcontrol->y2-tcontrol->y1;
if (fh>bh)  {
  fprintf(stderr,"textbox %s is not tall enough for this font of text.\n",tcontrol->name);
} else  {

//NOT FTControl OR ALT KEY
if (!(PStatus.flags&(PControl|PAlt)) && 
key!=XK_Caps_Lock && key!=XK_Control_L && key!=XK_Control_R && key!=XK_Shift_L && key!=XK_Shift_R &&
key!=XK_Alt_L && key!=XK_Alt_R && key!=XK_Num_Lock && key!=XK_Scroll_Lock) {
//-27: //caps lock
//-29:  //right ctrl
//-30:  //left shift
//-31:  //right shift left and right

//watch out for keys other than A-Z, a-z 0-9 and punctuation
//ignore these keys
//if (key!=27) //escape key
if (key!=XK_Escape) //escape key  
{
#if PINFO
  fprintf(stderr,"CheckFTWindowKeypress 2 key=0x%x\n",(int)key);
#endif

//could be backspace, delete, or enter
switch(key)
{
//case 0x64://-44: //left arrow
//case 0x51:  //shift+left arrow
case XK_Left:

	//todo make a function to scroll/move cursor (for horiz scroll bar with horiz mouse wheel)

	     //if there is text to the left of the cursor, select the previous character, and move the cursor
     if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
       tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;  //these are in characters
       tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;  //these are in characters
       //fprintf(stderr,"2==3\n");
     } //if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {


if (tcontrol->type==CTTextBox) {
	if (tcontrol->CursorX==0) {
		if (tcontrol->FirstCharacterShowing>0) {
			tcontrol->FirstCharacterShowing--; //just scroll left one character
			tcontrol->CursorLocationInChars--;
		}
	} else {
		tcontrol->CursorX--;  //move back Cursor X location
		tcontrol->CursorLocationInChars--;  //and cursor location in characters
	}
} else { //CTTextArea
	if (tcontrol->CursorX==0) { //may need to move up a line or scroll up a line
		if (tcontrol->CursorY>0) { //we can move up a line
			tcontrol->CursorY--;
			//determine new CursorX and CursorLocationInChars
			tcontrol->CursorLocationInChars--;
			//always starts at last character in line
			tcontrol->CursorX=GetNumCharsInRowFromEnd(tcontrol->text,tcontrol->CursorLocationInChars,maxwidth)-1;
		} else { //if (tcontrol->CursorY>0) {
			//CursorY==0 
			//we may need to scroll up a line
			if (tcontrol->FirstCharacterShowing>0) {
				//scroll up a line
				i=tcontrol->FirstCharacterShowing;
				//from FirstCharacterShowing -1 go back to chr(10) or maxwidth
				cx=GetNumCharsInRowFromEnd(tcontrol->text,i-1,maxwidth);
				tcontrol->FirstCharacterShowing-=cx;
				tcontrol->CursorX=cx-1;
				tcontrol->CursorLocationInChars--;
			} //if (tcontrol->FirstCharacterShowing>0) {
		} //if (tcontrol->CursorY>0) { 
	} else { //if (tcontrol->CursorX==0) { 
		//CursorX doesn't==0 so we can move the cursor back 1
		tcontrol->CursorX--;
		tcontrol->CursorLocationInChars--;
	} //if (tcontrol->CursorX==0) { 
} //if (tcontrol->type==CTextBox) {

	//if shift key is pressed select text
  if (PStatus.flags&PShift) {
        if (tcontrol->CursorLocationInChars<tcontrol->StartSelectedTextInChars) {
			tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		} else {
			tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		}
	}  //if (PStatus.flags&PShift) { end else no shift
	
break;
//case 0x62: //-43:  //up arrow
case XK_Up:  //do up/down on filemenu and dropdown here?
	if (tcontrol->type==CTTextBox) {  //up arrow for textbox
		if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
			   DelSelect(tcontrol);
		} 
		GoToPrevTab(twin); //shift is on
	} else { //CTTextArea - up arrow may move up a row 
		FT_TextAreaUp(tcontrol,1); //1=MoveCursor too
	} //CTTextBox
	
break; //up arrow
//case 0x66: //-45:  //right arrow
//case 0x53: //shift+right arrow  
case XK_Right:
	if (PStatus.flags&PShift) {
		//if there is text to the right of the cursor, select the next character, and move the cursor
		if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
		tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		//fprintf(stderr,"2==3\n");
		}
	} //if (PStatus.flags&PShift) {

	tmpstr=tcontrol->text;
	if (tcontrol->type==CTTextBox) {
		//see if we need to scroll text right
		i=tcontrol->CursorLocationInChars;
		if (tcontrol->CursorX==maxwidth) {  //cursor is at end of line 
			if (tmpstr[i]!=0) { //and is not last character
				tcontrol->FirstCharacterShowing++; //scroll text
				tcontrol->CursorLocationInChars++;
			}
		} else { //if (tcontrol->CursorX==maxwidth) 
			//cursor is not at the end of the control
			if (tmpstr[i]!=0) { //and is not last character
				tcontrol->CursorLocationInChars++;
				tcontrol->CursorX++;
			} //if (tmpstr[i+1]!=0) {
		} //if (tcontrol->CursorX==maxwidth) 
	} else {
		//is a TextArea
		//see if we need to move down a line
		if (tcontrol->CursorX==maxwidth) {
			if (tmpstr[tcontrol->CursorLocationInChars+1]!=0) { //not the end of text
				if (tcontrol->CursorY==maxheight) { //on last line
					//scroll text up a line
					i=tcontrol->FirstCharacterShowing;
					cx=GetNumCharsInRowFromStart(tcontrol->text,tcontrol->FirstCharacterShowing-1,maxwidth);
					tcontrol->FirstCharacterShowing+=cx;
					tcontrol->CursorX=0;
				} else { //if (tcontrol->CursorY==maxheight) { //on last line
					//cursor is not on the last line
					tcontrol->CursorY++; //move down a line
					tcontrol->CursorX=0;
					tcontrol->CursorLocationInChars++;
				} //if (tcontrol->CursorY==maxheight) { //on last line
			} //if (tmpstr[i+1]!=0) {
		} else { //if (tcontrol->CursorX==maxwidth) {
			//there is room to move right 
			if (tmpstr[tcontrol->CursorLocationInChars]!=0) { //and is not last character
				if (tmpstr[tcontrol->CursorLocationInChars]==10) { //rt arrow on cr, need to move down a row
					tcontrol->CursorX=0;
					tcontrol->CursorLocationInChars++;
					if (tcontrol->CursorY==maxheight) { //last line
						//need to scroll text up a line
						cx=GetNumCharsInRowFromEnd(tmpstr,tcontrol->FirstCharacterShowing-1,maxwidth);
						tcontrol->FirstCharacterShowing+=cx;						
					} else { //if (tcontrol->CursorY==maxheight) { //last line
						//not last line
						tcontrol->CursorY++;
					} //if (tcontrol->CursorY==maxheight) { //last line
				} else { //if (tmpstr[tcontrol->CursorLocationInChars]==10) { //rt arrow on cr, need to move down a row
					tcontrol->CursorX++;
					tcontrol->CursorLocationInChars++;
				} //if (tmpstr[tcontrol->CursorLocationInChars]==10) { //rt arrow on cr, need to move down a row
				
			} //if (tmpstr[i+1]!=0) {
		} //if (tcontrol->CursorX==maxwidth) {		 
	} //if (tcontrol->type==CTTextbox) {


   if (PStatus.flags&PShift) {
      if (tcontrol->CursorLocationInChars>tcontrol->EndSelectedTextInChars) {
		  tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
	  } else {
		  tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
	  }
   } //   if (PStatus.flags&PShift) {


break; //right arrow
//case 0x68: //-42:  //down arrow
case XK_Down:  //todo: perhaps use FT_DOWN_ARROW_KEY
	if (tcontrol->type==CTTextBox) {
		if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
			DelSelect(tcontrol);
		} 
		GoToNextTab(twin); 
	} else { //CTTextBox
		//is CTTextArea
		FT_TextAreaDown(tcontrol,1); //1=MoveCursor
	} //type==CTTextBox
//  fprintf(stderr,"got tab\n");
//  fprintf(stderr,"next tab=%d\n",GetNextTab(twin,twin->focus));
	//SetCursorXYFromLocationInChars(tcontrol);  //update the CursorX and CursorY - probably a time waster, but is convenient to know CursorX and Y
break;
//case 0x61://-40: //home also P
//case 0x50: //shift+home
case XK_Home:
//move cursor to beginning on current line

	if (PStatus.flags&PShift) {
	//this may be the first move with the shift key down
		 if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
		   tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		   tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		 }
	} //PShift

		if (tcontrol->type==CTTextArea) {
			i=tcontrol->CursorLocationInChars-1;
			while(i>=0) {
				tcontrol->CursorLocationInChars=i;
				if (tcontrol->text[i]==10) {
					tcontrol->CursorLocationInChars=i+1; //need to move ahead 1
					i=-1;
				}
				i--;
			}  //while i<=tlen
		} else { //TCTextArea
			tcontrol->CursorLocationInChars=0;
			tcontrol->FirstCharacterShowing=0;
			//is CTTextBox
		} //CTTextArea

if (PStatus.flags&PShift) {

		if (tcontrol->CursorLocationInChars<=tcontrol->StartSelectedTextInChars) {
			tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars; 
		} else {
			tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars; 
		}


/*
  //interesting phenom-selection goes from begin of selection
  if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is a selection
    tcontrol->CursorLocationInChars=tcontrol->StartSelectedTextInChars;   //move cursor to begin of selection
  }  //end if there is a selection
  tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
  tcontrol->CursorLocationInChars=0;
  tcontrol->FirstCharacterShowing=0;
  tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
  DrawFTControl(tcontrol);
  }
else {
  if (tcontrol->CursorLocationInChars>0 || tcontrol->FirstCharacterShowing>0) {
    tcontrol->CursorLocationInChars=0;
    tcontrol->FirstCharacterShowing=0;
  }
*/
}  //if (PStatus.flags&PShift) { PShift

	
break;
//case 0x67: //-41: //end
//case 0x57: //shift+end
case XK_End:


	//this might be first key after shift
	if (PStatus.flags&PShift) {

		   if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
		     tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		     tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		     //fprintf(stderr,"2==3\n");
		   }

	} //PShift

	if (tcontrol->type==CTTextArea) {

		tlen=strlen(tcontrol->text);
		i=tcontrol->CursorLocationInChars;
		while(i<=tlen) {
			tcontrol->CursorLocationInChars=i;
			if (tcontrol->text[i]==10) {
				i=tlen+1;
			}
			i++;
		}  //while i<=tlen
	} else { //TCTextArea
		tcontrol->CursorLocationInChars=strlen(tcontrol->text);
		if ((int)strlen(tcontrol->text)>maxwidth) {
			tcontrol->FirstCharacterShowing=strlen(tcontrol->text)-maxwidth;
		} //strlen //else {
			//tcontrol->FirstCharacterShowing=0;
		//} //strlen
		//is CTTextBox
	} //CTTextArea
	

	if (PStatus.flags&PShift) {

		if (tcontrol->CursorLocationInChars<=tcontrol->StartSelectedTextInChars) {
			tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars; 
		} else {
			tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars; 
		}
		
//mouse button or shift key is down - move selected text to end

/*
  //interesting phenom-selection goes from begin of selection
  if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is a selection
    //tcontrol->CursorLocationInChars=tcontrol->EndSelectedTextInChars;   //move cursor to begin of selection
		tcontrol->CursorLocationInChars=tcontrol->EndSelectedTextInChars;   //move cursor to begin of selection
  }  //end if there is a selection
  tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
  tcontrol->CursorLocationInChars=strlen(tcontrol->text);
  if (tcontrol->type==CTTextBox) {
	  if (strlen(tcontrol->text)>maxwidth) tcontrol->FirstCharacterShowing=tcontrol->CursorLocationInChars-maxwidth;
  	else tcontrol->FirstCharacterShowing=0;
	}
  tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;

*/
//  if (tcontrol->type==CTTextArea) {

//	} //CTTextArea

 
 // }
/*
else {  //not shift
  if (tcontrol->CursorLocationInChars<strlen(tcontrol->text)) {
    tcontrol->CursorLocationInChars=strlen(tcontrol->text);
    if (tcontrol->type==CTTextBox && strlen(tcontrol->text)>maxwidth)  tcontrol->FirstCharacterShowing=strlen(tcontrol->text)-maxwidth;
  }
*/
}  //	if (PStatus.flags&PShift) { end else not shift

	DrawFTControl(tcontrol);

break;
//case -28:  //left ctrl
//case -29:  //right ctrl
//case -30:  //left shift
//case -31:  //right shift left and right
//case -27: //caps lock
case XK_Control_L:
case XK_Control_R:
case XK_Shift_L:
case XK_Shift_R:
case XK_Caps_Lock:
break;
//case -1:  //delete key
case XK_Delete:
//not for CReadOnly
if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
     DelSelect(tcontrol);
} else {  //no text is selected
	if (tcontrol->CursorLocationInChars>=0) {
		strcpy(&tcontrol->text[tcontrol->CursorLocationInChars],&tcontrol->text[tcontrol->CursorLocationInChars+1]);

		if (tcontrol->type==CTTextBox && tcontrol->CursorLocationInChars==tcontrol->FirstCharacterShowing && tcontrol->FirstCharacterShowing>0)
		  {
		  tcontrol->FirstCharacterShowing--;
		  }
//fprintf(stderr,"%s\n",tcontrol->text);
	} //if (tcontrol->CursorLocationInChars>=0) {
}  //if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text else no text is selected


break;

case XK_Return:  //ignore enter (or chr(10) gets added to string)
if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
     DelSelect(tcontrol);
} 
//for CTTextArea add chr(10)
if (tcontrol->type==CTTextArea) {

//identical to default:
//add key to text unless readonly (can select text and move cursor in read only)
if (!(tcontrol->flags&CReadOnly)) {

  //insert chr(10) into text and move i[0]
	strcpy(tstr,tcontrol->text);
	tstr[tcontrol->CursorLocationInChars]=10;
	tcontrol->CursorLocationInChars++;
	tstr[tcontrol->CursorLocationInChars]=0;
	strcpy(istr,&tcontrol->text[tcontrol->CursorLocationInChars-1]);
	sprintf(tcontrol->text,"%s%s",tstr,istr);

	//change Cursor X and Y
	tcontrol->CursorY++;
	tcontrol->CursorX=0;

	//adjust first character showing text if necessary
	if (tcontrol->CursorY>maxheight) {
		//scroll text up by moving the FirstCharacterShowing down
		i=tcontrol->FirstCharacterShowing;
		cx=0;
		tlen=strlen(tcontrol->text);
		while (i<=tlen) {
			if (tcontrol->text[i]==10) {
				i++;
				tcontrol->FirstCharacterShowing=i;
				i=tlen+1; //exit loop
			} else { //if (tcontrol->text[i]==10) {
				if (cx>maxwidth) {
					tcontrol->FirstCharacterShowing=i;
					i=tlen+1; //exit loop
				} else {
					cx++;
					i++;
				}
			} //if (tcontrol->text[i]==10) {
		} //while (i<tlen) {
		tcontrol->CursorY--;  //move cursor back to last row in textarea control
	}//	if (tcontrol->CursorY>maxheight) {
} //CReadOnly

} //CTTextArea
//pass on to human (user) functions
break;

//case 8:   //backspace
case XK_BackSpace:   //backspace
//#if 0
	if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
		 DelSelect(tcontrol);
	} else {
	  if (tcontrol->CursorLocationInChars>0) {  //cursor is not at character 0
		//move all the text back one character
		strcpy(&tcontrol->text[tcontrol->CursorLocationInChars-1],&tcontrol->text[tcontrol->CursorLocationInChars]);

		//move cursor X position back
		if (tcontrol->CursorX>0) {
			tcontrol->CursorX--;
		} else { //if (tcontrol->CursorX>0) {
			//otherwise the cursor moves up a line if a textarea
			if (tcontrol->type==CTTextArea) {
				//determine new CursorX by going back to next new line or maxwidth
				i=tcontrol->CursorLocationInChars-1;
				tmpstr=tcontrol->text;
				cx=0;
				while((tmpstr[i]!=10 || cx==0) && cx<maxwidth) { //while no carriage return (except any at the end of the above line), go until the cr of the next line above or until a full line of characters has been reached
					cx++;
					i--;
				} //while
				if (cx==maxwidth) { 
					tcontrol->CursorX=maxwidth-1; //text on line above fills a full line
				} else {  
					tcontrol->CursorX=cx; //found a carriage return
				}

				//determine new CursorY
				tcontrol->CursorY--;
				if (tcontrol->CursorY<0) {  //have to scroll text down a line
					tcontrol->CursorY=0;
					tcontrol->FirstCharacterShowing-=cx;  //we already counted the number of character in the line above
				} //if (tcontrol->CursorY<0) {
			}  //if (tcontrol->type==CTTextArea) {

			//for a textbox CursorY never changes
			//if there is any text to the left of the control, scroll one character
			if (tcontrol->type==CTTextBox && tcontrol->CursorLocationInChars==tcontrol->FirstCharacterShowing) { //CursorX doesn't change, note that CursorLocationInChars is already>0 here
				tcontrol->FirstCharacterShowing--;
			}
		} //if (tcontrol->CursorX>0) {

		tcontrol->CursorLocationInChars--;
	  }
	} //end else if text selected
//#endif
break;
#if Linux
case XK_ISO_Left_Tab:
#endif
case XK_Tab:  //9 tab
	if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
		 DelSelect(tcontrol);
	} 


//  fprintf(stderr,"got tab\n");
//  fprintf(stderr,"next tab=%d\n",GetNextTab(twin,twin->focus));
  
//window tabs and so do not do extra tab
#if 0 
  if (PStatus.flags&PShift) {
		GoToPrevTab(twin); //shift is on
	}  else { 
		GoToNextTab(twin); 
	}
#endif

//  FT_SetFocus(twin,tcontrol); //give this FTControl the window focus, and the others lose focus

break;
case XK_Page_Up: //Page Up
//one interpretation of page up is that the entire page should shift leaving one line from the previous page - the cursor stays on the same row - gedit has this
//a second interpretation is that the page up is from the cursor position only
//either way - probably the cursor should stay on the same row

//probably storing row and col would be easier - and then deal with chr(10) when drawing only - either way the text probably has to be while looped through unless perhaps there is an array
//int line[rows];
//int size[rows];
//or perhaps text[row][col]

	if (tcontrol->type==CTTextArea) {

		FT_TextAreaPgUp(tcontrol,1);//MoveCursor=1

	
	} //CTTextArea

//			fprintf(stderr,"i[0]=%d i[1]=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing);
break; //case XK_Page_Up
case XK_Page_Down: //Page Down
	if (tcontrol->type==CTTextArea) {

			FT_TextAreaPgDown(tcontrol,1); //1=MoveCursor

	} //CTTextArea

//			fprintf(stderr,"i[0]=%d i[1]=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing);

break; //case XK_Page_Down

default: 
//Regular key (a,b,c,etc...)
//fprintf(stderr,"default key\n");
if (PStatus.flags&PInfo) {
	fprintf(stderr,"regular key i[0]=%d i[1]=%d i[2]=%d i[3]=%d PStatus.asckey=%c\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing,tcontrol->StartSelectedTextInChars,tcontrol->EndSelectedTextInChars,PStatus.asckey);
}



if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
     DelSelect(tcontrol);
} 

//add key to text unless readonly (can select text and move cursor in read only)
//and not an asckey==0 key (like a function key)
if (!(tcontrol->flags&CReadOnly) && PStatus.asckey!=0) {

	tcontrol->CursorX++; //cursor X position moves to right 1
	//if (tcontrol->type==CTTextBox && tcontrol->CursorLocationInChars-tcontrol->FirstCharacterShowing>=maxwidth) {
	if (tcontrol->type==CTTextBox && tcontrol->CursorX==maxwidth) {  //at the end of the textbox or textarea control
		tcontrol->text[tcontrol->CursorLocationInChars]=PStatus.asckey;
		tcontrol->text[tcontrol->CursorLocationInChars+1]=0;  //terminate string since this is the last character
		tcontrol->FirstCharacterShowing++;  //1st char in left of textbox
		tcontrol->CursorX--; //move cursor back to original position before key was pressed
	}	else {
		//is not a textbox and we have to scroll text to left

		//may have to scroll text to the right if inserting text (copy text one position to the right)
		tlen=strlen(tcontrol->text);
		if (tcontrol->CursorLocationInChars<tlen) {
			//have to scroll text to the right
			tcontrol->text[tlen+2]=0; //terminate the string
			for(a=tlen+1;a>tcontrol->CursorLocationInChars;a--) {  //and copy all the text after the new key one to the right
			  tcontrol->text[a]=tcontrol->text[a-1];
			}
		tcontrol->text[tcontrol->CursorLocationInChars]=PStatus.asckey;  //now insert the new key

		} else {
			//do not have to insert character and scroll text to the right - cursor is at end of string
#if Linux
			if (XKeysymToString(key)!=NULL) {
#endif
				tcontrol->text[tcontrol->CursorLocationInChars]=PStatus.asckey;
//			fprintf(stderr,"key is= %c\n",tcontrol->text[tcontrol->CursorLocationInChars]);
				tcontrol->text[tcontrol->CursorLocationInChars+1]=0;
#if Linux
			} //if (XKeysymToString(key)!=NULL) {
#endif

//		tcontrol->text[tcontrol->CursorLocationInChars]=XKeysymToKeycode(PStatus.xdisplay,key);
	} //if (tcontrol->CursorLocationInChars<tlen) {

	}  //	if (tcontrol->type==CTTextBox && tcontrol->CursorLocationInChars-tcontrol->FirstCharacterShowing>=maxwidth) {
	//end do not have to scroll left

	tcontrol->CursorLocationInChars++;

	//check to see if we need to move the cursor to the next line
	if (tcontrol->type==CTTextArea && tcontrol->CursorX>=maxwidth) {
		tcontrol->CursorX=0;
		tcontrol->CursorY++;
		if (tcontrol->CursorY>maxheight) {  //the new CursorY position is below the textarea control 
			//so scroll the text up one line by moving the first character showing down a line
			tmpstr=tcontrol->text;
			i=tcontrol->FirstCharacterShowing;
			cx=0;
			while(tmpstr[i]!=10 && cx<maxwidth && i<tlen) {  //while there is no carriage return, or the maximum width, or maximum length of the string haven't been reached
				i++;
				cx++;
			}
			tcontrol->FirstCharacterShowing=i;  //move the FirstCharacterShowing down a line
			tcontrol->CursorY--;  //move the CursorY back up to the last row of the textarea control
		} //if (tcontrol->CursorY>maxheight) {
	}//if (tcontrol->type==CTTextArea && tcontrol->CursorX>=maxwidth) {

} //if (!(tcontrol->flags&CReadOnly) && PStatus.asckey!=0) {


if (PStatus.flags&PInfo) {
fprintf(stderr,"Control text=%s strlen=%d\n",tcontrol->text,(int)strlen(tcontrol->text));
}

//

//cx+=fw;




break;
}  //end switch key


if (!(PStatus.flags&PShift)) {
//remove any selection
  tcontrol->StartSelectedTextInChars=0;
  tcontrol->EndSelectedTextInChars=0;
//  tcontrol->i[4]=0;
//  tcontrol->i[5]=0;
} //if (!(PStatus.flags&PShift)) {


}  //if (key!=XK_Escape) //escape key  end skip any keys like escape

} //end if FTControl, shift or alt 
}  //end if textbox height not enough for font
}  //end if textbox width not enough for font
} //end if FTControl - in Windows the control key can be released before processing is done and so an extra character appears

DrawFTControl(tcontrol);    //refresh drawing of text FTControl

if (strcmp(istr,tcontrol->text)!=0) {
  if (tcontrol->OnChange!=0) {
		(*tcontrol->OnChange) (twin,tcontrol);
		twin=GetFTWindow(winname); //check to see if window was deleted
		tcontrol=GetFTControl(conname); //check to see if control was deleted
		if (tcontrol==0 || twin==0) return; //user deleted control or window
	} //if (tcontrol->OnChange!=0) {
}//if (strcmp(istr,tcontrol->text)!=0) {

//}  //end if textbox height not enough for font
//}  //end if textbox width not enough for font
}  //end if textbox
//END Keypress IN TEXTBOX CODE



//START Keypress IN FILEMENU OR DROPDOWN FTControl
//if (tcontrol->tab==twin->focus && (tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown)) {
if (tcontrol==twin->FocusFTControl && (tcontrol->type==CTFileMenu || tcontrol->type==CTDropdown)) {

    switch (key)
      {

//move to freethought.c

//      case LSHIFT:
//      case RSHIFT:  //ignore shift keys
//        break;
      case XK_Return:
#if PINFO
        fprintf(stderr,"enter in filemenu or dropdown\n");        
#endif
					tcontrol->ilist->cur=1;
					//tcontrol->ilist->orig=1;
					OpenFTItemList(tcontrol->ilist); //Open will draw					


      break;  
      case XK_Up:
      break;
      case XK_Down:
//for now only dropdown controls (not filemenu controls) are tabable and therefore can be opened with a down arrow
// 				if (tcontrol->type==CTDropdown) {
					//Control ItemList is not open 
					//Down arrow opens dropdown (and filemenu? an unopened filemenu cannot be tabbed to - but if it can open too, I guess)
					//set current item to 1
					tcontrol->ilist->cur=1;
					//tcontrol->ilist->orig=1;
					OpenFTItemList(tcontrol->ilist); //Open will draw					
					//need to tcontrol->flags|=CItemListOpen ?
//				}
      break;
      case XK_Right:

      break;
      case XK_Left:
 
      break;


      case XK_Escape: //0x1b:	/*esc*/
				if (tcontrol!=0) {
//				fprintf(stderr,"Escape close %s ItemList\n",tcontrol->name);
					CloseFTItemList(tcontrol->ilist);
				}
        break;

      default:
        break;
      }  //end switch



//CheckHotKeys(twin,key); //done above
}  //end if CTFileMenu || CTDropdown
//END Keypress IN FILEMENU OR DROPDOWN FTControl


//START Keypress IN FILEOPEN FTControl
//File Open FTControls
if (tcontrol->type==CTFileOpen || tcontrol->type==CTItemList)
{
//fprintf(stderr,"CTFileOpen\n");
switch (key) {
case XK_Down:
	ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_DOWN|FT_SCROLL_FTITEMLIST_SELECTION);  //scroll down, scroll selection too
break;
case XK_Up:
	ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_UP|FT_SCROLL_FTITEMLIST_SELECTION);  //scroll up, scroll selection too
break;
case XK_Return:
//start autogenerated code
//for now, double clicking or pressing enter to go into a folder is done in human code
  if (tcontrol->ilist!=0) {
//  fprintf(stderr,"enter key\n");
  litem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->ilist->cur);
//if this is a folder, go in to the folder
  if (litem!=0 && litem->flags&FTI_FOLDER) {
    tcontrol->ilist->cur=1;//0;
    tcontrol->FirstItemShowing=1;//0;
#if Linux
    chdir(litem->name);
#endif
#if WIN32
    _chdir(litem->name);
#endif

		tcontrol->ilist->flags|=FTIL_REREAD;
#if Linux
		getcwd(tcontrol->text2,FTBigStr); //FTBiggestStr);
#endif
#if WIN32
		_getcwd(tcontrol->text2,FTBigStr); //FTBiggestStr);
#endif

		strcpy(tcontrol->filename,"");  
    DrawFTControl(tcontrol);  //redraw FTControl
		//update dir control?
    if (tcontrol->OnChange!=0) {
			(*tcontrol->OnChange) (twin,tcontrol);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol=GetFTControl(conname); //check to see if control was deleted
			if (tcontrol==0 || twin==0) return; //user deleted control or window
		} //OnChange

//    update_sb_foOpenFile2();  //update scroll button (in DrawFTControl i[0] is set to 0)
  }
//end autogenerated code

  }  //ilist!=0
break;
case XK_Page_Down:
  ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_PAGE_DOWN|FT_SCROLL_FTITEMLIST_SELECTION);  //scroll down, scroll selection too
break;
case XK_Page_Up:
  ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_PAGE_UP|FT_SCROLL_FTITEMLIST_SELECTION);  //scroll up, scroll selection too
break;
} //end switch


} //end if CTFileOpen || CTItemList




//need? FTControl can be deleted by Keypress function
//DrawFTControl(tcontrol);    //refresh drawing of FTControl

//fprintf(stderr,"Check Keypress func\n");
//now call any Keypress function that is attached to this FTControl
if (tcontrol->Keypress!=0)
{
//fprintf(stderr,"Keypress func %d\n",(int)key);
(*tcontrol->Keypress) (twin,tcontrol,key);
twin=GetFTWindow(winname); //check to see if window was deleted
tcontrol=GetFTControl(conname); //check to see if control was deleted
if (tcontrol==0 || twin==0) return; //user deleted control or window
}



//fprintf(stderr,"Done with Keypress func\n");




}  //if tcontrol!=0 some FTControl does have window focus
//could have else strcpy(conname,""); but there is no code below here that needs to see if a control was deleted

//window Keypress function is done in event function

} //exclusive FTControl

}  //end if twin!=0

if (PStatus.flags&PInfo)
  fprintf(stderr,"End CheckFTWindowKeypress\n");
}  //end CheckFTWindowKeypress

//returns the number of characters in a line starting from the first character
//tmpstr is the text, i is the CursorLocationInChars, maxwidth is the maximum num of characters on the line
int GetNumCharsInRowFromStart(char *tmpstr,int i,int maxwidth) {
	int ExitLoop,cx;

	//go forward until a newline or maxwidth
	ExitLoop=0;
	cx=0; //character count
	//i=tcontrol->CursorLocationInChars
	while(!ExitLoop) {  //same exact code as below- todo - make into a function
		cx++; //count this character
		if (tmpstr[i]==10) {  //found cr
			ExitLoop=1;
		} else {
			if (tmpstr[i]==0) {  //reached end of text
				ExitLoop=1; 
			} else { //i==0
				if (cx==maxwidth) { //reached maximum width
					ExitLoop=1;
				} else { //if (cx==maxwidth) { //reached maximum width
					i++;  //move forward to the next character
				} //if (cx==maxwidth) { //reached maximum width
			} //i==0
		} //if (tmpstr[i]==10 && cx!=0) {  //found cr
	} //while
	return(cx);
} //int GetNumCharsInRowFromStart(char *tmpstr,tcontrol->CursorLocationInChars,maxwidth) {

//returns the number of characters in a line starting from the last character
//tmpstr is the text, i is the CursorLocationInChars, maxwidth is the maximum num of characters on the line
int GetNumCharsInRowFromEnd(char *tmpstr,int i,int maxwidth) {
	int ExitLoop,cx;

	//go back until a newline or maxwidth
	ExitLoop=0;
	cx=0; //character count- starts on the last character
	//i=tcontrol->CursorLocationInChars
	while(!ExitLoop) {  //same exact code as below- todo - make into a function
		if (tmpstr[i]==10 && cx!=0) {  //found cr
			ExitLoop=1;
			i++; //move forward past cr of line above previous one
			//cx--;
		} else {
			cx++;
			if (i==0) {  //reached start of text
				ExitLoop=1; 
			} else { //i==0							
				//i>0
				if (cx==maxwidth) { //reached maximum width
					ExitLoop=1;
				} else {
					i--; //move back one more char
				}

			} //i==0
		} //if (tmpstr[i]==10 && cx!=0) {  //found cr
	} //while
	return(cx);
} //int GetNumCharsInRowFromEnd(char *tmpstr,tcontrol->CursorLocationInChars,maxwidth) {

#if 0 
//Get the X position of the cursor in a textbox or textarea
int GetCursorXFromLocationInChars(FTControl *tcontrol)
{
	int maxwidth,fw,i,j,CurrentCursor;
	char *textp;

	if (tcontrol!=0) {
		if (tcontrol->type==CTTextBox) {
			//control is a textbox
			//determining the cursor x and y position in chars is easy- y is always 0, and just subtract CursorLocationInChars from the FirstCharacterShowing in the textbox
			return(tcontrol->CursorLocationInChars-tcontrol->FirstCharacterShowing);
		} else {//if (tcontrol->type==CTTextBox) {
			if (tcontrol->type==CTTextArea) {
				//control is a textarea
				//to determine CursorX and CursorY, we need to count all the carriage returns from the first character showing, 
				//until we reach the CursorLocationInChars				
				fw=tcontrol->fontwidth;
				maxwidth=((int)(tcontrol->x2-tcontrol->x1)/fw)-1;
				//fh=tcontrol->fontheight;
				//maxheight=tcontrol->y2-tcontrol->y1/fh;
				i=0;
				j=0;
				CurrentCursor=tcontrol->CursorLocationInChars;
				//StartPage=tcontrol->FirstCharacterShowing;
				textp=tcontrol->text;
				//count backwards from cursor to line feed or beginning of text and then divide by maxwidth
				while(CurrentCursor>0 && (textp[CurrentCursor]!=10 || i==0)) {
					CurrentCursor--;
					i++;
				} //while
				i=(i%maxwidth)-1;  //remainer is column number
					//cursor is below first character showing
					//count to next line feed
/*
					while(CurrentCursor<CursorLoc) {
						if (textp[CurrentCursor]==10) {  //carriage return
							j++;
							i=0;
						} else {
							i++;
							if (i>maxwidth) {  //text is wrapped
								j++;
								i=0;
							} //if (i>maxwidth) {
						} //if (textp[CurrentCursor]==0x10) {  //carriage return
						CurrentCursor++;
					} //while
*/
				return(i);
			} //if (tcontrol->type==CTTextArea) {
		} //if (tcontrol->type==CTTextBox) {
		return(0);
	} //tcontrol!=0
	fprintf(stderr,"GetCursorXFromLocationInChars called with tcontrol==0\n");
	return(0);
} //int GetCursorXFromLocationInChars(FTControl *tcontrol)
#endif

#if 0 
//set the CursorX and CursorY (the position of the cursor relative to the textbox or textarea control in characters, first character=0)
int SetCursorXYFromLocationInChars(FTControl *tcontrol)
{
	int maxwidth,fw,i,j,CurrentCursor;
	char *textp;

	if (tcontrol!=0) {
		if (tcontrol->type==CTTextBox) {
			//control is a textbox
			//determining the cursor x and y position in chars is easy- y is always 0, and just subtract CursorLocationInChars from the FirstCharacterShowing in the textbox
			tcontrol->CursorY=0;
			tcontrol->CursorX=tcontrol->CursorLocationInChars-tcontrol->FirstCharacterShowing;
		} else {//if (tcontrol->type==CTTextBox) {
			if (tcontrol->type==CTTextArea) {
				//control is a textarea
				//to determine CursorX and CursorY, we need to count all the carriage returns from the first character showing, 
				//until we reach the CursorLocationInChars
				maxwidth=tcontrol->x2-tcontrol->x1;
				fw=tcontrol->fontwidth;
				//fh=tcontrol->fontheight;
				//maxheight=tcontrol->y2-tcontrol->y1/fh;
				i=0;
				j=0;
				CurrentCursor=tcontrol->FirstCharacterShowing;
				textp=tcontrol->text;
				while(CurrentCursor<tcontrol->CursorLocationInChars) {
					if (textp[CurrentCursor]==0x10) {  //carriage return
						j++;
						i=0;
					} else {
						i++;
						if (i>maxwidth) {  //text is wrapped
							j++;
							i=0;
						} //if (i>maxwidth) {
					} //if (textp[CurrentCursor]==0x10) {  //carriage return
					CurrentCursor++;
				} //while
				tcontrol->CursorX=i;
				tcontrol->CursorY=j;

			} //if (tcontrol->type==CTTextArea) {
		} //if (tcontrol->type==CTTextBox) {
		return(1);
	} //tcontrol!=0
	fprintf(stderr,"SetCursorXYFromLocationInChars called with tcontrol==0\n");
	return(0);
} //int SetCursorXYFromLocationInChars(FTControl *tcontrol)
#endif 

//CloseFreeThought ends the FreeThought program and destroys all the program's windows
//note that a FreeThought app can run with no windows
int CloseFreeThought(void)
{
FTWindow *twin,*nwin,*lwin;

#if WIN32
	//wait for FT_GetInput thread to stop
	//WaitForMultipleObjects(1,PStatus.GetInputThreadID,TRUE,INFINITE);

    // Close thread and mutex handles

//    for( i=0; i < THREADCOUNT; i++ )
    //CloseHandle(PStatus.GetInputThread);
	//TerminateThread(PStatus.GetInputThread,0);

    CloseHandle(PStatus.GetInputMutex);
	CloseHandle(PStatus.DrawFTControlsMutex);
#endif //WIN32

if (PStatus.flags&PInfo) {
  fprintf(stderr,"CloseFreeThought\n");
}

if (PStatus.iwindow!=0) {

//we need to close, and destroy (free) all remaining windows
//close so that OnClose functions get called

//probably this should go from last to first- and another reason a double-linked list for windows (and for controls too) is probably better- but it is somewhat rare to need to go backwards and slows coding a little to have 2 links

//This is one of the trickiest parts of FreeThought- and a lot of Windows coding
//that when the app is closing, some parts might be freed before other parts are done using them
//so it is best to destroye all the windows newest to oldest

	twin=PStatus.iwindow;
	while(twin!=0) {	

		if (twin->next==0) {
			//there is no next window
			lwin=twin; //then close and destroy this window
		} else {
			//there is a next window
			//go to the last window and free it
			nwin=twin->next;  //preserve next window
			while(nwin!=0) {
				lwin=nwin;
				nwin=nwin->next;
			}
		} //		if (twin->next==0) {

		//destroy the last window		
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"CloseFTWindow %s\n",lwin->name);
		}
		
		CloseFTWindow(lwin);  //this will call any twin->OnClose() functions
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"DestroyFTWindow %s\n",lwin->name);
		}
		DestroyFTWindow(lwin);

		twin=PStatus.iwindow; //go back to the top of the window list
	} //	while(twin!=0) {	
}
//fprintf(stderr,"CloseX XCloseDisplay\n");
#if Linux
XCloseDisplay (PStatus.xdisplay);
#endif

#if PINFO
fprintf(stderr,"Freethought done.\n");
#endif
return(1);
} //end CloseFreeThought

//SIGALRM signal handler
void catch_alarm(int sig) 
{
//fprintf(stdout,"got SIGALRM ");
//fprintf(stdout,"A ");
//There are X events
//XPending causes "Locking assertion failure." message
//if (XPending(PStatus.xdisplay)) {
	//ProcessEvents(); //process X events
//	}
//signal(sig,catch_alarm); //re-enable signal handler - apparently not needed
}

#if Linux
int InitX(void)
{
Window root_win;
int xr,yr;  //x,y
unsigned int wr,hr;  //width,height
unsigned int bwr;  //borderwidth
unsigned int dr;  //depth


//int a,fy;
//FTWindow *twin;
//char font_name[255];

//mallocs
//should be 8 not 20 ??
//root_win=(Window *)malloc(sizeof(Window));

/*
x_return=(int *)malloc(sizeof(int));
y_return=(int *)malloc(sizeof(int));
width_return=(unsigned int *)malloc(sizeof(unsigned int));
height_return=(unsigned int *)malloc(sizeof(unsigned int));
border_width_return=(unsigned int *)malloc(sizeof(unsigned int));
depth_return=(unsigned int *)malloc(sizeof(unsigned int));
*/


//font_path=(char **)malloc(1000);
//iwindow=malloc(sizeof (FTWindow));


//
XInitThreads(); //this needs to be called before any X Windows functions
//this allows threads to call Xlib functions. For example, if two threads are each trying to draw to the screen this will cause problems unless XInitThreads is called.


        /* connect to the X server */
        PStatus.xdisplay = XOpenDisplay(0);

        if (PStatus.xdisplay == 0)
	{
                fprintf (stderr, "cannot connect to server\n");
                exit(EXIT_FAILURE);
        }

        /* get default screen */
        PStatus.xscreen = DefaultScreen(PStatus.xdisplay);


XA_UTF8=XInternAtom(PStatus.xdisplay,"UTF8",0);
XA_UNICODE=XInternAtom(PStatus.xdisplay,"UNICODE",0);
XA_CLIPBOARD=XInternAtom(PStatus.xdisplay,"CLIPBOARD",0);
XA_WM_PROTOCOLS=XInternAtom(PStatus.xdisplay,"WM_PROTOCOLS",0);
XA_WM_DELETE_WINDOW=XInternAtom(PStatus.xdisplay,"WM_DELETE_WINDOW",0);
XA_MOTIF_WM_HINTS=XInternAtom(PStatus.xdisplay,"_MOTIF_WM_HINTS",True);
XA_WM_EXTENDED_HINTS=XInternAtom(PStatus.xdisplay,"WM_EXTENDED_HINTS",False);
XA_NET_WM_STATE=XInternAtom(PStatus.xdisplay,"_NET_WM_STATE",0);
XA_WM_TAKE_FOCUS=XInternAtom(PStatus.xdisplay,"WM_TAKE_FOCUS",0);
XA_TASKBAR=XInternAtom(PStatus.xdisplay,"_NET_WM_STATE_SKIP_TASKBAR",0);
XA_NET_WM_WINDOW_TYPE=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE",0);
XA_NET_WM_WINDOW_TYPE_SPLASH=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_SPLASH",0);
XA_NET_WM_WINDOW_TYPE_DIALOG=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_DIALOG",0);
XA_NET_WM_WINDOW_TYPE_MENU=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_MENU",0);
XA_NET_WM_WINDOW_TYPE_UTILITY=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_UTILITY",0);
XA_NET_WM_WINDOW_TYPE_SPLASH=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_SPLASH",0);
XA_NET_WM_WINDOW_TYPE_NORMAL=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_NORMAL",0);
XA_NET_WM_WINDOW_TYPE_TOOLBAR=XInternAtom(PStatus.xdisplay,"_NET_WM_WINDOW_TYPE_TOOLBAR",0);
XA_NET_WM_STATE_SKIP_TASKBAR=XInternAtom(PStatus.xdisplay,"_NET_WM_STATE_SKIP_TASKBAR",0);
//XA_NET_WM_STATE=XInternAtom(PStatus.xdisplay,"_NET_WM_STATE",0);
#if 0 
//will get italic
//font_name="*-helvetica-*-12-*";
//strcpy(font_name,"*charter*");
//strcpy(font_name,"*serif*12*");
//strcpy(font_name,"*courier*12*");
strcpy(font_name,"-adobe-courier-bold*12*");
//strcpy(font_name,"*mono-medium*12*");
//strcpy(font_name,"Monospace*");
tcontrol->font=XLoadQueryFont(PStatus.xdisplay,font_name);
if (!tcontrol->font)
   {
   fprintf(stderr, "Warning:  Could not load font %s, trying for *charter*12*.\n",font_name);
   strcpy(font_name,"*charter*12*");
   //strcpy(font_name,"*times*12*");
   tcontrol->font=XLoadQueryFont(PStatus.xdisplay,font_name);
   if (!tcontrol->font)
     {
     fprintf(stderr, "Error:  Could not load font %s.\n",font_name);
     exit(1);
     }
   }
#endif


PStatus.depth = DefaultDepth(PStatus.xdisplay, PStatus.xscreen);
if (PStatus.depth!=32 && PStatus.depth!=24) {
	fprintf(stderr,"Warning: Display depth is not 24 or 32. Freethought works best with 24 or 32 bit display depth. Current depth = %d\n",PStatus.depth);
}

//depth=24
PStatus.visual=DefaultVisual(PStatus.xdisplay,PStatus.xscreen);


PStatus.RootWin=RootWindow(PStatus.xdisplay,PStatus.xscreen);
PStatus.xgc=XCreateGC(PStatus.xdisplay,RootWindow(PStatus.xdisplay,DefaultScreen(PStatus.xdisplay)),0, NULL);

//get root window width and weight
XGetGeometry(PStatus.xdisplay,PStatus.RootWin,&root_win,&xr,&yr,&wr,&hr,&bwr,&dr);
PStatus.rw=wr;  
PStatus.rh=hr;  


//InitFreeThought();

//fprintf(stderr,"root window %d %d\n",wr,hr);
//XFlush(PStatus.xdisplay);
//XSelectInput(PStatus.xdisplay,PStatus.RootWin,xeventmask);
//XFlush(PStatus.xdisplay);

//fprintf(stderr, "here1\n");


//draw window 0 (first window)



return(1);

}  //end InitX
#endif  //Linux

#if WIN32
void InitWindows(void)
{
	//Get Depth (flagsPerPixel of display)
	PStatus.depth=GetDeviceCaps(GetDC(NULL),BITSPIXEL);
	

} //void InitWindows(void)
#endif //WIN32


void InitFreeThought(void) 
{
#if Linux
struct itimerval itime;
#endif

//INITIALIZE WINDOWS
#if Linux
InitX();
#endif
#if WIN32
InitWindows();
#endif

//INITIALIZE FREETHOUGHT
//initialize Freethought variables

//put in current time 
gettimeofday(&PStatus.utime,NULL);
gettimeofday(&PStatus.timertime,NULL); //last timer check (each 1ms)


//PStatus.timespace=7;
PStatus.PadY=4;
PStatus.PadX=4;
PStatus.ResizePix=6;
PStatus.dclickdelay=200;  //200ms
PStatus.ScrollPixelsX=5; //5 pixels per click
PStatus.ScrollPixelsY=5; //5 pixels per click
//PStatus.timescale=1;
//PStatus.FirstWindow=(FTControlfunc *)main_CreateFTWindow;
#if Linux
getcwd(PStatus.ProjectPath,FTBigStr);
strcat(PStatus.ProjectPath,"/");
#endif
#if WIN32
_getcwd(PStatus.ProjectPath,FTBigStr);
strcat(PStatus.ProjectPath,"\\");
#endif

//scroll bar button controls that are attached to other controls such as textarea and fileopen controls
strcpy(PStatus.PrefixVScrollUp,"_scr_up_");
strcpy(PStatus.PrefixVScrollDown,"_scr_down_");
strcpy(PStatus.PrefixVScrollSlide,"_scr_vslide_");
strcpy(PStatus.PrefixVScrollBack,"_scr_vback_");
strcpy(PStatus.PrefixHScrollLeft,"_scr_left_");
strcpy(PStatus.PrefixHScrollRight,"_scr_right_");
strcpy(PStatus.PrefixHScrollSlide,"_scr_hslide_");
strcpy(PStatus.PrefixHScrollBack,"_scr_hback_");
//file open button controls that are attached to other controls, like fileopen controls
strcpy(PStatus.PrefixName,"_fo_name_");
strcpy(PStatus.PrefixSize,"_fo_size_");
strcpy(PStatus.PrefixDate,"_fo_date_");


#if WIN32
PStatus.GetInputMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    if (PStatus.GetInputMutex == NULL) { 
        fprintf(stderr,"CreateMutex error: %d\n", GetLastError());
    }
#if 0 
  PStatus.GetInputThread = CreateThread( 
                     NULL,       // default security attributes
                     0,          // default stack size
                     (LPTHREAD_START_ROUTINE) FT_GetInput, 
                     NULL,       // no thread function arguments
                     0,          // default creation flags
                     &PStatus.GetInputThreadID); // receive thread identifier

        if( PStatus.GetInputThread == NULL ) {        
            fprintf(stderr,"CreateThread error: %d\n", GetLastError());
        }
#endif
#endif


#if WIN32
		PStatus.DrawFTControlsMutex = CreateMutex(
			NULL,              // default security attributes
			FALSE,             // initially not owned
			NULL);             // unnamed mutex

		if (PStatus.DrawFTControlsMutex == NULL) { 
			fprintf(stderr,"CreateMutex error: %d\n", GetLastError());
		}
#endif

FT_GetInput();



#if Linux
//Establish a handler for SIGALARM signal (for timer interrupt to allow xevent processing)
signal(SIGALRM, catch_alarm);

//set a timer which will send a SIGALARM signal at a periodic rate (every 1 ms)
itime.it_interval.tv_usec=100000;
itime.it_interval.tv_sec=0;
itime.it_value.tv_usec=100000;
itime.it_value.tv_sec=0;
#endif
//value is counted down and then set to interval
//setitimer - causes the read of the video image to fail - returns black
//perhaps interrupting the /dev/video0 file read causes a failure?
//verify that this is correct way to use setitimer
//setitimer(ITIMER_REAL,&itime,0);

#if Linux
#if USE_FFMPEG
//Initialize FFMPEG AV Libraries
/* register all the codecs */
//fprintf(stderr,"avcodec_register_all()\n");
avcodec_register_all();
#endif //USE_FFMPEG
#endif  //Linux

//call universal (global) function to make FirstWindow
//(*PStatus.FirstWindow) ();


} //InitFreeThought


//freethought programs call FT_GetInput from main() in a while loop until exiting
//#if Linux
void FT_GetInput(void)
//#endif
//#if WIN32
//DWORD WINAPI FT_GetInput(LPVOID lpParam)
//#endif
{
FTWindow *twin;//,*nwin;
FTControl *tcontrol;
unsigned long long tm1,tm2;
//char winname[FTMedStr];
//char conname[FTMedStr];
#if WIN32
MSG winmsg;
DWORD dwWaitResult;
#endif

//if (PStatus.flags&PInfo)
//  fprintf(stderr,"Start FT_GetInput\n");

//ProcessEvents();  //window can be deleted by input event
#if WIN32
//request the mutex
     dwWaitResult = WaitForSingleObject( 
            PStatus.GetInputMutex,    // handle to mutex
            INFINITE);  // no time-out interval


		if (dwWaitResult!= WAIT_OBJECT_0) {
			return;
		}
#endif

GetTime(); //put time into PStatus

//we need a software interrupt to make sure xevents get processed
//even during (interrupting) for example large bitmap drawing function calls
//process Windows (and FTControl) timer events every 10 milliseconds
//gettimeofday
//test time
//check on 02/25/09 - this loop processed every 7 or 8 usec, so very fast relative to a 1 msec timer
//fprintf(stdout, "%d.%d\n",(int)PStatus.utime.tv_sec,(int)PStatus.utime.tv_usec);
tm1=(unsigned long long)PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec;
tm2=(unsigned long long)PStatus.timertime.tv_sec*1000000+PStatus.timertime.tv_usec;

//fprintf(stderr,"tm1=%lli tm2=%lli\n",tm1,tm2);
//if (tm1 > tm2+10000000) { //each 1ms check for OnTimer events
//if (tm1 > tm2+1000000) { //each 1000ms check for OnTimer events
if (tm1 > tm2+1000) { //each 1ms check for OnTimer events (and Windows that have a close time)
//	fprintf(stderr,"1ms timercheck\n");
	//update timer check time
	PStatus.timertime.tv_sec=PStatus.utime.tv_sec;
	PStatus.timertime.tv_usec=PStatus.utime.tv_usec;

  twin=PStatus.iwindow;
  while (twin!=0) {
		//strcpy(winname,twin->name);


		//to disable a timer (for example until needed) set twin->timer=-1, =0 is constant
		if (twin->OnTimer !=0 && twin->timer>=0) {
			//fprintf(stderr,"Found OnTimer function in window %s\n",twin->name);
			//check if timer has past
			tm2=(long long)twin->timertime.tv_sec*1000000+twin->timertime.tv_usec;
			tm2+=twin->timer*1000; //add milliseconds
			if (tm1>tm2) { //timer interval passed
				//update last timer time
				twin->timertime.tv_sec=PStatus.utime.tv_sec;
				twin->timertime.tv_usec=PStatus.utime.tv_usec;
				//call window OnTimer function
				//perhaps call this is low priority thread
				//perhaps all calls out of the main input loop should be put in a low-priority thread, in order to keep the CPU focus on the polling for xevents (keypress, mouse move, resize, etc).
				(*twin->OnTimer) (twin);
				//twin=GetFTWindow(winname); //check to see if window was deleted
				//if (twin==0) {
				//	return; //user deleted control or window
				//}
			} //tm2>tm1
		} //twin->OnTimer !=0


//Go through each FTControl and call any timers

		tcontrol=twin->icontrol;
		while (tcontrol!=0) {
			//strcpy(conname,tcontrol->name);
#if Linux
			if (XPending(PStatus.xdisplay)) {
						//Process X events such as window resize, keypress, button press, etc.
						ProcessEvents();  //window can be deleted by input event
				}
#endif //Linux

#if WIN32
		//	 if (PeekMessage(&msg,hwnd,0,0,PM_REMOVE)) 
			while (PeekMessage(&winmsg,NULL,0,0,PM_REMOVE)) {        
				//if(!GetMessage(&winmsg,NULL,0,0)) {
			//        return winmsg.wParam;
				//}

				//if (!IsDialogMessage(hWin,&winmsg))
					// {
					TranslateMessage(&winmsg); 
					DispatchMessage(&winmsg);
					//}
			}

#endif //WIN32


			if (tcontrol->OnTimer !=0 && tcontrol->timer>=0) {
				//fprintf(stderr,"Found OnTimer function for FTControl %s in window %s\n",tcontrol->name,twin->name);
				//check if timer has past
				tm2=(long long)tcontrol->timertime.tv_sec*1000000+tcontrol->timertime.tv_usec;				
				tm2+=tcontrol->timer*1000; //add milliseconds
				if (tm1>tm2) { //timer interval passed
					//update last timer time
					tcontrol->timertime.tv_sec=PStatus.utime.tv_sec;
					tcontrol->timertime.tv_usec=PStatus.utime.tv_usec;
					//call window OnTimer function
					//perhaps call this is low priority thread
					//perhaps all calls out of the main input loop should be put in a low-priority thread, in order to keep the CPU focus on the polling for xevents (keypress, mouse move, resize, etc).
					(*tcontrol->OnTimer) (twin,tcontrol);
					//twin=GetFTWindow(winname); //check to see if window was deleted
					//tcontrol=GetFTControl(conname); //check to see if control was deleted
					//if (tcontrol==0 || twin==0) return; //user deleted control or window
				} //tm2>tm1
			} //twin->OnTimer !=0	
		tcontrol=tcontrol->next;
		}  //while tcontrol!=0


		//check for windows that have a close time
		if (twin->flags&FTW_CloseOnTimer) { 			
			//fprintf(stderr,"Found CloseTime in window %s\n",twin->name);
			//check if timer has past
			//already calculated tm1
			//tm1=(unsigned long long)PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec;
			tm2=(long long)twin->CloseTime.tv_sec*1000000+twin->CloseTime.tv_usec;
			if (tm1>tm2) { //Close time interval passed
				PStatus.mb=FTMB_TIMER_CLOSE; //to exit FTMessageBox while-GetInput loop
				CloseFTWindow(twin);
				return;
			} //tm2>tm1
		} //twin->OnTimer !=0


	twin=twin->next;
	} //twin!=0
	

} //tm2>tm1


if (PStatus.flags&PSelectingText) {
	//if user is selecting text, every 100ms check to see if we need to continue scrolling selecting with the mouse
	tm2=(long long)PStatus.timertime2.tv_sec*1000000+PStatus.timertime2.tv_usec;				
	if (tm1 > tm2+100000) { 
		PStatus.timertime2.tv_sec=PStatus.utime.tv_sec;
		PStatus.timertime2.tv_usec=PStatus.utime.tv_usec;
	  //if user is selecting text, keep checking to see if selection should be scrolling even if mouse is not moving and generating MouseMove events
		CheckFTWindowMouseMove(PStatus.FocusFTWindow,PStatus.LastMouseX,PStatus.LastMouseY);
	} //if (tm1 > tm2+100000) { 
} //if (PStatus.flags&PSelectingText) {

//done with OnTimer - now check other Windows events
#if Linux
//There are X events
if (XPending(PStatus.xdisplay))
  {

//  fprintf(stderr,"events pending\n");

//Some events were being missed with 
//   if (XCheckWindowEvent(PStatus.xdisplay, twin->xwindow,xeventmask,&xevent))

			//Process X events such as window resize, keypress, button press, etc.
      ProcessEvents();  //window can be deleted by input event



/*
    //delete any window that was closed by an input event
    twin=PStatus.iwindow;
    while (twin!=0)
      {  //go thru each window and check for events
      nwin=twin->next;
      if (twin->flags&WClose) {
    	  //fprintf(stderr,"b4 call to DestroyFTWindow\n");
	      //DestroyFTWindowByNum(twin->num);
				//DestroyFTWindow(twin); //safe to destroy FTWindow structure (twin) here
				
    	  //fprintf(stderr,"After call to DestroyFTWindow twin=%p\n",twin);
	 	  }
      //twin=(FTWindow *)twin->next;
      twin=nwin;
  	  //fprintf(stderr,"twin=%p\n",twin);
      //fprintf(stderr,"Done with get_window_input\n");
      }  //end while

*/
  }  //end if XPending
#endif
#if WIN32
//	 if (PeekMessage(&msg,hwnd,0,0,PM_REMOVE)) 
			while ( PeekMessage(&winmsg,NULL,0,0,PM_REMOVE ) )
        {
            //if(!GetMessage(&winmsg,NULL,0,0)) {
        //        return winmsg.wParam;
			//}

			//if (!IsDialogMessage(hWin,&winmsg))
			 // {
              TranslateMessage(&winmsg); 
              DispatchMessage(&winmsg);
			  //}
        }

#endif //WIN32


//  fprintf(stderr,"check for buttonhold\n");

  //now check for buttonhold events
  if (PStatus.flags&EButtonHold(0)) {
    //this is why ButtonWin is needed, to store the window a button is being held in
    CheckFTWindowButtonDownHold(PStatus.ButtonFTWindow[0],PStatus.ButtonX[0],PStatus.ButtonY[0],0);
  }
  if (PStatus.flags&EButtonHold(1)) {
    CheckFTWindowButtonDownHold(PStatus.ButtonFTWindow[1],PStatus.ButtonX[1],PStatus.ButtonY[1],1);
  }
  if (PStatus.flags&EButtonHold(2)) {
    CheckFTWindowButtonDownHold(PStatus.ButtonFTWindow[2],PStatus.ButtonX[2],PStatus.ButtonY[2],2);
  }
  

#if WIN32
  //end wait for mutex
    // Release ownership of the mutex object
    if (! ReleaseMutex(PStatus.GetInputMutex)) 
    { 
        // Handle error.
    } 
#endif //WIN32


//if (PStatus.flags&PInfo)
//  fprintf(stderr,"Done with FT_GetInput\n");
}  //end FT_GetInput



#if Linux
FTWindow * GetwinFromX(Window xwin)
{
FTWindow *twin;

twin=PStatus.iwindow;

while(twin!=0) {
  if (twin->xwindow==xwin) return(twin);
  twin=twin->next;

}  //end while

//this X event is related to a window that is not part of freethought
return 0;
} //GetwinFromX
#endif //Linux

//Process XWindows events
//such as windows resize, keypress
//this function checks for twin->Keypress functions
#if Linux
void ProcessEvents(void)
#endif
#if WIN32
LRESULT CALLBACK ProcessEvents(HWND hwindow, UINT message, WPARAM wParam, LPARAM lParam)
#endif
{
FTWindow *twin;//,*twin2;
#if Linux
int result;
KeySym symbol;
XEvent xevent;
XSelectionEvent xev;
XComposeStatus status_in_out;
Atom type;
int count;
unsigned long len;
char conname[FTMedStr];
int FTControlResized;
FTControl *tcontrol; 
int format,numvisible;
unsigned char *data;
unsigned long bytes_left;
char buffer[80];
#endif
int dw,dh;
int num;
char winname[FTMedStr];

int buffer_size=80;

#if WIN32
long newtime;
char nchar;
unsigned char keyboard_state[256];
RECT *trect;
#endif
//should check that num is valid window

//twin=GetFTWindowN(num);


#if Linux
//if (twin!=0)
//{
//   if (XCheckWindowEvent(PStatus.xdisplay, twin->xwindow,xeventmask,&xevent))
XNextEvent(PStatus.xdisplay,&xevent); //blocks until event is received

if (PStatus.flags&PInfo) {
	if (xevent.type!=6) //mouse move
  	fprintf(stderr,"xevent.type=%d ",xevent.type);
}

//possibly this event may have a parent window - not related to freethought
twin=GetwinFromX(xevent.xany.window);
if (twin==0) {
  //fprintf(stdout,"X Event not related to a freethought window.\n");
}
#endif
#if WIN32
			twin=GetFTWindowFromhWnd(hwindow);
#endif

if (twin!=0) {
strcpy(winname,twin->name);
}

//GetwinFromX returns 0 is event is from a window other than a window in freethought
//fprintf(stderr,"twin=%p\n",twin);

//if this event is not related to a freethought window, ignore
//if (twin!=0) {


   if (twin!=0)  //can happen if a window was deleted, but events for that window are still happening
     num=twin->num;
   else
     num=0;

#if Linux
   //fprintf(stderr,"xevent.type=%d\n",xevent.type);
//put in alphabetical order
   switch (xevent.type)
#endif
#if WIN32
	 switch(message)
#endif
    {
#if Linux
     case ReparentNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nReparentNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

//       fprintf(stderr,"\nReparentNotify\n\n");
     break; 
     case ConfigureNotify:
       //this includes window move, resize 
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nConfigureNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
//       fprintf(stderr,"x=%d y=%d\n",xevent.xconfigure.x,xevent.xconfigure.y);
       //for some reason XGetGeometry dows not return window position
       if (twin!=0) {
         if (twin->x!=xevent.xconfigure.x || twin->y!=xevent.xconfigure.y) {
#if PINFO
           fprintf(stderr,"Move Window %s\n",twin->name);
#endif
           twin->x=xevent.xconfigure.x;  //move
           twin->y=xevent.xconfigure.y;  //move
           if (twin->OnMove!=0) twin->OnMove(twin);
         }

         if (twin->w!=xevent.xconfigure.width ||
             twin->h!=xevent.xconfigure.height) {

           dw=xevent.xconfigure.width-twin->w;
           dh=xevent.xconfigure.height-twin->h; 
#if PINFO
           fprintf(stderr,"resize %d,%d\n",dw,dh);
#endif
		   ResizeFTWindowControls(twin,xevent.xconfigure.x,xevent.xconfigure.y,dw,dh);

//       twin->w=xevent.xresizerequest.width;
       twin->w=xevent.xconfigure.width;
       twin->h=xevent.xconfigure.height; 

       //fprintf(stderr,"win now= %d,%d\n",twin->w,twin->h);

       //send old and new coords?
       if (twin->OnResize !=0) {
					(*twin->OnResize) (twin);
					twin=GetFTWindow(winname); //check to see if window was deleted
					if (twin==0) return; //user deleted window
				}


			
	

       //if we did not get this event request we would not have to redraw
       DrawFTWindow(twin);  //this draws FTControls too


             //resize
           }  //if w or h changed
       } //twin!=0
     break; 
     case PropertyNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nPropertyNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
				//Sometime a propertynotify can cause the window in focus to lose focus
/*
				//if this happens set the focus back
				if (twin!=0) {
					if (twin!=PStatus.FocusFTWindow && PStatus.FocusFTWindow!=0) {
						twin=PStatus.FocusFTWindow;
			      tcontrol=GetFTControlWithTab(twin,twin->focus);
fprintf(stderr,"Set focus5\n");
						if (tcontrol!=0) {
							FT_SetFocus(twin,tcontrol);
						} //tcontrol!=0
					} //twin!=PStatus
				} //twin!=0
*/
     break; 
#endif //Linux
#if WIN32
	 case WM_SIZING:  
		 
		trect=(RECT *)lParam;
		//adjust rect for titlebar
		//AdjustWindowRect(&trect,WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN,0); //0=window has no menu
		dw=trect->right-trect->left-twin->w;
		dh=trect->bottom-trect->top-twin->h; //
		dh-=twin->WinYOffset; //adjust for window titlebar height
		dw-=twin->WinXOffset; //adjust for window border


		if ((dw!=0 || dh!=0) && twin!=0)  {  //if width or height changed
			//in Window dh includes the titlebar etc.
			ResizeFTWindowControls(twin,0,0,dw,dh);  //currently 0,0 is passed as mouse x,y

			twin->w+=dw;
			twin->h+=dh; 
			//fprintf(stderr,"win now= %d,%d\n",twin->w,twin->h);
			//send old and new coords?
			if (twin->OnResize !=0) {
				(*twin->OnResize) (twin);
				twin=GetFTWindow(winname); //check to see if window was deleted
				if (twin==0) { //user deleted window
					//also let Windows process message
					return DefWindowProc(hwindow,message,wParam,lParam);
				}
			}
			//if we did not get this event request we would not have to redraw
			//need? DrawFTWindow(twin);  //this draws FTControls too
       }  //if (dw!=0 || dh!=0) { if w or h changed


		break;
	 case WM_MOVE:  
		 //update the windows position
		 if (twin != 0) {
			 twin->x = (int)(short)LOWORD(lParam) - twin->WinXOffset;   // horizontal position 
			 twin->y = (int)(short)HIWORD(lParam) - twin->WinYOffset;   // vertical position 
		 }//if (twin != 0) {
		break;
#endif //WIN32
#if Linux
     case DestroyNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nDestroyNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

//       fprintf(stderr,"DestroyNotify\n");
       //call DestroyNotify
       //exitprogram=1;
				//make this call any OnClose function if one exists
       if (twin!=0) {  //window was already closed
				//call any OnClose functions before draw
				//this misses if the twin is already 0
/*
//done in CloseFTWindow now
					if (twin->OnClose!=0) {
						(*twin->OnClose) (twin);
						twin=GetFTWindow(winname); //check to see if window was deleted
						if (twin==0) return; //user deleted window
					}
*/
/*
					//also OnDestroy
					//fprintf(stderr,"before DestroyNotify close window\n");
         if (twin->flags&WClose) {  //FileMenus do not get WClose
           if (num==0) PStatus.flags|=PExit;  //exit program - misses OnClose
           else {
							//CloseFTWindowNum(num); //destroys xwindow
							//CloseFTWindow(twin); //destroys xwindow and sets WClose bit
//							DestroyFTWindow(twin); //need to destroy instead of close - close keeps FT window structure			
//							twin=0;
						}
         }
*/
					//fprintf(stderr,"after DestroyNotify close window\n");
       } //twin!=0
     break;  //destroy notify
#endif //Linux
#if WIN32
		 case WM_DESTROY:
			if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nWM_DESTROY\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
			if (twin!=0) {  //window was already closed
				//call any OnClose functions before draw
				//this misses if the twin is already 0
/*
//done is CloseFTWindow now
				if (twin->OnClose!=0) {
					(*twin->OnClose) (twin);
					twin=GetFTWindow(winname); //check to see if window was deleted
					if (twin==0) {
						//also let Windows process message
						return(DefWindowProc(hwindow,message,wParam,lParam));
					}
				}
*/

				if (twin->flags&WOpen) {  //or else endless loop because closeFTWindow calls DestroyWindow which gets here again
					CloseFTWindow(twin);
				}

		   } //twin!=0


			if (PStatus.iwindow->hwindow==hwindow) {  //closing main window ends freethought program
				PostQuitMessage(0);
				//PStatus.flags|=PExit;  //user may want to keep program alive with no windows
			 }
     break;
#endif
#if Linux
     case ClientMessage:   //click on x to destroy window
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nClientMessage\n");
					if (twin!=0) {
						fprintf(stderr,"in %s\n",twin->name);
					}
				}

       //fprintf(stderr,"Client Message\n");
				if(xevent.xclient.message_type == XA_WM_PROTOCOLS) { //XA_WM_PROTOCOLS
					if ((unsigned int)xevent.xclient.data.l[0] == XA_WM_DELETE_WINDOW)	{
	           //fprintf(stderr,"delete window\n");
  		       if (twin!=0) {
  		         if (twin->flags&WMsgBox) PStatus.mb=FTMB_CANCEL; //end FTMessageBox while loop
  		       }

//  		       CloseFTWindowNum(num);//XDestroyWindow(PStatus.xdisplay,twin->xwindow);
							if (twin->flags&WOpen) {  //or else endless loop because closeFTWindow calls DestroyWindow which gets here again
								CloseFTWindow(twin);
							}
					} //XA_WM_DELETE_WINDOW

//This issue of a program taking focus raises two different views- 1) let programs raise windows, 2) program should not be able to raise windows - only people should be able to
//I am for 1) because people could just keep creating new windows to be the window on top
//this can be configured by alt-f3 focus stealing prevention level: None
//possibly send buttondown message?
//This issue is addressed at: http://lists.freedesktop.org/archives/xorg/2006-january/012276.html
/*
					if (xevent.xclient.data.l[0] == XA_WM_TAKE_FOCUS)	{
	           fprintf(stderr,"WM_TAKE_FOCUS\n");
  		       if (twin!=0) {
								if (PStatus.FocusFTWindow!=twin && PStatus.FocusFTWindow!=0) {
									XSetInputFocus(PStatus.xdisplay,PStatus.FocusFTWindow->xwindow, RevertToParent,CurrentTime);
									XRaiseWindow(PStatus.xdisplay,PStatus.FocusFTWindow->xwindow);
									XSync(PStatus.xdisplay,0);
								} //PStatus.Focus
  		       } //twin!=0


//  		       CloseFTWindowNum(num);//XDestroyWindow(PStatus.xdisplay,twin->xwindow);
								//FT_SetFocus();
					} //XA_WM_TAKE_FOCUS
*/
       } //XA_WM_PROTOCOLS
     break;


     case SelectionNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nSelectionNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

/*
This event is generated by the X server in response to a ConvertSelection protocol request when there is no owner for the selection. When there is an owner, it should be generated by the owner of the selection by using XSendEvent(). The owner of a selection should send this event to a requestor when a selection has been converted and stored as a property or when a selection conversion could not be performed (which is indicated by setting the property member to None).

If None is specified as the property in the ConvertSelection protocol request, the owner should choose a property name, store the result as that property on the requestor window, and then send a SelectionNotify giving that actual property name.
*/

  /*
       fprintf(stderr,"Selection Notify\n");
       fprintf(stderr,"twin=%x\n",(unsigned int)twin->xwindow);
       fprintf(stderr,"Requester=%x selection=%d target=%d property=%d\n",(unsigned int)xevent.xselection.requestor,(int)xevent.xselection.selection,(int)xevent.xselection.target,(int)xevent.xselection.property);
       fprintf(stderr,"target=%s\n",XGetAtomName(PStatus.xdisplay,xevent.xselection.target));
       fprintf(stderr,"property=%s\n",XGetAtomName(PStatus.xdisplay,xevent.xselection.property));
*/

        //indicate to this program that the SelectionNotify was received
        PStatus.flags|=PGotSelection;


     break;

     case SelectionRequest:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nSelectionRequest\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

/*
The X server reports SelectionRequest events to the owner of a selection. The X server generates this event whenever a client requests a selection conversion by calling XConvertSelection() for the owned selection.
*/
//Somebody wants our data



/*
       fprintf(stderr,"Selection Request\n");
       fprintf(stderr,"twin=%x\n",(unsigned int)twin->xwindow);
       fprintf(stderr,"Requester=%x selection=%d target=%d property=%d\n",(unsigned int)xevent.xselectionrequest.requestor,(int)xevent.xselectionrequest.selection,(int)xevent.xselectionrequest.target,(int)xevent.xselectionrequest.property);
       fprintf(stderr,"target=%s\n",XGetAtomName(PStatus.xdisplay,xevent.xselectionrequest.target));
       fprintf(stderr,"property=%s\n",XGetAtomName(PStatus.xdisplay,xevent.xselectionrequest.property));
*/

//       test=malloc(100);
//       strcpy(test,"This is a test.");
			if (twin!=0) {
					XGetWindowProperty(PStatus.xdisplay,twin->xwindow,XA_PRIMARY,0,10000000L,0,XA_STRING,&type,&format,&len,&bytes_left,&data);				
//fprintf(stderr,"twin primary type=%d format=%d len=%d data=%s\n",(int)type,format,(int)len,data);
				} //twin!=0

	      //Try to change the property
        //Put the XA_PRIMARY string into the requested property of requesting window
	      result=XChangeProperty(PStatus.xdisplay,xevent.xselectionrequest.requestor,xevent.xselectionrequest.property,xevent.xselectionrequest.target,8,PropModeReplace,
					 data,len);
	      if (result == BadAlloc
           || result == BadAtom || result == BadMatch
           || result == BadValue || result == BadWindow)
          {
		      fprintf(stderr,"XChangeProperty failed %d\n",result);
          }

//    free(test);

	  //make SelectionNotify event
	  xev.type = SelectionNotify;
	  xev.send_event = True;
	  xev.display = PStatus.xdisplay;
	  xev.requestor = xevent.xselectionrequest.requestor;
	  xev.selection = xevent.xselectionrequest.selection;
	  xev.target = xevent.xselectionrequest.target;
	  xev.property = xevent.xselectionrequest.property;
	  xev.time = xevent.xselectionrequest.time;



	  //Send message to requesting window that operation is done
	  result = XSendEvent (PStatus.xdisplay,xev.requestor,0,0L,(XEvent *) &xev);
	  if (result==BadValue || result==BadWindow)
	      fprintf(stderr,"send SelectionRequest failed\n");



			if (data!=0) {
				XFree(data);
				data=0;
			}
     break;
	case SelectionClear:       
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nSelectionClear\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

//fprintf(stderr,"Selection clear\n");
     break;
   case Expose: //resize, gotfocus
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nExpose\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

//     fprintf(stderr,"lastexpose=%p\n",PStatus.lastexpose);

			//way to pass redraw window to Xwindows?
			//this causes flicker
      if (xevent.xexpose.count==0 && twin!=0) {
			//if (twin!=0) {
					
	     	//fprintf(stderr,"lastexpose=%p\n",PStatus.lastexpose);
				//do not redraw for exposes that take place in milliseconds - lastexpose isn't implemented yet
				//fprintf(stderr,"Expose count=%d\n",xevent.xexpose.count);
        DrawFTWindow(twin);


				//twin->flags&=~WWaitUntilDoneDrawing; //Window is done drawing now
			}


/*
     if (PStatus.ctime->tm_sec==PStatus.secs) {
        fprintf(stderr,"secs=%d %d\n",PStatus.secs,PStatus.ctime->tm_sec);
        fprintf(stderr,"ctime=%p\n",PStatus.ctime);
        PStatus.secs=PStatus.ctime->tm_sec+5;
  
     }
*/
//     if (xevent.xexpose.count==0)
//       {
       //UpdateScreen(0);
       //XFlush (PStatus.xdisplay);
//       if (xevent.xexpose.count > 0)
//          break;
//      fprintf(stderr,"expose=%d\n",xevent.xexpose.count);
//        }
     break;
#endif //Linux

#if Linux
   case ButtonPress:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nButton Press\n");
					if (twin!=0) {
						fprintf(stderr,"in %s\n",twin->name);
					}
				}

/*
//set shift, control, or alt if key is pressed
		    if (xevent.xbutton.state&ShiftMask) {
		      PStatus.flags|=PShift;
				}  else {
					PStatus.flags&=~PShift;
				}
        if (xevent.xbutton.state & ControlMask) {
         PStatus.flags|=PControl;
         } else {
					PStatus.flags&=~PControl;
				}
       if (xevent.xbutton.state & Mod1Mask) {
         PStatus.flags|=PAlt;
        } else {
					PStatus.flags&=~PAlt;
				}
*/


  //   fprintf(stderr,"mouse button %d press\n ",xevent.xbutton.button);
     switch(xevent.xbutton.button)
       {
       case Button1:
				//twin might=0
				 if (twin!=0) {PStatus.ButtonFTWindow[0]=twin;}
         PStatus.ButtonX[0]=xevent.xbutton.x;
         PStatus.ButtonY[0]=xevent.xbutton.y;

//Need to wait to see if this will be a double-click

  //       fprintf(stderr,"button 1 press (%d,%d)\n ",xevent.xbutton.x,xevent.xbutton.y);

//problem of:
// 1st click of double-click will cause ButtonDown events
//perhaps wait until first click can not be double-click
//before calling ButtonDown events

//q: call double-click functions on second button down of double-click, or second button-up of double-click?
//gedit does double-click on ButtonDown
					if (twin!=0) {
		
//	           fprintf(stderr,"lastclick=%li delay=%d time=%d\n",twin->lastclick[0],PStatus.dclickdelay,(int)xevent.xbutton.time);
		       if (twin->lastclick[0]!=0 && (twin->lastclick[0]+PStatus.dclickdelay)>xevent.xbutton.time) {
//	           fprintf(stderr,"double-dn click\n ");
	           twin->lastclick[0]=xevent.xbutton.time;
		         CheckFTWindowButtonDoubleClick(twin,xevent.xbutton.x,xevent.xbutton.y,0);
		         }
		       else {
	//           fprintf(stderr,"single-dn click\n ");
	           twin->lastclick[0]=xevent.xbutton.time;
		         CheckFTWindowButtonDown(twin,xevent.xbutton.x,xevent.xbutton.y,0);
		       }

				} //twin!=0

        break;
	case Button2:

         if (twin!=0) {PStatus.ButtonFTWindow[1]=twin;}
         PStatus.ButtonX[1]=xevent.xbutton.x;
         PStatus.ButtonY[1]=xevent.xbutton.y;

				if (twin!=0) {
         if (twin->lastclick[1]!=0 && (twin->lastclick[1]+PStatus.dclickdelay)>xevent.xbutton.time) {
//           fprintf(stderr,"double-dn click\n ");
           CheckFTWindowButtonDoubleClick(twin,xevent.xbutton.x,xevent.xbutton.y,1);
           }
         else {
//#if PINFO
//           fprintf(stderr,"single-dn button 2 click\n ");
//#endif
           CheckFTWindowButtonDown(twin,xevent.xbutton.x,xevent.xbutton.y,1);
         }
				} //twin!=0
        break;
	case Button3:  //also button2 on 2 button mouse

//	fprintf(stderr,"button 2 press (%d,%d)\n ",xevent.xbutton.x,xevent.xbutton.y);
				
					if (twin!=0) { PStatus.ButtonFTWindow[2]=twin;}
         PStatus.ButtonX[2]=xevent.xbutton.x;
         PStatus.ButtonY[2]=xevent.xbutton.y;

					if (twin!=0) {
		       if (twin->lastclick[2]!=0 && (twin->lastclick[2]+PStatus.dclickdelay)>xevent.xbutton.time) {
	//           fprintf(stderr,"double-dn click\n ");
		         CheckFTWindowButtonDoubleClick(twin,xevent.xbutton.x,xevent.xbutton.y,2);
		         }
		       else {
//	#if PINFO
//		         fprintf(stderr,"single-dn button 3 click\n ");
//	#endif
		         CheckFTWindowButtonDown(twin,xevent.xbutton.x,xevent.xbutton.y,2);
		       }
				} //twin!=0
	break;
	case Button4:  //mouse wheel up

				if (twin!=0) { 
					PStatus.ButtonFTWindow[3]=twin;
				}
         PStatus.ButtonX[3]=xevent.xbutton.x;
         PStatus.ButtonY[3]=xevent.xbutton.y;

					if (twin!=0) {

		 //is mouse wheel movement outside of this window recorded?
//		 PStatus.LastMouseX=PStatus.MouseX;
//		 PStatus.LastMouseY=PStatus.MouseY;
//		 PStatus.MouseX=GET_X_LPARAM(lParam);
//		 PStatus.MouseY=GET_Y_LPARAM(lParam);;
		 PStatus.MouseVWheelDelta=1;//GET_WHEEL_DELTA_WPARAM(wParam);
		 //PStatus.MouseXRoot=xevent.xmotion.x_root;
		 //PStatus.MouseYRoot=xevent.xmotion.y_root;

	//     fprintf(stderr,"x:%d  y:%d\n",PStatus.MouseX,PStatus.MouseY);
//fprintf(stderr,"x:%d  y:%d\n",PStatus.ButtonX[4],PStatus.ButtonY[4]);
			CheckFTWindowMouseVWheelMove(twin,PStatus.ButtonX[3],PStatus.ButtonY[3],PStatus.MouseVWheelDelta);
//CheckFTWindowMouseVWheelMove(twin,PStatus.MouseX,PStatus.MouseY,PStatus.MouseVWheelDelta);
//			twin=GetFTWindow(winname); //check to see if window was deleted
	//if (twin==0) {
				//return;  //user could have deleted window in CheckFTButtonUp
			//}
		} //if (twin!=0)

	break; //Button4
	case Button5:  //mouse wheel down
				if (twin!=0) { 
					PStatus.ButtonFTWindow[4]=twin;
				}
         PStatus.ButtonX[4]=xevent.xbutton.x;
         PStatus.ButtonY[4]=xevent.xbutton.y;

					if (twin!=0) {

		 //is mouse wheel movement outside of this window recorded?
//		 PStatus.LastMouseX=PStatus.MouseX;
//		 PStatus.LastMouseY=PStatus.MouseY;
//		 PStatus.MouseX=GET_X_LPARAM(lParam);
//		 PStatus.MouseY=GET_Y_LPARAM(lParam);;
		 PStatus.MouseVWheelDelta=-1;//GET_WHEEL_DELTA_WPARAM(wParam);
		 //PStatus.MouseXRoot=xevent.xmotion.x_root;
		 //PStatus.MouseYRoot=xevent.xmotion.y_root;

	//     fprintf(stderr,"x:%d  y:%d\n",PStatus.MouseX,PStatus.MouseY);
//fprintf(stderr,"x:%d  y:%d\n",PStatus.ButtonX[5],PStatus.ButtonY[5]);
			CheckFTWindowMouseVWheelMove(twin,PStatus.ButtonX[4],PStatus.ButtonY[4],PStatus.MouseVWheelDelta);
//CheckFTWindowMouseVWheelMove(twin,PStatus.MouseX,PStatus.MouseY,PStatus.MouseVWheelDelta);
//			twin=GetFTWindow(winname); //check to see if window was deleted
	//if (twin==0) {
				//return;  //user could have deleted window in CheckFTButtonUp
			//}
		} //if (twin!=0)


	break; //Button5

	}//end switch
     break;  //end ButtonPress
#endif //Linux
#if WIN32
		 		case WM_LBUTTONDOWN:
					if (PStatus.flags&PInfo) {
						fprintf(stderr,"\nLeft Button Down\n");
						if (twin!=0) {
							fprintf(stderr,"in %s\n",twin->name);
						}
					} //if (PStatus.flags&PInfo) {
					if (twin!=0) {
						PStatus.ButtonFTWindow[0]=twin;
					}
					PStatus.ButtonX[0]=GET_X_LPARAM(lParam); 
					PStatus.ButtonY[0]=GET_Y_LPARAM(lParam); 
					gettimeofday(&PStatus.utime,NULL); // PStatus.utime is not updating fast enough
					newtime=PStatus.utime.tv_sec*1000+PStatus.utime.tv_usec/1000;  //in ms -
					//In Xwindows need to wait to see if this will be a double-click
					//but the window manager in Windows specifically detects double-click with WM_LBUTTONDBLCLK
					//although possibly I could

					//In XWindows - the time parameter is set to the time (in ms) when the click occured (starting when?)

					if (twin!=0) {		
						//	           fprintf(stderr,"lastclick=%li delay=%d time=%d\n",twin->lastclick[0],PStatus.dclickdelay,(int)xevent.xbutton.time);
						//if (twin->lastclick[0]!=0 && (twin->lastclick[0]+PStatus.dclickdelay)>(int)xevent.xbutton.time) {
						//*1000 because in Windows click time is in us
						if (twin->lastclick[0]!=0 && (twin->lastclick[0]+PStatus.dclickdelay)>newtime) {
						//	           fprintf(stderr,"button down is double-click\n ");
						//twin->lastclick[0]=newtime;//  xevent.xbutton.time;
						CheckFTWindowButtonDoubleClick(twin,PStatus.ButtonX[0],PStatus.ButtonY[0],0);
					} else {
						//           fprintf(stderr,"single button down\n ");
						//twin->lastclick[0]=newtime;//xevent.xbutton.time;
						CheckFTWindowButtonDown(twin,PStatus.ButtonX[0],PStatus.ButtonY[0],0);
					}
				} //twin!=0
				twin->lastclick[0]=newtime;//xevent.xbutton.time;
				break; //WM_LBUTTONDOWN:
				//case WM_LBUTTONDBLCLK:
				//CheckFTWindowButtonDoubleClick(twin,xevent.xbutton.x,xevent.xbutton.y,0);
				//break;
		 		case WM_RBUTTONDOWN:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nRight Button Down\n");
					if (twin!=0) {
						fprintf(stderr,"in %s\n",twin->name);
					}
				} //if (PStatus.flags&PInfo) {
				 if (twin!=0) {PStatus.ButtonFTWindow[2]=twin;}
					 PStatus.ButtonX[2]=GET_X_LPARAM(lParam); 
					 PStatus.ButtonY[2]=GET_Y_LPARAM(lParam); 
					 //newtime=PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec;
					 gettimeofday(&PStatus.utime,NULL); // PStatus.utime is not updating fast enough
					 newtime=PStatus.utime.tv_sec*1000+PStatus.utime.tv_usec/1000;  //in ms					
//In Xwindows need to wait to see if this will be a double-click
				 //but the window manager in Windows specifically detects double-click with WM_LBUTTONDBLCLK
				 //although possibly I could

				 //In XWindows - the time parameter is set to the time (in ms) when the click occured (starting when?)

					if (twin!=0) {		
//	           fprintf(stderr,"lastclick=%li delay=%d time=%d\n",twin->lastclick[0],PStatus.dclickdelay,(int)xevent.xbutton.time);
		       //if (twin->lastclick[0]!=0 && (twin->lastclick[0]+PStatus.dclickdelay)>(int)xevent.xbutton.time) {
						if (twin->lastclick[2]!=0 && (twin->lastclick[2]+PStatus.dclickdelay)>newtime) {
//	           fprintf(stderr,"double-dn click\n ");
							//twin->lastclick[2]=newtime;//  xevent.xbutton.time;
							CheckFTWindowButtonDoubleClick(twin,PStatus.ButtonX[2],PStatus.ButtonY[2],0);
						 } else {
			//           fprintf(stderr,"single-dn click\n ");
							//twin->lastclick[2]=newtime;//xevent.xbutton.time;
							CheckFTWindowButtonDown(twin,PStatus.ButtonX[2],PStatus.ButtonY[2],0);
						}
				} //twin!=0
				twin->lastclick[2]=newtime;//  xevent.xbutton.time;
				break;  //WM_RBUTTONDOWN:

#endif //WIN32

#if Linux
   case ButtonRelease:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nButtonRelease\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

     //fprintf(stderr,"mouse button %d press\n ",xevent.xbutton.button);
     switch(xevent.xbutton.button)
       {
       case Button1:
//         stoprecord=1;
//#if PINFO
 //        fprintf(stderr,"button 1 time = %li\n ",xevent.xbutton.time);
//#endif
         //fprintf(stderr,"button 1 time = %li\n ",xevent.xbutton.time);
         //fprintf(stderr,"button 1 time = %li\n ",twin->lastclick[0]);
  //       fprintf(stderr,"button 1 release (%d,%d)\n ",xevent.xbutton.x,xevent.xbutton.y);
         //check if any FTControls had ButtonUp events
//         CheckFTWindowButtonUp(num,xevent.xbutton.x,xevent.xbutton.y);
//         if (twin->lastclick[0]!=0 && twin->lastclick[0]+PStatus.dclickdelay>xevent.xbutton.time) {

//         if (twin->lastclick[0]!=0 && (twin->lastclick[0]+PStatus.dclickdelay)>xevent.xbutton.time) {
//           fprintf(stderr,"double-up click\n ");
//           CheckFTWindowButtonDoubleClick(twin,xevent.xbutton.x,xevent.xbutton.y);
//           }
//         else {
//           fprintf(stderr,"single-up click\n ");
					if (twin!=0) {  //CheckFTWindowButtonUp if twin=0?
           CheckFTWindowButtonUp(twin,xevent.xbutton.x,xevent.xbutton.y,0);

	         if (twin!=0) { //user could have deleted window in CheckFTButtonUp
						twin->lastclick[0]=xevent.xbutton.time;
						} //twin!=0
					} //twin!=0

       break;
	case Button2:  //is mouse wheel button press
		if (twin!=0) {
			CheckFTWindowButtonUp(twin,xevent.xbutton.x,xevent.xbutton.y,1);
			if (twin!=0) { //user could have deleted window in CheckFTButtonUp
				twin->lastclick[1]=xevent.xbutton.time;
			} //twin!=0
		}
       break;
	case Button3:  //right click
		if (twin!=0) {
			CheckFTWindowButtonUp(twin,xevent.xbutton.x,xevent.xbutton.y,2);
			if (twin!=0) { //user could have deleted window in CheckFTButtonUp
				twin->lastclick[2]=xevent.xbutton.time;
			}
		}
	break;
	}//end switch
     break;  //end ButtonPress
#endif //Linux
#if WIN32
	case WM_LBUTTONUP:
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"\nLeft Button Up\n");
			if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
		}

    //PStatus.ButtonX[0]=GET_X_LPARAM(lParam); 
    //PStatus.ButtonY[0]=GET_Y_LPARAM(lParam); 


		if (twin!=0) {  //CheckFTWindowButtonUp if twin=0?
			CheckFTWindowButtonUp(twin,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),0);
			 
			if (twin!=0) { //user could have deleted window in CheckFTButtonUp
				//twin->lastclick[0]=PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec;//xevent.xbutton.time;
				gettimeofday(&PStatus.utime,NULL); // PStatus.utime is not updating fast enough
				twin->lastclick[0]=PStatus.utime.tv_sec*1000+PStatus.utime.tv_usec/1000;//xevent.xbutton.time;
				} //twin!=0
		} //twin!=0
	break; //WM_LBUTTONUP
	case WM_RBUTTONUP:
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"\nRight Button Up\n");
			if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
		}

    //PStatus.ButtonX[0]=GET_X_LPARAM(lParam); 
    //PStatus.ButtonY[0]=GET_Y_LPARAM(lParam); 


		if (twin!=0) {  //CheckFTWindowButtonUp if twin=0?
	      CheckFTWindowButtonUp(twin,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),2);
			 
	    if (twin!=0) { //user could have deleted window in CheckFTButtonUp
			//twin->lastclick[2]=PStatus.utime.tv_sec*1000000+PStatus.utime.tv_usec;//xevent.xbutton.time;
			gettimeofday(&PStatus.utime,NULL); // PStatus.utime is not updating fast enough
			twin->lastclick[2]=PStatus.utime.tv_sec*1000+PStatus.utime.tv_usec/1000;//xevent.xbutton.time;
			} //twin!=0
		} //twin!=0
	break; //WM_RBUTTONUP

#endif
#if Linux
   case KeyPress:
			//see /usr/include/X11/keysymdef.h
			//see XmbLookupString, Xutf8LookupString for multibyte key translation
      //symbol=XLookupKeysym(&xevent.xkey,0);
			//this does not work for capital/shifted/caplock letters/numbers for the KeySym
			//(ISO/IEC 8859-1 = Unicode U+0020..U+00FF)
			count=XLookupString(&xevent.xkey,buffer,buffer_size,&symbol,&status_in_out);
			//if ((int)symbol==0) { //cntrl-c count>0 but symbol=0
			//	fprintf(stderr,"Symbol==0 \n");
				//count=XLookupString(&xevent.xkey[count],buffer,buffer_size,&symbol,&status_in_out);
			//	symbol=XLookupKeysym(&xevent.xkey,0);
				//symbol=XKeycodeToKeysym(PStatus.xdisplay,xevent.xkey.keycode,0);
			//}  //PStatus.key==0

			PStatus.key=symbol;
			PStatus.asckey=buffer[0];

			//this is one way to make the keysym have the correct capitalization
			//but this does not work for number lock, or exclamation point, etc
			//but this erases arrow keys etc.  
//			if (symbol>=XK_space && symbol<=XK_asciitilde) { //ascii key
			//count==1 when a control key is pressed
			if (count==0 && ((symbol>=XK_0 && symbol<=XK_9) || (symbol>=XK_A && symbol<=XK_Z) || (symbol>=XK_a && symbol<=XK_z))) { //ascii key 0-9 A-Z a-z 
				PStatus.key=XStringToKeysym(buffer);
			}
      //fprintf(stderr,"count=%d key=%x asckey=%c  buffer=%s buffer[count]=%d keycode=%d symbol=%d\n",count,(int)PStatus.key,PStatus.asckey,buffer,buffer[count],xevent.xkey.keycode,(int)symbol);

			//symbol has the code listed in /usr/include/X11/keysymdef.h - although it does not understand that the shift key is pressed and so 0x20 needs to be subtracted for capital letters

      //symbol = XKeycodeToKeysym(PStatus.xdisplay, xevent.xkey.keycode, 0);
//      string = XKeysymToString(symbol);

//      PStatus.key=mapkey[xevent.xkey.keycode];
//      PStatus.key=symbol;//mapkey[xevent.xkey.keycode];

 //     fprintf(stderr,"KeyPress xevent.xkey=%x PStatus.key.keycode=%x symbol=%x",xevent.xkey.keycode,(int)PStatus.key,(int)symbol);
/*
      if (IsModifierKey(symbol))
        fprintf(stderr,"modifier=%s",string);
      else
        fprintf(stderr,"key=%s",string);
*/

//      PStatus.key=symbol;
      //PStatus.key=xevent.xkey.keycode;
      //PStatus.key=xevent.xkey.keycode;

      //PStatus.key=XKeysymToKeycode(PStatus.xdisplay, symbol);
      //PStatus.key=XLookupKeysym(&xevent.xkey,0);
      //Keypress=XKeysymToString(PStatus.key);
//mask for alt?
//todo: determine constant variable names for various symbol keys
			if ((xevent.xkey.state&ShiftMask) || symbol==XK_Shift_L || symbol==XK_Shift_R) {
#if PINFO
        printf("Masked with shift\n");
#endif
        PStatus.flags|=PShift;
			} else {
        PStatus.flags&=~PShift;
				if (xevent.xkey.state & LockMask) {
  	      //printf("Masked with a lock\n");
				} else { //end if caps lock
       //not caps lock
          //numlock
          if (xevent.xkey.state&NumLockMask) {
            PStatus.flags|=PNumLock;
            //if (xevent.xkey.keycode>=0x4f && xevent.xkey.keycode<=0x5b)  //Numpad 
            //    PStatus.key=mapkey[xevent.xkey.keycode+0x80];
          } else {
						PStatus.flags&=~PNumLock;
					}
          //scroll lock
          if (xevent.xkey.state&ScrollLockMask) {
            PStatus.flags|=PScrollLock;
          } else {
						PStatus.flags&=~PScrollLock;
					}
				} //			if (xevent.xkey.state & LockMask) {
				}	//		if (xevent.xkey.state&ShiftMask) {

        if ((xevent.xkey.state&ControlMask) || symbol==XK_Control_L || symbol==XK_Control_R) {
//         PStatus.key|=FTControl_KEY;
         PStatus.flags|=PControl;
#if PINFO
         printf("Masked with PControl\n");
#endif
         } else {
						PStatus.flags&=~PControl;
				}
       if ((xevent.xkey.state&Mod1Mask) || symbol==XK_Shift_L || symbol==XK_Shift_R) {
//         PStatus.key|=ALT_KEY;
         PStatus.flags|=PAlt;
#if PINFO
         printf("Masked with Alt (Mod1Mask)\n");
#endif
        } else {
					PStatus.flags&=~PAlt;
				}      

//   fprintf(stderr,"key to Keypress=%c",PStatus.key);

		if (twin!=0) {
		//check for FTWindow (and FTControl) Keypress functions
	    CheckFTWindowKeypress(twin,PStatus.key);
 		}
	   //fprintf(stderr,"after  CheckFTWindowkey=%d",PStatus.key);
     break;  //KeyPress
#endif //Linux
#if WIN32
		 //in Windows KEYUP messages are keypress events and do not track the case of the key
		 //case is captured in WM_CHAR events

		//arrow keys
		case WM_SYSKEYDOWN:  //alt,shift,control keys, F10 (but not other function keys)
			switch(wParam) {
				case VK_LMENU:
				case VK_RMENU:
				case VK_MENU:
					PStatus.flags|=PAlt;
				break;
				case VK_LCONTROL:
				case VK_RCONTROL:
				case VK_CONTROL:
					PStatus.flags|=PControl;
				break;
				case VK_LSHIFT:
				case VK_RSHIFT:
				case VK_SHIFT:
					PStatus.flags|=PShift;
				break;
			} //switch

			PStatus.key=wParam;
			PStatus.asckey=MapVirtualKey(wParam,MAPVK_VK_TO_CHAR);

			if (twin!=0) {
		//check for FTWindow (and FTControl) Keypress functions
	    CheckFTWindowKeypress(twin,PStatus.key);
 		}


		break; //WM_SYSKEYDOWN

		case WM_KEYDOWN:  //captures arrow keys
			switch(wParam) {
				case VK_LMENU:
				case VK_RMENU:
				case VK_MENU:
					PStatus.flags|=PAlt;
				break;
				case VK_LCONTROL:
				case VK_RCONTROL:
				case VK_CONTROL:
					PStatus.flags|=PControl;
				break;
				case VK_LSHIFT:
				case VK_RSHIFT:
				case VK_SHIFT:
					PStatus.flags|=PShift;
				break;
				default:  //some other key
				PStatus.key=wParam;
				PStatus.asckey=MapVirtualKey(wParam,MAPVK_VK_TO_CHAR);


				GetKeyboardState(keyboard_state);
				nchar=0;
				ToAscii(wParam,lParam,keyboard_state,(LPWORD)&nchar,0); //possible error because LPWORD is a long pointer to a word (16-bits)

				//set numlock flag if on or clear flag if off
				if (keyboard_state[VK_NUMLOCK]) {
					PStatus.flags|=PNumLock;
				} else {
					PStatus.flags&=~PNumLock;
				}

				//for letters, numbers and symbols use the ascii character
				if (nchar!=0) {
					//PStatus.key should be the physical key code- not necessarily an ascii code- for example VK_NUMPAD5 not '5'
					//PStatus.key=nchar;
					PStatus.asckey=nchar;
				} else {  //not a character
					//for Windows I had to specially map some characters/keys, for example VK_NEXT (pgdn)
					//because they have the same VK code as the ascii code of a different character (VK_NEXT=0x21='!' character)

					switch(wParam) {
					case VK_PRIOR: //=33 (0x21)
						PStatus.asckey=XK_Page_Up;
						PStatus.key=XK_Page_Up;
					break;
					case VK_NEXT:
						PStatus.asckey=XK_Page_Down;
						PStatus.key=XK_Page_Down;
					break;
					case VK_UP:
						PStatus.asckey=XK_Up;
						PStatus.key=XK_Up;
					break;
					case VK_DOWN:
						PStatus.asckey=XK_Down;
						PStatus.key=XK_Down;
					break;
					case VK_LEFT:
						PStatus.asckey=XK_Left;
						PStatus.key=XK_Left;
					break;
					case VK_RIGHT:
						PStatus.asckey=XK_Right;
						PStatus.key=XK_Right;
					break;
					case VK_HOME:
						PStatus.asckey=XK_Home;
						PStatus.key=XK_Home;
					break;
					case VK_END:
						PStatus.asckey=XK_End;
						PStatus.key=XK_End;
					break;
					case VK_DELETE:
						PStatus.asckey=XK_Delete;
						PStatus.key=XK_Delete;
					break;
					case VK_INSERT:
						PStatus.asckey=XK_Insert;
						PStatus.key=XK_Insert;
					break;
/*
					case VK_NUMPAD2:
						if (!(PStatus.flags&PNumLock) {
							PStatus.key=XK_KP_Down; //remap to unique key so users don't have to check for NumLock
						}
					break;
					case VK_NUMPAD4:
						PStatus.key=XK_KP_Left; //remap to unique key so users don't have to check for NumLock
					break;
					case VK_NUMPAD6:
						PStatus.key=XK_KP_Right; //remap to unique key so users don't have to check for NumLock
					break;
					case VK_NUMPAD8:
						PStatus.key=XK_KP_Up; //remap to unique key so users don't have to check for NumLock
					break;
*/
					} //switch(PStatus.asckey) {

				} //nchar!=0

/*
				//for keys different characters depending on the shift, alt and control key
				//we need to determine the correct character
				//there is WM_CHAR - but that misses arrow keys, pgup, etc - any non-letter, number, or symbol (!@#,etc)
				if (PStatus.flags&PShift) {
				//upper case
				PStatus.key=wParam;
				PStatus.asckey=MapVirtualKey(wParam,MAPVK_VK_TO_CHAR);
			} else {
				//lower case
				PStatus.key=wParam;//+0x20;
				PStatus.asckey=MapVirtualKey(wParam,MAPVK_VK_TO_CHAR);//+0x20;
			}
*/

			if (twin!=0) {
		//check for FTWindow (and FTControl) Keypress functions
	    CheckFTWindowKeypress(twin,PStatus.key);
 		}

		break; //some other key
			} //switch

		break;  //WM_KEYDOWN
		//case WM_SYSCHAR:
		//case WM_CHAR:  //to determine if key is lowercase

#endif //WIN32
#if Linux
   case KeyRelease:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nKeyRelease\n");
					if (twin!=0) {
						fprintf(stderr,"in %s\n",twin->name);
					}
				}

//      PStatus.key=XLookupKeysym(&xevent.xkey,0);
//      fprintf(stderr,"KeyRelease xevent.xkey.keycode=%x PStatus.key=%x",xevent.xkey.keycode,(int)PStatus.key);
//XConvertCase(Keypress, lower_return, upper_return)

			count=XLookupString(&xevent.xkey,buffer,buffer_size,&symbol,&status_in_out);
			//if ((int)symbol==0) { //cntrl-c count>0 but symbol=0
			//	fprintf(stderr,"Symbol==0 \n");
				//count=XLookupString(&xevent.xkey[count],buffer,buffer_size,&symbol,&status_in_out);
			//	symbol=XLookupKeysym(&xevent.xkey,0);
				//symbol=XKeycodeToKeysym(PStatus.xdisplay,xevent.xkey.keycode,0);
			//}  //PStatus.key==0

			PStatus.key=symbol;
			PStatus.asckey=buffer[0];




    switch (PStatus.key)
      {
//      case -28:  //cntrl_r
//      case -29:  //cntrl_l
//      case -30:
//      case -31:  //ignore shift keys
//      case XK_Control_R:
//      case XK_Control_L:
			case XK_Control_L:
			case XK_Control_R:
        PStatus.flags&=~PControl;
//	fprintf(stderr,"FTControl up\n");
      break;
//      case XK_Alt_R:
//      case XK_Alt_L:
			case XK_Alt_L:
			case XK_Alt_R:
        PStatus.flags&=~PAlt;
//	fprintf(stderr,"FTControl up\n");
      break;
//      case XK_Shift_R:
//      case XK_Shift_L: 
			case XK_Shift_L:
			case XK_Shift_R:
        PStatus.flags&=~PShift;
//	fprintf(stderr,"shift up\n");
        break;
      default:		/*alpha or other symbol*/
//do something here?  keyup
//keyup functions
        //check window Keypress function
//        if (twin->Keypress>0) (*twin->Keypress) (Keypress);
	//and check for FTControl Keypress functions
  //      CheckFTWindowKeypress(num,Keypress);
	break;
      }                            /*end switch on Keypress*/
     break;  //end keyrelease
#endif //Linux
#if WIN32
		case WM_SYSKEYUP:  //alt,shift,control keys
			switch(wParam) {
				case VK_LMENU:
				case VK_RMENU:
				case VK_MENU:
					PStatus.flags&=~PAlt;
				break;
				case VK_LCONTROL:
				case VK_RCONTROL:
				case VK_CONTROL:
					PStatus.flags&=~PControl;
				break;
				case VK_LSHIFT:
				case VK_RSHIFT:
				case VK_SHIFT:
					PStatus.flags&=~PShift;
				break;
			} //switch
		break; //WM_SYSKEYUP
	 case WM_KEYUP:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nWM_KEYUP event\n");

					if (twin!=0) {
						fprintf(stderr,"in %s\n",twin->name);
					}
				}


			switch(wParam) {
				case VK_LMENU:
				case VK_RMENU:
				case VK_MENU:
					PStatus.flags&=~PAlt;
				break;
				case VK_LCONTROL:
				case VK_RCONTROL:
				case VK_CONTROL:
					PStatus.flags&=~PControl;
				break;
				case VK_LSHIFT:
				case VK_RSHIFT:
				case VK_SHIFT:
					PStatus.flags&=~PShift;
				break;
			} //switch



			//PStatus.key=wParam;
			//PStatus.asckey=MapVirtualKey(wParam,MAPVK_VK_TO_CHAR);


		break; //WM_KEYUP
#endif //WIN32
#if Linux
    case MotionNotify:


		if (PStatus.flags&PInfo) {
    	fprintf(stderr,"MotionNotify\n");
		}
     //mouse movement outside of this window is not recorded
     PStatus.LastMouseX=PStatus.MouseX;
     PStatus.LastMouseY=PStatus.MouseY;
     PStatus.MouseX=xevent.xmotion.x;
     PStatus.MouseY=xevent.xmotion.y;
     PStatus.MouseXRoot=xevent.xmotion.x_root;
     PStatus.MouseYRoot=xevent.xmotion.y_root;



//     PStatus.curwin=twin;  //to determine if focusout to submenu (then no eraseallmenus)
//     fprintf(stderr,"x:%d  y:%d\n",PStatus.MouseX,PStatus.MouseY);
			if (twin!=0) {
				CheckFTWindowMouseMove(twin,xevent.xmotion.x,xevent.xmotion.y);
				twin=GetFTWindow(winname); //check to see if window was deleted
       	if (twin==0) {
					return;  //user could have deleted window in CheckFTButtonUp
				}
			}
     //xevent.xmotion.state
     //xevent.xmotion.x
     //xevent.xmotion.y
     //xevent.xmotion.time

     break; //motion notify - mouse moved in this window (only with button down?)
#endif
#if WIN32
	case WM_MOUSEMOVE:
     //mouse movement outside of this window is not recorded
     PStatus.LastMouseX=PStatus.MouseX;
     PStatus.LastMouseY=PStatus.MouseY;
     PStatus.MouseX=GET_X_LPARAM(lParam);
     PStatus.MouseY=GET_Y_LPARAM(lParam);
     //PStatus.MouseXRoot=xevent.xmotion.x_root;
     //PStatus.MouseYRoot=xevent.xmotion.y_root;

//     fprintf(stderr,"x:%d  y:%d\n",PStatus.MouseX,PStatus.MouseY);
	if (twin!=0) {
		CheckFTWindowMouseMove(twin,PStatus.MouseX,PStatus.MouseY);
		twin=GetFTWindow(winname); //check to see if window was deleted
//if (twin==0) {
			//return;  //user could have deleted window in CheckFTButtonUp
		//}
	}
	break; //WM_MOUSEMOVE
#endif //WIN32
#if Linux
#endif //Linux
#if WIN32
	case WM_MOUSEHWHEEL: //mouse horizontal wheel is moving
	break; //case WM_MOUSEHWHEEL: //mouse horizontal wheel is moving
	case WM_MOUSEWHEEL: //mouse vertical wheel is moving
		 //is mouse wheel movement outside of this window recorded?
		 PStatus.LastMouseX=PStatus.MouseX;
		 PStatus.LastMouseY=PStatus.MouseY;
		 PStatus.MouseX=GET_X_LPARAM(lParam);
		 PStatus.MouseY=GET_Y_LPARAM(lParam);;
		 PStatus.MouseVWheelDelta=GET_WHEEL_DELTA_WPARAM(wParam);
		 //PStatus.MouseXRoot=xevent.xmotion.x_root;
		 //PStatus.MouseYRoot=xevent.xmotion.y_root;

	//     fprintf(stderr,"x:%d  y:%d\n",PStatus.MouseX,PStatus.MouseY);
		if (twin!=0) {
			CheckFTWindowMouseVWheelMove(twin,PStatus.MouseX,PStatus.MouseY,PStatus.MouseVWheelDelta);
//			twin=GetFTWindow(winname); //check to see if window was deleted
	//if (twin==0) {
				//return;  //user could have deleted window in CheckFTButtonUp
			//}
		}
	break; //case WM_MOUSEVWHEEL: //mouse vertical wheel is moving
#endif

#if Linux
    case EnterNotify:
    //xevent.xcrossing.state
     //xevent.xcrossing.x
     //xevent.xcrossing.y
     //xevent.xcrossing.time

     break; //enter notify - mouse moved into window?
     case LeaveNotify:
    //xevent.xcrossing.state
     //xevent.xcrossing.x
     //xevent.xcrossing.y
     //xevent.xcrossing.time

     break; //leave notify - mouse moved out of window?
#endif //Linux
#if Linux
     case FocusIn:  //
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nFocusIn\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

       if (twin!=0) { 
				
//        fprintf(stderr,"win=%s\n",twin->name);

        //set FTControl to first visible tab?  
//        PStatus.focus=twin->num;



        PStatus.FocusFTWindow=twin;
        if (twin->flags&WExclusive) {  //if window has exclusive, update global bits
          PStatus.flags|=PExclusive;
          PStatus.ExclusiveFTWindow=twin;
          //PStatus.ExclusiveFTControl=0;
        }


        //CheckWindowGotFocus(twin);
        //perhaps should be popup,filemenu, etc...
        //if (twin->flags&WItemList) twin->focus=1;
        //else twin->focus=0; //no FTControl can have focus

        if (twin->GotFocus!=0) {
					(*twin->GotFocus) (twin);
					twin=GetFTWindow(winname); //check to see if window was deleted
					if (twin==0) return; //user deleted window
				} //GotFocus
       }  //twin!=0
     break;  //FocusIn
#endif //Linux
#if WIN32
     case WM_SETFOCUS:  //
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nWM_SETFOCUS\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}

       if (twin!=0) { 

		//Set the focus window unless it is a subwindow
		 //Now I set even sub menu windows
		//if (!(twin->flags&(WItemList|WSplash))) {
			PStatus.FocusFTWindow=twin;
		//}

		if (twin->flags&WExclusive) {  //if window has exclusive, update global bits
		  PStatus.flags|=PExclusive;
		  PStatus.ExclusiveFTWindow=twin;
		  //PStatus.ExclusiveFTControl=0;
		}


        if (twin->GotFocus!=0) {
					(*twin->GotFocus) (twin);
					twin=GetFTWindow(winname); //check to see if window was deleted
					//if (twin==0) return; //user deleted window
				} //GotFocus
       }  //twin!=0
     break;
#endif
#if Linux
     case FocusOut:
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"FocusOut event\n");
				if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
			}
//XWindows needs to focus out windows when resizing and redrawing

/*
//			if (PStatus.flags&PExclusive) {
			if (PStatus.flags&PItemListOpen) {
				PStatus.flags&=~PItemListOpen;
				CloseAllItemLists();

				if (PStatus.MouseOverFTControl!=0) {
					if (PStatus.MouseOverFTControl->flags&CMouseOver) {
						PStatus.MouseOverFTControl->flags&=~CMouseOver;
						DrawFTControl(PStatus.MouseOverFTControl);  //redraw control without mouseover
						PStatus.MouseOverFTControl=0;
					} //PStatus.MouseOverFTControl->flags&CMouseOver

				}  //MouseOver!=0

			}//PItemListOpen
*/


/*
		//if FocusOut in a window that does not have the focus of the FT program, give focus back to FT Window
		  if (twin!=0) {
				if (PStatus.FocusFTWindow!=twin &&PStatus.FocusFTWindow!=0) {
					twin=PStatus.FocusFTWindow;
		      tcontrol=GetFTControlWithTab(twin,twin->focus);
					if (tcontrol!=0) {
						FT_SetFocus(twin,tcontrol);
					} //tcontrol!=0
				} //PStatus.FocusFTWindow
			} //twin!=0

*/
/*
       if (twin!=0) {
				//all focus out of any window causes any and all itemlist windows to be closed
         //check for file menu click to close
//         if ((twin->flags&WItemList) && !(twin->flags&WNotVisible)) {  //button up causes focusout




//         if (!(twin->flags&WItemList) && !(twin->flags&WNotVisible)) {  //button up causes focusout

//         if (twin->flags&WItemList && (twin->flags&WItemListOpen)) {  //button up causes focusout
//         if (!(twin->flags&WItemList) && (twin->flags&WItemListOpen)) {  //button up causes focusout
//         if (twin->flags&WItemListOpen) {  //button up causes focusout

          //   fprintf(stderr,"PStatus.flags&PCloseItemListOnUp %d\n",PStatus.flags&PCloseItemListOnUp);
          //   if (!(PStatus.flags&PCloseItemListOnUp)) {
            //if (!(PStatus.flags&PNoFMErase)) {

//fprintf(stderr,"focusout for %s\n",twin->name);



         //FTControl lostfocus func
         //tcontrol=GetFTControlWithTab(twin,PStatus.focus);
         tcontrol=GetFTControlWithTab(twin,twin->focus);
				//we have to temporarily operate on this window and then return focus?
	
				
				// tcontrol=GetFTControlWithTab(PStatus.FocusWindow,PStatus.FocusWindow->focus);    
	
         if (tcontrol!=0) {

//           fprintf(stderr,"control with tab= FTControl=%s twin->focus=%d\n",tcontrol->name,twin->focus);

           //twin->focus=0;  //only to remove cursor
           if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
             tcontrol->flags|=CEraseCursor;
//							fprintf(stderr,"before draw\n");
             DrawFTControl(tcontrol);  //remove cursor
           }
//fprintf(stderr,"after draw\n");
           if (tcontrol->LostFocus!=0) (*tcontrol->LostFocus) (twin,tcontrol);
           //reset focus for twin?
         }  //tcontrol!=0

  



	       //PStatus.focus=0;
         //have to figure out how erase menus when window loses focus to filemenu
         if (twin->LostFocus!=0) (*twin->LostFocus) (twin);


		
          
       } else { //twin!=0
       //twin==0
//         twin=GetFTWindowN(PStatus.focus);
        twin=PStatus.FocusFTWindow;
         if (twin!=0) {          
#if PINFO
           fprintf(stderr,"no window, last with focus is %s\n",twin->name);
#endif
           //this only happens for menu windows, when they are closed they lose focus
           //get parent of menu
           //tcontrol=GetFTControlWithTab(twin,twin->focus);
           tcontrol=GetFTControlWithTab(twin,twin->focus);
//         FT_SetFocus(twin2,tcontrol);  //last FTControl
           FT_SetFocus(twin,tcontrol);  //last FTControl
         } //!=0
 
       } //twin!=0

*/

				//set focus back to correct window - causes feedback loop hang - because XWindows needs to get focus

/*
			twin=PStatus.FocusFTWindow;
      tcontrol=GetFTControlWithTab(twin,twin->focus);
			if (tcontrol!=0) {
				FT_SetFocus(twin,tcontrol);
			}

*/
//			if (twin!=0) {
//				fprintf(stderr,"PStatus window with focus= %s\n",PStatus.FocusFTWindow->name);
           //tcontrol=GetFTControlWithTab(twin,twin->focus);
          // FT_SetFocus(twin,tcontrol);  //last FTControl
//			} //twin!=0
//fprintf(stderr,"end LostFocus\n");
     break;  //FocusOut
#endif //Linux
#if WIN32
     case WM_KILLFOCUS:
			if (PStatus.flags&PInfo) {
				fprintf(stderr,"WM_KILLFOCUS message\n");
				if (twin!=0) {
					fprintf(stderr,"in %s\n",twin->name);
				}
			}
//XWindows needs to focus out windows when resizing and redrawing

			//Note that WM_KILLFOCUS does not get called for WS_CHILD windows that lose focus

//			if (PStatus.flags&PExclusive) {
//			if ((PStatus.flags&PItemListOpen) || (PStatus.flags&PExclusive)) {
//				CloseAllItemLists();
			/*
			if (PStatus.flags&PItemListOpen) {
				PStatus.flags &= ~PItemListOpen;
				CloseAllItemLists();

				if (PStatus.MouseOverFTControl!=0) {
					if (PStatus.MouseOverFTControl->flags&CMouseOver) {
						PStatus.MouseOverFTControl->flags&=~CMouseOver;
						DrawFTControl(PStatus.MouseOverFTControl);  //redraw control without mouseover
						PStatus.MouseOverFTControl=0;
					} //PStatus.MouseOverFTControl->flags&CMouseOver

				}  //MouseOver!=0

			}//PItemListOpen
			*/


     break;  //WM_KILLFOCUS
#endif //WIN32
#if Linux
     case MapNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nMapNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
//				if (twin!=0) {
//					twin->flags&=~WWaitUntilDoneDrawing; //Window is done drawing now
//				}
     break; 
     case UnmapNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nUnmapNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
     break;
     case VisibilityNotify:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nVisibilityNotify\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
     break;
     case ConfigureRequest:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nConfigureRequest\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
     break;
     case ResizeRequest: //resize
//       fprintf(stderr,"Resize request\n");
       //I guess this program is left to handle the resize of the window
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nResizeRequest\n");
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
        //This has to be an X error, because the window looks like it resizes
       //doing a resize makes an endless loop of resizes
       //result=XResizeWindow(PStatus.xdisplay,twin->xwindow,xevent.xresizerequest.width,xevent.xresizerequest.height);

//       fprintf(stderr,"result=%d\n",result);
       break;

#endif //Linux
/*
			case StructureNotifyMask:
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nStructure Notify xevent %d\n",xevent.type);
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
				break;
*/

#if WIN32
		case WM_ACTIVATEAPP:
    break;
	 		case WM_PAINT:
			//get FTWindow from Windows hwindow
			//PStatus.flags|=0; //for debug line
			//redraw window insides
			if (twin!=0) { //this makes stuff much slower, but otherwise filemenu text remains
				//todo add a flag so this is not done too much
				//like a FTW_NEEDS_WM_PAINT- which is only set when an itemlist is closed
				//if (twin->flags&FTW_NEEDS_WM_PAINT) {
				DrawFTWindow(twin);
				//} 
				//twin->flags&=~FTW_NEEDS_WM_PAINT;
				//GdiFlush();
			}
		break;

		case WM_ERASEBKGND:
			//return 0; //don't process erase background
			break;
#endif

#if Linux
     default: //
				if (PStatus.flags&PInfo) {
					fprintf(stderr,"\nUnknown xevent %d\n",xevent.type);
					if (twin!=0) fprintf(stderr,"in %s\n",twin->name);
				}
      break;
#endif

#if Linux			
}  //end switch on eventtype
#endif //Linux
#if WIN32
		}//end switch(message)
#endif




#if WIN32
		//also let Windows process message
		return DefWindowProc(hwindow,message,wParam,lParam);
#endif

//if (PStatus.flags&PInfo)
  //fprintf(stderr,"done get_window_input\n");
} //end ProcessEvents()



//x,y=mouse position, dw,dh=change in width and height
void ResizeFTWindowControls(FTWindow *twin,int x,int y,int dw,int dh)
{
	FTControl *tcontrol;
	int numvisible,FTControlResized;
	char conname[FTMedStr];
	char winname[FTMedStr];
	
	strcpy(winname,twin->name);
    tcontrol=twin->icontrol;
    while(tcontrol!=0) {
		FTControlResized=0;
		strcpy(conname,tcontrol->name);

		  //fprintf(stderr,"scale FTControl %s\n",tcontrol->name);
		  //  fprintf(stderr,"scalex=%f scaley=%f\n",scalex,scaley);
		  //  fprintf(stderr,"x2=%d bw=%d bh=%d\n",tcontrol->x2,bw,bh);
		  //  fprintf(stderr,"x2=%d bw=%d bh=%d\n",tcontrol->x2,bw,bh);
 
		  //fprintf(stderr,"dw=%i dh=%i\n",twin->dw,twin->dh);

		  //Move FTControls with resize
		  //adds however many pixels were added with resize
		  //in order to keep FTControls together, amount of movement is not scaled, but added
		  if (tcontrol->flags&CGrowX1) {
			tcontrol->x1+=dw;
					FTControlResized=1;
		  }

		  if (tcontrol->flags&CGrowY1) {
			tcontrol->y1+=dh;
					FTControlResized=1;
		  }

		  if (tcontrol->flags&CGrowX2) {
			tcontrol->x2+=dw;
					FTControlResized=1;
	//in Dropdown, Radio, Checkboxes x3,y3 is used 
			if (tcontrol->type==CTDropdown||tcontrol->type==CTOption||tcontrol->type==CTCheckBox) tcontrol->x3+=dw; //x3 is far right of dropdown
	//        if (tcontrol->type==CTDropdown||tcontrol->type==CTOption||tcontrol->type==CTCheckBox) tcontrol->x3=tcontrol->x2; //x3 is far right of dropdown

		  }

		  if (tcontrol->flags&CGrowY2) {
			tcontrol->y2+=dh;
					FTControlResized=1;
	//y3 may start as 0 for dropdowns so set y3 to y2
	//        if (tcontrol->type==CTDropdown||tcontrol->type==CTOption||tcontrol->type==CTCheckBox) tcontrol->y3+=dh; 
	//        if (tcontrol->type==CTDropdown||tcontrol->type==CTOption||tcontrol->type==CTCheckBox) tcontrol->y4=tcontrol->y2;
			if (tcontrol->type==CTDropdown||tcontrol->type==CTOption||tcontrol->type==CTCheckBox) tcontrol->y3=tcontrol->y2;  

		  }


		  if (tcontrol->flags&CScaleX1) {
			tcontrol->x1+=tcontrol->sx1*dw;
					FTControlResized=1;
		  }

		  if (tcontrol->flags&CScaleY1) {
			tcontrol->y1+=tcontrol->sy1*dh;
					FTControlResized=1;
		  }

		  if (tcontrol->flags&CScaleX2) {
			tcontrol->x2+=tcontrol->sx2*dw;
					FTControlResized=1;
		  }

		  if (tcontrol->flags&CScaleY2) {
			tcontrol->y2+=tcontrol->sy2*dh;
					FTControlResized=1;
		  }
      
	   //if fileopen, update scrolls
		if (tcontrol->type==CTFileOpen || tcontrol->type==CTItemList) {
		   //in resize of fileopen, i[0] can be changed
		   if (tcontrol->ilist!=0) {
		   //nums=number of FTItems now available
			 //numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent);
			   numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->fontheight);
	#if PINFO
			 fprintf(stderr,"i[0]=%d numv=%d list->numv=%d\n",tcontrol->FirstItemShowing,numvisible,tcontrol->ilist->num);
	#endif
			 if ((tcontrol->FirstItemShowing+numvisible-1)>tcontrol->ilist->num) {
				 tcontrol->FirstItemShowing=tcontrol->ilist->num-numvisible-1;
			 }
			 if (tcontrol->FirstItemShowing<1) { 
				 tcontrol->FirstItemShowing=1;
			 }
	//

			 //tcontrol->numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent);
			 tcontrol->numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->fontheight);
	//         numvisible=(tcontrol->y2-tcontrol->y1)/(tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.ascent);
	//         if (tcontrol->ilist->num-tcontrol->CursorLocationInChars+tcontrol->numvisible>numvisible) {
	//           tcontrol->CursorLocationInChars-=nums-tcontrol->ilist->num;  
	//           if (tcontrol->CursorLocationInChars<0) tcontrol->CursorLocationInChars=0;
	//         }
			 if (tcontrol->flags&CVScroll) {
				CalcVScrollSlide(twin,tcontrol);
			 }
 			 if (tcontrol->flags&CHScroll) {
				CalcHScrollSlide(twin,tcontrol);
			 }

			 //if (tcontrol->FTC_VScrollSlide!=0) {
			//	CalcVScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
			//}
			//if (tcontrol->FTC_HScrollSlide!=0) {
			//	CalcHScrollSlide(tcontrol->window,tcontrol);  //determine size, and position of the scroll slider button
			//}
			FTControlResized=1;
	//       UpdateVScrollSlide(tcontrol);
		  } //ilist!=0
		}

//also call user resize callback function for all controls on this window
		if (FTControlResized) {
		   if (tcontrol->OnResize !=0) {
						//(*tcontrol->OnResize) (twin,tcontrol,xevent.xconfigure.x,xevent.xconfigure.y);
					(*tcontrol->OnResize) (twin,tcontrol,x,y);
						twin=GetFTWindow(winname); //check to see if window was deleted
						if (twin==0) return; //user deleted window
	//					twin=GetFTWindow(winname); //check to see if controls were deleted
						//if (twin==0) return; //user deleted window
					}
		}

    tcontrol=tcontrol->next; 
    } //while tcontrol!=0

} //void ResizeFTWindowControls(FTWindow *twin,int x,int y,int dw,int dh)



#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
	tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}
#if 0 
//    #include <windows.h>
     
int gettimeofday( struct timeval *tv, struct timezone *tz )
{
time_t rawtime;
LARGE_INTEGER tickPerSecond;
LARGE_INTEGER tick; // a point in time
//SYSTEMTIME stime;

/*
GetSystemTime(&stime);
tv->tv_sec=stime.wSecond;
tv->tv_usec=stime.wMilliseconds*1000;
*/

//somehow with this code- the microseconds could be > 999999
//this is because gettimeofday "gives the number of seconds and microseconds since the Epoch"
//presumably they are equivalent (not to be added)
time(&rawtime);
tv->tv_sec = (long)rawtime;
     
// here starts the microsecond resolution:
     
     
// get the high resolution counter's accuracy
QueryPerformanceFrequency(&tickPerSecond);
     
// what time is it ?
QueryPerformanceCounter(&tick);
     
// and here we get the current microsecond
tv->tv_usec = (long int)(tick.QuadPart % tickPerSecond.QuadPart);  //note LONGLONG to long loss of data


return 0;
}
#endif
#endif // _WIN32_

void GetTime(void)
{
//fprintf(stderr,"GetTime\n");
PStatus.ttime=time(NULL);
PStatus.ctime=localtime(&PStatus.ttime);  //is mallocing?
gettimeofday(&PStatus.utime,NULL);
}  //end GetTime




//===========
//GRAPHICS FUNCTIONS
//=============
//FT_CreateBitmap
//mallocs memory and initializes bitmap parameters
//this function expects caller to free memory when done
//probably should add 32-bit aligned padding
unsigned char *FT_CreateBitmap(int w,int h,int bpp,unsigned char *data)
{
unsigned char *bmp;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
int bmpsize,Bytespp;

bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*bpp);

bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

Bytespp=bpp>>3;

bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*Bytespp;

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=w;
bmi->biHeight=h;
bmi->biPlanes=1;
bmi->biBitCount=bpp;
bmi->biSizeImage=Bytespp*w*h;

//probably a better way of creating a variable based on passed data type
/*
switch(bpp) {
	case	8:
	case	24:
		memcpy((unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
	case	16:
		memcpy((unsigned short *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
	case	32:
		memcpy((unsigned int *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),Bytespp,data,w*h);
		break;
} //switch(bpp)
*/

//for now just copy directly - ignore byte order
		memcpy((unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER)),data,Bytespp*w*h);

return(bmp);
} //FT_CreateBitmap

//FT_CreateBitmapHeader - Malloc memory, and initialize a bitmap header only, caller copies data into
//note that memory is allocated for the bitmap data
unsigned char *FT_CreateBitmapHeader(int w,int h,int bpp) 
{
unsigned char *bmp;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
int bmpsize,Bytespp;

bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*bpp);

bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

Bytespp=bpp>>3;
bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+w*h*Bytespp;

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=w;
bmi->biHeight=h;
bmi->biPlanes=1;
bmi->biBitCount=bpp;
bmi->biSizeImage=Bytespp*w*h;

return(bmp);
} //FT_CreateBitmapHeader(int w,int h,int bpp)

#if Linux
//Load Bitmap and return XImage
XImage * FT_LoadBitmapXImage(char *filename)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int nw,i,j,bc,x,y;
XImage *img;
unsigned char *offset;
float rx,ry;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapXImage\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  fprintf(stderr,"Error opening %s\n",filename);
else
{
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

x=bmi.biWidth;
y=bmi.biHeight;

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0) {
	nw=bmi.biWidth*bc+4-nw;
} else {
  nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
    *(bdata2+i*x*4+j*4)=*offset;
    *(bdata2+i*x*4+j*4+1)=*(offset+1);
    *(bdata2+i*x*4+j*4+2)=*(offset+2);
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,visual,depth,ZPixmap,0,bdata2,x,y,8,x*3);
img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);



free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

return(img);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapXImage\n");
}

return(0);
} //XImage * FT_LoadBitmapXImage(char *filename);
#endif 

//Load Bitmap and return unsigned char *
unsigned char * FT_LoadBitmap(char *filename)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,x,y,bmphsize;
unsigned char *b1offset,*b2offset;
/*
float rx,ry;
unsigned char *offset;
int bidx;
*/

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmap\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

x=bmi.biWidth;
y=bmi.biHeight;

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0)  {
	nw=bmi.biWidth*bc+4-nw;
} else {
  nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"image bitcount=%d\n",bmi.biBitCount);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);
//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
//bdata2=(unsigned char *)malloc(x*y*4);
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now make enough for a 32-bit bmp
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;


#if 0
rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);

	//bidx=i*x*4+j*4;
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
	*(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl
#endif

//#if 0 

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {

	  //nw includes bc
	  //b1offset=(y-i-1)*nw+j*bc;
	  //b1offset=i*x*4+j*4;
	  b1offset=bdata+(y-i-1)*nw+j*bc;
	  //b1offset=bdata+i*nw+j*bc;
	  b2offset=bdata2+i*x*4+j*4;

	   memcpy(b2offset,b1offset,3);
	  *(b2offset+3)=0;
//	  *(b2offset)=*(b1offset);
//	  *(b2offset+1)=*(b1offset+1);
//	  *(b2offset+2)=*(b1offset+2);
//	  *(b2offset+3)=0;

//	  memcpy(b2offset,b1offset,3);
//	  *(b2offset+3)=0;
/*	
	  *(bdata2+b2offset)=*(bdata+b1offset);
	  *(bdata2+b2offset+1)=*(bdata+b1offset+1);
	  *(bdata2+b2offset+2)=*(bdata+b1offset+2);
	  *(bdata2+b2offset+3)=0;
*/
/*
    //offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
	  offset=bdata+(y-i-1)*nw+j*bc;
  //  fprintf(stderr,"%p ",offset);
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
	*(bdata2+bidx+3)=0;
*/
  }  //end j
}  //end i
//copy image to 32-bit image on FTControl

//#endif
//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//#endif


free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

//return(img);
return(bmp);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmap\n");
}

return(0);
} //unsigned char * FT_LoadBitmap(char *filename);

//currently only for 24 bit images
//returns pointer to a bitmap (including file header, info header and data)
//XImage * FT_LoadBitmapFileAndScale(char *filename,int x,int y)
unsigned char * FT_LoadBitmapFileAndScale(char *filename,int x,int y)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,BitmapDepthInBytes,bmphsize,bidx;
unsigned char *offset;
float rx,ry;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapFileAndScale\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	//fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

BitmapDepthInBytes=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*BitmapDepthInBytes,4.0);
if (nw>0)  {
	nw=bmi.biWidth*BitmapDepthInBytes+4-nw;
} else {
  nw=bmi.biWidth*BitmapDepthInBytes;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"image bitcount=%d\n",bmi.biBitCount);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
//bdata2=(unsigned char *)malloc(x*y*4);
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now make enough for a 32-bit bmp
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

rx=(float)nw/((float)x*(float)BitmapDepthInBytes);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*BitmapDepthInBytes;
  //  fprintf(stderr,"%p ",offset);

	//bidx=i*x*4+j*4;
		bidx=i*x*4+j*4;
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
		*(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);



free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

//return(img);
//return(bdata2);
return(bmp);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapFileAndScale\n");
}

return(0);
}  //FT_LoadBitmapFileAndScale


//Load a Bitmap file and shade with mixcolor - for selected buttons
unsigned char *FT_LoadBitmapFileAndShade(char *filename,int x,int y,unsigned int mixcolor)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,bidx,bmphsize;
unsigned char *offset;
float rx,ry;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapFileAndShade\n");
}

fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);
} else {
//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

//fprintf(stderr,"w=%d h=%d\n",bmi.biWidth,bmi.biHeight);
//fprintf(stderr,"bicount=%d\n",bmi.biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi.biBitCount/8;

nw=(int)fmodf((float)bmi.biWidth*bc,4.0);
if (nw>0)  {
	nw=bmi.biWidth*bc+4-nw;
} else {
	nw=bmi.biWidth*bc;
}



//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//read data
bdata=(unsigned char *)malloc(nw*bmi.biHeight);
fread(bdata,nw*bmi.biHeight,1,fptr);
fclose(fptr);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now only use depth 3
bmf.bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=32; //make 32 bit
bmi.biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

rx=(float)nw/((float)x*(float)bc);
ry=(float)bmi.biHeight/(float)y;
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    offset=bdata+((int)((y-i-1)*ry))*nw+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
	bidx=i*x*4+j*4;
    *(bdata2+bidx)=(*offset+ (mixcolor&0x000000ff))/2;
    *(bdata2+bidx+1)=(*(offset+1)+ ((mixcolor&0x0000ff00)>>8))/2;
    *(bdata2+bidx+2)=(*(offset+2)+ ((mixcolor&0x00ff0000)>>16))/2;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
//img=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4); //32-bit

free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);

return(bmp);
//return(img);
}//end if bitmap file found


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapFileAndShade\n");
}

return(0);
}  //FT_LoadBitmapFileAndShade

//load bitmap data to a control and shade (for mouse over and button click)
//presumes no resize or 4-byte alignment is needed and source bmp is 32-bit image
//this is mostly called internally by FreeThought to shade bitmaps on buttons
unsigned char * FT_LoadBitmapAndShade(unsigned char *bmfile,unsigned int mixcolor)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdataBMP,*bdataNEWBMP,*bmp;
int i,j,bmphsize,bidx;
unsigned char *offsetBMP,*offsetNEWBMP;
int bmpx,bmpy;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_LoadBitmapAndShade\n");
}

if (bmfile==0) {
	fprintf(stderr,"Error: FT_LoadBitmapAndShade called with bitmap data=0\n");
	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End FT_LoadBitmapAndShade\n");
	}

}

bmf=(BITMAPFILEHEADER *)bmfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",bmfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));
bdataBMP=(unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER));

//bc=bmi->biBitCount/8;

bmpx=bmi->biWidth;
bmpy=bmi->biHeight;

//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(bmpx*bmpy*4+bmphsize); //for now only use depth 3
memcpy(bmp,(unsigned char *)bmf,sizeof(BITMAPFILEHEADER));
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)bmi,sizeof(BITMAPINFOHEADER));
bdataNEWBMP=bmp+bmphsize;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work

for(i=0;i<bmpy;i++) {
  for(j=0;j<bmpx;j++) {
	bidx=i*bmpx*4+j*4;
    offsetBMP=bdataBMP+bidx;
	offsetNEWBMP=bdataNEWBMP+bidx;

	*(offsetNEWBMP)=(unsigned char)(*offsetBMP+ (mixcolor&0x000000ff))/2;
    *(offsetNEWBMP+1)=(unsigned char)(*(offsetBMP+1)+ ((mixcolor&0x0000ff00)>>8))/2;
    *(offsetNEWBMP+2)=(unsigned char)(*(offsetBMP+2)+ ((mixcolor&0x00ff0000)>>16))/2;
	*(offsetNEWBMP+3)=0;
  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//tcontrol->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,nw,y,32,nw*4);
#endif
#if WIN32
//CreateBitmap x,y is actual (not padded)
//tcontrol->hbmp[0]= CreateBitmap(x,y,1,32,bdataCTL);
#endif

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_LoadBitmapAndShade\n");
}

return(bmp);
} //FT_LoadBitmapAndShade


//Load an image to one of the FTControl images (and resize, or shade depending on flags)
int LoadImageFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
	char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadImageFileToFTControl\n");
}

#if Linux
	if (strcasecmp(&filename[strlen(filename)-3],"bmp")==0) {
#endif
#if WIN32
if (stricmp(&filename[strlen(filename)-3],"bmp")==0) {
#endif
	//image if bitmap
	if (!(LoadBitmapFileToFTControl(filename,tcontrol,flags,mixcolor))) {
		fprintf(stderr,"Error: LoadBitmapFileToFTControl failed\n");
		return(0);
	} else {
		return(1);
	}
} else { //if (stricmp(filename,strlen(filename)-3,"bmp")==0) {
#if Linux
	if (strcasecmp(&filename[strlen(filename)-3],"jpg")==0) {
#endif
#if WIN32
	if (stricmp(&filename[strlen(filename)-3],"jpg")==0) {
#endif
		if (!(LoadJPGFileToFTControl(filename,tcontrol,flags,mixcolor))) {
			fprintf(stderr,"Error: LoadJPGFileToFTControl failed\n");
			return(0);
		} else {
			return(1);
		}
	} else { //if (stricmp(filename,strlen(filename)-3,"jpg")==0) {
		fprintf(stderr,"Error: In LoadImageFileToFTControl(): Support for file type for %s not implemented.\n",filename);
		sprintf(tstr,"Error: In LoadImageFileToFTControl(): Support for file type for %s not implemented.\n",filename);
		FTMessageBox(tstr,FTMB_OK,"Error",0);
		return(0);
	} ////if (stricmp(filename,strlen(filename)-3,"jpg")==0) {
} //if (stricmp(filename,strlen(filename)-3,"bmp")==0) {

}  //int FT_LoadImageFileToFTControl(char *filename,FTControl *tcontrol,unsigned int Flags)

//currently only for 24 bit images
//not scalable
//maybe make FT_LoadBitmapToFTControlScale - Scale to FTControl
//int FT_LoadBitmapToFTControl(FTControl *tcontrol,unsigned char *bmfile,int ResizeImage)
int LoadBitmapToFTControl(unsigned char *bmpfile,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdataBMP,*bdataCTL,*bmp;
int PaddedBitmapWidth,PaddedControlBitmapWidth,i,j,bmphsize,bidx,BitmapDepthInBytes,DisplayDepthInBytes;
unsigned char *offsetBMP,*offsetCTL;
int x,y,bmpx,bmpy,wordalign;
float rx,ry;
int ImageNum;


if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapToFTControl\n");
}

if (bmpfile==0) {
	fprintf(stderr,"Error: LoadBitmapToFTControl called with bitmap data=0\n");
	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End LoadBitmapToFTControl\n");
	}

}


bmf=(BITMAPFILEHEADER *)bmpfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",bmpfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmpfile+sizeof(BITMAPFILEHEADER));
bdataBMP=(unsigned char *)((unsigned char *)bmi+sizeof(BITMAPINFOHEADER));

BitmapDepthInBytes=bmi->biBitCount/8;

bmpx=bmi->biWidth;
bmpy=bmi->biHeight;

PaddedBitmapWidth=bmpx*BitmapDepthInBytes;
wordalign=(int)fmodf((float)PaddedBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedBitmapWidth+=4-wordalign;
}

x=tcontrol->x2-tcontrol->x1;
y=tcontrol->y2-tcontrol->y1;

//do not scale
if (!(flags&FT_SCALE_IMAGE)) {
	rx=1.0;
	ry=1.0;
	//clip bmp to control if bitmap is larger than control
	if (bmpx>=x) {
		bmpx=x;
		PaddedBitmapWidth=x*BitmapDepthInBytes;
	} else {
		//clip control to bitmap
		x=bmpx;
	}
	if (bmpy>=y) {
		bmpy=y;
	} else {
	//clip control to bitmap
		y=bmpy;
	}


} else {	
//rx=ratio of x of original bitmap and destination (x or 0 if not scaled to control)
//ry=ratio of y of original bitmap and destination (y or 0 if not scaled to control)
	rx=(float)bmpx/(float)x;
	ry=(float)bmpy/(float)y;
} //!ResizeImage


//  fprintf(stderr,"depth=%d\n",depth);

ImageNum=flags&0x3;

//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
//see if we need to pad control bitmap
#if Linux
//XCreateImage only works for 8, 16, or 32 bit depths (not 24)
DisplayDepthInBytes=4;
#endif
#if WIN32
DisplayDepthInBytes=PStatus.depth/8;
#endif
PaddedControlBitmapWidth=x*DisplayDepthInBytes;
wordalign=(int)fmodf((float)PaddedControlBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedControlBitmapWidth+=4-wordalign;
}

bmp=(unsigned char *)malloc(PaddedControlBitmapWidth*y+bmphsize); //control bitmap has the same depth as the current display setting

tcontrol->image[ImageNum]=bmp;
tcontrol->bmpinfo[ImageNum]=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
//add pointer to bitmap data to control data structure
tcontrol->bmpdata[ImageNum]=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//bmf->bfSize=bmphsize+x*y*4;
bmf->bfSize=bmphsize+PaddedControlBitmapWidth*y;
memcpy(bmp,(unsigned char *)bmf,sizeof(BITMAPFILEHEADER));
//adjust bpp
#if Linux
bmi->biBitCount=32;//can only be 32 for createximage was-PStatus.depth;
#endif
#if WIN32
bmi->biBitCount=PStatus.depth;//can only be 32 for createximage was-PStatus.depth;
#endif
bmi->biWidth=x;
bmi->biHeight=y;
//bmi->biSizeImage=x*y*4;
bmi->biSizeImage=PaddedControlBitmapWidth*y;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)bmi,sizeof(BITMAPINFOHEADER));
bdataCTL=bmp+bmphsize;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//fprintf(stderr,"%p\n",bdata);
//copy, scale, and shade bitmap data to control image
if (flags&FT_SHADE_IMAGE) {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
		offsetBMP=bdataBMP+((bmpy-(int)(i*ry))-1)*PaddedBitmapWidth+((int)(j*rx))*BitmapDepthInBytes;
		offsetCTL=bdataCTL+bidx;
		*(offsetCTL)=(*offsetBMP+ (mixcolor&0x000000ff))/2;
		*(offsetCTL+1)=(*(offsetBMP+1)+ ((mixcolor&0x0000ff00)>>8))/2;
		*(offsetCTL+2)=(*(offsetBMP+2)+ ((mixcolor&0x00ff0000)>>16))/2;
		if (DisplayDepthInBytes==4) {
			*(offsetCTL+3)=0;
		}
	  }  //end j
	}  //end i
} else {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
		offsetBMP=bdataBMP+((bmpy-(int)(i*ry))-1)*PaddedBitmapWidth+((int)(j*rx))*BitmapDepthInBytes;
		offsetCTL=bdataCTL+bidx;
		memcpy(offsetCTL,offsetBMP,3);
		if (DisplayDepthInBytes==4) {
			*(offsetCTL+3)=0;
		}
	  }  //end j
	}  //end i
} //if (flags&FT_SHADE_IMAGE) {

//add border to control bitmap
if (flags&FT_ADD_BORDER_TO_IMAGE) {
	for(i=0;i<y;i++) {
	  for(j=0;j<x;j++) {
		  if (i==0 || i==y-1 || j==0 || j==x-1) {
			bidx=i*PaddedControlBitmapWidth+j*DisplayDepthInBytes;
			offsetCTL=bdataCTL+bidx;
			*(unsigned int *)offsetCTL=mixcolor;
		  } //if (i==0 || i==y-1 || j==0 || j==x-1) {
	  }  //end j
	}  //end i

} //if (flags&FT_ADD_BORDER_TO_IMAGE) {

//add dashed-border
if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {

} //if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {


//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
//tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,nw,y,32,nw*4);
//nw is now the 32-bit aligned width*bpp
//tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,nw/4,y,32,nw);
//bitmap must  be 32 it depth, but first depth in XCreateImage must match the display depth which can be 24
tcontrol->ximage[ImageNum]=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdataCTL,x,y,32,PaddedControlBitmapWidth);

#endif
#if WIN32
//CreateBitmap x,y is actual (not padded)
tcontrol->hbmp[ImageNum]= CreateBitmap(x,y,1,PStatus.depth,bdataCTL);
#endif


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapToFTControl\n");
}

//    fprintf(stderr,"Got Image %p\n",img);
//return(tximage);
//return(bmp);
return(1);
} //FT_LoadBitmapToFTControl



//currently only for 16, 24 or 32 bit images
//Resize=1, bitmap is scaled to fit control
//int FT_LoadBitmapFileToFTControl(FTControl *tcontrol,char *bmpname,int ResizeImage)
int LoadBitmapFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bmp,*bdata;
int PaddedBitmapWidth,wordalign,BitmapDepthInBytes,bmpx,bmpy;
char tstr[FTMedStr];

//NOTE - this probably needs to be changed - see LoadBitmapToFTWindow for working version

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapFileToFTControl\n");
}


fptr=fopen(filename,"rb");
if (fptr==0)  {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"Done LoadBitmapFileToFTControl\n");
	}
	return(0);
}


//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,1,2,fptr);
fread(&bmf.bfSize,1,4,fptr);
fread(&bmf.bfReserved1,1,2,fptr);
fread(&bmf.bfReserved2,1,2,fptr);
fread(&bmf.bfOffBits,1,4,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,1,4,fptr);
fread(&bmi.biWidth,1,4,fptr);
fread(&bmi.biHeight,1,4,fptr);
fread(&bmi.biPlanes,1,2,fptr);
fread(&bmi.biBitCount,1,2,fptr);
fread(&bmi.biCompression,1,4,fptr);
fread(&bmi.biSizeImage,1,4,fptr);
fread(&bmi.biXPelsPerMeter,1,4,fptr);
fread(&bmi.biYPelsPerMeter,1,4,fptr);
fread(&bmi.biClrUsed,1,4,fptr);
fread(&bmi.biClrImportant,1,4,fptr);

BitmapDepthInBytes=bmi.biBitCount/8;

bmpx=bmi.biWidth;
bmpy=bmi.biHeight;

PaddedBitmapWidth=bmpx*BitmapDepthInBytes;
wordalign=(int)fmodf((float)PaddedBitmapWidth,4.0);
if (wordalign>0)  {
	PaddedBitmapWidth+=4-wordalign;
}

//allocate bitmap in memory
bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PaddedBitmapWidth*bmi.biHeight);
//and copy file and info header into memory
memcpy(bmp,&bmf,sizeof(BITMAPFILEHEADER));
memcpy(bmp+sizeof(BITMAPFILEHEADER),&bmi,sizeof(BITMAPINFOHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//read data into memory
fread(bdata,1,PaddedBitmapWidth*bmi.biHeight,fptr);
fclose(fptr);

//load the bitmap in memory to the control, scaling to control if ResizeImage is set
LoadBitmapToFTControl(bmp,tcontrol,flags,mixcolor);

free(bmp); //free original image

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Done LoadBitmapFileToFTControl\n");
}

//return(tximage);
return(1);
}  //FT_LoadBitmapFileToFTControl


void WriteBitmap(unsigned char *name, unsigned char *bmfile) {
FILE *fptr;
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmd;
int i,bpp,h,linelen;

bmf=(BITMAPFILEHEADER *)bmfile;
bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));
bmd=(unsigned char *)bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

//fprintf(stdout,"bitmap %s file size= %d\n",name,bmf->bfSize);
//fprintf(stdout,"bitmap image size= %d\n",bmi->biSizeImage);

fptr=fopen((char*)name,"wb");
if (fptr==0) {
	fprintf(stdout,"Could not open file %s for writing bitmap\n",name);
} else {
//have to write in little endian - fwrite does not know to use structures, such as the BITMAPFILEHEADER structure

//fprintf(stdout,"bmf->bfType=%x fptr=%x\n",bmf->bfType,fptr);
	fwrite(&bmf->bfType,1,2,fptr);
//fprintf(stdout,"bmf->bfType=%x\n",bmf->bfType);
	fwrite(&bmf->bfSize,1,4,fptr);
	fwrite(&bmf->bfReserved1,1,2,fptr);
	fwrite(&bmf->bfReserved2,1,2,fptr);
	fwrite(&bmf->bfOffBits,1,4,fptr);

//fprintf(stdout,"bmi->biSize=%x\n",bmi->biSize);
	fwrite(&bmi->biSize,1,4,fptr);
	fwrite(&bmi->biWidth,1,4,fptr);
	fwrite(&bmi->biHeight,1,4,fptr);
	fwrite(&bmi->biPlanes,1,2,fptr);
	fwrite(&bmi->biBitCount,1,2,fptr);
	fwrite(&bmi->biCompression,1,4,fptr);
	fwrite(&bmi->biSizeImage,1,4,fptr);
	fwrite(&bmi->biXPelsPerMeter,1,4,fptr);
	fwrite(&bmi->biYPelsPerMeter,1,4,fptr);
	fwrite(&bmi->biClrUsed,1,4,fptr);
	fwrite(&bmi->biClrImportant,1,4,fptr);

	//fwrite(bmd,bmi->biSizeImage,1,fptr);
//turn bmpdata upside down
bpp=bmi->biBitCount/8;
h=bmi->biHeight;
linelen=bmi->biWidth*bpp;
for(i=0;i<h;i++) {
	fwrite(bmd+(h-i-1)*linelen,linelen,1,fptr);
} //i

} //fopen
	fclose(fptr);

//fprintf(stdout,"Done WriteBitmap\n");
} //WriteBitmap

//currently only for 24 bit images
//ResizeWindow=0 resize bmp to fit window, 1= resize window to fit bmp
//possibly change ResizeWindow to flags or Settings and (FT_LoadBitmap_RESIZEWINDOW)
//probably need faster function, without malloc, 
//possibly use sws_scale to scale BMP
int LoadBitmapToFTWindow(unsigned char *bmfile,FTWindow *twin,int ResizeWindow)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bdata,*bdata2,*bmp;
int nw,i,j,bc,bmphsize,bidx;
unsigned char *offset;
int x,y;
float rx,ry;

//add scaling (rx,ry)

//bitmap should already have all header info

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapToFTWindow\n");
}

if (bmfile==0 || twin<=0) {
  fprintf(stderr,"Error: LoadBitmapToFTWindow passed NULL pointer\n"); 
	return 0;
}



bmf=(BITMAPFILEHEADER *)bmfile;

if (bmf->bfType!=0x4d42) {
  fprintf(stderr,"bmp does not start with 0x4d42 (but with %x).\n",*(unsigned short *)bmfile);
  return 0;
}

bmi=(BITMAPINFOHEADER *)((unsigned char *)bmfile+sizeof(BITMAPFILEHEADER));

//if (twin->bmpinfo!=0) free(twin->bmpinfo);
//bmpinfo should probably be static - not pointer
//if (twin->bmpinfo==0) {  //don't bother to free+malloc again if memory is already malloc'd
//	twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//}
//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));

//fprintf(stderr,"w=%d h=%d\n",bmi->biWidth,bmi->biHeight);
//fprintf(stderr,"bitcount=%d\n",bmi->biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=bmi->biBitCount/8;

nw=(int)fmodf((float)bmi->biWidth*bc,4.0);
if (nw>0) {
	nw=bmi->biWidth+4-nw;
} else {
  nw=bmi->biWidth;
}

if (ResizeWindow) {
	if (twin->w!=bmi->biWidth || twin->h!=bmi->biHeight) {
		twin->w=bmi->biWidth;
		twin->h=bmi->biHeight;
#if Linux
		if (twin->flags&WOpen && twin->xwindow!=0) {
#endif
#if WIN32
		if (twin->flags&WOpen && twin->hwindow!=0) {
#endif
			//we need to resize an existing window
			ResizeFTWindow(twin);
		} //WOpen

	}
}

//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi->biWidth;
  twin->h=bmi->biHeight;
}

//twin at 32 bpp should always be 4-byte aligned - but could be a problem on other display resolutions
x=twin->w;
y=twin->h;


//if (!ResizeWindow) {
	//rx=(float)nw/((float)twin->x*(float)bc);
	//1.0 so no overlap
	rx=((float)nw-1.0)/(float)x;
	ry=((float)bmi->biHeight-1.0)/(float)y;

//fprintf(stderr,"rx=%f ry=%f w=%d h=%d\n",rx,ry,x,y);
//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//} 

bdata=(unsigned char *)bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned

//if (twin->bmpdata!=0) {
//	free(twin->bmpdata); //free any previous mallc'd bitmap data
//}
bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(x*y*4+bmphsize); //for now only use depth 3
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)((unsigned char *)bmp+sizeof(BITMAPFILEHEADER));
bmf->bfSize=bmphsize+x*y*4;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi->biBitCount=32;
bmi->biSizeImage=x*y*4;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

twin->bmpdata=bdata2; //store in order to track to free
//memset(bdata2,0,sizeof(x*y*4));


//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);

//#if PINFO
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work
		//offset=bdata;


for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
		bidx=i*x*4+j*4;
    
		if (!ResizeWindow) {
			offset=bdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
		}		else {
			offset=bdata+i*nw*bc+j*bc;
		}

  //  fprintf(stderr,"%p ",offset);
//		memcpy(bdata2+x4+y4,offset,3);
    *(bdata2+bidx)=*offset;
    *(bdata2+bidx+1)=*(offset+1);
    *(bdata2+bidx+2)=*(offset+2);
    *(bdata2+bidx+3)=0;
  }  //end j
}  //end i
//scale image to FTControl



//fprintf(stderr,"before XCreateImage\n");
//display and bitmap must be same depth (bits per pixel)?
//XCreateImage does not allocate space for the image itself
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(nw,y,1,32,bdata2);
#endif

//twin->image[0]=tximage;
//and DrawFTWindow?
//DrawFTWindow(twin);

//can free bdata and bdata2 - does XCreateImage do its own malloc?
//yes I think so, use XDestroyImage to free XImage
//but free(bdata2) causes X crashes
//free(bdata); //presumes bdata was malloc'd
//free(bdata2); //free original image

//how and when can we free bdata2?


if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapToFTWindow\n");
}

//    fprintf(stderr,"Got Image %p\n",img);
//return(tximage);
return(1);



} //LoadBitmapToFTWindow



//todo: Needs to create BITMAPFILEHEADER assign to twin->image[0], BITMAPINFOHEADER assign to twin->bmpinfo

//LoadBitmapDataToFTWindow() - Loads Bitmap data to window (expects bmfile to have no header info as LoadBitmapToFTWindow does).
//currently only for 24 bit images
//ResizeWindow=0 resize bmp to fit window, 1= resize window to fit bmp
//possibly change ResizeWindow to flags or Settings and (FT_LoadBitmap_RESIZEWINDOW)
//possibly use sws_scale to scale BMP
unsigned char * LoadBitmapDataToFTWindow(unsigned char *bmpdata,FTWindow *twin,int ResizeWindow,int width,int height,int depth)
{
//BITMAPFILEHEADER *bmf;
//BITMAPINFOHEADER *bmi;
//FILE *fptr;
unsigned char *bdata2;//,*bdata;
int nw,i,j,bc,bidx;
//XImage *img;
unsigned char *offset;
int x,y;
float rx,ry;

//add scaling (rx,ry)


if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapDataToFTWindow\n");
}

if (bmpdata==0 || twin<=0) {
  fprintf(stderr,"Error: LoadBitmapDataToFTWindow passed NULL pointer\n"); 
	return 0;
}




//fprintf(stderr,"w=%d h=%d\n",bmi->biWidth,bmi->biHeight);
//fprintf(stderr,"bitcount=%d\n",bmi->biBitCount);
//fprintf(stderr,"remainder=%f \n",fmodf(7.0,4.0));
//fprintf(stderr,"remainder=%f \n",fmodf((float)bmi.biWidth,8.0));

bc=depth/8;

nw=(int)fmodf((float)width*bc,4.0);
if (nw>0) {
	nw=width+4-nw;
} else {
  nw=width;
}

if (ResizeWindow) {
	if (twin->w!=width || twin->h!=height) {
		twin->w=width;
  		twin->h=height;
#if Linux
		if (twin->flags&WOpen && twin->xwindow!=0) {
#endif
#if WIN32
		if (twin->flags&WOpen && twin->hwindow!=0) {
#endif
			//we need to resize an existing window
			ResizeFTWindow(twin);
		} //WOpen

	}
}

//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=width;
  twin->h=height;
}

//twin at 32 bpp should always be 4-byte aligned - but could be a problem on other display resolutions
x=twin->w;
y=twin->h;


//if (!ResizeWindow) {
	//rx=(float)nw/((float)twin->x*(float)bc);
	//1.0 so no overlap
	rx=((float)nw-1.0)/(float)x;
	ry=((float)height-1.0)/(float)y;

//fprintf(stderr,"rx=%f ry=%f w=%d h=%d\n",rx,ry,x,y);
//fprintf(stderr,"w=%d h=%d\n",nw,bmi.biHeight);
//fprintf(stderr,"depth=%d\n",depth);
//} 

//bdata=bmfile+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//  fprintf(stderr,"depth=%d\n",depth);

//important nw includes *3
//display is 4 bpp
//should make sure new bitmap will be 32 bit aligned

if (twin->bmpdata!=0) {
	free(twin->bmpdata); //free any previous mallc'd bitmap data
}
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
twin->bmpdata=bdata2; //store in order to track to free
//memset(bdata2,0,sizeof(x*y*4));


//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);

//#if PINFO
//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
//#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work
		//offset=bdata;
//do 8 and 16 bit seperately

switch(depth) {
	case 8:	//8 bit
		for(i=0;i<y;i++) {
					//  fprintf(stderr,"\n\n");
					for(j=0;j<x;j++) {
						bidx=i*x*4+j*4;
				
						if (!ResizeWindow)
							offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
						else 
							offset=bmpdata+i*nw*bc+j*bc;

					//  fprintf(stderr,"%p ",offset);
				//		memcpy(bdata2+x4+y4,offset,3);
						*(bdata2+bidx)=*offset;
						*(bdata2+bidx+1)=*(offset);
						*(bdata2+bidx+2)=*(offset);
				    *(bdata2+bidx+3)=0;
					}  //end j
				}  //end i
				//scale image to FTControl
		break;
	case 16: //16 bit
		for(i=0;i<y;i++) {
			//  fprintf(stderr,"\n\n");
			for(j=0;j<x;j++) {
				bidx=i*x*4+j*4;
				
				if (!ResizeWindow)
					offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
				else 
					offset=bmpdata+i*nw*bc+j*bc;

			//  fprintf(stderr,"%p ",offset);
		//		memcpy(bdata2+x4+y4,offset,3);
					//presumes 454
				*(bdata2+bidx)=*offset&0xf;
				*(bdata2+bidx+1)=((*(offset)&0xd0)>>5)|((*(offset+1)&0x3)<<5);
				*(bdata2+bidx+2)=((*(offset+1)&0xfc)>>2);
		    *(bdata2+bidx+3)=0;
			}  //end j
		}  //end i
		//scale image to FTControl
		break;
	case 24:
	case 32:

		for(i=0;i<y;i++) {
			//  fprintf(stderr,"\n\n");
			for(j=0;j<x;j++) {
				bidx=i*x*4+j*4;
				
				if (!ResizeWindow)
					offset=bmpdata+((int)(i*ry))*nw*bc+(((int)(j*rx))*bc);
				else 
					offset=bmpdata+i*nw*bc+j*bc;

			//  fprintf(stderr,"%p ",offset);
		//		memcpy(bdata2+x4+y4,offset,3);
				*(bdata2+bidx)=*offset;
				*(bdata2+bidx+1)=*(offset+1);
				*(bdata2+bidx+2)=*(offset+2);
		    *(bdata2+bidx+3)=0;
			}  //end j
		}  //end i
		//scale image to FTControl
		break; //24,32
	} //switch


//fprintf(stderr,"before XCreateImage\n");
//display and bitmap must be same depth (bits per pixel)?
//XCreateImage does not allocate space for the image itself
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,nw*4);
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(nw,y,1,32,bdata2);
#endif

//if (twin->image[0]!=0) {
//	XDestroyImage(twin->image[0]);  //causes segfault
//}

//and DrawFTWindow?
//DrawFTWindow(twin);

//can free bdata and bdata2 - does XCreateImage do its own malloc?
//yes I think so, use XDestroyImage to free XImage
//but free(bdata2) causes X crashes
//free(bdata); //presumes bdata was malloc'd
//free(bdata2); //free original image


//Presumably DrawWindow will draw image
//perhaps DrawFTWindow should be called here

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapDataToFTWindow\n");
}

return(bdata2);
} //LoadBitmapDataToFTWindow


//currently only for 24 bit images
int LoadBitmapFileToFTWindow(char *filename,FTWindow *twin)
{
BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*bmp;
int PaddedBitmapWidth,PaddedWindowBitmapWidth,i,j,BitmapDepthInBytes,WindowDepthInBytes,bmphsize;
//XImage *img;
unsigned char *offset;
int x,y,bidx;
float rx,ry;
char tstr[FTMedStr];


//NOTE THIS CODE IS INACCURATE - remove rx,ry - fix nw and loop

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadBitmapFileToFTWindow %s\n",filename);
}

fptr=fopen(filename,"rb");
if (fptr==0) {
	fprintf(stderr,"Error opening %s\n",filename);
	sprintf(tstr,"Error opening %s\n",filename);
	FTMessageBox(tstr,FTMB_OK,"Error",0);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"End LoadBitmapFileToFTWindow\n");
	}
	return(0);
}


//fread(&bmf,sizeof(BITMAPFILEHEADER),1,fptr);
fread(&bmf.bfType,2,1,fptr);
if (bmf.bfType!=0x4d42) {
  fprintf(stderr,"%s does not start with 0x4d42.\n",filename);
  return 0;
}
fread(&bmf.bfSize,4,1,fptr);
fread(&bmf.bfReserved1,2,1,fptr);
fread(&bmf.bfReserved2,2,1,fptr);
fread(&bmf.bfOffBits,4,1,fptr);
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
//fread(&bmi,sizeof(BITMAPINFOHEADER),1,fptr);
fread(&bmi.biSize,4,1,fptr);
fread(&bmi.biWidth,4,1,fptr);
fread(&bmi.biHeight,4,1,fptr);
fread(&bmi.biPlanes,2,1,fptr);
fread(&bmi.biBitCount,2,1,fptr);
fread(&bmi.biCompression,4,1,fptr);
fread(&bmi.biSizeImage,4,1,fptr);
fread(&bmi.biXPelsPerMeter,4,1,fptr);
fread(&bmi.biYPelsPerMeter,4,1,fptr);
fread(&bmi.biClrUsed,4,1,fptr);
fread(&bmi.biClrImportant,4,1,fptr);

BitmapDepthInBytes=bmi.biBitCount/8;
PaddedBitmapWidth=(int)fmodf((float)bmi.biWidth*BitmapDepthInBytes,4.0);
if (PaddedBitmapWidth>0)	{
	PaddedBitmapWidth=bmi.biWidth+4-PaddedBitmapWidth;
} else {
  PaddedBitmapWidth=bmi.biWidth;
}

//read data
bdata=(unsigned char *)malloc(PaddedBitmapWidth*bmi.biHeight);
fread(bdata,1,PaddedBitmapWidth*bmi.biHeight,fptr);
fclose(fptr);



//now determine bitmap for window padding
//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi.biWidth;
  twin->h=bmi.biHeight;
}

x=twin->w;
y=twin->h;

#if Linux
//XCreateImage only works for 8, 16, or 32 bit depths (not 24)
WindowDepthInBytes=4;
#endif
#if WIN32
WindowDepthInBytes=PStatus.depth/8;
#endif
PaddedWindowBitmapWidth=(int)fmodf((float)twin->x*WindowDepthInBytes,4.0);
if (PaddedWindowBitmapWidth>0)	{
	PaddedWindowBitmapWidth=twin->x+WindowDepthInBytes-PaddedWindowBitmapWidth;
} else {
  PaddedWindowBitmapWidth=twin->x*WindowDepthInBytes;
}

bmphsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
bmp=(unsigned char *)malloc(PaddedWindowBitmapWidth*y+bmphsize); 
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
twin->bmpdata=(unsigned char *)twin->bmpinfo+sizeof(BITMAPINFOHEADER);
bmf.bfSize=bmphsize+PaddedWindowBitmapWidth*y;
memcpy(bmp,(unsigned char *)&bmf,sizeof(BITMAPFILEHEADER));
bmi.biBitCount=PStatus.depth;
bmi.biSizeImage=PaddedWindowBitmapWidth*y;
memcpy(bmp+sizeof(BITMAPFILEHEADER),(unsigned char *)&bmi,sizeof(BITMAPINFOHEADER));
bdata2=bmp+bmphsize;

//rx=(float)PaddedBitmapWidth/((float)x*(float)bc);
//ry=(float)bmi.biHeight/(float)y;
rx=(float)PaddedBitmapWidth/(float)PaddedWindowBitmapWidth;
ry=(float)bmi.biHeight/(float)y;

//fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
#if PINFO
fprintf(stderr,"%f %f %d %d\n",rx,ry,x,y);
#endif
//fprintf(stderr,"%p\n",bdata);
//note:  depth is 24, but PutImage needs 32 bit, I could only get depth=4 to work


//for(i=y-1;i>-1;i--) {
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
	//for(j=x-1;j>-1;j--) {
//    offset=bdata+((int)((y-i-1)*ry))*PaddedBitmapWidth+((int)(j*rx))*bc;
  //  fprintf(stderr,"%p ",offset);
//    *(bdata2+i*x*4+j*4)=*offset;
//    *(bdata2+i*x*4+j*4+1)=*(offset+1);
//    *(bdata2+i*x*4+j*4+2)=*(offset+2);
		bidx=(i*x+j)*WindowDepthInBytes;
    
		offset=bdata+((int)((float)(y-i-1)*ry))*PaddedBitmapWidth*BitmapDepthInBytes+(((int)((float)j*rx))*BitmapDepthInBytes);

  //  fprintf(stderr,"%p ",offset);
    //*(bdata2+x4+y4)=*offset;
    //*(bdata2+x4+y4+1)=*(offset+1);
    //*(bdata2+x4+y4+2)=*(offset+2);
		*(bdata2+bidx)=*offset;
		*(bdata2+bidx+1)=*(offset+1);
		*(bdata2+bidx+2)=*(offset+2);
		*(bdata2+bidx+3)=0;

  }  //end j
}  //end i
//scale image to FTControl


//display and bitmap must be same depth (bits per pixel)?
#if Linux
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,WindowDepthInBytes,PaddedWindowBitmapWidth);
#endif
#if WIN32
twin->hbmp= CreateBitmap(PaddedWindowBitmapWidth,y,1,WindowDepthInBytes,bdata2);
#endif

free(bdata); //free original image


//    fprintf(stderr,"Got Image %p\n",img);
if (PStatus.flags&PInfo) {
  fprintf(stderr,"End LoadBitmapFileToFTWindow\n");
}

return 1;
} //LoadBitmapFileToFTWindow




//NOTE: I was initially going to convert all JPG encoding/decoding to FFMPEG Libraries 
//but libjpeg will function for compressing/decompressing jpg and making and reading mjpg files
#if 0 
//currently only for 24 bit images
XImage * FT_LoadJPGFile(char *filename)
{
//BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int i,j;
//int nw,i,j,bc;
//XImage *img;
//unsigned char *offset;
//float rx,ry;
int x,y;
//JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
//int image_height;	/* Number of rows in image */
//int image_width;		/* Number of columns in image */
JSAMPARRAY buffer;		/* Output row buffer */
//JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
int row_stride;		/* physical row width in image buffer */
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
//struct jpeg_decompress_struct cinfo;
struct jpeg_decompress_struct dinfo;
//struct my_error_mgr jerr;
struct jpeg_error_mgr jerr;
//typedef struct my_error_mgr *my_error_ptr;
//struct my_error_mgr           jerr;
XImage *tximage;


if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow %s\n",filename);


if ((fptr = fopen(filename, "rb")) == NULL) {
            fprintf(stderr, "can't open %s\n", filename);
            return(0);
        }



	dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
//fprintf(stderr,"before jpeg_stdio_src\n");
        dinfo.output_components = 3;
        dinfo.out_color_space = JCS_RGB;

        dinfo.dct_method = JDCT_FASTEST;  //need?

        jpeg_stdio_src(&dinfo, fptr);
//fprintf(stderr,"before read header\n");
        (void) jpeg_read_header(&dinfo, TRUE);
//fprintf(stderr,"before decompress\n");
        jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width * dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);


  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*dinfo.mem->alloc_sarray)
		((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//   buffer=malloc(x*4);



//fprintf(stderr,"before while\n");
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (dinfo.output_scanline < dinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&dinfo, buffer, 1);
//    (void) jpeg_read_scanlines(&cinfo, bdata+cinfo.output_scanline*x, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//    put_scanline_someplace(buffer[0], row_stride);
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
    memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
  }

   jpeg_finish_decompress(&dinfo);

   fclose(fptr);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

memset(&bmi,0,sizeof(BITMAPINFOHEADER));
//only need width and height
bmi.biWidth=x;
bmi.biHeight=y;

//if (twin->bmpinfo!=0) free(twin->bmpinfo);
//twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));

//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));
//
//scale image to contorl
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    *(bdata2+i*x*4 + j*4+2)=*(bdata+i*x*3 + j*3);
    *(bdata2+i*x*4 + j*4+1)=*(bdata+i*x*3 + j*3+1);
    *(bdata2+i*x*4 + j*4)=*(bdata+i*x*3 + j*3+2);
    *(bdata2+i*x*4 + j*4+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?
/*
     The XCreateImage function allocates the memory needed for an XImage
       structure for the specified display but does not allocate space for the
       image itself.  Rather, it initializes the structure byte-order, bit-
       order, and bitmap-unit values from the display and returns a pointer to
       the XImage structure.
*/
tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
/*
      Note that when the image is created using XCreateImage, XGetImage, or
       XSubImage, the destroy procedure that the XDestroyImage function calls
       frees both the image structure and the data pointed to by the image
       structure.
*/
free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);


//}//end if bitmap file found
if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow complete\n");

return(tximage);

}  //end FT_LoadJPGFile
#endif


//currently only for 24 bit images
int LoadJPGFileToFTWindow(char *filename,FTWindow *twin)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
FILE *fptr;
unsigned char *bdata,*bdata2,*offset,*offset2;
int i,j;
int x,y,bmpsize;
//JSAMPARRAY buffer;		/* Output row buffer */
int row_stride;		/* physical row width in image buffer */
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
struct jpeg_decompress_struct dinfo;
struct jpeg_source_mgr mgr;
struct jpeg_error_mgr jerr;
int numbytes,jpglen,ex;
unsigned char *jpgdata,*bmp;
char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadJPGFileToFTWindow %s\n",filename);
}


if ((fptr=fopen(filename,"rb")) == NULL) {
	sprintf(tstr,"LoadJPGFileToFTWindow error: %s could not be opened.\n",filename);
	FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);

    fprintf(stderr,"Can't open %s\n",filename);
    return(0);
    }

//read in JPEG data
jpglen=0;
numbytes=0;
ex=0;
jpgdata=(unsigned char *)malloc(FT_MAXJPGSIZE);
while(!ex) {
	numbytes=fread(jpgdata+jpglen,1,512,fptr);
	if (numbytes<=0) {
		ex=1;
	} else {
		jpglen+=numbytes;
	}
} //while !ex
fclose(fptr);


PStatus.jpgbufdata=jpgdata;
PStatus.jpgbufsize=jpglen;


memset(&dinfo,0,sizeof(dinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));


  jpeg_create_decompress(&dinfo);

	mgr.next_input_byte=jpgdata;
  mgr.bytes_in_buffer=jpglen;
  mgr.init_source=mem_init_source;
  mgr.fill_input_buffer=mem_fill_input_buffer;
  mgr.skip_input_data=mem_skip_input_data;
  mgr.resync_to_restart=mem_resync_to_restart;
  mgr.term_source=mem_term_source;
//fprintf(stderr,"before jpeg_stdio_src\n");

	dinfo.err=jpeg_std_error(&jerr);
	dinfo.src=&mgr;

	dinfo.output_components = 3;
  dinfo.out_color_space = JCS_RGB;
	dinfo.dct_method = JDCT_FASTEST;  //need?

	//jpeg_stdio_src(&dinfo,fptr);
//fprintf(stderr,"before read header\n");
	jpeg_read_header(&dinfo,TRUE);
//fprintf(stderr,"before decompress\n");
  jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width*dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);  //*4 for word-align padding

	for(j=0;j<y;j++) {
		offset=bdata+x*j*3;
		jpeg_read_scanlines(&dinfo,(JSAMPARRAY)&offset,1);
	}  //for j


  /* Make a one-row-high sample array that will go away when done with image */
  //buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//fprintf(stderr,"before while\n");
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
//  while (dinfo.output_scanline < dinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
//    jpeg_read_scanlines(&dinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
 //   memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
 // }

   jpeg_finish_decompress(&dinfo);
	 jpeg_destroy_decompress(&dinfo);
   
	 free(jpgdata);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

//fprintf(stderr,"jpg x=%d y=%d\n",x,y);
bmp=(unsigned char *)malloc(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+x*y*4);
twin->image=bmp;
twin->bmpinfo=(BITMAPINFOHEADER *)(twin->image+sizeof(BITMAPFILEHEADER));
//add pointer to bitmap data to FTWindow data structure
twin->bmpdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));


bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+x*y*4;
//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=x;
bmi->biHeight=y;
bmi->biPlanes=1;
bmi->biBitCount=32;
bmi->biSizeImage=4*x*y;

bdata2=bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);



//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi->biWidth;
  twin->h=bmi->biHeight;
}

//if (twin->bmpinfo!=0) {
//	free(twin->bmpinfo);
//}
//twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//copy bitmap info header to FTControl
//memcpy(twin->bmpinfo,bmi,sizeof(BITMAPINFOHEADER));

//scale image to control

//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
		offset=bdata+(i*x + j)*3;
		offset2=bdata2+(i*x + j)*4;
    *(offset2+2)=*bdata;
    *(offset2+1)=*(bdata+1);
    *(offset2)=*(bdata+2);
    *(offset2+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?
/*
     The XCreateImage function allocates the memory needed for an XImage
       structure for the specified display but does not allocate space for the
       image itself.  Rather, it initializes the structure byte-order, bit-
       order, and bitmap-unit values from the display and returns a pointer to
       the XImage structure.
*/
#if Linux
//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
twin->ximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);
#endif
#if WIN32
twin->hbmp= CreateBitmap(x,y,1,32,bdata2);
//twin->hbmp=CreateDIBSection(twin->hdcWin,(BITMAPINFO *)twin->bmpinfo,DIB_RGB_COLORS,(void **)bdata2,NULL,NULL);
//SelectObject(twin->hdcWin,
//twin->hbmp=CreateCompatibleBitmap(twin->hdcWin,x,y);
//SetDIflags(twin->hdcWin,twin->hbmp,0,y,bdata2,(BITMAPINFO *)twin->bmpinfo,DIB_RGB_COLORS);
#endif
//twin->image=bmp;
/*
      Note that when the image is created using XCreateImage, XGetImage, or
       XSubImage, the destroy procedure that the XDestroyImage function calls
       frees both the image structure and the data pointed to by the image
       structure.
*/
free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);



//}//end if bitmap file found
if (PStatus.flags&PInfo) {
  fprintf(stderr,"LoadJPGFileToFTWindow complete\n");
}

return(1);
//return(bmp);
}  //end LoadJPGFileToFTWindow


//need to convert to ffmpeg
//currently only for 24 bit images
int LoadJPGFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor)
{
	int numbytes, jpglen, ex;
	unsigned char *jpgdata;
	FILE *fptr;
	char tstr[FTMedStr];

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"LoadJPGFileToFTControl %s\n",filename);
	}

	if ((fptr = fopen(filename, "rb")) == NULL) {
		sprintf(tstr,"LoadJPGFileToFTWindow error: %s could not be opened.\n",filename);
		FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);
		fprintf(stderr, "can't open %s\n", filename);
		return(0);
		}

	//read in JPEG data
	jpglen=0;
	numbytes=0;
	ex=0;
	jpgdata=(unsigned char *)malloc(FT_MAXJPGSIZE);
	while(!ex) {
		numbytes=fread(jpgdata+jpglen,1,512,fptr);
		if (numbytes<=0) {
			ex=1;
		} else {
			jpglen+=numbytes;
		}
	} //while !ex
	fclose(fptr);

	LoadJPGToFTControl(jpgdata, jpglen, tcontrol, flags, mixcolor);

	free(jpgdata);

//    fprintf(stderr,"Got Image %p\n",img);

	if (PStatus.flags&PInfo) {
	  fprintf(stderr,"LoadJPGFileToFTControl complete\n");
	}

	return(1);
}  //end LoadJPGFileToFTControl


//load a jpg from memory to a control (currently only control->image[0])
int LoadJPGToFTControl(unsigned char *jpgdata, int jpglen, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor)
{
	BITMAPFILEHEADER *bmf;
	BITMAPINFOHEADER *bmi;
	unsigned char *bdata, *bdata2, *offset, *offsetCTL, *bmp;
	int i, j;
	int x, y, jpgx, jpgy, bc, bc2, bmpsize;
	//JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in image buffer */
	//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
	struct jpeg_decompress_struct dinfo;
	struct jpeg_source_mgr mgr;
	struct jpeg_error_mgr jerr;
	int numbytes, ex;
	float rx, ry;
	int ImageNum;
#if 0//WIN32
	DWORD dwWaitResult;
#endif


	/*
	if (!CheckJPG(jpgdata, jpglen)) {
	return(0);
	}
	*/

	//we need to lock the DrawFTControls mutex so that we are not writing over image data that is currently being drawn
#if 0//WIN32
	//request the mutex
	dwWaitResult = WaitForSingleObject(
		PStatus.DrawFTControlsMutex,    // handle to mutex
		INFINITE);  // no time-out interval

	if (dwWaitResult != WAIT_OBJECT_0) {
		return;
	}
#endif


	PStatus.jpgbufdata = jpgdata;
	PStatus.jpgbufsize = jpglen;


	memset(&dinfo, 0, sizeof(dinfo));
	memset(&jerr, 0, sizeof(jerr));
	memset(&mgr, 0, sizeof(mgr));


	jpeg_create_decompress(&dinfo);

	mgr.next_input_byte = jpgdata;
	mgr.bytes_in_buffer = jpglen;
	mgr.init_source = mem_init_source;
	mgr.fill_input_buffer = mem_fill_input_buffer;
	mgr.skip_input_data = mem_skip_input_data;
	mgr.resync_to_restart = mem_resync_to_restart;
	mgr.term_source = mem_term_source;

	dinfo.err = jpeg_std_error(&jerr);
	dinfo.src = &mgr;

	dinfo.output_components = 3;
	dinfo.out_color_space = JCS_RGB;
	dinfo.dct_method = JDCT_FASTEST;  //need?

	//jpeg_stdio_src(&dinfo,fptr);
	//fprintf(stderr,"before read header\n");
	jpeg_read_header(&dinfo, TRUE);
	//fprintf(stderr,"before decompress\n");
	jpeg_start_decompress(&dinfo);

	row_stride = dinfo.output_width*dinfo.output_components;
	//fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

	jpgx = dinfo.output_width;
	jpgy = dinfo.output_height;
	bdata = (unsigned char *)malloc(jpgx*jpgy * 4);  //just to take care of word align padding

	for (j = 0; j < jpgy; j++) {
		offset = bdata + jpgx*j * 3;
		jpeg_read_scanlines(&dinfo, (JSAMPARRAY)&offset, 1);
	}  //for j


	/* Make a one-row-high sample array that will go away when done with image */
	//buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

	//fprintf(stderr,"before while\n");
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	//  while (dinfo.output_scanline < dinfo.output_height) {
	/* jpeg_read_scanlines expects an array of pointers to scanlines.
	* Here the array is only one element long, but you could ask for
	* more than one scanline at a time if that's more convenient.
	*/
	//    jpeg_read_scanlines(&dinfo, buffer, 1);
	/* Assume put_scanline_someplace wants a pointer and sample count. */
	//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
	//  fprintf(stderr,"row_stride:%d\n",row_stride);
	//   memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
	//fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
	// }

	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);

	//free(jpgdata);

	x = tcontrol->x2 - tcontrol->x1;
	y = tcontrol->y2 - tcontrol->y1;

	ImageNum = flags & 0x3;
	//fprintf(stderr,"jpg x=%d y=%d\n",x,y);
	bmp = (unsigned char *)malloc(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y * 4);
	tcontrol->image[ImageNum] = bmp;
	tcontrol->bmpinfo[ImageNum] = (BITMAPINFOHEADER *)(bmp + sizeof(BITMAPFILEHEADER));
	//add pointer to bitmap data to control data structure
	tcontrol->bmpdata[ImageNum] = (unsigned char *)(bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	bmf = (BITMAPFILEHEADER *)bmp;
	bmi = (BITMAPINFOHEADER *)(bmp + sizeof(BITMAPFILEHEADER));
	bmpsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y * 4;
	//memset(bmf,0,sizeof(BITMAPFILEHEADER));
	bmf->bfType = 0x4d42;
	bmf->bfSize = bmpsize;
	bmf->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	memset(bmi, 0, sizeof(BITMAPINFOHEADER));
	//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
	bmi->biSize = sizeof(BITMAPINFOHEADER);
	bmi->biWidth = x;
	bmi->biHeight = y;
	bmi->biPlanes = 1;
	bmi->biBitCount = 32;
	bmi->biSizeImage = 4 * x*y;

	bdata2 = bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Note: jpeglib does not byte-align RGB data

	bc = 3; //jpg is always 3 bytes/pixel bmi->biBitCount/8;
	/*
	nw=(int)fmodf((float)jpgx*bc,4.0);
	if (nw>0)  {
	nw=jpgx*bc+4-nw;
	} else {
	nw=jpgx*bc;
	}
	*/

	bc2 = 4;
	//nw2=x*4;

	//  if (cx==0 && y==0) //scale to FTControl

	//scale image to FTControl
	//rx=(float)nw/((float)jx*(float)bc);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)nw/(float)(jx*bc2);
	//ry=(float)bmi.biHeight/(float)jy;
	//rx=(float)(jx*bc2)/(float)nw;
	//rx=(float)nw2/(float)nw;
	//if (ResizeImage) {
	if (!(flags&FT_SCALE_IMAGE)) {
		rx = (float)jpgx / (float)x;
		ry = (float)jpgy / (float)y;//bmi.biHeight;
	}
	else {
		rx = 1.0;
		ry = 1.0;
		//if control is bigger than jpg
		if (x > jpgx) {
			x = jpgx;
		}
		if (y > jpgy) {
			y = jpgy;
		}
	}


	//todo: make a function for below and call - see FTLoadBitmapFileToControl


	if (flags&FT_SHADE_IMAGE) {
		//windows can stretchblt, if xwindows can too, then this is unnecessary
		//have to copy to 32 bit, even though display is 24 bit
		for (i = 0; i < y; i++) {
			//  fprintf(stderr,"\n\n");
			for (j = 0; j < x; j++) {
				offsetCTL = bdata2 + (i*x + j)*bc2;
				offset = bdata + (((int)(i*ry))*jpgx + ((int)(j*rx)))*bc;
				//offset=bdata+(i*jpgx+j)*bc;
				//  fprintf(stderr,"%p ",offset);
				*(offsetCTL) = (*offset + (mixcolor & 0x000000ff)) / 2;
				*(offsetCTL + 1) = (*(offset + 1) + ((mixcolor & 0x0000ff00) >> 8)) / 2;
				*(offsetCTL + 2) = (*(offset + 2) + ((mixcolor & 0x00ff0000) >> 16)) / 2;
				*(offsetCTL + 3) = 0;
			}  //j
		}  //i
	}
	else {
		for (i = 0; i < y; i++) {
			//  fprintf(stderr,"\n\n");
			for (j = 0; j < x; j++) {
				offsetCTL = bdata2 + (i*x + j)*bc2;
				offset = bdata + (((int)(i*ry))*jpgx + ((int)(j*rx)))*bc;
				//offset=bdata+(i*jpgx+j)*bc;
				//  fprintf(stderr,"%p ",offset);
				*offsetCTL = *(offset + 2);
				*(offsetCTL + 1) = *(offset + 1);
				*(offsetCTL + 2) = *offset;
				*(offsetCTL + 3) = 0;
			}  //j
		}  //i
	} //if (flags&FT_SHADE_IMAGE) {


	//add border
	if (flags&FT_ADD_BORDER_TO_IMAGE) {
		for (i = 0; i < y; i++) {
			for (j = 0; j < x; j++) {
				if (i == 0 || i == y - 1 || j == 0 || j == x - 1) {
					offsetCTL = bdata2 + (i*x + j)*bc2;
					*(unsigned int *)offsetCTL = mixcolor;
				} //if (i==0 || i==y-1 || j==0 || j==x-1) {
			}  //end j
		}  //end i

	} //if (flags&FT_ADD_BORDER_TO_IMAGE) {

	//add dashed-border
	if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {

	} //if (flags&FT_ADD_DASHED_BORDER_TO_IMAGE) {



	//display and bitmap must be same depth (bits per pixel)?
	/*
	The XCreateImage function allocates the memory needed for an XImage
	structure for the specified display but does not allocate space for the
	image itself.  Rather, it initializes the structure byte-order, bit-
	order, and bitmap-unit values from the display and returns a pointer to
	the XImage structure.
	*/

#if Linux
	//tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *)bdata2,x,y,32,x*4);
	if (tcontrol->ximage[ImageNum]!=0) {
		XDestroyImage(tcontrol->ximage[ImageNum]);
	}
	tcontrol->ximage[ImageNum] = XCreateImage(PStatus.xdisplay, PStatus.visual, PStatus.depth, ZPixmap, 0, (char *)bdata2, x, y, 32, x * 4);
#endif
#if WIN32
	//free any existing image
	if (tcontrol->hbmp[ImageNum] != 0) {
		DeleteBitmap(tcontrol->hbmp[ImageNum]);
	}

	tcontrol->hbmp[ImageNum] = CreateBitmap(x, y, 1, 32, bdata2);
	//tcontrol->hbmp[ImageNum]=CreateDIBSection(PStatus.iwindow->hdcWin,(BITMAPINFO *)tcontrol->bmpinfo,DIB_RGB_COLORS,(void **)bdata2,NULL,NULL);
#endif
	//tcontrol->image=bmp;

	
#if 0//WIN32
	//end wait for mutex
	// Release ownership of the mutex object
	if (!ReleaseMutex(PStatus.DrawFTControlsMutex))
	{
		// Handle error.
	}
#endif //WIN32
	


	/*
	Note that when the image is created using XCreateImage, XGetImage, or
	XSubImage, the destroy procedure that the XDestroyImage function calls
	frees both the image structure and the data pointed to by the image
	structure.
	*/
	free(bdata); //free original image


	if (PStatus.flags&PInfo) {
		fprintf(stderr, "LoadJPGToFTControl complete\n");
	}

	return(1);
} //int LoadJPGToFTControl(char *ImageData, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor)

int CheckJPG(unsigned char *jpgdata, int jpglen)
{
	int i,endloop,len;

	//verify that jpgdata ends with FF D9
	if (jpglen <= 0) {
		fprintf(stderr, "Jpeg length <= 0");
		return(0);
	}

	//verify that jpgdata ends with FF D9
	if (jpgdata[0] != 0xff || jpgdata[1] != 0xd8) {
		fprintf(stderr, "Jpeg doesn't start with ffd8");
		return(0);
	}

	//verify that jpgdata ends with FF D9
	if (jpgdata[jpglen - 1] != 0xd9 || jpgdata[jpglen - 2] != 0xff) {
		fprintf(stderr, "Jpeg doesn't end with ffd9");
		return(0);
	}

	//check other markers
	i = 2;
	endloop = 0;
	while (!endloop) {
		switch (jpgdata[i]) {
		case 0xff: //marker
			switch (jpgdata[i + 1]) {
			case 0x00: //
			case 0x01: //
			case 0xd0: // 0xdn n=0-7 restart
			case 0xd1: //
			case 0xd2: //
			case 0xd3: //
			case 0xd4: //
			case 0xd5: //
			case 0xd6: //
			case 0xd7: //
				i+=2; //skip- no length
				break;
			case 0xd8: //start of image
				return(0); //two SOI markers
				break;
			case 0xdd: //DRI- payload=four bytes
				i += 5;
			case 0xd9: //EOI
				return(1);
				break;
			case 0xff: //every marker can be padded with any number of 0xff's
				i += 1; //skip ahead one for any 0xffff
				break;
			case 0xda: //start of scan
				//SOS, start of scan, 0xffda is followed by header length, but the size of the entropy data is not given
				//and there is no way I currently know of to verify the length of the "entropy" data
				//for now just exit once ffda is found
				/* I found that the below code, doesn't catch the jpeg library "premature end of data segment" popup window
				//for now scan to the end of the file, just to check against the known file length
				while (!endloop) {
					if (i+1 > jpglen) {
						return(0);
					}
					if (jpgdata[i] == 0xff && jpgdata[i + 1] == 0xd9) {
						if (i + 2 == jpglen) {
							return(1);
						}
						else {
							//ffd9 can be in the image data
							if (i+1 < jpglen) {
								i++;
							}
							else { //but if we are past the jpeglen
								return(0); //length scanned doesn't match file size
							}
						}
					}
					else {
						i++;
					} //if (jpgdata[i] = 0xff && jpgdata[i + 1] == 0xd9) {
				} //while
				*/
				return(1);
				break;
			default:

				//get length of marker block - is Big Endian
				len = (unsigned short int) ((jpgdata[i + 2] << 8) | jpgdata[i + 3]);
				i += 2 + len; //move to the next marker (header length includes 2 len bytes)
				
				break;
			} //switch (jpgdata[i + 1]) {
			break;
		default:
			fprintf(stderr, "JPG file error: failed markers + sizes check");
			return(0);
		} //switch (jpgdata[i]) {
	}

	return(1);
} //int CheckJPG(unsigned char *jpgdata, int jpglen)

/*
//currently only for 24 bit images
//XImage * FT_LoadJPGFileToFTWindow(FTWindow *twin,char *filename)
XImage * FT_LoadJPGFileToFTWindow(FTWindow *twin,unsigned char *jpg)
{
//BITMAPFILEHEADER bmf;
BITMAPINFOHEADER bmi;
FILE *fptr;
unsigned char *bdata,*bdata2;
int i,j;
//int nw,i,j,bc;
//XImage *img;
//unsigned char *offset;
//float rx,ry;
int x,y;
//JSAMPLE * image_buffer;	// Points to large array of R,G,B-order data 
JSAMPARRAY buffer;		// Output row buffer 
//JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s] 
int row_stride;		// physical row width in image buffer 
//This struct contains the JPEG decompression parameters and pointers to working space (which is allocated as needed by the JPEG library).
//struct jpeg_decompress_struct cinfo;
struct jpeg_decompress_struct dinfo;
//struct my_error_mgr jerr;
struct jpeg_error_mgr jerr;
//typedef struct my_error_mgr *my_error_ptr;
//struct my_error_mgr           jerr;
XImage *tximage;


if (PStatus.flags&PInfo)
  fprintf(stderr,"LoadJPGToFTWindow %s\n",filename);




	dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
//fprintf(stderr,"before jpeg_stdio_src\n");
        dinfo.output_components = 3;
        dinfo.out_color_space = JCS_RGB;

        dinfo.dct_method = JDCT_FASTEST;  //need?


//need to just send jpeg data directly
        jpeg_stdio_src(&dinfo, fptr);
//				dinfo
//fprintf(stderr,"before read header\n");
        (void) jpeg_read_header(&dinfo, TRUE);
//fprintf(stderr,"before decompress\n");
        jpeg_start_decompress(&dinfo);

  row_stride = dinfo.output_width * dinfo.output_components;
  //fprintf(stderr,"w=%d h=%d\n",dinfo.output_width,dinfo.output_height);

  x=dinfo.output_width;
  y=dinfo.output_height;
  bdata=(unsigned char *)malloc(x*y*4);


  // Make a one-row-high sample array that will go away when done with image 
  buffer = (*dinfo.mem->alloc_sarray)
		((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

//   buffer=malloc(x*4);



//fprintf(stderr,"before while\n");
  // Here we use the library's state variable cinfo.output_scanline as the
   // loop counter, so that we don't have to keep track ourselves.
   
  while (dinfo.output_scanline < dinfo.output_height) {
    // jpeg_read_scanlines expects an array of pointers to scanlines.
    // Here the array is only one element long, but you could ask for
     // more than one scanline at a time if that's more convenient.
     
    (void) jpeg_read_scanlines(&dinfo, buffer, 1);
//    (void) jpeg_read_scanlines(&cinfo, bdata+cinfo.output_scanline*x, 1);
    // Assume put_scanline_someplace wants a pointer and sample count. 
//    put_scanline_someplace(buffer[0], row_stride);
//fprintf(stderr,"scanline %d, rowstride=%d x=%d",dinfo.output_scanline,row_stride,x);
  //  fprintf(stderr,"row_stride:%d\n",row_stride);
    memcpy(bdata+(dinfo.output_scanline-1)*row_stride,buffer[0],row_stride);
  //fprintf(stderr,"%x%x%x ",buffer[0][0],buffer[0][1],buffer[0][2]);
  }

   jpeg_finish_decompress(&dinfo);

   fclose(fptr);

//fprintf(stderr,"after jpeg read\n");
//free(buffer);

memset(&bmi,0,sizeof(BITMAPINFOHEADER));
//only need width and height
bmi.biWidth=x;
bmi.biHeight=y;


//if this is a new window - set the dimensions to the bitmap dimensions
if (twin->w==0 || twin->h==0) {
  twin->w=bmi.biWidth;
  twin->h=bmi.biHeight;
}

if (twin->bmpinfo!=0) free(twin->bmpinfo);
twin->bmpinfo=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
//copy bitmap info header to FTControl
memcpy(twin->bmpinfo,&bmi,sizeof(BITMAPINFOHEADER));

//scale image to contorl
bdata2=(unsigned char *)malloc(x*y*4); //for now only use depth 3
//have to copy to 32 bit, even though display is 24 bit
for(i=0;i<y;i++) {
  //  fprintf(stderr,"\n\n");
  for(j=0;j<x;j++) {
    *(bdata2+i*x*4 + j*4+2)=*(bdata+i*x*3 + j*3);
    *(bdata2+i*x*4 + j*4+1)=*(bdata+i*x*3 + j*3+1);
    *(bdata2+i*x*4 + j*4)=*(bdata+i*x*3 + j*3+2);
    *(bdata2+i*x*4 + j*4+3)=0;
   }  //j
}  //i

//display and bitmap must be same depth (bits per pixel)?

//     The XCreateImage function allocates the memory needed for an XImage
//       structure for the specified display but does not allocate space for the
//       image itself.  Rather, it initializes the structure byte-order, bit-
//       order, and bitmap-unit values from the display and returns a pointer to
//       the XImage structure.

tximage=XCreateImage(PStatus.xdisplay,PStatus.visual,PStatus.depth,ZPixmap,0,(char *) bdata2,x,y,32,x*4);

//      Note that when the image is created using XCreateImage, XGetImage, or
//       XSubImage, the destroy procedure that the XDestroyImage function calls
//       frees both the image structure and the data pointed to by the image
//       structure.

free(bdata); //free original image

//    fprintf(stderr,"Got Image %p\n",img);


//}//end if bitmap file found
if (PStatus.flags&PInfo)
  fprintf(stderr,"FT_LoadJPGFileToFTWindow complete\n");

return(tximage);

}  //end FT_LoadJPGFileToFTWindow
*/

/*
//currently only for 24 bit images
//ResizeWindow=0 resize jpg to fit window, =1 resize window to fit jpg
void FT_LoadJPGToFTWindow(FTWindow *twin,unsigned char *jpg,int ResizeWindow)
{
if (PStatus.flags&PInfo)
  fprintf(stderr,"LoadJPGToFTWindow\n");


  if (twin!=0) {
		//initialize FFMPEG (AVLibs) if not done already
		if (!(twin->flags&WAVCodec_Init)) {
			//FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			if (ResizeWindow) {
				//determine jpg width and height
				//set windows new dimensions
				//twin->w=;
				FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			} else {
				FTInit_AVCodec(twin); //initialize AV Codec settings for this window
			}
		} //WAVCodec_Init
		//this 
		//ConvertFTWinBMPtoJPG(twin);
		//twin->jpgdata=jpg;
		ConvertFTWinJPGtoBMP(twin);

	} //twin!=0

//ConvertJPGToBMP
//LoadBMPToFTWindow


}  //end FT_LoadJPGToFTWindow
*/


void mem_init_source (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr = cinfo->src;
    mgr->next_input_byte = PStatus.jpgbufdata;
    mgr->bytes_in_buffer = PStatus.jpgbufsize;
    //printf("init %d\n", size - mgr->bytes_in_buffer);
}

boolean mem_fill_input_buffer (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr;

		mgr = cinfo->src;
    //printf("fill %d\n", PStatus.jpgbufsize - mgr->bytes_in_buffer);
    return 0;
}

void mem_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr* mgr;
		
		mgr = cinfo->src;
    //printf("skip %d +%d\n", PStatus.jpgbufsize - mgr->bytes_in_buffer, num_bytes);
    if(num_bytes<=0)
	return;
    mgr->next_input_byte += num_bytes;
    mgr->bytes_in_buffer -= num_bytes;
}

boolean mem_resync_to_restart (j_decompress_ptr cinfo, int desired)
{
    struct jpeg_source_mgr* mgr;

		mgr=cinfo->src;
    //printf("resync %d\n", size - mgr->bytes_in_buffer);
    mgr->next_input_byte = PStatus.jpgbufdata;
    mgr->bytes_in_buffer = PStatus.jpgbufsize;
    return 1;
}

void mem_term_source (j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr* mgr;
		mgr=cinfo->src;
    //printf("term %d\n", size - mgr->bytes_in_buffer);
}



#if Linux
#if USE_FFMPEG
//InitAVCodec
//initialize AVCodec part of FFMPEG - using the MJPEG encoder and decoder
//Each video may have a different size and so needs a different avcodec and avcodeccontext
void FTInit_AVCodec(FTWindow *twin) 
{
int width,height,depth;

/* find the mpeg1 video encoder */
//    codec_out = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
//fprintf(stderr,"CODEC_ID_MJPEG=%x\n",CODEC_ID_MJPEG);
//twin->avc = avcodec_find_encoder(CODEC_ID_MJPEG);
twin->avc = avcodec_find_decoder(CODEC_ID_MJPEG);
//codec_out = avcodec_find_encoder(CODEC_ID_NONE);
if (!twin->avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
//exit(1);
return;
}


//twin->avcc=avcodec_alloc_context3();
avcodec_alloc_context3(twin->avcc);


//need for each camera in which video will be saved or sent
//will get moved to iipvp[call]
twin->mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
twin->mjpeg_yuv= avcodec_alloc_frame();

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
twin->mjpeg_bmp->quality=1; 
twin->mjpeg_yuv->quality=1;

//camera dimensions must be defined before this
width=twin->w;
height=twin->h;
depth=PStatus.depth;//24;//

/* put sample parameters */
twin->avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
twin->avcc->width = width;
twin->avcc->height = height;
/* frames per second */
twin->avcc->time_base= (AVRational){1,30}; //{1,25};
twin->avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(twin->avcc,twin->avc) < 0) {
if (avcodec_open2(twin->avcc,twin->avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return;
} else {
	twin->flags|=WAVCodec_Init;
}



/* alloc image and output buffer */
twin->mjpeg_frame_size = width*height*(depth/8); //allocate full uncompressed image size
twin->mjpeg_frame = (uint8_t *)malloc(twin->mjpeg_frame_size);


//fprintf(stderr,"icam.swsctx set\n");
//yuv 2 1 1 16bpp = 44 4 4
//initialize sws_scale conversion
//adjust for PStatus.depth
twin->swsctx = sws_getContext(
width,  //source width, height, format
height,
PIX_FMT_BGR24,//PIX_FMT_BGR24, //PIX_FMT_RGB24, //source format  //to me still a mystery why BGR instead of RGB
width, //dest width, height, format
height,
PIX_FMT_YUV422P,  //dest format  
SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//image is backwards

if (twin->swsctx == 0) {
	fprintf(stderr,"Error initializing the Scaler conversion context.\n");
} //icam.swsctx

twin->mjpeg_yuv->data[0] =(uint8_t *) malloc(2*width*height); //
twin->mjpeg_yuv->data[1]=twin->mjpeg_yuv->data[0]+width*height; //not /2
twin->mjpeg_yuv->data[2]=twin->mjpeg_yuv->data[1]+width*height/2;
twin->mjpeg_yuv->linesize[0]=width;
twin->mjpeg_yuv->linesize[1]=width/2;
twin->mjpeg_yuv->linesize[2]=width/2;


//important to provide addresses to data[0],[1] and [2]
//I guess these are used even though [0][1][2] point to continuous memory
twin->mjpeg_bmp->data[0]=(uint8_t *)(twin->bmpdata);  //set bitmap data to AVFrame
twin->mjpeg_bmp->data[1]=0;
twin->mjpeg_bmp->data[2]=0;
twin->mjpeg_bmp->linesize[0] = width*3;//c->width;
twin->mjpeg_bmp->linesize[1] = 0;
twin->mjpeg_bmp->linesize[2] = 0;

} //FTInit_AVCodec
#endif //USE_FFMPEG
#endif //Linux


#if Linux
#if USE_FFMPEG
//This converts the jpg attached to a twin to the bmp in the twin structure
//this is the fastest method of drawing a jpg to a window
int ConvertFTWinJPGtoBMP(FTWindow *twin)
{
int len;
//FILE *fptr;

//possibly add sws_getContext here
//and malloc

if (twin!=0) {

//	fprintf(stderr,"sws_scale\n");
	//convert image data in bgr24 format to scaled yuv format (no header info is involved only image data)
	sws_scale(twin->swsctx,twin->mjpeg_bmp->data,twin->mjpeg_bmp->linesize,0,twin->h,twin->mjpeg_yuv->data,twin->mjpeg_yuv->linesize);


//this is avcodec_decode_video
//	fprintf(stderr,"avcodec_encode_video\n");
	//convert yuv to jpg image (I think this adds header info, huffman table, etc)
	//this returns a writable jpg image file in icam.mjpeg_frame
	len = avcodec_encode_video(twin->avcc,twin->mjpeg_frame,twin->mjpeg_frame_size,twin->mjpeg_yuv);

//	fprintf(stderr,"converted %d bytes to JPEG\n",len);

	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

	return(len);
} //twin!=0
return(0);
} //ConvertFTWinBMPtoJPG(FTWindow *twin)


//ConvertJPGtoBMPmalloc
//possibly go back to libjpeg
//This converts a jpg file data to a bmp file data
//jpg is the jpg file, bmp includes header info, the bmp length is returned
//bmp is malloc'd in this function - otherwise jpg dimensions need to be passed
int ConvertJPGtoBMPmalloc(unsigned char *jpg,int jpglen,unsigned char **bmp)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmpdata;
int len,bmpsize;//,jpglen;
//FILE *fptr;
//int width,height,depth;
AVCodec *avc;  //format to save video calls - currently set to MJPEG
AVCodecContext *avcc;
struct SwsContext  *swsctx;
//uint8_t *sws_data;
uint8_t *sws_dest[3];
int sws_stride[3];
int got_picture;
AVFrame *mjpeg_bmp;  //
AVFrame *mjpeg_yuv; 
//unsigned short *jmarker;
AVPacket avpkt;


//need jpg size for decoder
//jpg should have a header with size, width, height but doesn't
//parse jpg until ffd9 to get size
//could get jpg size from ipvp packet data size



/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(jpg,1,jpglen,fptr);
	fclose(fptr);
	} //fptr!=0
*/


/*
fprintf(stderr,"Determining jpg size\n");
jmarker=(unsigned short *)jpg;
jpglen=0;
//no jpeg larger than 999,999,999 ~1GB
while(*jmarker!=0xd9ff && jpglen<999999999) {
	//fprintf(stderr,"%x ",*jmarker);
	jmarker++;
	jpglen+=2;
}

jpglen+=2;

if (jpglen>999999999) {
	fprintf(stderr,"Error: ConvertJPGtoBMP: JPG either has no ffd9 end marker, or is too large >1GB\n");
}
fprintf(stderr,"Got size %d\n",jpglen);
*/

/*
bmf=(BITMAPFILEHEADER *)bmp;
bmi=bmp+sizeof(BITMAPFILEHEADER);
bmpdata=bmi=sizeof(BITMAPINFOHEADER);
width=bmi.biWidth;
height=bmi.biHeight;
depth=bmi.biBitCount;
*/


//avcodec_init();
//avcodec_register_all();

avc = avcodec_find_decoder(CODEC_ID_MJPEG);
//avc = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO);
if (!avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
	//fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MPEG2VIDEO);
return(0);
}


//avcc=avcodec_alloc_context3();
avcodec_alloc_context3(avcc);

mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
mjpeg_yuv= avcodec_alloc_frame();


//need?
//if(avc->capabilities&CODEC_CAP_TRUNCATED)
//	avcc->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
mjpeg_bmp->quality=1; 
mjpeg_yuv->quality=1;

//memset(avcc,0,sizeof(AVCodecContext)); //need?

/* put sample parameters */
//avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
//avcc->width = width;
//avcc->height = height;
//jpg or bmp dimensions?
//just make larger than biggest possible jpg
//avcc->width = 320;//352;//320;//352;//640;//3000;//640;//352;//320;//640;//352;//320;//width;
//avcc->height = 240;//288;//240;//288;//480;//2000;//480;//288;//240;//480;//288;//240;//height;

/* frames per second */
//avcc->time_base= (AVRational){1,30}; //{1,25};

//icam.avcc->pix_fmt = PIX_FMT_NONE;
//avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
//icam.avcc->pix_fmt = PIX_FMT_YUV422P;  //this is correct format for some digital cameras
//icam.avcc->pix_fmt = PIX_FMT_YU4422P;  //this is correct format for some digital cameras

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(avcc,avc) < 0) {
if (avcodec_open2(avcc,avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return(0);
}

//possibly want to use:
//FF_BUFFER_TYPE_INTERNAL or FF_BUFFER_TYPE_USER (user deallocated buffer)
memset(&avpkt,0,sizeof(AVPacket));
avpkt.data=(uint8_t *)jpg;
avpkt.size=jpglen+FF_INPUT_BUFFER_PADDING_SIZE;


//   fprintf(stderr,"before avcc->width=%d height=%d\n",avcc->width,avcc->height);
//    len = avcodec_decode_video(avcc,mjpeg_yuv,&got_picture,(uint8_t *)jpg,jpglen);
	len = avcodec_decode_video2(avcc,mjpeg_yuv,&got_picture,&avpkt);

//decode_video2 adjusts avcc width and height - but it is better to know before what dimensions are

//	fprintf(stderr,"decode returns %d bytes\n",len);

//	if (mjpeg_yuv->pan_scan!=0) {
//		fprintf(stderr,"mjpeg_yuv->pan_scan->height=%d\n",mjpeg_yuv->pan_scan->height);
//	}
//	fprintf(stderr," linesize[0]=%d\n",mjpeg_yuv->linesize[0]);
	 // printf("after avcode_decode_video\n");
	  //printf("lossless=%d\n",mjpeg->ls);

//   fprintf(stderr,"after avcc->width=%d height=%d\n",avcc->width,avcc->height);

//	  fprintf(stderr,"mjpeg_yuv->linesize[0]=%d\n",mjpeg_yuv->linesize[0]);



	  //fprintf(stderr,"after avcode_decode_video len=0x%x (%d)\n",len,len);
	  if (got_picture) {
//	   fprintf(stderr,"got picture len=0x%x (%d)\n",len,len);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//    fprintf(stderr, "(avcod)c->pix_fmt=%x=%x(PIX_FMT_YUVJ422P)\n",c->pix_fmt,PIX_FMT_YUVJ422P);
	   //fprintf(stderr,"c->width %d c->height %d\n",c->width,c->height);

//static	int		    sws_context_flags_ = 0;
//SwsContext  *swsctx;
//uint8_t *sws_data;
//uint8_t *sws_dest[3];
//int sws_stride[3];

	switch(PStatus.depth) {
		case 24:
	//			PIX_FMT_YUVJ422P, //source format
	swsctx = sws_getContext(
				avcc->width,  //source
				avcc->height,
				PIX_FMT_YUV422P, //source format
				avcc->width,  //dest
				avcc->height,//c->height,
				PIX_FMT_BGR24,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
		break;
		case 32:
	//			PIX_FMT_YUVJ422P, //source format
	swsctx = sws_getContext(
				avcc->width,  //source
				avcc->height,
				PIX_FMT_YUV422P, //source format
				avcc->width,  //dest
				avcc->height,//c->height,
				PIX_FMT_RGB32,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
			break;
		default:
				fprintf(stderr,"Error: Unknown display depth.\n");
				return(0);
	} //switch
		

//image is backwards
	   
//fprintf(stderr,"after sws_getContext\n");

//remember to free at some point

bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(PStatus.depth/8)*avcc->width*avcc->height;
//*bmp = malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);
*bmp = (unsigned char*)malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);

bmf=(BITMAPFILEHEADER *)*bmp;
bmi=(BITMAPINFOHEADER *)(*bmp+sizeof(BITMAPFILEHEADER));
bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));




//sws_scale appears to return many zeros (empty header info)?

//uint8_t *dest[3]= {data, data+c->width*c->height, data+c->width*c->height*2};
//sws_dest[0]=bmpdata;
sws_dest[0]=*bmp;
sws_dest[1]=0;//sws_data+c->width*c->height;
sws_dest[2]=0;//sws_data+c->width*c->height*2;
//int stride[3]={c->width*3,0,0};  //put data in one channel w*3
sws_stride[0]=avcc->width*(PStatus.depth/8);
sws_stride[1]=0;
sws_stride[2]=0;
//void *testdst;


//printf("important: picture->data[0]=%x ls[0]=%d d[1]=%x l[1]=%d d[2]=%x l[2]=%d\n",picture->data[0],picture->linesize[0],picture->data[1],picture->linesize[1],picture->data[2],picture->linesize[2]);

//testdst=malloc(320*240*4);
//fprintf(stderr,"before sws_scale\n");
//fprintf(stderr,"&dst->data=%x =%x\n",&dst->data,dst->data);
//int sws_scale(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,
//              int srcSliceH, uint8_t* dst[], int dstStride[]){
//sws_scale( ctx, src->data, src->stride,0, src->height,dst->data, dst->stride );
//fprintf(stderr,"before sws_scale\n");
//can possibly write directly to bmpaddress if not reversed
//	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
//fprintf(stderr,"before sws_free\n");
	//free later sws_freeContext(swsctx);
//sws_scale apparently returns an empty header

//this is just data - needs header info to

//fprintf(stderr,"after sws_scale\n");

//return 1560 of 0s padding? 0x618+36 unknown bytes=1596 bytes

//*bmp+=1596; //cant keep because of malloc

bmf=(BITMAPFILEHEADER *)*bmp;
bmi=(BITMAPINFOHEADER *)(*bmp+sizeof(BITMAPFILEHEADER));

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=avcc->width;
bmi->biHeight=avcc->height;
bmi->biPlanes=1;
bmi->biBitCount=PStatus.depth;
bmi->biSizeImage=(PStatus.depth/8)*avcc->width*avcc->height;




	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

//add bmp header info?


/*
	//write bmp to file
	fptr=fopen("testjpg.bmp","wb");
	if (fptr!=0) {
//		fwrite(*bmp,1,len,fptr);
	fwrite(*bmp,1,bmpsize,fptr);
	fclose(fptr);
	} //fptr!=0
*/

	return(len);


		} else { //got_picture
	fprintf(stderr,"avcodec_decode_video2 did not return a picture\n");
} //got_picture


/*

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);
*/

return(0);

} //ConvertJPGtoBMPmalloc


//ConvertJPGtoBMP
//possibly go back to libjpeg
//This converts a jpg file data to a bmp file data
//jpg is the jpg file, bmp includes header info, the bmp length is returned
//bmp is not malloc'd in this function - so bmp needs to contain enough allocated memory
//dimensions are taken from jpg
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int *w,int *h, int *d)
//int ConvertJPGtoBMP(unsigned char *jpg,int jpglen,unsigned char *bmp)
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d)
//**bmp because bmp needs to change the address of bmp forward after the header
//int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char **bmp,int w,int h, int d)
int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d)
{
//BITMAPFILEHEADER *bmf;
//BITMAPINFOHEADER *bmi;
//unsigned char *bmpdata;
int len;//,bmpsize;//,jpglen;
//FILE *fptr;
//int width,height,depth;
AVCodec *avc;  //format to save video calls - currently set to MJPEG
AVCodecContext *avcc;
struct SwsContext  *swsctx;
//uint8_t *sws_data;
uint8_t *sws_dest[3];
int sws_stride[3];
int got_picture;
AVFrame *mjpeg_bmp;  //
AVFrame *mjpeg_yuv; 
//unsigned short *jmarker;
AVPacket avpkt;


//need jpg size for decoder
//jpg should have a header with size, width, height but doesn't
//parse jpg until ffd9 to get size
//could get jpg size from ipvp packet data size



/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(jpg,1,jpglen,fptr);
	fclose(fptr);
	} //fptr!=0
*/


/*
fprintf(stderr,"Determining jpg size\n");
jmarker=(unsigned short *)jpg;
jpglen=0;
//no jpeg larger than 999,999,999 ~1GB
while(*jmarker!=0xd9ff && jpglen<999999999) {
	//fprintf(stderr,"%x ",*jmarker);
	jmarker++;
	jpglen+=2;
}

jpglen+=2;

if (jpglen>999999999) {
	fprintf(stderr,"Error: ConvertJPGtoBMP: JPG either has no ffd9 end marker, or is too large >1GB\n");
}
fprintf(stderr,"Got size %d\n",jpglen);
*/

/*
bmf=(BITMAPFILEHEADER *)bmp;
bmi=bmp+sizeof(BITMAPFILEHEADER);
bmpdata=bmi=sizeof(BITMAPINFOHEADER);
width=bmi.biWidth;
height=bmi.biHeight;
depth=bmi.biBitCount;
*/


//avcodec_init();
//avcodec_register_all();

//add this to PStatus

avc = avcodec_find_decoder(CODEC_ID_MJPEG);
//avc = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO);
if (!avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
	//fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MPEG2VIDEO);
return(0);
}


//avcc=avcodec_alloc_context3();
avcodec_alloc_context3(avcc);

mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
mjpeg_yuv= avcodec_alloc_frame();


//need?
//if(avc->capabilities&CODEC_CAP_TRUNCATED)
//	avcc->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
mjpeg_bmp->quality=1; 
mjpeg_yuv->quality=1;

//memset(avcc,0,sizeof(AVCodecContext)); //need?

/* put sample parameters */
//avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
avcc->width = w;
avcc->height = h;
//jpg or bmp dimensions?
//just make larger than biggest possible jpg
//avcc->width = 320;//352;//320;//352;//640;//3000;//640;//352;//320;//640;//352;//320;//width;
//avcc->height = 240;//288;//240;//288;//480;//2000;//480;//288;//240;//480;//288;//240;//height;

/* frames per second */
//avcc->time_base= (AVRational){1,30}; //{1,25};

//icam.avcc->pix_fmt = PIX_FMT_NONE;
avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common or generic
//icam.avcc->pix_fmt = PIX_FMT_YUV422P;  //this is correct format for some digital cameras
//icam.avcc->pix_fmt = PIX_FMT_YU4422P;  //this is correct format for some digital cameras

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(avcc,avc) < 0) {
if (avcodec_open2(avcc,avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return(0);
}

//possibly want to use:
//FF_BUFFER_TYPE_INTERNAL or FF_BUFFER_TYPE_USER (user deallocated buffer)
memset(&avpkt,0,sizeof(AVPacket));
avpkt.data=(uint8_t *)jpg;
avpkt.size=jpglen+FF_INPUT_BUFFER_PADDING_SIZE;


//   fprintf(stderr,"before avcc->width=%d height=%d\n",avcc->width,avcc->height);
//    len = avcodec_decode_video(avcc,mjpeg_yuv,&got_picture,(uint8_t *)jpg,jpglen);
	len = avcodec_decode_video2(avcc,mjpeg_yuv,&got_picture,&avpkt);

//decode_video2 adjusts avcc width and height - but it is better to know before what dimensions are

//	fprintf(stderr,"decode returns %d bytes\n",len);

//	if (mjpeg_yuv->pan_scan!=0) {
//		fprintf(stderr,"mjpeg_yuv->pan_scan->height=%d\n",mjpeg_yuv->pan_scan->height);
//	}
//	fprintf(stderr," linesize[0]=%d\n",mjpeg_yuv->linesize[0]);
	 // printf("after avcode_decode_video\n");
	  //printf("lossless=%d\n",mjpeg->ls);

//   fprintf(stderr,"after avcc->width=%d height=%d\n",avcc->width,avcc->height);

//	  fprintf(stderr,"mjpeg_yuv->linesize[0]=%d\n",mjpeg_yuv->linesize[0]);



	  //fprintf(stderr,"after avcode_decode_video len=0x%x (%d)\n",len,len);
	  if (got_picture) {
//	   fprintf(stderr,"got picture len=0x%x (%d)\n",len,len);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//    fprintf(stderr, "(avcod)c->pix_fmt=%x=%x(PIX_FMT_YUVJ422P)\n",c->pix_fmt,PIX_FMT_YUVJ422P);
	   //fprintf(stderr,"c->width %d c->height %d\n",c->width,c->height);

//static	int		    sws_context_flags_ = 0;
//SwsContext  *swsctx;
//uint8_t *sws_data;
//uint8_t *sws_dest[3];
//int sws_stride[3];

	switch(PStatus.depth) {
		case 24:
	//			PIX_FMT_YUVJ422P, //source format
				swsctx = sws_getContext(
				w,  //source
				h,
				PIX_FMT_YUV422P, //source format
				w,  //dest
				h,//c->height,
				PIX_FMT_BGR24,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
		break;
		case 32:
	//			PIX_FMT_YUVJ422P, //source format
				swsctx = sws_getContext(
				w,  //source
				h,
				PIX_FMT_YUV422P, //source format
				w,  //dest
				h,//c->height,
				PIX_FMT_BGR32,////PIX_FMT_BGR24,  //dest format  - I thought I am using RGB
				SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
				);
			break;
		default:
				fprintf(stderr,"Error: Unknown display depth.\n");
				return(0);
	} //switch
		

//image is backwards
	   
//fprintf(stderr,"after sws_getContext\n");

//remember to free at some point

//bmpsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(PStatus.depth/8)*avcc->width*avcc->height;
//*bmp = malloc(bmpsize+FF_INPUT_BUFFER_PADDING_SIZE);

//bmf=(BITMAPFILEHEADER *)bmp;
//bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
//bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));




//sws_scale appears to return many zeros (empty header info)?

//uint8_t *dest[3]= {data, data+c->width*c->height, data+c->width*c->height*2};
//sws_dest[0]=bmpdata;
sws_dest[0]=bmp;
sws_dest[1]=0;//sws_data+c->width*c->height;
sws_dest[2]=0;//sws_data+c->width*c->height*2;
//int stride[3]={c->width*3,0,0};  //put data in one channel w*3
sws_stride[0]=w*(PStatus.depth>>3);
sws_stride[1]=0;
sws_stride[2]=0;
//void *testdst;


//printf("important: picture->data[0]=%x ls[0]=%d d[1]=%x l[1]=%d d[2]=%x l[2]=%d\n",picture->data[0],picture->linesize[0],picture->data[1],picture->linesize[1],picture->data[2],picture->linesize[2]);

//testdst=malloc(320*240*4);
//fprintf(stderr,"before sws_scale\n");
//fprintf(stderr,"&dst->data=%x =%x\n",&dst->data,dst->data);
//int sws_scale(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,
//              int srcSliceH, uint8_t* dst[], int dstStride[]){
//sws_scale( ctx, src->data, src->stride,0, src->height,dst->data, dst->stride );
//fprintf(stderr,"before sws_scale\n");
//can possibly write directly to bmpaddress if not reversed
//	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,avcc->height,sws_dest,sws_stride);
	sws_scale(swsctx,mjpeg_yuv->data,mjpeg_yuv->linesize,0,h,sws_dest,sws_stride);
//fprintf(stderr,"before sws_free\n");
	//free later sws_freeContext(swsctx);
//sws_scale apparently returns an empty header

//this is just data - needs header info to

//fprintf(stderr,"after sws_scale\n");

//return 1560 of 0s padding? 0x618+36 unknown bytes=1596 bytes

//*bmp+=1596; //cant keep because of malloc
#if 0 
bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));

//memset(bmf,0,sizeof(BITMAPFILEHEADER));
bmf->bfType=0x4d42;
bmf->bfSize=bmpsize;
bmf->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


//memset(bmi,0,sizeof(BITMAPINFOHEADER));
//fprintf(stderr,"bs=%d hd=%x size=%x Off=%x\n",sizeof(BITMAPFILEHEADER),bmf.bfType,bmf.bfSize,bmf.bfOffBits);
bmi->biSize=sizeof(BITMAPINFOHEADER);
bmi->biWidth=avcc->width;
bmi->biHeight=avcc->height;
bmi->biPlanes=1;
bmi->biBitCount=PStatus.depth;
bmi->biSizeImage=(PStatus.depth/8)*avcc->width*avcc->height;
#endif



	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

//add bmp header info?


/*
	//write bmp to file
	fptr=fopen("testjpg.bmp","wb");
	if (fptr!=0) {
//		fwrite(*bmp,1,len,fptr);
	fwrite(*bmp,1,bmpsize,fptr);
	fclose(fptr);
	} //fptr!=0
*/



//	bmp+=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		av_free(mjpeg_bmp);
		av_free(mjpeg_yuv);

		//free avcodec
		avcodec_close(avcc);

	return(len);


		} else { //got_picture
	fprintf(stderr,"avcodec_decode_video2 did not return a picture\n");
} //got_picture


/*

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);
*/

return(0);

} //ConvertJPGtoBMP

#if 0 //now done with libjpeg

//ConvertBMPtoJPG
//This converts a bmp file to a jpg file
//bmp includes header info, jpg is the jpg file, the jpeg length is returned
//needs to pass address pointer **jpg
int ConvertBMPtoJPG(unsigned char *bmp,unsigned char *jpg)
{
BITMAPFILEHEADER *bmf;
BITMAPINFOHEADER *bmi;
unsigned char *bmpdata;
int len;
//FILE *fptr;
int width,height,depth;
AVCodec *avc;  //format to save video calls - currently set to MJPEG
AVCodecContext *avcc;
struct SwsContext  *swsctx;
int mjpeg_frame_size;
AVFrame *mjpeg_bmp;  //
AVFrame *mjpeg_yuv; 



bmf=(BITMAPFILEHEADER *)bmp;
bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bmpdata=(unsigned char *)(bmi+sizeof(BITMAPINFOHEADER));
width=bmi->biWidth;
height=bmi->biHeight;
depth=bmi->biBitCount;


//avcodec_init();
//avcodec_register_all();

avc = avcodec_find_encoder(CODEC_ID_MJPEG);
if (avc) {
	fprintf(stderr, "Codec mjpeg (%x) not found.\n",CODEC_ID_MJPEG);
return(0);
}


//avcc=avcodec_alloc_context3();
avcodec_alloc_context3(avcc);

mjpeg_bmp= avcodec_alloc_frame();  //this does malloc
mjpeg_yuv= avcodec_alloc_frame();

/*
* quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
* - encoding: Set by libavcodec. for coded_picture (and set by user for input).\
* - decoding: Set by libavcodec.\
*/
mjpeg_bmp->quality=1; 
mjpeg_yuv->quality=1;

/* put sample parameters */
avcc->bit_rate = 20000000;//1000000;//8000000;//4000000;//2400000;//1200000;
/* resolution must be a multiple of two */
//dimensions of output video are for now window size
//VIDWIDTH, VIDHEIGHT maybe is future
avcc->width = width;
avcc->height = height;
/* frames per second */
avcc->time_base= (AVRational){1,30}; //{1,25};
//c->gop_size = 10; /* emit one intra frame every ten frames */
//c->max_b_frames=1;
//c->pix_fmt = PIX_FMT_YUV420P;
//    c->pix_fmt = PIX_FMT_YUVJ440P;
//    c->pix_fmt = PIX_FMT_YUVJ444P;

avcc->pix_fmt = PIX_FMT_YUVJ422P;  //this is correct format for some digital cameras
//avcc->pix_fmt = PIX_FMT_YUV422P;  //possibly this is more common?

//fprintf(stderr,"PIX_FMT_YUVJ422P=%x\n",PIX_FMT_YUVJ422P);
//PIX_FMT_RGB24 gives:
//colorspace is not supported in jpeg
//so possibly we need to sws_scale from PIX_FMT_RGB24 to PIX_FMT_YUVJ422P

/* open it */
//if (avcodec_open(avcc,avc) < 0) {
if (avcodec_open2(avcc,avc,0) < 0) {
fprintf(stderr, "could not open codec for output\n");
//exit(1);
return(0);
}


/* alloc image and output buffer */
mjpeg_frame_size = width*height*(depth/8); //allocate full uncompressed image size

//malloc here or expect malloc'd memory?
//if here, caller needs to free
jpg = (unsigned char *)malloc(mjpeg_frame_size+FF_INPUT_BUFFER_PADDING_SIZE);


//PIX_FMT_YUVJ422P,  //dest format  
//yuv 2 1 1 16bpp = 44 4 4
//initialize sws_scale conversion
swsctx = sws_getContext(
width,  //source width, height, format
height,
PIX_FMT_BGR24,//PIX_FMT_BGR24, //PIX_FMT_RGB24, //source format  //to me still a mystery why BGR instead of RGB
width, //dest width, height, format
height,
PIX_FMT_YUVJ422P,  //dest format  
SWS_BICUBIC,NULL,NULL,NULL  //SWS_FAST_BILINEAR=1,SWS_BILINEAR=2,SWS_BICUBIC=4
);
//    PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
//    PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
//image is backwards

if (swsctx == 0) {
	fprintf(stderr,"Error initializing the Scaler conversion context.\n");
} //swsctx

mjpeg_yuv->data[0] =(uint8_t *)malloc(2*width*height);
mjpeg_yuv->data[1]=mjpeg_yuv->data[0]+width*height;
mjpeg_yuv->data[2]=mjpeg_yuv->data[1]+width*height/2;
mjpeg_yuv->linesize[0]=width;
mjpeg_yuv->linesize[1]=width/2;
mjpeg_yuv->linesize[2]=width/2;


//important to provide addresses to data[0],[1] and [2]
//I guess these are used even though [0][1][2] point to continuous memory
mjpeg_bmp->data[0]=(uint8_t *)(bmpdata);  //set bitmap data to AVFrame
mjpeg_bmp->data[1]=0;
mjpeg_bmp->data[2]=0;
mjpeg_bmp->linesize[0] = width*3;//c->width;
mjpeg_bmp->linesize[1] = 0;
mjpeg_bmp->linesize[2] = 0;


	fprintf(stderr,"sws_scale\n");
	//convert image data in bgr24 format to scaled yuv format (no header info is involved only image data)
	sws_scale(swsctx,mjpeg_bmp->data,mjpeg_bmp->linesize,0,height,mjpeg_yuv->data,mjpeg_yuv->linesize);

//	fprintf(stderr,"avcodec_encode_video\n");
	//convert yuv to jpg image (I think this adds header info, huffman table, etc)
	//this returns a writable jpg image file in mjpeg_frame
//	len = avcodec_encode_video(avcc,mjpeg_frame,mjpeg_frame_size,mjpeg_yuv);
len = avcodec_encode_video(avcc,(uint8_t *)jpg,mjpeg_frame_size,mjpeg_yuv);


//	fprintf(stderr,"converted %d bytes to JPEG\n",len);

	/*
	//write jpg to file
	fptr=fopen("test.jpg","wb");
	if (fptr!=0) {
		fwrite(icam.mjpeg_frame,1,len,fptr);
	fclose(fptr);
	} //fptr!=0
	*/

//free temp malloc
	if (mjpeg_yuv->data[0]!=0) {
		free(mjpeg_yuv->data[0]);
	} 

//	if (mjpeg_frame!=0) {
//		free(mjpeg_frame);
//	}

	//free AVFrames
	av_free(mjpeg_bmp);
	av_free(mjpeg_yuv);
	avcodec_close(avcc);

	return(len);
} //ConvertBMPtoJPG
#endif //0
#endif //USE_FFMPEG
#endif  //Linux


//void GetFTControlBackground(FTWindow *twin,FTControl *tcontrol)
void GetFTControlBackground(FTControl *tcontrol)
{
int bh,bw;
FTWindow *twin;

///*
twin=tcontrol->window;

if (twin!=0 && tcontrol!=0) {
//use clip points because control may be very large
//  bw=tcontrol->x2-tcontrol->x1;
//  bh=tcontrol->y2-tcontrol->y1;
  bw=tcontrol->cx2-tcontrol->cx1;
  bh=tcontrol->cy2-tcontrol->cy1;
  if (tcontrol->erase!=0) {
	free(tcontrol->erase); 
    tcontrol->erase=0;

  }
#if Linux
  if (tcontrol->xerase!=0) {
#if PINFO
    fprintf(stderr,"XDestroyImage tcontrol->xerase=%p\n",tcontrol->xerase);
#endif
    XDestroyImage(tcontrol->xerase);
    tcontrol->xerase=0;
#if PINFO
    fprintf(stderr,"after XDestroyImage tcontrol->xerase=%p\n",tcontrol->xerase);
#endif
  }
#endif

  if (bw<=0 || bh<=0) {
    //fprintf(stderr,"GetFTControlBackground called with 0 or less width or height FTControl->\n");
  }
  else {
//    if (tcontrol->x1<0 || tcontrol->y1<0 || (tcontrol->x1+bw)>(twin->x1+twin->w) || (tcontrol->y1+bh)>(twin->y1+twin->h)) {
//data control uses lx1,lx2, etc
//delete this part because control is already clipd
//    if (tcontrol->x1<0 || tcontrol->y1<0 || (tcontrol->x1+bw)>twin->w || (tcontrol->y1+bh)>twin->h) {
    if (tcontrol->cx1<0 || tcontrol->cy1<0 || (tcontrol->cx1+bw)>twin->w || (tcontrol->cy1+bh)>twin->h) {
      //fprintf(stderr,"GetFTControlBackground called with a FTControl outside of the window.\n");
    }
    else {
//      fprintf(stderr,"Pstatus.xdisplay=%p twin->xwin=%p\n",PStatus.xdisplay,(unsigned int *)twin->xwindow);
#if PINFO
      fprintf(stderr,"before getimage for tcontrol->erase=%p\n",tcontrol->xerase);
      fprintf(stderr,"cx1=%d cy1=%d bw=%d bh=%d\n",tcontrol->cx1,tcontrol->cy1,bw,bh);
#endif
//      tcontrol->erase=XGetImage(PStatus.xdisplay,twin->xwindow,tcontrol->x1,tcontrol->y1,bw,bh,AllPlanes,ZPixmap);
#if Linux
      tcontrol->xerase=XGetImage(PStatus.xdisplay,twin->xwindow,tcontrol->cx1,tcontrol->cy1,bw,bh,AllPlanes,ZPixmap);
#endif
#if PINFO
      fprintf(stderr,"after getimage for tcontrol->erase=%p\n",tcontrol->xerase);
#endif
    }
  } 
}  //if tcontrol!=0
#if PINFO
fprintf(stderr,"end GetFTControlBackground.\n\n");
#endif
//*/
}  //end GetFTControlBackground

//void EraseFTControl(FTWindow *twin, FTControl *tcontrol)
void EraseFTControl(FTControl *tcontrol)
{
#if Linux
	FTWindow *twin;
#endif
int bw,bh;
///*


if (tcontrol!=0) {
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"\nStart EraseFTControl %s\n",tcontrol->name);
	}

//  bw=tcontrol->x2-tcontrol->x1;
//  bh=tcontrol->y2-tcontrol->y1;
  bw=tcontrol->cx2-tcontrol->cx1;
  bh=tcontrol->cy2-tcontrol->cy1;

//  fprintf(stderr,"ctl x1=%d y1=%d bw=%d bh=%d\n",tcontrol->x1,tcontrol->y1,bw,bh);
//  fprintf(stderr,"win x1=%d y1=%d bw=%d bh=%d\n",twin->x1,twin->y1,twin->w,twin->h);
  if (bw<=0 || bh<=0) {
    fprintf(stderr,"EraseFTControl called with 0 or less width or height FTControl->\n");
  }
  else {
#if Linux
		//Draw background image (if any)
    if (tcontrol->xerase!=0) {
			twin=tcontrol->window;
//#if PINFO
//      fprintf(stderr,"XPutImage %p cx=%d cy=%d w=%d h=%d\n",tcontrol->erase,tcontrol->cx1,tcontrol->cy1,bw,bh);
//#endif

      XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->xerase,0,0,tcontrol->cx1,tcontrol->cy1,bw,bh);

    //XDestroyImage(tcontrol->erase);
    }
#endif
  } //bw<=0
  //get new background image
  
} //tcontrol!=0
//fprintf(stderr,"End EraseFTControl %s\n\n",tcontrol->name);
//*/
}  //end EraseFTControl


#if 0 
int FTDrawChar(FTWindow *twin, char c, int fw, int fh, int cx, int cy, int ul) {

#if WIN32
	WSTR wc;
	POINT tp[5];
#endif //WIN32

	//instead of XDrawImageString - which draws background color
#if Linux
//	XDrawString(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy, &tstr[ex3], 1);
	XDrawString(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy, &c, 1);
#endif
#if WIN32
	//MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&tstr[ex3], 1, (LPWSTR)tcontrol->text_16, FTMedStr);
	//TextOut(twin->hdcWin, cx, cy, (LPWSTR)tcontrol->text_16, 1);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&c, 1, &wc, FTMedStr);
	TextOut(twin->hdcWin, cx, cy, &wc, 1);
	//tcontrol->rect.left=cx;		tcontrol->rect.top=cy; 		tcontrol->rect.right=cx+fw; 		tcontrol->rect.bottom=cy+fh;
	//DrawText(twin->hdcWin,(LPWSTR)tcontrol->text_16,1,&tcontrol->rect,DT_CENTER|DT_NOCLIP);
#endif
	if (ul == 1) {
#if Linux
		XDrawLine(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy + 1, cx + fw, cy + 1);
#endif
#if WIN32
		tp[0].x = cx;	tp[0].y = cy + fh - 1;	tp[1].x = cx + fw;	tp[1].y = cy + fh - 1; //+fh because Win text origin is top -left, Xwin bottom-left
		Polyline(twin->hdcWin, tp, 2);
#endif
	} //if (ul == 1) {

} //int FTDrawChar(FTWindow *twin, char c, int fw, int fh, int cx, int cy, int ul) {
#endif


//Draw a string of text 
//note that underscore no longer draws underline
//I noticed that Windows no longer uses the underscore for file menus, it makes the code much more complicated
//in addition, somebody might want an underscore in the text- possibly a different method like setting a variable or flag could be done
int FTDrawText(FTWindow *twin, char *tstr, int numc, int fw, int fh, int cx, int cy) {


#if WIN32
	wchar_t wc[FTMedStr];
//	POINT tp[5];
#endif //WIN32

	//to do, remove any underscore and draw a line at the correct cx

	//instead of XDrawImageString - which draws background color
#if Linux
	//	XDrawString(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy, &tstr[ex3], 1);
	XDrawString(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy, tstr, numc);
#endif
#if WIN32
	//MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&tstr[ex3], 1, (LPWSTR)tcontrol->text_16, FTMedStr);
	//TextOut(twin->hdcWin, cx, cy, (LPWSTR)tcontrol->text_16, 1);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)tstr, numc, wc, FTMedStr);
	TextOut(twin->hdcWin, cx, cy, wc, numc);
	//tcontrol->rect.left=cx;		tcontrol->rect.top=cy; 		tcontrol->rect.right=cx+fw; 		tcontrol->rect.bottom=cy+fh;
	//DrawText(twin->hdcWin,(LPWSTR)tcontrol->text_16,1,&tcontrol->rect,DT_CENTER|DT_NOCLIP);
#endif
//	if (ul == 1) {
#if Linux
//		XDrawLine(PStatus.xdisplay, twin->xwindow, twin->xgc, cx, cy + 1, cx + fw, cy + 1);
#endif
#if WIN32
//		tp[0].x = cx;	tp[0].y = cy + fh - 1;	tp[1].x = cx + fw;	tp[1].y = cy + fh - 1; //+fh because Win text origin is top -left, Xwin bottom-left
//		Polyline(twin->hdcWin, tp, 2);
#endif
//	} //if (ul == 1) {
	return(1);

} //int FTDrawChar(FTWindow *twin, char c, int fw, int fh, int cx, int cy, int ul) {



//DrawButtonFTControls
//Draw Buttons - this include regular CTButton, but also FolderTab buttons
void DrawButtonFTControls(FTControl *tcontrol) 
{
FTWindow *twin;
int x1,y1,x2,y2,bw,bh,cx,cy,fw,fh,ul,ex3,len,numy,loopnum;
char tstr[FTMedStr],*pstr;
char winname[FTMedStr];
char conname[FTMedStr];
FTWords twords;
int curword, lastword, numwords, wx, wy, ecx, numspaces, sbw,i;


#if WIN32
POINT tp[5];
HGDIOBJ curObj,origObj;
int result;
#endif
int NotMouseOverImage, NotButtonDownImage;


	if (PStatus.flags&PInfo) {
		fprintf(stderr,"\nStart DrawButtonFTControls %s\n",tcontrol->name);
	}



if (tcontrol->window==0) {
	fprintf(stderr,"\nError: DrawButtonFTControls: The control %s needs to have a tcontrol->window\n",tcontrol->name);
}
twin=tcontrol->window;


if (twin->name>0) {
	strcpy(winname,twin->name);
} 
strcpy(conname,tcontrol->name);
//use clipped coords
x1=tcontrol->cx1;
y1=tcontrol->cy1;
x2=tcontrol->cx2;
y2=tcontrol->cy2;

bw=x2-x1+1; //+1 because both points x1 and x2 are drawn
bh=y2-y1+1;

fw=0;
fh=0;
if (tcontrol->font!=0) {
#if Linux
	fw=tcontrol->font->max_bounds.width;
	fh=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
	fw=tcontrol->fontwidth;
	fh=tcontrol->fontheight;
#endif
}

if (fw==0 || fh==0) {
	//fprintf(stderr,"There is no available font for control %s\n",tcontrol->name);
	sprintf(tstr,"There is no available font for control %s\n",tcontrol->name);
	FTMessageBox(tstr,FTMB_OK,(char *)"Error",0);
} //fw==0

#if WIN32
//In Windows the font origin is top left
//tcontrol->cy1-=fh;
//y1-=fh;
#endif


//    if (PStatus.flags&PInfo) 
//      fprintf(stderr,"FTControl %s is a button\n",tcontrol->name);

//    fprintf(stderr,"FTControl %s %d\n",tcontrol->name,tcontrol->x2);
//    if (tcontrol->data==0)
//      {  //is not track data
      //clip was done earlier
//      if (!(tcontrol->flags&CStartTime))
//        {  //is a normal button (do not draw buttons that depend on starttime)
//        if (!(twin->flags&WItemListOpen)) { 


//draw a border around the button if the button has the tab (twin->focus==tcontrol->tab)

//if (twin->focus==tcontrol->tab) {
//if (twin->FocusFTControl==tcontrol) {
//		fprintf(stderr,"%s has tab focus\n",tcontrol->name);
//	}

//draw rectangle of button
	if (tcontrol->Normal_Image==0) {  //skip if this control has an image

      	if (((tcontrol->flags&CToggle) && tcontrol->value && !(tcontrol->flags&CNoColorChangeButton(0)))|| tcontrol->flags&CButtonDown(0))  { //is a toggle button
#if Linux
			XSetForeground(PStatus.xdisplay, twin->xgc, tcontrol->color[1]);
#endif
#if WIN32
			//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]);
			//SelectObject(twin->hdcWin,tcontrol->brush);
			SetDCBrushColor(twin->hdcWin,tcontrol->color[1]);
			SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
#endif
//            else
  //            XSetForeground(PStatus.xdisplay, twin->xgc, tcontrol->color[0]);
		} else {  //is not a selected toggle or is a normal button
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[0]); //normal
#endif
#if WIN32
			//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]); 
			//SelectObject(twin->hdcWin,tcontrol->brush);
			SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
			SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
#endif

	        if (!(tcontrol->flags&CNoColorChangeButton(0)) && tcontrol->flags&CButtonDown(0)) { //button is down- change color of button
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[1]);
#endif
#if WIN32
				//DeleteObject(tcontrol->brush);
				//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]); 
				//SelectObject(twin->hdcWin,tcontrol->brush);
				SetDCBrushColor(twin->hdcWin,tcontrol->color[1]);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
#endif

  	          }  //if (!(tcontrol->flags&CNoColorChangeButton(0)) && tcontrol->flags&CButtonDown(0)) { //button is down- change color of button
		  

		//	DeleteObject(tcontrol->brush);
		 }  //if is toggle

//        }  //if NoChange for mouse over 
//        else  //the menu is open   
//          XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]);




        //fprintf(stderr,"xfillrect\n");
#if Linux
//		if (tcontrol->type==CTFolderTab) { //a foldertab control is a button located at the top of the folder, the bottom is determined by y3 
//			XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw-1,tcontrol->y3-y1-1);
//		} else {
			XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw-1,bh-1);
//		}
#endif
#if WIN32
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//GetClientRect(twin->hwindow,&twin->rect);
		//SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

		tcontrol->rect.left=x1;
		tcontrol->rect.top=y1;
		tcontrol->rect.right=x2;
//		if (tcontrol->type==CTFolderTab) { //a foldertab control is a button located at the top of the folder, the bottom is determined by y3 
//			tcontrol->rect.bottom=tcontrol->y3;
//		} else {
			tcontrol->rect.bottom=y2;
//		}
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,GetStockObject(DC_BRUSH));
#endif

			//or else draw the 2 3D button lines:

			if (!(tcontrol->flags&C2D)) {
			  //draw 3D lines 2 white, 2 black
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,white);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XSetForeground(PStatus.xdisplay,twin->xgc,black);
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,white);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SelectObject(twin->hdcWin,GetStockObject(WHITE_PEN));
				tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
				Polyline(twin->hdcWin,tp,2);
				tp[1].x=x1;	tp[1].y=y2-1;
				Polyline(twin->hdcWin,tp,2);
				//DeleteObject(tcontrol->pen);
				//tcontrol->pen=CreatePen(PS_SOLID,1,black);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SelectObject(twin->hdcWin,GetStockObject(WHITE_PEN));

#endif
/*
			//see if tcontrol->type==CTFolderTab ever calls this function I don't think it does
			if (tcontrol->type!=CTFolderTab) { //don't draw a line for selected FolderTab - so the folder button looks like the rest of the selected folder
#if Linux
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
				tp[0].x=x1;	tp[0].y=y2-1;	tp[1].x=x2-1;	tp[1].y=y2-1;
				Polyline(twin->hdcWin,tp,2);
#endif
			} else {
				tcontrol2=tcontrol->FolderTabGroup;
				if (tcontrol2!=0) {
					if (tcontrol2->FolderTab!=tcontrol) {
#if Linux
						XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
						tp[0].x=x1;	tp[0].y=y2-1;	tp[1].x=x2-1;	tp[1].y=y2-1;
						Polyline(twin->hdcWin,tp,2);
#endif
					} //if (tcontrol2->FolderTab!=tcontrol) {
				} //tcontrol2!=0
			}//if (tcontrol->type!=CTFolderTab) {
#if Linux
		XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y2-1);
#endif
#if WIN32
		tp[0].x=x2-1;	tp[0].y=y1;	
		Polyline(twin->hdcWin,tp,2);
		//DeleteObject(tcontrol->pen);
#endif
		*/
			}  //end if !C2D
//		} //			} //if (!(tcontrol->flags&CNoChangeOnMouseOver) && (tcontrol->flags&CMouseOver || twin->FocusFTControl==tcontrol) && !(tcontrol->flags&CButtonDown(0))) {  
	} else { //if (tcontrol->Normal_Image==0) {  //skip if this control has an image

	//Button has an image associated with it

	//image[0] (Normal_Image) is normal bitmap
	//image[1] (MouseOver_Image) is mouse-over or tab-selected image					
	//image[2] (ButtonDown_Image) is button-down-on control image

	//fprintf(stderr,"before drawing button bitmap image\n");


	//if the control has an image determine which image to draw
//    if (tcontrol->Normal_Image!=0) {  //image[0]
      //draw scaled bitmap onto button
      //fprintf(stderr,"Draw Image\n");
          //draw 3D lines on buttons with an image? = for now yes

		//check if selected or mouse over, if yes, draw image[1]
     	if (tcontrol->MouseOver_Image!=0 &&  //has a selected color change image
			(!(tcontrol->flags&CNoChangeOnMouseOver) && (tcontrol->flags&CMouseOver)) &&  //mouse is over control 
			!(tcontrol->flags&CButtonDown(0)) &&  //and button is not down on
			!((tcontrol->flags&CToggle) && tcontrol->value)) { //and is not a toggle control that is selected
				NotMouseOverImage=0;
			//may need to be clipped
#if Linux
			if (tcontrol->MouseOver_XImage!=0) {
				//fprintf(stderr,"draw MouseOver_XImage\n");
				XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->MouseOver_XImage,0,0,x1,y1,bw-1,bh-1); 
				//fprintf(stderr,"after draw ximage[2]\n");
			}
#endif
#if WIN32
			if (tcontrol->MouseOver_hbmp!=0) {
				curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
				if (curObj!=tcontrol->MouseOver_hbmp) {
					origObj=SelectObject(twin->hdcMem,tcontrol->MouseOver_hbmp);  //copy bitmap to hdc- doing this again causes the image to look mostly black
					}
				if (!(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { //leave space for border
					//leave space for border and outline
					result=BitBlt(twin->hdcWin,tcontrol->x1+1,tcontrol->y1+1,bw-2,bh-2,twin->hdcMem,0,0,SRCCOPY);
				} else {
					result=BitBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,bw-1,bh-1,twin->hdcMem,0,0,SRCCOPY);
				} 
				if (origObj!=tcontrol->MouseOver_hbmp) {  //put back original selected object
					SelectObject(twin->hdcMem,origObj);
				}
				//StretchBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,bw-1,bh-1,twin->hdcMem,0,0,tcontrol->bmpinfo->biWidth,tcontrol->bmpinfo->biHeight,SRCCOPY);
			} else {
				fprintf(stderr,"Error: tried to BitBlt image when tcontrol->MouseOver_hbmp==0\n");
			}
#endif

		} else { //if (tcontrol->MouseOver_Image!=0 &&  //has a selected color change image
			NotMouseOverImage=1;
		} //if (tcontrol->MouseOver_Image!=0 &&  //has a selected color change image


		//check for button-down image
		if (NotMouseOverImage && (tcontrol->ButtonDown_Image!=0)  && //there is a button-down image
			(((tcontrol->flags&CToggle) && tcontrol->value) || (tcontrol->flags&CButtonDown(0))) && //and the control is toggled on or the button is down on it
			!(tcontrol->flags&CNoColorChangeButton(0))) { //and the color of the control is allowed to change
				NotButtonDownImage=0;
#if Linux
			if (tcontrol->ButtonDown_XImage!=0) {  //there is an ximage
				XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->ButtonDown_XImage,0,0,x1,y1,bw-1,bh-1);
			} 
#endif
#if WIN32
			if (tcontrol->ButtonDown_hbmp!=0) {  //there is an hbmp
				curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
				if (curObj!=tcontrol->ButtonDown_hbmp) {
					origObj=SelectObject(twin->hdcMem,tcontrol->ButtonDown_hbmp);  //copy bitmap to hdc- doing this again causes the image to look mostly black
				}
				if (!(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { //leave space for border
					//leave space for border and outline
					result=BitBlt(twin->hdcWin,tcontrol->x1+1,tcontrol->y1+1,bw-2,bh-2,twin->hdcMem,0,0,SRCCOPY);
				} else {
					BitBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,bw-1,bh-1,twin->hdcMem,0,0,SRCCOPY);
				} //if (PStatus.flags&PNoFocusHighlight && twin->FocusFTControl==tcontrol) { //leave space for border
				//StretchBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,bw-1,bh-1,twin->hdcMem,0,0,tcontrol->bmpinfo->biWidth,tcontrol->bmpinfo->biHeight,SRCCOPY);
				if (origObj!=tcontrol->ButtonDown_hbmp) {  //put back original selected object
					SelectObject(twin->hdcMem,origObj);
				}
			} else {
				fprintf(stderr,"Error: tried to BitBlt image when tcontrol->hbmp_ButtonDown==0\n");
			}

#endif

		} else { //		if (((tcontrol->ButtonDown_Image!=0)  || //there is a button-down image
			NotButtonDownImage=1;
		}//		if (((tcontrol->ButtonDown_Image!=0)  || //there is a button-down image


	


		//if no mouse over, not toggled, and no button down

		if (NotMouseOverImage && NotButtonDownImage) {
				// not a selected, button-down on, or toggled button - just draw normal image
		   //fprintf(stderr,"not a selected or toggled button\n");
				//mouse over non-selected or toggled button
#if Linux
			if (tcontrol->Normal_XImage!=0) {
         XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->Normal_XImage,0,0,x1,y1,bw-1,bh-1);
			}
#endif
#if WIN32
			if (tcontrol->Normal_hbmp!=0) {
				curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
				if (curObj!=tcontrol->Normal_hbmp) {
					origObj=SelectObject(twin->hdcMem,tcontrol->Normal_hbmp);  //copy bitmap to hdc- doing this again causes the image to look mostly black
					//origObj=SelectObject(twin->hdcMem,tcontrol->MouseOver_hbmp);  //copy bitmap to hdc- doing this again causes the image to look mostly black
				}
				if (!(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight)) { //leave space for border
					//leave space for border and outline
					result=BitBlt(twin->hdcWin,tcontrol->x1+1,tcontrol->y1+1,bw-2,bh-2,twin->hdcMem,0,0,SRCCOPY);
				} else {
					BitBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,bw-1,bh-1,twin->hdcMem,0,0,SRCCOPY);
				}
				if (origObj!=tcontrol->Normal_hbmp) {  //put back original selected object
					SelectObject(twin->hdcMem,origObj);
				}
			} else {
				fprintf(stderr,"Error: tried to BitBlt image when tcontrol->hbmp[2]==0\n");
			}

#endif
		} //if (NotMouseOverImage && NotButtonDownImage) {

//    } //if (tcontrol->Image_Normal!=0) {  //end if button has a bitmap


	} //if (tcontrol->Normal_Image==0) {  //skip if this control has an image



		//if mouse is over the control, and button1 is not down on the control, draw a border around the button 
        if (!(tcontrol->flags&CNoChangeOnMouseOver) && (tcontrol->flags&CMouseOver) && !(tcontrol->flags&CButtonDown(0))) {  
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,gray);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
				SetDCBrushColor(twin->hdcWin,gray);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
				tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
				tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
				tp[4].x=x1;	tp[4].y=y1;
				Polyline(twin->hdcWin,tp,5);
#endif

		} //if (!(tcontrol->flags&CNoChangeOnMouseOver) && (tcontrol->flags&CMouseOver) && !(tcontrol->flags&CButtonDown(0))) {  


		//if button has focus and mouse is not over draw a black border
        if (twin->FocusFTControl==tcontrol && !(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight) &&
			(!(tcontrol->flags&CNoChangeOnMouseOver) && !(tcontrol->flags&CMouseOver))) {  
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,black);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
				SetDCBrushColor(twin->hdcWin,black);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
				tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
				tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
				tp[4].x=x1;	tp[4].y=y1;
				Polyline(twin->hdcWin,tp,5);
#endif

		} //if (twin->FocusFTControl==tcontrol && !(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight) &&


//fprintf(stderr,"after drawing button bitmap image\n");

        if (!(tcontrol->flags&CDoNotShowText)) {
//fprintf(stderr,"in CDoNotShowText\n");
#if Linux
            XSetForeground(PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]); //normal
#endif
#if WIN32
			//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]);
			//SelectObject(twin->hdcWin,tcontrol->pen);
			SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
			SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
			SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		

#endif

//            XSetBackground(PStatus.xdisplay, twin->xgc, tcontrol->textcolor[1]); //
//            if (PStatus.MouseX>tcontrol->x1 && PStatus.MouseX<tcontrol->x2 && PStatus.MouseY>tcontrol->y1 && PStatus.MouseY<tcontrol->y2) {  //mouse is over FTControl
            if (!(tcontrol->flags&CNoChangeOnMouseOver) && (tcontrol->flags&CMouseOver)) {  //mouse is over FTControl
#if Linux
				XSetForeground (PStatus.xdisplay, twin->xgc, ~tcontrol->textcolor[0]);
#endif
#if WIN32
				//DeleteObject(tcontrol->pen);
				//tcontrol->pen=CreatePen(PS_SOLID,1,~tcontrol->textcolor[0]);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,~tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
				SetTextColor(twin->hdcWin,~tcontrol->textcolor[0]);
				SetBkColor(twin->hdcWin,~tcontrol->textcolor[1]);		
#endif
//            if (!(tcontrol->flags&CNoColorChangeButton) && tcontrol->flags&CButtonDown) { //button is down
//              XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[1]);
//            }  //if button is down (and mouse is over - only way can be)

            } //mouse is over

         

//Draw each character and wrap or cut text
					//may need to be clipped

				//center text
				//wrap text

				ex3=0;
//				ul=0;
				//len=FTstrlen(tcontrol->text);
				len = strlen(tcontrol->text);
				wx=bw/fw;
				wy=bh/fh;
				pstr = tcontrol->text;
//fprintf(stderr,"here1\n");
				if (len>(wx*wy)) {  //string is too big for button
					//strncpy(tstr,tcontrol->text,wx*wy);
					len=wx;
				} else {
					//strcpy(tstr,tcontrol->text);
				} //len>wx*wy

//				if (len>wx) { //need to break words on space

//fprintf(stderr,"here2 len=%d cx=%d cy=%d fh=%d\n",len,cx,cy,fh);

//determine first x,y
//for now if len>cx just make cx=0
//later determine if centered, left or right justified
//and try to fit words breaking on spaces
				if (len>wx) {  //text goes over 1 line
					numy = len / wx; //how many rows text occupies
									 //numy=cy/(cx*cy-len); //how many rows text occupies
					cy = y1 + ((wy - numy) >> 1) + fh;
					cx = x1;
				}
				else {  //text only occupies 1 line
						//center text
					cx = x1 + ((bw - len*fw) >> 1);
					cy = y1 + ((bh - fh) >> 1) + fh;
				}  //if (len>cx)

#if WIN32
				   //In Windows the font origin is top left, in XWindows bottom left
				cy -= fh;
				SetBkMode(twin->hdcWin, TRANSPARENT); //so no text background color
													  //SetBkColor(twin->hdcWin,tcontrol->color[0]);
#endif




				//len2 = strlen(tstr);  //actual length (includes underline char)

#if 0 
				if (tcontrol->flags2&CTextNotCentered) {

					//draw each character one at a time

					while (ex3 < len2) // && cx<x2-fw && y1+fh+1<y2)
					{
						if (tstr[ex3] == 0x26) { //& underline
							ul = 1;
						} else {  //no underline - regular character

							//draw the character (w ul if needed)
							FTDrawChar(twin, &tstr[ex3], fw, fh, cx, cy, ul);

							if (ul) {
								ul = 0;
							}
							//advance the cursor
							cx += fw;
							if (cx + fw >= x2) {
								cx = tcontrol->x1 + fw + tcontrol->textx;
								cy += fh;
								if (cy >= y2) {
									ex3 = len2; //exit loop without printing text - clip
								}
							} //cx>x2
						}  //underline
						ex3++;
					}  //end while ex3

				}
				else { //if (tcontrol->flags2&CTextNotCentered) {
#endif

					
					loopnum = 0; //first pass determine how many rows to determine which row to start text on, then second pass actually output text
					numy = 0;
					cy = y1; //todo clean up above code that sets initial cx and cy
#if WIN32
					cy -= fh; //Windows uses top of char, Linux uses bottom
#endif

					while (loopnum < 2) {
						//parse and draw text on button
						//just uncentered, center, full, left, or right justify text
//						ecx = 0; //estimated cursor x
						if (len > 0) {  //at least 1 char
							if (loopnum) {
								//calculate the row to start on
								cy = y1+((wy - numy)/2+1)*fh;
#if WIN32
								cy -= fh; //Windows uses top of char, Linux uses bottom
#endif
								numy = (wy - numy) / 2; //so below while will work
								//numy = 0;
							}
							SplitTextIntoWords(&twords, pstr);  //split string into words (space delimited)
							i = 0;
							curword = 0;
							lastword = 0;
							//for each row on the button
//							while (cy < y2) {  //while cursor y is still within the control
							//while (cy < y2 && curword < twords.num) {  //while cursor y is still within the control
							while (numy < wy && curword < twords.num) {  //while cursor y is still within the control and there are still words to draw
								ecx = 0; //estimated cursor x for this row
								//if cur word is bigger than line, just break it
								if (twords.len[curword]>wx) {
									//possibly could change FTDrawChar(twin, &tstr[ex3], fw, fh, cx, cy, ul);
									//to draw more than 1 char, but ul would have to be detected
									//draw each character
									//draw the character (w ul if needed)
									if (loopnum) {
										FTDrawText(twin, &pstr[twords.i[curword]], wx, fw, fh, cx, cy);
									}
									//make broken word next word
									twords.i[curword] += wx;
									twords.len[curword] -= wx;


								}
								else { //if (twords.len[curword]>wx) {
								 //first word fits on line, see how many other words will fit on the line
									ecx += twords.len[curword];  //+1 for space (note ecx could have something from above)
									lastword = curword;
									ex3 = 0;
									numspaces = 0;  //how many spaces are needed
									while (!ex3) {
										if (lastword == twords.num - 1) {
											ex3 = 1; //processed last word, exit loop
											if (!loopnum) {
												curword = lastword+1; //update the current word in the first loop to calculate how many rows are needed
											}
										}
										else {
											lastword++; //go to next word
											numspaces++; //would need another space
											if (ecx + twords.len[lastword] + numspaces <= wx) { //+1 for space
												//word fits, add length of this word and continue to next word
												ecx += twords.len[lastword];
											}
											else {
												//next word does not fit, exit loop
												lastword--; //go back to previous word
												numspaces--; //remove extra space
												ex3 = 1; //exit while
												if (!loopnum) {
													curword = lastword+1; //update the current word in the first loop to calculate how many rows are needed
												}
											}
										} //if (lastword == twords.num) {
									} //while


									//draw text
									if (loopnum) { //only needed after the first loop has calculated how many rows are needed
										//determine number of spaces in line
										numspaces = wx - ecx; //note that above numspaces is no longer needed
										numwords = lastword - curword + 1; //number of words on the line
										//if 1 word move cursor in by half the spaces
										if (numwords == 1) {
											//move the cursor by half the number of spaces to center word
											cx = x1 + (numspaces / 2)*fw;
											if (loopnum) {
												FTDrawText(twin, &pstr[twords.i[curword]], twords.len[curword], fw, fh, cx, cy);
											}
											curword++; //so the loop will work
										}
										else {
											//more than 1 word on line

											//determine how many spaces between each word
											if (tcontrol->flags2&CTextNotCentered) {
												//user specified that text should not be centered
												//curently only full justified is implemented if (tcontrol->flags2&CTextFullJustify) {
												sbw = numspaces / (numwords - 1); //for 2 words, only 1 set of spaces, 3 words, 2 sets of spaces
												cx = x1; //start at beginning
											}
											else {
												//center text (the default option)
												//sbw = numwords - 1; //only 1 space between all words

												//for 2 words, 3 set of spaces, 1 set at the start, 1 set in the middle, and 1 set at the end
												//note that the spaces on the outside are the same as on the inside
												//if (numspaces > numwords) {  //more spaces than words
													//split the spaces into the minimum needed
												sbw = numspaces / (numwords + 1);
												//}
												//else {
												//	sbw = 1;  //otherwise just use 1 space before and after each word
												//}
												cx = x1 + sbw*fw;
											}



											//draw each word
											while (curword <= lastword) {
												if (curword == lastword && (tcontrol->flags2&CTextFullJustify)) {
													//if fulljustify, and last word, because sbw is average, just right justify last word
													cx = (wx - twords.len[curword] - 1)*fw;
												}

												if (loopnum) {
													FTDrawText(twin, &pstr[twords.i[curword]], twords.len[curword], fw, fh, cx, cy);
												}
												if (sbw == 0) {  //in the case where there are 2 words and only 1 space, sbw=0, this will add the needed space
													sbw = 1;
												}
												cx += (twords.len[curword] + sbw)*fw; //move cursor by word len and spaces															
												curword++;
											} //while

										} //if (numwords == 1) {
									} //if (loopnum) { //only needed after the first loop has calculated how many rows are needed
								} //if (twords.len[curword]>wx) {
								//if (curword < twords.num) { //last word was not reached yet
								cy += fh; //go to next line
								//if (curword < twords.num - 1) {
								numy++;  //only add a line if the last word was not reached
								//}
								//}

							} //while (numy < wy && curword < twords.num) {  //while cursor y is still within the control and there are still words to draw


						} //if (len > 0) {
						loopnum++;
					} //while (loopnum<2)

//				} //if (tcontrol->flags2&CTextNotCentered) {

//fprintf(stderr,"here4\n");
		 //DeleteObject(tcontrol->pen);
          } // if (!(tcontrol->flags&CDoNotShowText))

//fprintf(stderr,"after CDoNotShowText\n");

			//If this is a scrollbar up, down, left, right, slider, or back button, make sure the controls connected to the scrollbar are updated (text scrolled in textarea, controls in a frame, etc) it's size and position are correct.
			//scrollbars are updated when the buttons are pressed on
			//UpdateScrollBar
					//UpdateVScrollSlide(tcontrol);  //determine dimensions of scroll back FTControl ?
          //CalcVScrollSlide(tcontrol);  //determine size, needs numvisible

			//if this is a scrollbar back button, redraw scrollbar slide button which is above the back


		  /*
		  //probably can skip this, and leave up to FT/user to draw in correct order- but just to be on the safe side (in addition depth sort of controls is probably needed anyway)
			if (tcontrol->flags&CVScroll) {
				DrawFTControl(tcontrol->FTC_VScrollSlide);
			} //if (tcontrol->flags&CVScroll) {

			if (tcontrol->flags&CHScroll) {
				DrawFTControl(tcontrol->FTC_HScrollSlide);
			} //if (tcontrol->flags&CHScroll) {
			*/

#if Linux
			XFlush(PStatus.xdisplay); //to update button drawing
#endif
#if WIN32
			GdiFlush();
#endif

//    fprintf(stderr,"FTControl2 %s %d\n",tcontrol->name,tcontrol->x2);

      //call any onDraw functions
      if (tcontrol->OnDraw!=0) {
 			   //fprintf(stderr,"call OnDraw function for %s\n",tcontrol->name);
				(*tcontrol->OnDraw) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			}
  // fprintf(stderr,"after flush\n");
//    fprintf(stderr,"FTControl3 %s %d\n",tcontrol->name,tcontrol->x2);
//		} //			if (!(tcontrol->flags&(CNotVisible|CNotShowing))) {
	if (PStatus.flags&PInfo) {
		fprintf(stderr,"\nEnd DrawButtonFTControls %s\n",tcontrol->name);
	}


} //DrawButtonFTControls()


//Split text into words for aligning onto a button
int SplitTextIntoWords(FTWords *twords,char *tstr)
{
	int i, numwords,len,nw,curword;

	len = strlen(tstr);
	i = 0;
	nw = 1;
	curword = 0;
	twords->num = 0;
	twords->i[0] = 0;
	twords->len[0] = 0;
	while (i < len) {
		if (tstr[i] == 0x20) { //space  //0x26=_
			if (!nw) {  //could be multiple spaces
				curword++;
			}
			nw = 1; //new word
			
		} else {
			if (nw) {
				//start of new word
				if (curword < FTBigStr) { //to protect against overflow of array
					//record new word
					twords->num++;
					twords->i[curword] = i;
					twords->len[curword]=1;
					nw = 0;					
				} //if (curword < FTBigStr) {
			} else { //if (nw) {
				//not new word
				//but is char, just keep adding
				twords->len[curword]++;
			} //if (nw) {
		} //if (tstr[i] == 0x26 || tstr[i] == 0x20) { //_ or space
	i++;
	} //while (i < len) {

	return(1);
} //int SplitTextIntoWords(FTWords *twords)

void DrawFTControl(FTControl *tcontrol)
{

	if (tcontrol->flags&(CNotVisible|CNotShowing)) {
		return;
	}
	//DrawFTControls(-1,tcontrol);
	DrawFTControls(0,tcontrol);
}  //end DrawFTControl


//todo: add a lock so that this function cannot be called while it is already processing
//tcontrol==0 to draw all FTControls in this window
//I need to add clipping, z clipping too - either higher controls need to be redrawn, or parts of lower controls need to be not drawn.
void DrawFTControls(FTWindow *twin,FTControl *tcontrol)
{
int bw,bh,ypos,bw2;//,ex;  //x2,y2
//long long time0,time1,endtime,starttime,tx,a,b;//,c,tinc;
long long a,b;//tx,c,tinc;
//long long startx,endx,lindex,dindex,mask;
unsigned long long lindex,mask;//,dindex
//int pixels;//,startx,endx;
//unsigned long long rx;
long double rx,rx2;//,rxi; //samples/pixel
//long long lrx;
long double nextsample;
int bps; //bytes/sample
//unsigned int dc;
double sy;
FTControl *tcontrol2,*tcontrolz,*tTimeLine;
FTControl *tslide,*tback;
char tstr[FTMedStr],tstr2[FTMedStr];
#if WIN32
wchar_t tstrw[FTMedStr];
#endif
long long tsample[MaxNumChannels];  //videoedit
//FTWindow *twin2;
int ul,cx,cx2,cy,cy2,ex3,maxwidth,maxheight,cw;//,numrow;  //for filemenu ex2
int numrow,currow;
#if Linux
XPoint *points;
#endif
#if Linux
DIR *dirp;
struct dirent *direp;
struct stat statBuffer;
#endif
#if WIN32
WIN32_FIND_DATA fileinfo;
int result;
#endif
#if WIN32
HANDLE dirp;
#endif
int i,j,ti,tj;
int h,k,match,datasigned;
FTItem *litem;//,*litem2;

int allFTControls;  //draw all FTControls for window
//int check;  //FTControl grew
int tlen; //drawlater,tlen;
#if Linux
Cursor watch;
Pixmap pmap;
XImage *Timage;
#endif
//Window root_win;
//int xr,yr;  //x,y
//unsigned int wr,hr;  //width,height
//int maxx,maxy;
int fw,fh; //font width and height
//unsigned int bwr;  //borderwidth
//unsigned int dr;  //depth
//float scalex,scaley;  //window old size to new
FTItem *titem;
char winname[FTMedStr];
char conname[FTMedStr];
int x1,y1,x2,y2; //control clipped coordinates
//long long lx1,ly1,lx2,ly2; 
FTControl *tFrame;
//fpos_t fposition;
long long fposition;
int clipped;
long double DurationVisible,TimeScale,StartSelected,EndSelected;
int TextChanged;
#if Linux
struct stat statbuf;
#endif
#if WIN32
struct _stat statbuf;
POINT tp[5];
unsigned int curcolor;
HRGN hrgn;
HGDIOBJ curObj,origObj;
#endif
//long double fi;
//int result;
#if 0//WIN32
DWORD dwWaitResult;
#endif

#if 0//caused problems with drawing images - need to figure out why WIN32
//request the mutex
dwWaitResult = WaitForSingleObject(
	PStatus.DrawFTControlsMutex,    // handle to mutex
	INFINITE);  // no time-out interval

if (dwWaitResult != WAIT_OBJECT_0) {
	return;
}
#endif


if (twin==0 && tcontrol==0) { 
  fprintf(stderr,"DrawControls called with no FTWindow or FTControl->\n");
	return;
}

if (PStatus.flags&PInfo) {
	if (twin==0 && tcontrol!=0) {
	  fprintf(stderr,"Start DrawFTControls tcontrol=%s\n",tcontrol->name);
	} else {
	 	fprintf(stderr,"Start DrawFTControls window=%s\n",twin->name);
	}
} //PInfo

//if (tcontrol!=0)
//  fprintf(stderr,"tcontrol->next=%p\n",tcontrol->next);

//fprintf(stderr,"DrawFTControls win=%d name=%s\n",win,tcontrol->name);
 
//Only the FTControls for 1 window can be drawn 

//Get window number win (0 is root window)
//if tcontrol==0 this is to draw all FTControls in this window
allFTControls=0;
if (tcontrol==0) {
  //twin=GetFTWindowN(win);
  allFTControls=1;
  tcontrol=twin->icontrol;
  //fprintf(stderr,"Draw all FTControls\n");
  }
else {
//  twin=GetFTWindowN(tcontrol->win);
  twin=tcontrol->window;
	if (twin==0) {
		fprintf(stderr,"Error: Control '%s' has no associated window\n",tcontrol->name);
	}

	if (PStatus.flags&PInfo) {
		if (twin!=0) {
			fprintf(stderr,"In window %s\n",twin->name);
		} 
	} //PInfo

//  win=tcontrol->win;
}
//tcontrol=icontrol;

if (twin<=0) {
  fprintf(stderr,"Window not found.\n");
  return;
}


//if (twin->flags&WNotVisible || twin->xwindow==0) {
#if Linux
if (!(twin->flags&WOpen) || twin->xwindow==0) {
#endif
#if WIN32
if (!(twin->flags&WOpen) || twin->hwindow==0) {
#endif

	//fprintf(stderr,"Window not open or has no xwindow.\n");
  return;
}

strcpy(winname,twin->name);


//if (tcontrol!=0) {  //there are FTControls



//fprintf(stderr,"tcontrol!=0\n");
//fprintf(stderr,"control name=%s\n",tcontrol->name);
//fprintf(stderr,"twin->xwindow=%p &root_win=%p\n",twin->xwindow,&root_win);

//XGetGeometry causes malloc crashes - assign 0 to variables to see if this helps
//figure out way to not have to use XGetGeometry
/*
root_win=0;
xr=0;
yr=0;
wr=0;
hr=0;
bwr=0;
dr=0;
XGetGeometry(PStatus.xdisplay,twin->xwindow,&root_win,&xr,&yr,&wr,&hr,&bwr,&dr);
//fprintf(stderr,"after GetGeometry\n");
//XGetGeometry(PStatus.xdisplay,twin->xwindow,&root_win,x_return,y_return,width_return,height_return,border_width_return,depth_return);
//get window borders  even though X clips
maxx=wr;
maxy=hr;
*/





//if (twin->flags&WResize) { //may be used unitialized warning if not
  //get scalex and scaley
//  scalex=(float)twin->w/(float)twin->ow;
//  scaley=(float)twin->h/(float)twin->oh;
//}


//maxx=*width_return;
//maxy=*height_return;
//fprintf(stderr,"0 in grow maxx=%d ,maxy=%d\n",maxx,maxy);
//fprintf(stderr,"0 twin x=%d ,y=%d\n",twin->w,twin->h);


//draw background behind tracks with data (may be 1+ data on 1 track so not in loop
//  if (track>0)
//    ClearTracks(track);

//if (win==0 && allFTControls && track==0)  //clear entire window
//  ClearTracks(0);  //have to clear all showing tracks

//fprintf(stderr,"after clear tracks\n");
//fprintf(stderr,"while\n");
//ex=0;
//while(ex==0)
//This could be all controls or only 1 control
while (tcontrol!=0) {
//{
//if (allFTControls==0) {
//	ex=1;  //only 1 FTControl, no while
//}

strcpy(conname,tcontrol->name);

//fprintf(stderr,"name=%s\n",tcontrol->name);

//for each FTControl
if (!(tcontrol->flags&(CNotVisible|CNotShowing)))  //FTControl is visible
{



//get background for ->flags&CErase
if (tcontrol->erase==0 && tcontrol->flags&(CErase|CMove)) { 
//#if PINFO
//  fprintf(stderr,"get background\n");
//#endif
  GetFTControlBackground(tcontrol);
}


//if (tcontrol->flags&CMove) { EraseFTControl(twin,tcontrol); }
//erase movable FTControl

//#if 0 

//#endif




#if 0 
  if (name>0)
  {
  //one time - only draw 1 FTControl
  tcontrol=GetFTControl(name);
  //fprintf(stderr,"single FTControl %s in win %d\n",tcontrol->name,win);
  win=tcontrol->win;
  ex=1;
  }
#endif

//fprintf(stderr,"name=%s\n",tcontrol->name);
//fprintf(stderr,"name=%p\n",tcontrol->name);
//draw all if name==0 or only 1
//if ((tcontrol->name==0&&track==0) || (tname>0 && strcmp(tcontrol->name,name)==0) || (track>0 && tcontrol->track==track))
//if ((tcontrol->name==0&&track==0) || (track>0 && tcontrol->track==track))
//{
//draw all FTControls or 1

//fprintf(stderr,"FTControl %s in win %d\n",tcontrol->name,win);
//fprintf(stderr,"%d: FTControl name %s, next %p\n",a,tcontrol->name,tcontrol->next);

//clip tracks?  not needed in X, but must clip text

//resize if needs be

//fprintf(stderr,"b4 cleartracks\n");
//should not really draw track data by name because there could be other data on a track
//if (tcontrol->name!=0 && tcontrol->track>0) 
//    ClearTracks(tcontrol->track);  //may not be a data FTControl being drawn
//fprintf(stderr,"after cleartracks\n");


//fprintf(stderr,"b4 grow ");
/*
//this needs to be on window resize event
if (tcontrol->win==win && (tcontrol->flags&CGrowX1 ||tcontrol->flags&CGrowY1 ||tcontrol->flags&CGrowX2 ||tcontrol->flags&CGrowY2 ||tcontrol->flags&CGrowX3 ||tcontrol->flags&CGrowY3))
  {
//fprintf(stderr,"in grow maxx=%d ,maxy=%d\n",maxx,maxy);
  check=0;
//  XGetGeometry(PStatus.xdisplay,twin->xwindow,root_win,x_return,y_return,width_return,height_return,border_width_return,depth_return);

//if movable FTControl
//erase, getimage

  if (tcontrol->flags&CGrowX1)
    {
      //tcontrol->x1=*(width_return)+tcontrol->gx1;
      tcontrol->x1=wr+tcontrol->gx1;
      check=1;
    }
  if (tcontrol->flags&CGrowY1)
    {
//      tcontrol->y1=*(height_return)+tcontrol->gy1;
      tcontrol->y1=hr+tcontrol->gy1;
      check=1;
    }
  if (tcontrol->flags&CGrowX2)
    {
//fprintf(stderr,"x2 is added \n");
//      tcontrol->x2=*(width_return)+tcontrol->gx2;
      tcontrol->x2=wr+tcontrol->gx2;
      check=1;
    }
  if (tcontrol->flags&CGrowY2)
    {
//      tcontrol->y2=*(height_return)+tcontrol->gy2;
      tcontrol->y2=hr+tcontrol->gy2;
      check=1;
    }
    if (tcontrol->flags&CGrowX3)
    {
//      tcontrol->x3=*(width_return)+tcontrol->gx3;
      tcontrol->x3=wr+tcontrol->gx3;
      check=1;
    }
    if (tcontrol->flags&CGrowY3)
    {
//      tcontrol->y3=*(height_return)+tcontrol->gy3;
      tcontrol->y3=hr+tcontrol->gy3;
      check=1;
    }

//    if (check==1 tcontrol->type&CTFileOpen) {
//      if (tcontrol->ilist!=0) tcontrol->ilist->flags|=FTIL_REREAD;
//    }
  }  //FTControls that move/grow with the window

*/




//if (tcontrol->win==win)
//This control is in this window - need?
if (tcontrol->window==twin)  {
  //if (tcontrol->tab==0)  //this FTControl has initial focus
  //  {
    //pwin[win].focus=numc;
 //   twin->focus=tcontrol->num;  //not sure FTControls have a number
  //  }


//perhaps we should determine x1 and x2 of data controls here because a resize is not resizing data controls
//if this is a data control, we need to determine it's x1,x2 because if there was a resize more or less might be showing
//and it might be clipped relative to its associated timeline
	if (tcontrol->type==CTData) {
		AdjustDataFTControlDimensions(tcontrol);
	}
	//Clip control to window (is done by X-windows anyway)
	//set clip flags FTC_CLIP_X1,FTC_CLIP_Y1, etc
	//since there are only rectangular controls, we only need to check x1,y1 and x2,y2
//	Clipflags1=0; //point 1 x1,y1
//	Clipflags2=0; //point 2 x2,y2

	x1=tcontrol->x1;
	x2=tcontrol->x2;
	y1=tcontrol->y1;
	y2=tcontrol->y2;
	tcontrol->cx1=x1;
	tcontrol->cy1=y1;
	tcontrol->cx2=x2;
	tcontrol->cy2=y2;

//	if (tcontrol->type==CTData) {  //use 64-bit control dimension values (lx1,ly1,lx2,ly2)
//		lx1=tcontrol->lx1;
//		lx2=tcontrol->lx2;
//		ly1=tcontrol->ly1;
//		ly2=tcontrol->ly2;

//	} //CTData

#if 0 
	if (tcontrol->x2<twin->x || tcontrol->y2<twin->y || tcontrol->x1>twin->x+twin->w || tcontrol->y1>twin->y+twin->h) {
		return; //control is completely clipped
	}

	if (tcontrol->x1<twin->x) {
		x1=twin->x;
		tcontrol->cx1=x1;
	} else {
		x1=tcontrol->x1;
	}

	if (tcontrol->x1>twin->x+twin->w) {
		x2=twin->x+twin->w;
		tcontrol->cx2=x2;
	} else {
		x2=tcontrol->x2;
	}

	if (tcontrol->y1<twin->y) {
		y1=twin->y;
		tcontrol->cy1=y1;
	} else {
		y1=tcontrol->y1;
	}
	if (tcontrol->y2>twin->y+twin->h) {
		y2=twin->y+twin->h;
		tcontrol->cy2=y2;
	} else {
		y2=tcontrol->y2;
	}

#endif

//		fprintf(stderr,"is clipped Control %s?\n",tcontrol->name);
//Clip control to frame
	clipped=0;
//text and image on buttons will have to be clipped
	if (tcontrol->FrameFTControl!=0) {

		//fprintf(stderr,"Control %s belongs to a frame\n",tcontrol->name);
		//control belongs to frame
		tFrame=tcontrol->FrameFTControl;
#if 0 
		if (tcontrol->type==CTData) {  //use 64-bit control dimension values (lx1,ly1,lx2,ly2)

			lx1=tcontrol->lx1;
			lx2=tcontrol->lx2;
			ly1=tcontrol->ly1;
			ly2=tcontrol->ly2;
		
			if (lx1>(long long)tFrame->x2 || lx2<(long long)tFrame->x1 || ly1>(long long)tFrame->y2 || ly2<(long long)tFrame->y1) {
				//fprintf(stderr,"lx1=%lli >%lli lx2=%lli < %lli\n",lx1,(long long)tFrame->x2,lx2,(long long)tFrame->x1);
				clipped=1; //control is totally clipped by frame
			} //lx1>tFrame->x2
			if (lx1<(long long)tFrame->x1) {
				x1=tFrame->x1; //clip x1 to frame
				tcontrol->cx1=x1;
			}
			if (lx2>(long long)tFrame->x2) {
				x2=tFrame->x2; //clip x2 to frame
				tcontrol->cx2=x2;
			}

			if (ly1<(long long)tFrame->y1) {
				y1=tFrame->y1; //clip y1 to frame
				tcontrol->cy1=y1;
			}
			if (ly2>(long long)tFrame->y2) {
				y2=tFrame->y2; //clip y2 to frame
				tcontrol->cy2=y2;
			}
	//		fprintf(stderr,"Clipped bit=%d x1=%d x2=%d y1=%d y2=%d\n",clipped,x1,x2,y1,y2);

		} else { //use 32-bit control dimension values (x1,y1,x2,y2)
#endif
			if (x1>tFrame->x2 || x2<tFrame->x1 || y1>tFrame->y2 || y2<tFrame->y1) {
				clipped=1; //control is totally clipped by frame
			} //x1>tFrame->x2
			if (x1<tFrame->x1) {
				x1=tFrame->x1; //clip x1 to frame
				tcontrol->cx1=x1;
			}
			if (x2>tFrame->x2) {
				x2=tFrame->x2; //clip x2 to frame
				tcontrol->cx2=x2;
			}

			if (y1<tFrame->y1) {
				y1=tFrame->y1; //clip y1 to frame
				tcontrol->cy1=y1;
			}
			if (y2>tFrame->y2) {
				y2=tFrame->y2; //clip y2 to frame
				tcontrol->cy2=y2;
			}

//		} //CTData
	} //FrameFTControl!=0



//Clip any controls that are on a FolderTabGroup
//text and image on buttons will have to be clipped
	if (tcontrol->FolderTabGroup!=0) {

		//fprintf(stderr,"Control %s belongs to a FolderTabGroup\n",tcontrol->name);
		//control belongs to FolderTabGroup
		tFrame=tcontrol->FolderTabGroup;
		if (tcontrol->type!=CTFolderTab && tcontrol->FolderTab==0) {  //invalid FolderTab - just clip for now
			fprintf(stderr,"Error: Control %s has a FolderTabGroup but no FolderTab\n",tcontrol->name);
			clipped=1; 
		} 
		if (tcontrol->FolderTab!=tFrame->CurrentFolderTab) { //the FolderTab this control is on is not the current FolderTab
				clipped=1;
		} else { // the FolderTab this control is on is the current FolderTab

			if (x1>tFrame->x2 || x2<tFrame->x1 || y1>tFrame->y2 || y2<tFrame->y1) {
				clipped=1; //control is totally clipped by FolderTabGroup
			} //x1>tFrame->x2
			if (x1<tFrame->x1) {
				x1=tFrame->x1; //clip x1 to frame
				tcontrol->cx1=x1;
			}
			if (x2>tFrame->x2) {
				x2=tFrame->x2; //clip x2 to frame
				tcontrol->cx2=x2;
			}

			if (y1<tFrame->y1) {
				y1=tFrame->y1; //clip y1 to frame
				tcontrol->cy1=y1;
			}
			if (y2>tFrame->y2) {
				y2=tFrame->y2; //clip y2 to frame
				tcontrol->cy2=y2;
			}
		} //		if (tcontrol->FolderTab!=tFrame->FolderTab) { 
	} //	if (tcontrol->FolderTabGroup!=0) {




	if (!clipped) {


  //fprintf(stderr,"FTControl %d in window %d\n",a,win);
//  bw=tcontrol->x2-tcontrol->x1+1; //+1 because both points x1 and x2 are drawn
//  bh=tcontrol->y2-tcontrol->y1+1;
  bw=x2-x1+1; //+1 because both points x1 and x2 are drawn
  bh=y2-y1+1;

//  fprintf(stderr,"0bw=%d bh=%d\n",bw,bh);
	//clip control to window - why not use twin->x,y and w,h? they must not be changed when window size changes
	//x1 and y1 could be > 
/*
  if (tcontrol->x2>maxx) //+1?
    {
    bw=maxx-tcontrol->x1;
    }
  if (tcontrol->y2>maxy)
    {
    bh=maxy-tcontrol->y1;
    }
*/

//	if (tcontrol->

	//Clip control to frame, if any


  //fprintf(stderr,"FTControl type=%d\n",icontrol.type);
  //fprintf(stderr,"1bw=%d bh=%d\n",bw,bh);

  if (tcontrol->font==0) {
		if (tcontrol->type!=CTTimer) {
    	fprintf(stderr,"FTControl with name %s has no font. Each FTControl (except a timer) needs to have a font. Assigning font *ISO8859*.\n",tcontrol->name);
			//assign a default font
#if Linux
			tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
#endif
			if (tcontrol->font==0) {
				fprintf(stderr,"Could not find font *ISO8859*.\n");
				fw=0;
				fh=0;		
			} 
		} //!=CTTimer
  }  //font==0

#if Linux
    XSetFont(PStatus.xdisplay,twin->xgc,tcontrol->font->fid);
#endif
		fw=tcontrol->fontwidth;
		fh=tcontrol->fontheight;


//  fprintf(stderr,"font width=%d\n",tcontrol->font->max_bounds.width);

//getimage  behind movable FTControl


//fprintf(stderr,"switch\n");
  switch (tcontrol->type)
  {
  case CTButton:  //button
//      fprintf(stderr,"Before FTControl %s is a button\n",tcontrol->name);
				//check for visible is done above
//			if (!(tcontrol->flags&(CNotVisible|CNotShowing))) {
		//fw=tcontrol->font->max_bounds.width;
		//fh=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;

		DrawButtonFTControls(tcontrol); //DrawCTButton

    break;
  case CTLabel:  //label


		//fw=tcontrol->font->max_bounds.width;
		//fh=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;

//    if (PStatus.flags&PInfo) 
//      fprintf(stderr,"FTControl %s is label in window %d\n",tcontrol->name,win);
#if Linux
    XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[0]);
    XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw,bh);
    XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  
#endif
#if WIN32
//		tcontrol->brush=CreateSolidBrush(tcontrol->color[0]); //background color
//		SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		SelectObject(twin->hdcWin,(HFONT)tcontrol->font);
		//SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
		SetBkMode(twin->hdcWin,TRANSPARENT); //allow text background color
		SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
		SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
		//GetClientRect(twin->hwindow,&twin->rect);
		tcontrol->rect.left=tcontrol->x1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x1+bw;
		tcontrol->rect.bottom=tcontrol->y1+bh;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));
		//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]);
		//SelectObject(twin->hdcWin,tcontrol->pen);
		//tcontrol->brush=CreateSolidBrush(tcontrol->textcolor[0]); //brush sets text color of TextOut?
		//SelectObject(twin->hdcWin,tcontrol->brush);
#endif

//    XSetBackground(PStatus.xdisplay, twin->xgc, tcontrol->textcolor[1]);  //background color of text
		//text may need to be clipped
    cx=x1+1;//tcontrol->cx;//+tcontrol->font->max_bounds.width;


		//tlen=((strlen(tcontrol->text)+1)*fw<bw)?strlen(tcontrol->text):(bw/fw)-1;
		//if ((int)(strlen(tcontrol->text)+1)*fw<bw) {
	//if text exceeds control, clip text to control
	//note that this requires a monospace font- otherwise a total width of any specific string, or individual characters needs to be added
		if ((int)((strlen(tcontrol->text))*fw)<=bw) {
			tlen=strlen(tcontrol->text);
		} else {
			tlen=(bw/fw);
		}

//instead of image string - don't need custom background color for label
#if Linux
    XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1+fh,tcontrol->text,tlen);
    XFlush (PStatus.xdisplay);
#endif
#if WIN32
		MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tcontrol->text,tlen,(LPWSTR)tcontrol->text_16,FTMedStr);
		TextOut(twin->hdcWin,tcontrol->x1,tcontrol->y1,(LPWSTR)tcontrol->text_16,tlen);
		GdiFlush();
#endif
   // XSync(PStatus.xdisplay,1);  
      //call any onDraw functions
      if (tcontrol->OnDraw!=0) {
				(*tcontrol->OnDraw) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			} //OnDraw

    break;  //end label
	case CTTextBox:  //textbox
	case CTTextArea: //textarea


		if (PStatus.flags&PInfo) {
		  if (tcontrol->type==CTTextBox) {
  			fprintf(stderr,"Control type = textbox\n");
				fprintf(stderr,"tcontrol->text=%s\n",tcontrol->text);		
			} else {
			  fprintf(stderr,"Control type = textarea\n");
			}
		} //PInfo
//fprintf(stderr,"textarea\n");
//    if (PStatus.flags&PInfo) 
     // fprintf(stderr,"FTControl %s is textbox in window %d\n",tcontrol->name,win);
     //fprintf(stderr,"flags=%x\n",tcontrol->flags);
#if Linux
		XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[1]); //dark gray- button down control color-and border
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw,bh);
		//XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[0]);  //only white?
		XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //text background color (white)
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1+1,y1+1,bw-1,bh-1);
		XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  //text color
		XSetBackground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //background color of text
#endif
#if WIN32
		//todo: change color[0], textcolor[0], etc. to union with backgroundcolor, textcolor,textbackgroundcolor
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]);//textbox/area border color
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//GetClientRect(twin->hwindow,&twin->rect);
		//tcontrol->rect.left=tcontrol->x1;
		//tcontrol->rect.top=tcontrol->y1;
		//tcontrol->rect.right=tcontrol->x1+bw;
		//tcontrol->rect.bottom=tcontrol->y1+bh;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);

		//tcontrol->brush=CreateSolidBrush(tcontrol->textcolor[1]); //textbox/area background color white
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->textcolor[1]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		//GetClientRect(twin->hwindow,&twin->rect);
		tcontrol->rect.left=tcontrol->x1+1;
		tcontrol->rect.top=tcontrol->y1+1;
		tcontrol->rect.right=tcontrol->x1+bw-1;
		tcontrol->rect.bottom=tcontrol->y1+bh-1;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,GetStockObject(DC_BRUSH));

		//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); //pen is for lines
		//SelectObject(twin->hdcWin,tcontrol->pen);
		SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
		SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
		SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
		SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
#endif

		if (!(tcontrol->flags&CNoBorder) && !(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif

		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl


		if (!(PStatus.flags&PNoMouseOverHighlight) && (tcontrol->flags&CMouseOver)) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SetDCPenColor(twin->hdcWin,tcontrol->color[1]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif
		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl



    maxwidth=bw/fw-1; //width of textarea or textbox 
    maxheight=bh/fh; //height of textarea or textbox


//		fprintf(stderr,"i[0]=%d i[1]=%d len=%d\n",tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing,(int)strlen(tcontrol->text));										

//Go through each character and print text until reached end
//chr(10)-adds extra row (another approach is go through and count actual strlen - then for loop)
//also check for HTML tags if CTHTML bit is set
//checking for selected text by looking at tcontrol->i[4] and i[5]

//a person might erase part or all of tcontrol->text, and so i[0] and i[1] needs to be reset
//if there ever is a change to ->text
//I think for now the best I can do is say: if i[0]>strlen()  i[0]=strlen(text);
//ideally, FT would get called anytime tcontrol->text is changed programatically to adjust i[0], etc
//the cursor position should never be > the length of the string - if it is, a user probably
//changed the text in a textbox or textarea - just move cursor to end of text if any

		tlen=strlen(tcontrol->text);

//possibly detect changes in textarea and textbox by copying into text2
//this helps for knowing when new text was entered at the bottom and the scroll in an autoscroll should go down to the bottom
		if (tcontrol->CursorLocationInChars>tlen) { //user changed text programatically
//			fprintf(stderr,"i[0]>strlen (%d>%d)\n",tcontrol->CursorLocationInChars,(int)strlen(tcontrol->text));
			tcontrol->CursorLocationInChars=tlen;
			if (tcontrol->type==CTTextBox) {
				if (tlen>maxwidth) { //move first showing character to start of line
					tcontrol->FirstCharacterShowing=tlen-maxwidth;
				} else {
					tcontrol->FirstCharacterShowing=0;
				} //strlen
			}
			//not sure why these were here
			//tcontrol->StartSelectedTextInChars=strlen(tcontrol->text);
			//tcontrol->EndSelectedTextInChars=strlen(tcontrol->text);
		} //strlen(tcontrol->text)



	TextChanged=0;
	if (strcmp(tcontrol->text,tcontrol->text2)!=0) { //text changed since last draw
		if (PStatus.flags&PInfo) {
				fprintf(stderr,"text changed\n");		
		} //PInfo

		strcpy(tcontrol->text2,tcontrol->text);
		TextChanged=1;
	} //strcmp



		//for all characters print - to catch chr(10)

//		fprintf(stderr,"cx=%d cy=%d i=%d tlen=%d y2=%d\n",cx,cy,i,tlen,tcontrol->y2);


//

		//if CAutoscroll and text has changed, scroll i[1] until last text is at bottow row
		//move cursor i[0] too?
		//for autoscroll only move i[0] (cursor) not i[1] - so that a person may use the scroll bar
		//only adjust scrollbar and i[1] if scroll slide is at bottom
		//perhaps also if new text was added - but how to detect this? using text2?

//there is the possibility of only drawing if text has changed or a CRedraw bit is set (by a scrollbar change)


//for textareas if there is a scrollbar, and the scrollbar is at the bottom (or there is no scrollbar), autoscroll to the bottom (adjust i[1]), unless the CDoNotScrollY or the CVScrollSlideHoldsText bits are set
//if text has changed, scrollbar needs to be adjusted, unless scrollbar is not at bottom
//moving the slide button on the vertical scrollbar will adjust i[1]

#if 0 
		if (tcontrol->type==CTTextArea && !(tcontrol->flags2&CDoNotScrollY) && !(tcontrol->flags2&CVScrollSlideHoldsText)) {

			//if there is new text, scroll to bottom - if no text has changed, we don't bother changing the i[1]

			//determine if a scrollbar is at the bottom
//			tslide=tcontrol->FTC_VScrollSlide;
//			tback=tcontrol->FTC_VScrollBack;

//			if (tslide!=0 && tback!=0) {
//				if (tslide->y2>=tback->y2-1) { //slide button at bottom - autoscroll to bottom if text was added
					
//				}//tslide->y2>=tback->y2-1
//			} //tslide!=0 && tback!=0

			if (TextChanged) {  //text changed since last draw (tcontrol->text != tcontrol->text2), keys, user programming, etc. changes text in a textarea
				//see if we need to scroll the text (probably this should be done when any key is added to the text)
				//todo: probably I should track CursorX and CursorY, when a regular key is added, CursorX++ if > maxwidth =0 and CursorY++, is CursorY>maxhwight, FirstCharacterShowing+to wrap or cr
				//if the CursorLocationInChars is below the maxheight move the FirstCharacterShowing up
				//move backwards one character at a time, to determine the FirstCharacterShowing
				i=tlen;
				tcontrol->CursorLocationInChars=i;

				numrow=0;
				while(i>=0) {
					if (tcontrol->text[i]==10) { //there can be a carriage return or word wrap
						numrow++;
						if (numrow==maxheight) {
							tcontrol->FirstCharacterShowing=i+1; //first character showing in textbox
							//tcontrol->FirstCharacterShowing=i+1; //first
							//tcontrol->CursorLocationInChars=i; //need?
							//tcontrol->CursorLocationInChars=i; //need?
							i=-1;
						} //numrow==maxheight
					} //==10
					i--;
				} //while i

			} //TextChanged
		} //CDoNotScrollY or CVScrollSlideHoldsText
#endif


//Redraw TextArea/TextBox
//if there is a Vertical scrollbar, i[1] should be already adjusted
		//cx=x1+2;  //cursor position in pixels  PStatus.PadX
		cx=x1+PStatus.PadX;  //cursor position in pixels  PStatus.PadX- may effect Linux (was x1+2)
		cy=y1+fh-1; 
		i=tcontrol->FirstCharacterShowing; //cursor position in characters, first showing character
//		while(i<=tlen && ((tcontrol->type==CTTextArea && cy<tcontrol->y2) || (tcontrol->type==CTTextBox && cx<tcontrol->x2))) {
//		while(i<=tlen && cy<tcontrol->y2) {
		while(i<=tlen) {


			if (tcontrol->text[i]!=10) {  //not carriage return
				//if cx is large enough draw character
				if (tcontrol->text[i]!=0) { // && cx+fw<tcontrol->x2-1) {
				//if selected draw in selected colors
					if (tcontrol->StartSelectedTextInChars!=tcontrol->EndSelectedTextInChars) { //there is selected text
						if (i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars) { //draw selected
#if Linux
			      			XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color
			      			XSetBackground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[3]); // selected text background color
#endif
#if WIN32
							SetTextColor(twin->hdcWin,tcontrol->textcolor[2]);
							SetBkColor(twin->hdcWin,tcontrol->textcolor[3]);		
#endif


						} else { 	//if (i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars) { //draw selected
//draw unselected
#if Linux

			      			XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]); 
			      			XSetBackground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]); 

#endif
#if WIN32
							SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
							SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
#endif

						} //if (i>=tcontrol->StartSelectedTextInChars && i<tcontrol->EndSelectedTextInChars) { //draw selected
					} else { //[2]!=[3] - there is no selected text
#if Linux
			      	XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]); 
			      	XSetBackground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]); 
#endif
#if WIN32
							SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
							SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
#endif
					}
#if Linux
			  XDrawImageString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,&tcontrol->text[i],1);
#endif
#if WIN32
				MultiByteToWideChar(CP_UTF8,0,(LPCSTR)&tcontrol->text[i],1,(LPWSTR)tcontrol->text_16,FTMedStr);
				TextOut(twin->hdcWin,cx,cy-fh+2,(LPWSTR)tcontrol->text_16,1);
#endif
				//fprintf(stderr,"%d ",tcontrol->text[i]);					
				} //tcontrol->text[i]!=0 && cx+fw<tcontrol->x2-1 textarea is large enough				
			} //text==10 cr

//			fprintf(stderr,"i=%d i[0]=%d i[1]=%d\n",i,tcontrol->CursorLocationInChars,tcontrol->FirstCharacterShowing);										
//			if (tcontrol->CursorLocationInChars==i || tcontrol->CursorLocationInChars==tlen+1) { //cursor position is here - needs to be after character is drawn but before character is advanced
			if (tcontrol->CursorLocationInChars==i) { //cursor position is here - needs to be
			//	fprintf(stderr,"test Draw Cursor tcontrol->tab  %d ==%d twin->focus\n",tcontrol->tab,twin->focus);
				//cursor appears before character
					//Draw Cursor if in focus  draw if readonly? for arrow key selecting
//				if (tcontrol->tab==twin->focus && !(tcontrol->flags&CNoTab) && !(tcontrol->flags&CReadOnly)) {
				if (tcontrol==twin->FocusFTControl && !(tcontrol->flags&CNoTab) && !(tcontrol->flags&CReadOnly)) {
				  if (tcontrol->flags&CEraseCursor) {
							tcontrol->flags&=~CEraseCursor;  
					}
/*
					if (tcontrol->CursorLocationInChars==tlen+1) {  //if cursor is at last character draw now
						cx+=fw;
						if (cx>tcontrol->x2-fw) {
							cy+=fh;
							cx=tcontrol->x1+1;
						} //cx>tcontrol->x2-fw
					} //tcontrol->CursorLocationInChars==tlen+1
 */
//					fprintf(stderr,"Draw Cursor at position i=%d %d %d\n",i,cx,cy-fh+3);
					//DRAW CURSOR
#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy-fh+3,cx,cy);
#endif
#if WIN32
					tp[0].x=cx;	tp[0].y=cy-fh+3;	tp[1].x=cx;	tp[1].y=cy; //+fh because Win text origin is top -left, Xwin bottom-left
					Polyline(twin->hdcWin,tp,2);
#endif

				}//tcontrol==twin->FocusFTControl
			} //i[0]==i


			//line feed has to happen after drawing cursor
			if (tcontrol->text[i]==10 && tcontrol->type==CTTextArea) {
				//cx=x1+1;
				cx=x1+PStatus.PadX;
				cy+=fh;
				if (cy>y2) {
					i=tlen; //exit
				}  //cy>
			} 

			//advance cursor
			if (tcontrol->type==CTTextArea) {
				if (tcontrol->text[i]!=0 && tcontrol->text[i]!=10) {
					cx+=fw;
					if (cx>=x2-fw) { //In Linux I found that the = is needed or else an extra character can be drawn
						//cx=x1+1;
						cx=x1+PStatus.PadX;
						cy+=fh;
						if (cy>y2) { //reached bottom y of text control
							i=tlen; //exit while
						}//cy>
					} //cx>tcontrol->x2-fw
				} //!=0 && !=10			
			} else { //tcontrol->type==CTTextArea
				//CTTextBox
				cx+=fw;
#if WIN32
				if (cx>x2-fw) {
#endif
#if Linux
				if (cx>x2-fw) {
#endif
//					fprintf(stderr,"exit cx too large\n");
					//last cursor if at end of string
					if (tcontrol->CursorLocationInChars==i+1) { //cursor position is here - needs to be
							//Draw Cursor if in focus  draw if readonly? for arrow key selecting
//						if (tcontrol->tab==twin->focus && !(tcontrol->flags&CNoTab) && !(tcontrol->flags&CReadOnly)) {
						if (tcontrol==twin->FocusFTControl && !(tcontrol->flags&CNoTab) && !(tcontrol->flags&CReadOnly)) {
							if (tcontrol->flags&CEraseCursor) {
									tcontrol->flags&=~CEraseCursor;  
							}

							//DRAW CURSOR
//							fprintf(stderr,"Draw Cursor at position i=%d %d %d\n",i,cx,cy-fh+3);
#if Linux
							XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy-fh+3,cx,cy);
#endif
#if WIN32
							tp[0].x=cx;	tp[0].y=cy-fh+3;	tp[1].x=cx;	tp[1].y=cy; 
							Polyline(twin->hdcWin,tp,2);
#endif

						}//focus
					} //if (tcontrol->CursorLocationInChars==i) { //cursor position is here - needs to be
					i=tlen; //exit while

				} //cx>

				
			} //tcontrol->type==CTTextArea
	

			i++;
		} //while




		//Adjust and draw any associated scroll bars
		if (tcontrol->flags&CVScroll) {
			//textarea has vertical scroll bar
			//adjust the middle button to scale with the number of rows of text
			//determine number of rows of text - 
			//perhaps: numrows, currow, curcol, should be variables in control structure
			i=0;
			numrow=0;
			currow=0;
			while(i<=(int)strlen(tcontrol->text)) {
//				if (i==tcontrol->FirstCharacterShowing) {
//					firstrow=numrow;
//				}
				if (i==tcontrol->FirstCharacterShowing) {  //used i[0] but now i[1] since scroll follows page not cursor
					currow=numrow;
				}
				if (tcontrol->text[i]==10) {
					numrow++;
				} //==10
				i++;
			} //while i
	
			//scroll might be clipped
			//perhaps clip and then use tcontrol2->cx1, etc. 
			//scale middle button: size=maxheight/numrow, start position= currow/numrow
			//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
			//tcontrol2=GetFTControl(tstr);
			tslide=tcontrol->FTC_VScrollSlide;
			if (tslide!=0) {
				//sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
				//tcontrol3=GetFTControl(tstr);
				tback=tcontrol->FTC_VScrollBack;
				if (tback!=0) {
					if (numrow>=maxheight) {
					fprintf(stderr,"currow=%d numrow=%d\n",currow,numrow);
						//maxheight is number of characters that can fit in Y of textarea
						cy=(int)(((float)(tback->y2-tback->y1)*(float)maxheight)/((float)numrow+1.0)); //button size
						if (cy<fh) {
							cy=fh; //minimum scroll slide button size
						} //cy<fh
						//tslide->y1=tback->y1+((float)(tback->y2-tback->y1-cy)*(float)currow/((float)numrow+1.0));
						tslide->y1=(int)(tback->y1+((float)(tback->y2-tback->y1)*(float)currow/((float)numrow+1.0)));
						tslide->y2=tslide->y1+cy; //maxheight/numrow * scroll back button size
						fprintf(stderr,"slide y1=%d y2=%d back y1=%d y2=%d ratio=%f back height=%d maxheight of textarea=%d numrow+1=%d currow=%d\n",tslide->y1,tslide->y2,tback->y1,tback->y2,((float)(tback->y2-tback->y1-cy)*(float)currow/((float)numrow+1.0)),(tback->y2-tback->y1-cy),maxheight,numrow+1,currow);

					} //numrow>0
					if (PStatus.flags&PInfo) {
			    	fprintf(stderr,"Draw back and slide controls for vertical scroll control\n");
					}

					DrawFTControl(tback); //redraw scroll bar back - to erase any scroll bar button
					DrawFTControl(tslide); //redraw scroll bar button

				} //tcontrol3!=0
			} //tcontrol2!=0
		} //CVScroll

#if Linux
		XFlush(PStatus.xdisplay);
#endif
#if WIN32
		GdiFlush();
#endif

		//call any OnChange function
		if (tcontrol->OnChange!=0 && TextChanged) {
			(*tcontrol->OnChange) (twin,tcontrol);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol=GetFTControl(conname); //check to see if control was deleted
			if (tcontrol==0 || twin==0) return; //user deleted control or window
		} //OnChange

    //call any onDraw function
    if (tcontrol->OnDraw!=0) {
			(*tcontrol->OnDraw) (twin,tcontrol);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol=GetFTControl(conname); //check to see if control was deleted
			if (tcontrol==0 || twin==0) return; //user deleted control or window
		} //OnDraw


//fprintf(stderr,"here end\n");
    break;  //CTTextbox or CTTextArea

    case CTCheckBox:  //checkbox

//     if (PStatus.bits&PInfo) 
//       fprintf(stderr,"FTControl %s is checkbox in window %d\n",tcontrol->name,win);
    //first time drawn, adjust boundary to font
//to test for click x2 and y2 should define clickable FTControl

//x1 is the start of the checkbox
//x2 is the end of the control (checkbox+text)
//x3 is the end of the checkbox and start of checkbox text

//   bw=tcontrol->x3-tcontrol->x1;
//   bh=tcontrol->y3-tcontrol->y1; 


//x3 is the end of the checkbox and start of checkbox text
	if (tcontrol->x3<=0 || tcontrol->x3>tcontrol->x2) {
		tcontrol->x3=tcontrol->x1+fw*2;
	}
	bh=tcontrol->y2-tcontrol->y1; 


	bw=tcontrol->x2-tcontrol->x1;
	bw2=tcontrol->x3-tcontrol->x1; //width of checkbox square
//   bh=tcontrol->y2-tcontrol->y1; 

//adjust bw to end of text?

//   tcontrol->x2=tcontrol->x1+bw+(fw*(strlen(tcontrol->text)+1));
   //tcontrol->y2=tcontrol->y3;
    //fprintf(stderr,"checkbox FTControl %s is button in window %d\n",tcontrol->name,win);



//draw rectangle underneath or else outline square when control has focus is always there
//was:
//do not draw gray rectangle under text, just put text on top of whatever is there
#if Linux
		XSetForeground(PStatus.xdisplay,twin->xgc,twin->color);
#endif
#if WIN32
		//DeleteObject(tcontrol->brush);
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]); 
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,twin->color);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
#endif	
        //fprintf(stderr,"xfillrect\n");
#if Linux
        XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw-1,bh-1);
#endif
#if WIN32
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//GetClientRect(twin->hwindow,&twin->rect);
		//SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

		tcontrol->rect.left=x1;
		tcontrol->rect.top=y1;
		tcontrol->rect.right=x2;
		tcontrol->rect.bottom=y2;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,GetStockObject(DC_BRUSH));
#endif




    //clip
		//draw square for checkbox - use button height for both dimensions to be sure X will be squarely aligned
#if Linux
    XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[1]);  //dark gray - control button color (and border)
//    XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1, bw, bh);
    //XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1,bw2, bh);
		XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1,bh, bh);
    XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]);  //light gray
		//XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[1]);  //text bkgnd color (white)
//    XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw-1, bh-1);
    //XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw2-1, bh-1);
		XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1,tcontrol->y1+1,bh-1,bh-1);
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]); 
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//tcontrol->rect.left=tcontrol->x1;
		//tcontrol->rect.top=tcontrol->y1;
		//tcontrol->rect.right=tcontrol->x1+bw2;
		//tcontrol->rect.bottom=tcontrol->y1+bh;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);  //light gray
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

		tcontrol->rect.left=tcontrol->x1;//+1;
		tcontrol->rect.top=tcontrol->y1;//+1;
		//tcontrol->rect.right=tcontrol->x1+bw2-1;
		//tcontrol->rect.bottom=tcontrol->y1+bh-1;
		tcontrol->rect.right=tcontrol->x1+bh;//-1;
		tcontrol->rect.bottom=tcontrol->y1+bh;//-1;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,GetStockObject(DC_BRUSH));
#endif

    //is checked draw X
    //fprintf(stderr,"checkbox value =%d\n",tcontrol->value);
    if (tcontrol->value)
      {
      //fprintf(stderr,"drawing line color =%d\n",tcontrol->color[1]);
				//draw X in checkbox (2 lines for each bar)
#if Linux
      XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);
      XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+2,tcontrol->y1+1,tcontrol->x1+bh-1,tcontrol->y2-1);
      XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+1,tcontrol->y1+1,tcontrol->x1+bh-2,tcontrol->y2-1);
      XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+bh-2,tcontrol->y1+1,tcontrol->x1+1,tcontrol->y2-1);
      XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+bh-1,tcontrol->y1+1,tcontrol->x1+2,tcontrol->y2-1);
#endif
#if WIN32
	//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
	//SelectObject(twin->hdcWin,tcontrol->pen);
	SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
	SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

	tp[0].x=tcontrol->x1+2;	tp[0].y=tcontrol->y1+1;	tp[1].x=tcontrol->x1+bh-1;	tp[1].y=tcontrol->y2-1;
	Polyline(twin->hdcWin,tp,2);
	tp[0].x=tcontrol->x1+1;	tp[1].x=tcontrol->x1+bh-2; //tp[1].y=tcontrol->y2-1;	
	Polyline(twin->hdcWin,tp,2);
	tp[0].x=tcontrol->x1+bh-2;	tp[0].y=tcontrol->y1+1;	tp[1].x=tcontrol->x1+1;	tp[1].y=tcontrol->y2-1;
	Polyline(twin->hdcWin,tp,2);
	tp[0].x=tcontrol->x1+bh-1;	tp[1].x=tcontrol->x1+2;
	Polyline(twin->hdcWin,tp,2);
#endif

      }

    if (!(tcontrol->flags&CDoNotShowText)) {  //for now show text right of checkbox
#if Linux
      XSetForeground (PStatus.xdisplay, twin->xgc,tcontrol->textcolor[0]);
//todo: clip to tcontrol->x2
      XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x3+fw,tcontrol->y1+fh+1,tcontrol->text,strlen(tcontrol->text));
//      XDrawImageString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x2+fw,tcontrol->y1+fh,tcontrol->text,strlen(tcontrol->text));
#endif
#if WIN32
			SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
			SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
			MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tcontrol->text,-1,(LPWSTR)tcontrol->text_16,FTMedStr);
			TextOut(twin->hdcWin,tcontrol->x3+fw,tcontrol->y1+1,(LPWSTR)tcontrol->text_16,strlen(tcontrol->text));
#endif
      //fprintf(stderr,"%d = x2\n",tcontrol->x2);
      }

//sometimes user doesn't make text long enough and box cuts off text- the entire focus highlight probably should not be the default
//and is not used in most programs anymore
		if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif

		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl

/* currently not implemented because for a text area we might have to redraw a lot of text with each mouse move
		//highlight checkbox if mouse over
		if (!(PStatus.flags&PNoMouseOverHighlight) && (tcontrol->flags&CMouseOver)) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SetDCPenColor(twin->hdcWin,tcontrol->color[1]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif
		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
*/


#if Linux
		XFlush(PStatus.xdisplay);
#endif
//        XSync(PStatus.xdisplay,1);  
#if WIN32
		GdiFlush();
#endif


      //call any onDraw functions
      if (tcontrol->OnDraw!=0) {
				(*tcontrol->OnDraw) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			} //OnDraw


    break;  //end checkbox

    case CTOption:  //Option (or radio, or radial control)

			//Explain how radio button is implemented: is 1 control with many options, or multiple controls that are options

	//todo: change CTOption to use x1->x2 as full size and x3=point between checkbox and label text

//      if (PStatus.flags&PInfo) 
//        fprintf(stderr,"FTControl %s is radio in window %d\n",tcontrol->name,win);
      //first time drawn, adjust boundary to font

		 //tcontrol->x2=tcontrol->x1+bw+(tcontrol->font->max_bounds.width*(strlen(tcontrol->text)+1));
//     tcontrol->y2=tcontrol->y3;
    //fprintf(stderr,"radio FTControl %s is button in window %d\n",tcontrol->name,win);

//x3 is the end of the radio button and start of radio button text
//in theory person could adjust this, but I see no reason why somebody would want to
//	   if (tcontrol->x3<=0 || tcontrol->x3>tcontrol->x2) {
//				tcontrol->x3=tcontrol->x1+fw;
//		}

		bw2=tcontrol->x3-tcontrol->x1; //width of radio button circle

//draw rectangle underneath or else outline square when control has focus is always there
//was:
//do not draw gray rectangle under text, just put text on top of whatever is there
#if Linux
		XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->ControlBackgroundColor);
#endif
#if WIN32
		//DeleteObject(tcontrol->brush);
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]); 
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBackgroundColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
#endif	
        //fprintf(stderr,"xfillrect\n");
#if Linux
        XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw-1,bh-1);
#endif
#if WIN32
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//GetClientRect(twin->hwindow,&twin->rect);
		//SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

		tcontrol->rect.left=x1;
		tcontrol->rect.top=y1;
		tcontrol->rect.right=x2;
		tcontrol->rect.bottom=y2;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,GetStockObject(DC_BRUSH));
#endif



    //clip
#if Linux
		XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlBorderColor);
		//XSetFillStyle(PStatus.xdisplay,twin->xgc, FillSolid  );
		//XDrawArc(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,bw2,bh,0,23040); //360*64=23040
		//XDrawArc is too thin (and it's better to make it proportional-XFillArc is just the fast and easy answer without too much gain)
		XFillArc(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,bw2,bh,0,23040); //360*64=23040
			//XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1, bw, bh);
		XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlBackgroundColor);
		XFillArc(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+bw2/12,tcontrol->y1+bh/12,bw2*5/6,bh*5/6,0,23040);
#endif
#if WIN32
//probably could do this faster with a single ellipse with different fill and background color
		//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[1]); //ellipse outline
		//SelectObject(twin->hdcWin,tcontrol->pen);
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]); //ellipse filled color
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCPenColor(twin->hdcWin,tcontrol->ControlBorderColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBackgroundColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

		Ellipse(twin->hdcWin,tcontrol->x1,tcontrol->y1,tcontrol->x1+bw2,tcontrol->y1+bh);
		//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[0]); //ellipse outline
		//SelectObject(twin->hdcWin,tcontrol->pen);
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]); //ellipse filled color
		//SelectObject(twin->hdcWin,tcontrol->brush);

//	Ellipse(twin->hdcWin,tcontrol->x1+bw/12,tcontrol->y1+bh/12,tcontrol->x1+bw2*5/6,tcontrol->y1+bh*5/6);
//Arc(twin->hdcWin,tcontrol->x1,tcontrol->y1,tcontrol->x1+bw2,tcontrol->y1+bh,tcontrol->x1,0,tcontrol->x1,0);  //start and end on same point, left center on arc (ellipse)
//Arc(twin->hdcWin,tcontrol->x1+bw/12,tcontrol->y1+bh/12,tcontrol->x1+bw2*5/6,tcontrol->y1+bh*5/6,tcontrol->x1+bh/12,0,tcontrol->x1+bh/12,0);
#endif
    //XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw-1, bh-1);

    //is checked draw X
    //fprintf(stderr,"checkbox value =%d\n",tcontrol->value);
    if (tcontrol->value) {
		//option control is selected
      //fprintf(stderr,"drawing line color =%d\n",tcontrol->color[1]);
#if Linux
      XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlButtonDownColor);
//XFillArc(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+bw*3/16,tcontrol->y1+bh*3/16,bw*5/8,bh*5/8,0,360*64);
      XFillArc(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+bw2/4,tcontrol->y1+bh/4,bw2/2,bh/2,0,23040);
#endif
#if WIN32
//			tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[1]); //ellipse outline
//			SelectObject(twin->hdcWin,tcontrol->pen);
//			tcontrol->brush=CreateSolidBrush(tcontrol->color[1]); //ellipse filled color
//			SelectObject(twin->hdcWin,tcontrol->brush);
			SetDCPenColor(twin->hdcWin,tcontrol->ControlButtonDownColor);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			SetDCBrushColor(twin->hdcWin,tcontrol->ControlButtonDownColor);
			SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

			//Ellipse(twin->hdcWin,tcontrol->x1+bw2/4,tcontrol->y1+bh/4,tcontrol->x1+bw2/2,tcontrol->y1+bh/2);
			Ellipse(twin->hdcWin,tcontrol->x1,tcontrol->y1,tcontrol->x1+bw2,tcontrol->y1+bh);
#endif
      } //tcontrol->value

// Currently focus highlight/outline is disabled and probably should not be the default- most apps don't use the tabbing highlight anymore
//and so it is a low priority
			//If this option button has the focus draw a border around it- note only for option with selection- not around all options
		if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SetDCPenColor(twin->hdcWin,tcontrol->color[1]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif
		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl


		//highlight option if mouse over
		if (!(PStatus.flags&PNoMouseOverHighlight) && tcontrol->flags&CMouseOver) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif
		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl




    if (!(tcontrol->flags&CDoNotShowText)) {  //for now show text right of checkbox
#if Linux
		XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);
		XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x3+fw,tcontrol->y1+fh+1,tcontrol->text,strlen(tcontrol->text));
#endif
#if WIN32
		SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
		SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
		MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tcontrol->text,-1,(LPWSTR)tcontrol->text_16,FTMedStr);
		TextOut(twin->hdcWin,tcontrol->x3+fw,tcontrol->y1+1,(LPWSTR)tcontrol->text_16,strlen(tcontrol->text));
#endif
			//fprintf(stderr,"%d = x2\n",tcontrol->x2);
      }

#if Linux
      XFlush(PStatus.xdisplay);
#endif
     // XSync(PStatus.xdisplay,1);  
#if WIN32
	GdiFlush();
#endif


      //call any onDraw functions
      if (tcontrol->OnDraw!=0) {
				(*tcontrol->OnDraw) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			}


    break;  //end radio

    case CTFileOpen:  //File Open Window (lists files and folders)
		if (PStatus.flags&PInfo) {
			fprintf(stderr,"CTFileOpen control\n");
		}

		//Draw background white box
#if Linux
		pmap = XCreatePixmap(PStatus.xdisplay,twin->xwindow,bw,bh,PStatus.depth);
			if (pmap<=0) {
				fprintf(stderr,"XCreatePixmap failed\n");
			}
		//XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[1]);
		//XFillRectangle(PStatus.xdisplay,pmap, twin->xgc, tcontrol->x1, tcontrol->y1, bw, bh);
		//possibly just drawing 2 lines is faster - this is to pmap
		//XFillRectangle(PStatus.xdisplay,pmap,twin->xgc,0,0,bw,bh);
		XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[0]);  //only white?
		//      XFillRectangle(PStatus.xdisplay,pmap, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw-1, bh-1);
		XFillRectangle(PStatus.xdisplay,pmap,twin->xgc,1,1,bw-1,bh-1);
		XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[1]);
		XDrawLine(PStatus.xdisplay,pmap,twin->xgc,0,bh,bw-1,bh);
		XDrawLine(PStatus.xdisplay,pmap,twin->xgc,bw-1,0,bw-1,bh);
 
		XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  //text color
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->ControlBackgroundColor); //background color
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBackgroundColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x1+bw-1;
		tcontrol->rect.bottom=tcontrol->y1+bh-1;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));

		 //capture image of control into pmap?
		//tcontrol->brush=CreateSolidBrush(tcontrol->ControlBorderColor); //border color
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBorderColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tp[0].x=tcontrol->x1;	tp[0].y=tcontrol->y2-1;	tp[1].x=tcontrol->x2-1;	tp[1].y=tcontrol->y2-1;
		Polyline(twin->hdcWin,tp,2);
		tp[0].x=tcontrol->x2-1;	tp[0].y=tcontrol->y1;	
		Polyline(twin->hdcWin,tp,2);

		SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
		SetTextColor(twin->hdcWin,tcontrol->TextColor);
		SetBkColor(twin->hdcWin,tcontrol->TextBackgroundColor);		
#endif
		cx=0;  //x location of cursor

//see if curdir has changed (by user)
#if Linux
		getcwd(tstr,FTMedStr);
#endif
#if WIN32
		_getcwd(tstr,FTMedStr);
#endif
		//if the current directory has changed from the one listed in the control (text2) or the reread flag is set, redraw the itemlist
		if (strcmp(tstr,tcontrol->text2) || tcontrol->ilist==0 || tcontrol->ilist->item==0 || (tcontrol->ilist->flags&FTIL_REREAD)) {
			//delete any items that were in the itemlist before
			if (tcontrol->ilist!=0) {
			//         fprintf(stderr,"free FTItem list %p\n",tcontrol->ilist);
				DelFTItemList(tcontrol->ilist);
				tcontrol->ilist=0;
			}  //ilist!=0

#if Linux
			getcwd(tcontrol->FileOpenPath,FTBigStr); //FTBiggestStr); //union with text2
#endif
#if WIN32
			_getcwd(tcontrol->FileOpenPath,FTBigStr); //FTBiggestStr); //
#endif

			//if a textbox in the same window is linked to this FileOpen control's path update it's path name text
			if (tcontrol->TextBoxPath!=0) {
				strcpy(tcontrol->TextBoxPath->text,tcontrol->text2);
				DrawFTControl(tcontrol->TextBoxPath); //redraw the textbox control
			} //if (tcontrol->TextBoxPath!=0) {

			//tcontrol->CursorLocationInChars=0;  //first FTItem is top of window
			tcontrol->FirstItemShowing=1;  //0 first FTItem is top of window
#if WIN32
			dirp=FindFirstFile(TEXT("*"),&fileinfo);
			if (dirp != INVALID_HANDLE_VALUE) {
#endif

#if Linux
			dirp=opendir(tcontrol->text2);
			if (dirp != 0) {
#endif
			   //make a temporary FTItem
			   litem=(FTItem *)(FTItem *)malloc(sizeof(FTItem));
			   memset(litem,0,sizeof(FTItem));
#if Linux
				watch=XCreateFontCursor(PStatus.xdisplay,XC_watch);
				XDefineCursor(PStatus.xdisplay,twin->xwindow,watch);
				XFlush(PStatus.xdisplay); ///need to update cursor
#endif
#if WIN32
				GdiFlush();
#endif

				//go through all the folders and files and add them to the itemlist
#if Linux
				while ((direp = readdir(dirp))) {
				   //fprintf(stderr,"%s\n",direp->d_name);

		//           fprintf(stderr,"%d\n",stat(direp->d_name, &statBuffer));
		  //         fprintf(stderr,"tcontrol->ilist=%p\n",tcontrol->ilist);
				   if (strcmp(direp->d_name,".")!=0) {

					 if (stat(direp->d_name, &statBuffer)==0) {
		//               if (S_ISDIR(statBuffer.st_mode))  //is directory
		//               tcontrol->ilist=AddFTItem(tcontrol->ilist,direp->d_name,1);
		//               tcontrol->ilist=AddFTItem(tcontrol->ilist,direp->d_name,S_ISDIR(statBuffer.st_mode));
 
						 //litem->name=(char *)malloc(strlen(direp->d_name)+1);
						strcpy(litem->name,direp->d_name);
						litem->flags=S_ISDIR(statBuffer.st_mode);
		//ctime=time of last change
		//mtime=last modification
		//atime=last access
						litem->idate = *localtime(&statBuffer.st_mtime);
						litem->size = statBuffer.st_size;
		//                 AddFTItem(tcontrol->ilist,litem);
						AddFTItem(tcontrol,litem);
						 //free(litem->name);
						 //litem->name=0;
						strcpy(litem->name,"");
           			} //stat
				   } //strcmp

		//		   	FindNextFile(dirp,&fileinfo);  //go to next folder/file
				} //while
				rewinddir(dirp);
				//fprintf(stderr,"after adding FTItems\n");
				XUndefineCursor(PStatus.xdisplay,twin->xwindow);
				free(litem);
#endif //Linux

#if WIN32
				do {
					if (wcscmp(fileinfo.cFileName,TEXT("."))!=0) {  // not "." symbol

						if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  //is a directory
							litem->flags|=FTI_FOLDER;
						} else {
							 litem->flags&=~FTI_FOLDER;
						}
						//strcpy(litem->name,fileinfo->cFileName);
						//MultiByteToWideChar(CP_UTF8,0,(LPCSTR)litem->name,sizeof(fileinfo.cFileName),fileinfo.cFileName,sizeof(fileinfo.cFileName));
						WideCharToMultiByte(CP_UTF8,0,fileinfo.cFileName,sizeof(fileinfo.cFileName),(LPSTR)litem->name,sizeof(fileinfo.cFileName),NULL,NULL);
						wcscpy(litem->name_16,fileinfo.cFileName);
                 
						//ctime=time of last change
						//mtime=last modification
						//atime=last access
						 //litem->idate = *localtime(&statBuffer.st_mtime);
						 //litem->idate = FileTimeToLocalFileTime(fileinfo.ftCreationTime);
						FileTimeToSystemTime(&fileinfo.ftLastWriteTime,&litem->idate);
						litem->size = ((unsigned long long)fileinfo.nFileSizeHigh<<32) | fileinfo.nFileSizeLow;
						AddFTItem(tcontrol,litem);
						strcpy(litem->name,"");
					} //if (wcscmp(fileinfo.cFileName,TEXT("."))!=0) {
				   //} //strcmp
				} 
				while (FindNextFile(dirp, &fileinfo) !=0); //do-while
				free(litem);
#endif //WIN32

				tcontrol->numvisible=(tcontrol->y2-tcontrol->y1-1)/fh;   //is calc again later, because the control may have been resized with no new file read
				if (tcontrol->flags&CVScroll) {
					UpdateVScrollSlide(tcontrol);  //determine dimensions of scroll back FTControl ?
					CalcVScrollSlide(twin,tcontrol);  //determine size, needs numvisible
				} //tcontrol->flags
				if (tcontrol->flags&CHScroll) {
				  //UpdateHScrollSlide(tcontrol);  //determine dimensions of scroll back FTControl ?
				  //CalcHScroll(tcontrol);  //determine size, needs numvisible
				  CalcHScrollSlide(twin,tcontrol);  //determine size, and position
				} //tcontrol->flags

#if Linux

			} else  { //dirp!=NULL
      
			fprintf(stderr,"Could not open the current directory\n");
		}	//dirp!=NULL

		closedir(dirp);
#endif

#if WIN32
		} else  { //if (dirp != INVALID_HANDLE_VALUE
			fprintf(stderr,"Could not open the current directory\n");
		}	//if (dirp != INVALID_HANDLE_VALUE
#endif

		//Sort the item list if it was last sorted
        tcontrol->ilist->flags&=~(FTIL_REREAD); //clear reread flag
        //i[0]=start of FTControl
        //i[1]=sort 0=name,1=size,2=date
        //i[2]=direction 0=asc,1=desc
        //SortFTItemList(tcontrol->ilist,tcontrol->FirstCharacterShowing,tcontrol->StartSelectedTextInChars); 
		SortFTItemList(tcontrol->ilist,tcontrol->SortKind,tcontrol->SortDirection); 
        }  //end if ilist==0 || tcontrol->ilist->flags&I_REREAD

		//location of text - list of folders and files
        //cx=1;  //CursorX
		cx=0;  //CursorX
		//cy=tcontrol->y1+1;  //CursorY
#if Linux
		cy=fh+2;  //CursorY - because this is drawn to an image, not to the window, the initial Y offset is only 2, not tcontrol->y1
#endif
#if WIN32
		cy=tcontrol->y1+2;  //CursorY
#endif
		litem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->FirstItemShowing);  //get first item showing (first item number=1)
        if (litem!=0) {  //there is an item
	       //numvisible, perhaps should be i[4] or something
		    tcontrol->numvisible=(tcontrol->y2-tcontrol->y1-1)/fh;
			//maxwidth=(bw-fh-1)/fw;  //-fh for file/folder image
			maxwidth=(bw-fw-fh-1-2*fw)/fw;  //-2*fw for initial and last space, -fh for  file/folder image
#if Linux
			maxwidth-=fh;  //in Linux folder/file.bmp width is 2xfh
#endif

			tcontrol->ilist->width=maxwidth*fw; //needed for DrawSelectedFTItem which uses maxwidth
			//j=the number of rows with actual items
			j=(tcontrol->numvisible>tcontrol->ilist->num)?tcontrol->ilist->num:tcontrol->numvisible;

			//ti=end of first field, tj=end of second field in characters- probably should make i[3],i[4],...
            ti=maxwidth;
            tj=ti;
            if (tcontrol->flags&CFOName) {
				sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				ti=(int)(maxwidth*((float)tcontrol2->x2/(float)tcontrol->x2));
				//fprintf(stderr,"%s %d %d %d\n",tcontrol2->name,ti,tcontrol2->x2,tcontrol->x2);
            }
            if (tcontrol->flags&CFOSize) {
				sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				tj=(int)(maxwidth*((float)tcontrol2->x2/(float)tcontrol->x2));
				//fprintf(stderr,"%s %d %d %d\n",tcontrol2->name,ti,tcontrol2->x2,tcontrol->x2);
            }


			//for each FTItem showing
			//for(i=tcontrol->CursorLocationInChars;i<tcontrol->CursorLocationInChars+j;i++) {
			for(i=tcontrol->FirstItemShowing;i<tcontrol->FirstItemShowing+j;i++) {
				if (cy<tcontrol->y2) { //if there is enough vertical space
//#if Linux
//					sprintf(tstr,"%s %d %02d/%02d/%02d",litem->name,litem->size,litem->idate.tm_mon+1,litem->idate.tm_mday,litem->idate.tm_year-100);
//#endif
//#if WIN32
//					sprintf(tstr,"%s %d %02d/%02d/%02d",litem->name,litem->size,litem->idate.wMonth,litem->idate.wDay,litem->idate.wYear);
//#endif
					memset(tstr,0x20,maxwidth+1);  //clear the temporary string with spaces
					cx=strlen(litem->name);  //cx=length of the file or folder name
					memcpy(tstr,litem->name,ti>cx?cx:ti);  //copy the file or folder name to the temporary string
					
					//Draw File Sizes if that control flag is set
					if (tcontrol->flags&CFOSize) {

						if (litem->size>1000000000) {
						  sprintf(tstr2,"%d GB",litem->size/1000000000);
						} else {
							if (litem->size>1000000) {
								sprintf(tstr2,"%d MB",litem->size/1000000);
							} else {
								if (litem->size>1000) {
									sprintf(tstr2,"%d KB",litem->size/1000);
								} else {
									sprintf(tstr2,"%d  B",litem->size/1000000);
								}
							}
						}
						cx=strlen(tstr2);  
						cw=tj-cx-1<ti?ti:tj-cx-1;
						memcpy(&tstr[cw],tstr2,tj-ti>cx?cx:tj-ti);
					}  //end if (tcontrol->flagas&CFOSize) {

					//Draw File/Folder Date if that control flag is set
					if (tcontrol->flags&CFODate) {
#if Linux
						if (litem->idate.tm_hour>12)              
							sprintf(tstr2,"%02d/%02d/%02d %02d:%02d:%02d PM",litem->idate.tm_mon+1,litem->idate.tm_mday,litem->idate.tm_year-100,litem->idate.tm_hour-12,litem->idate.tm_min,litem->idate.tm_sec);
						else
							sprintf(tstr2,"%02d/%02d/%02d %02d:%02d:%02d AM",litem->idate.tm_mon+1,litem->idate.tm_mday,litem->idate.tm_year-100,litem->idate.tm_hour,litem->idate.tm_min,litem->idate.tm_sec);
#endif
#if WIN32
						if (litem->idate.wHour>12)              
							sprintf(tstr2,"%02d/%02d/%02d %02d:%02d:%02d PM",litem->idate.wMonth,litem->idate.wDay,litem->idate.wYear,litem->idate.wHour-12,litem->idate.wMinute,litem->idate.wSecond);
						else
							sprintf(tstr2,"%02d/%02d/%02d %02d:%02d:%02d AM",litem->idate.wMonth,litem->idate.wDay,litem->idate.wYear,litem->idate.wHour,litem->idate.wMinute,litem->idate.wSecond);

#endif
						cx=strlen(tstr2);
						cw=maxwidth-cx-1<tj?tj:maxwidth-cx-1;
						memcpy(&tstr[cw],tstr2,maxwidth-tj>cx?cx:maxwidth-tj);
					} //if (tcontrol->flags&CFODate)


					tstr[maxwidth+1]=0;  //terminate temporary string
					cw=maxwidth;  //cw=total text width (in characters)

					//if this is the currently selected item and this control has focus- set the selected text color, otherwise set normal color text
					//if (i==tcontrol->ilist->cur && twin->FocusFTControl==tcontrol) { 
					if ((litem->flags&FTI_SELECTED) && twin->FocusFTControl==tcontrol) { 

						//if a textbox in the same window is linked to this FileOpen control's selected filename update it's file name text
						//should only get set when selected - not when redrawn
						//if (tcontrol->TextBoxFile!=0) {
						//	strcpy(tcontrol->TextBoxFile->text,tcontrol->FileOpenFile); //union with ->text (could also copy tcontrol->filename)
						//	DrawFTControl(tcontrol->TextBoxFile); //redraw textbox
						//} //if (tcontrol->TextBoxPath!=0) {

#if Linux
						XSetForeground(PStatus.xdisplay, twin->xgc, tcontrol->SelectedTextBackgroundColor); //blue
						XFillRectangle(PStatus.xdisplay,pmap,twin->xgc,0,cy-fh,bw-1,fh);

						//draw folder/file image
						XSetForeground(PStatus.xdisplay, twin->xgc,tcontrol->SelectedTextColor);     //white
						XSetBackground(PStatus.xdisplay, twin->xgc,tcontrol->SelectedTextBackgroundColor);    //blue
#endif
#if WIN32
						SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
						SetTextColor(twin->hdcWin,tcontrol->SelectedTextColor);
						SetBkColor(twin->hdcWin,tcontrol->SelectedTextBackgroundColor);		

#endif

					} else {
#if Linux
						XSetForeground (PStatus.xdisplay, twin->xgc,tcontrol->TextColor);    //black
						XSetBackground (PStatus.xdisplay, twin->xgc,tcontrol->TextBackgroundColor);     //white
#endif
#if WIN32
						SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
						SetTextColor(twin->hdcWin,tcontrol->TextColor);
						SetBkColor(twin->hdcWin,tcontrol->TextBackgroundColor);		
#endif
					} //if (i==tcontrol->ilist->cur && twin->FocusFTControl==tcontrol) { 

					//draw folder image
					if (litem->flags&FTI_FOLDER) {
#if Linux
						cx=1;
						if (tcontrol->ximage[0]<=0) {
							fprintf(stderr,"XPutImage would fail because tcontrol->ximage[0]<=0 for folder.bmp\n");
						} else {
							//XPutImage(PStatus.xdisplay,pmap,twin->xgc,tcontrol->ximage[0],0,0,cx,cy-fh,fh,fh);
							XPutImage(PStatus.xdisplay,pmap,twin->xgc,tcontrol->ximage[0],0,0,fw,cy-fh,fh,fh);
						}
#endif
#if WIN32
						cx=tcontrol->x1;
						if (tcontrol->image[0]<=0) {
							fprintf(stderr,"Draw Bitmap would fail because tcontrol->image[0]<=0 for folder.bmp\n");
						} else {
							curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
							if (curObj!=tcontrol->hbmp[0]) {
								origObj=SelectObject(twin->hdcMem,tcontrol->hbmp[0]);  //copy bitmap to hdc- doing this again causes the image to look mostly black
							}
							if (tcontrol->bmpinfo[0]==0) {
								fprintf(stderr,"Control bmpinfo structure needs to be set to draw folder in FileOPen control.\n");
							} else {
								StretchBlt(twin->hdcWin,cx+fw,cy-2,fh,fh,twin->hdcMem,0,0,tcontrol->bmpinfo[0]->biWidth,tcontrol->bmpinfo[0]->biHeight,SRCCOPY);
							}
						} //if (tcontrol->image[0]<=0) {
#endif
					} else {  //if (litem->flags&FTI_FOLDER) {
						//Draw File image
#if Linux
						cx=1;
						if (tcontrol->ximage[1]<=0) {
							fprintf(stderr,"XPutImage would fail because tcontrol->ximage[1]<=0 for file.bmp\n");
						} else {
							//XPutImage(PStatus.xdisplay,pmap,twin->xgc,tcontrol->ximage[1],0,0,cx,cy-fh,fh,fh);
							XPutImage(PStatus.xdisplay,pmap,twin->xgc,tcontrol->ximage[1],0,0,fw,cy-fh,fh,fh);
						}
#endif
#if WIN32
						cx=tcontrol->x1;  

						if (tcontrol->image[1]<=0) {
							fprintf(stderr,"Draw Bitmap would fail because tcontrol->image[1]<=0 for file.bmp\n");
						} else {
							curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
							if (curObj!=tcontrol->hbmp[1]) {
								origObj=SelectObject(twin->hdcMem,tcontrol->hbmp[1]);  //copy bitmap to hdc- doing this again causes the image to look mostly black
							}
							if (tcontrol->bmpinfo==0) {
								fprintf(stderr,"Control bmpinfo structure needs to be set to draw file in FileOPen control.\n");
							} else {
								StretchBlt(twin->hdcWin,cx+fw,cy-2,fh,fh,twin->hdcMem,0,0,tcontrol->bmpinfo[1]->biWidth,tcontrol->bmpinfo[1]->biHeight,SRCCOPY);
							}
						}
#endif
					} //if (litem->flags&FTI_FOLDER)
             
	                //cx+=fh+1; //add a character to CursorX- fh because icons are square based on height
					cx+=fh; //add a character to CursorX- fh because icons are square based on height

	//              sprintf(tstr,"%s %2d/%2d/%2d",litem->name,litem->idate.tm_mon+1,litem->idate.tm_mday,litem->idate.tm_year);
	//              fprintf(stderr,"%s\n",tstr);

					//Draw the text (in the temporary string)
#if Linux
					cx+=fh; //folder or file width image is 2x fh in Linux
					XDrawString(PStatus.xdisplay,pmap,twin->xgc,cx,cy-2,tstr,cw);         
#endif
	//              XDrawImageString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy,litem->name,cw);         
#if WIN32
					MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tstr,-1,(LPWSTR)tstrw,FTMedStr);
					result=TextOut(twin->hdcWin,cx+fh,cy-2,tstrw,cw);
#endif
	            }//(cy<tcontrol->y2)
				cy+=fh;  //fh because icons are square based on height
				//cy+=fh+PStatus.PadY;  //fh because icons are square based on height

				//go to next item
				if (litem->next !=0) {
					litem=(FTItem *)litem->next;
				}
           }  //end for i - for each item showing


        }  //end if litem!=0

//#endif


//fprintf(stderr,"after itemlist\n");

#if Linux
		//In Linux, we draw everything to a bitmap, then capture to an image, and refresh using that which is faster than redrawing all the text, and smoother looking than just drawing directly to the screen- it may no longer be an issue on more modern computers to just draw text directly as is done in the Windows version
		Timage= XGetImage(PStatus.xdisplay,pmap,0,0,bw,bh,AllPlanes,ZPixmap);
		XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,Timage,0,0,tcontrol->x1,tcontrol->y1,bw,bh);
		XDestroyImage(Timage);
		Timage=0;
#endif

      //call any onDraw functions
		if (tcontrol->OnDraw!=0) {
			(*tcontrol->OnDraw) (twin,tcontrol);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol=GetFTControl(conname); //check to see if control was deleted
			if (tcontrol==0 || twin==0) return; //user deleted control or window
		} //OnDraw

#if Linux
		XFreePixmap(PStatus.xdisplay,pmap);
		pmap=0;
#endif
  //    fprintf(stderr,"after closedir\n");

		//update scrollbars
		if (tcontrol->flags&CVScroll) {
		//UpdateVScrollSlide(tcontrol); 
		//Draw the scroll FTControls
			/*
			sprintf(tstr,"%s%s",PStatus.PrefixVScrollUp,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
			sprintf(tstr,"%s%s",PStatus.PrefixVScrollDown,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
			sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
			sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
			*/
			DrawFTControl(tcontrol->FTC_VScrollUp);
			DrawFTControl(tcontrol->FTC_VScrollDown);
			DrawFTControl(tcontrol->FTC_VScrollBack);
			//todo: the size and position of the scroll slide should be only calculated when changed, not every time drawn
			//CalcVScrollSlide(twin,tcontrol);  //determine size, and position
			DrawFTControl(tcontrol->FTC_VScrollSlide);
		}  //CVScroll
		if (tcontrol->flags&CHScroll) {
			DrawFTControl(tcontrol->FTC_HScrollLeft);
			DrawFTControl(tcontrol->FTC_HScrollRight);
			DrawFTControl(tcontrol->FTC_HScrollBack);
			//CalcHScrollSlide(twin,tcontrol);  //determine size, and position
			DrawFTControl(tcontrol->FTC_HScrollSlide);
		}

		//update title buttons
		if (tcontrol->flags&CFOName) {  
			sprintf(tstr,"%s%s",PStatus.PrefixName,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
		}
		if (tcontrol->flags&CFOSize) {
			sprintf(tstr,"%s%s",PStatus.PrefixSize,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
		}
		if (tcontrol->flags&CFODate) {
			sprintf(tstr,"%s%s",PStatus.PrefixDate,tcontrol->name);
			tcontrol2=GetFTControl(tstr);
			DrawFTControl(tcontrol2);
		}

#if Linux
		XFlush(PStatus.xdisplay);
#endif
#if WIN32
		GdiFlush();
#endif
       // fprintf(stderr,"Done drawing fileopen\n");
    break;  //end CTFileOpen
    case CTImage:


//#if PINFO
//      fprintf(stderr,"DrawImage %s\n",tcontrol->name);
//      fprintf(stderr,"x,y,w,h %d %d %d %d\n",tcontrol->x1,tcontrol->y1,bw,bh);
//#endif
      if (tcontrol->image[0]!=0) {
#if Linux
        XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->ximage[0],0,0,tcontrol->x1,tcontrol->y1,bw,bh);
#endif
#if WIN32
			//if the user changed the image data, we need to CreateBitmap again
				DeleteObject(tcontrol->hbmp[0]);
				tcontrol->hbmp[0]=CreateBitmap(tcontrol->bmpinfo[0]->biWidth,tcontrol->bmpinfo[0]->biHeight,1,tcontrol->bmpinfo[0]->biBitCount,tcontrol->bmpdata[0]) ;
			//	tcontrol->image[0]=FT_LoadBitmapFileToFTControl(tcontrol,"images/test.bmp",0,0);
				curObj=GetCurrentObject(twin->hdcMem,OBJ_BITMAP);
				if (curObj!=tcontrol->hbmp[0]) {
					origObj=SelectObject(twin->hdcMem,tcontrol->hbmp[0]);  //copy bitmap to hdc- doing this again causes the image to look mostly black
				}
				//SelectObject(twin->hdcWin,tcontrol->hbmp[0]);  //copy bitmap to hdc
				StretchBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,tcontrol->x2-tcontrol->x1,tcontrol->y2-tcontrol->y1,twin->hdcMem,0,0,tcontrol->bmpinfo[0]->biWidth,tcontrol->bmpinfo[0]->biHeight,SRCCOPY);
				//BitBlt(twin->hdcWin,tcontrol->x1,tcontrol->y1,tcontrol->x2-tcontrol->x1,tcontrol->y2-tcontrol->y1,twin->hdcMem,0,0,SRCCOPY);
				GdiFlush();
				//origObj=SelectObject(twin->hdcMem,origObj);  //copy original object back to hdc
#endif
      }  //tcontrol->ximage[0]!=0

    break;  //CTImage
	case CTItemList:
		DrawFTItemList(tcontrol->ilist);

		//update scrollbars
		if (tcontrol->flags&CVScroll) {
			DrawFTControl(tcontrol->FTC_VScrollUp);
			DrawFTControl(tcontrol->FTC_VScrollDown);
			DrawFTControl(tcontrol->FTC_VScrollBack);
			//CalcVScrollSlide(twin,tcontrol);  //determine size, and position - 
			//a problem occurs when a mouse drags the slider not enough to change the first character- 
			//this resets the control y1 and y2
			//what is needed is that for an ItemList control, when an item is added or subtracted, the scroll needs to be updated then
			DrawFTControl(tcontrol->FTC_VScrollSlide);
		}  //CVScroll
		if (tcontrol->flags&CHScroll) {
			DrawFTControl(tcontrol->FTC_HScrollLeft);
			DrawFTControl(tcontrol->FTC_HScrollRight);
			DrawFTControl(tcontrol->FTC_HScrollBack);
			//CalcHScrollSlide(twin,tcontrol);  //determine size, and position
			DrawFTControl(tcontrol->FTC_HScrollSlide);
		}  //CHScroll



	break; //CTItemList
    case CTFileMenu:
    case CTDropdown:



	//When drawing a filemenu or dropdown item list, draw control, plus all open itemlists

	//Draw actual control
         bw=tcontrol->x2-tcontrol->x1+1;  //may need a scroll bar for text that is larger
         bh=tcontrol->y2-tcontrol->y1+1;

	//Draw all open itemlists connected to this control

//      if (tcontrol->type==CTDropdown  && !(tcontrol->flags&CSubMenu))  {	//dropdown box
      if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList)  {	//dropdown box
       

//Draw actual dropdown box control (not any open windows below it)
        if (PStatus.flags&PInfo) {
			if (tcontrol->type==CTDropdown) {
				fprintf(stderr,"Draw Dropdown control\n");
			} else {
				fprintf(stderr,"Draw ItemList control\n");
			}

		}

       //if (tcontrol->x3==0) //over write start x location of box with triangle
        //   tcontrol->x3=tcontrol->x2-20;// just make size of box standard at 32 pixels bw/5;
  
    //fprintf(stderr,"Draw dropdown box FTControl %s is button in window %d\n",tcontrol->name,win);

         cx=tcontrol->x1;
         cx2=tcontrol->x2;
         cy=tcontrol->y1;
         cy2=tcontrol->y2;

    //border //use textcolor or color?
				 //Draw actual dropdown box control, two filled rectangles, one for border, and one for inside
				 //probably an unfilled rect might be faster for border
#if Linux
          //XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]);  //lt gray (border)-probably don't need
//          XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1, bw+bw/5, bh);
          //XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1, bw-1,bh-1);
          XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[1]);  //white- text background color
//          XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw-1+bw/5, bh-1);
          XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1+1, tcontrol->y1+1, bw-2, bh-2);
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//tcontrol->rect.left=tcontrol->x1;
		//tcontrol->rect.top=tcontrol->y1;
		//tcontrol->rect.right=tcontrol->x2;
		//tcontrol->rect.bottom=tcontrol->y2;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);

		//tcontrol->brush=CreateSolidBrush(tcontrol->textcolor[1]);  //white text background color
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->textcolor[1]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x2-1;
		tcontrol->rect.bottom=tcontrol->y2-1;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));
#endif


        if (!(tcontrol->flags&C2D)) {
          //draw 3D lines 2 white, 2 black
#if Linux
          XSetForeground(PStatus.xdisplay,twin->xgc,white);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-1,tcontrol->y1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x1,tcontrol->y2-1);
          XSetForeground(PStatus.xdisplay,twin->xgc,black);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y2-1,tcontrol->x2-1,tcontrol->y2-1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x2-1,tcontrol->y1,tcontrol->x2-1,tcontrol->y2-1);
#endif
#if WIN32
		  //todo: implement for windows
					//tcontrol->pen=CreatePen(PS_SOLID,1,white); 
					//SelectObject(twin->hdcWin,tcontrol->pen);
					//tp[0].x=tcontrol->x1;	tp[0].y=tcontrol->y1-1;	tp[1].x=tcontrol->x2-1;	tp[1].y=tcontrol->y1-1;
					//Polyline(twin->hdcWin,tp,2);
					//tp[1].x=tcontrol->x1;	tp[1].y=tcontrol->y2;	
					//Polyline(twin->hdcWin,tp,2);
					//tcontrol->pen=CreatePen(PS_SOLID,1,black); 
					//SelectObject(twin->hdcWin,tcontrol->pen);
					//tp[0].y=tcontrol->y2;	tp[1].x=tcontrol->x2-1;	
					//Polyline(twin->hdcWin,tp,2);
					//tp[0].x=tcontrol->x2-1;	tp[0].y=tcontrol->y1;
					//Polyline(twin->hdcWin,tp,2);
#endif
        }  //end if !C2D

#if Linux
          XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]); //color of dropdown button lt gray
#endif
#if WIN32
			//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
			//SelectObject(twin->hdcWin,tcontrol->brush);
			SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
			SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

#endif


          //if (PStatus.MouseX>cx && PStatus.MouseX<cx+cx2 && PStatus.MouseY>=cy && PStatus.MouseY<=cy2)
          if ((tcontrol->flags&CMouseOver) || twin->FocusFTControl==tcontrol) {  //mouse is over FTControl
              //fprintf(stderr,"draw selected color=%x\n",tcontrol->color[2]);
              if (tcontrol->flags&CButtonDown(0)) { //and button is down
#if Linux
				XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[3]);
#endif
#if WIN32
				//tcontrol->brush=CreateSolidBrush(tcontrol->color[3]);
				//SelectObject(twin->hdcWin,tcontrol->brush);
				SetDCBrushColor(twin->hdcWin,tcontrol->color[3]);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

#endif

			} else {
								//mouse over dropdown control or control has focus but not with button down
#if Linux
                XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[2]);
#endif
#if WIN32
				//tcontrol->brush=CreateSolidBrush(tcontrol->color[2]);
				//SelectObject(twin->hdcWin,tcontrol->brush);
				SetDCBrushColor(twin->hdcWin,tcontrol->color[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));

#endif
			}

          } //end if mouse over  tcontrol->flags&CMouseOver

//         fprintf(stderr,"Draw dropdown\n");

//Draw triangle button on right side of dropdown control-    box behind triangle, and then filled triangle
#if Linux
          XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x3, tcontrol->y1+1,tcontrol->x2-tcontrol->x3,bh-2);


          XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]); //tcontrol->color[1]);
//      XDrawLine(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x2+1, tcontrol->y1+1, (bw/10)-1, bh-1);
          points=(XPoint *)malloc(sizeof(XPoint)*3);

          points[0].x=tcontrol->x3+2;
          points[0].y=tcontrol->y1+1+bh/3;
          points[1].x=tcontrol->x3+(tcontrol->x2-tcontrol->x3)/2;
          points[1].y=tcontrol->y1+1+bh*2/3;
          points[2].x=tcontrol->x2-2; 
          points[2].y=tcontrol->y1+1+bh/3;

//Draw a filled triangle - could do this for scroll bars
          XFillPolygon(PStatus.xdisplay,twin->xwindow,twin->xgc,points,3,Nonconvex,CoordModeOrigin);
 //XFillPolygon(PStatus.xdisplay,twin->xwindow,twin->xgc,points,3,Nonconvex,CoordModePrevious);
          free(points);
#endif

#if WIN32
		tcontrol->rect.left=tcontrol->x3;
		tcontrol->rect.top=tcontrol->y1+1;
		tcontrol->rect.right=tcontrol->x2;
		tcontrol->rect.bottom=tcontrol->y2-1;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));

		SetBkMode(twin->hdcWin,OPAQUE); //allow text background color
		SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
		SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
		
		//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
		//SelectObject(twin->hdcWin,tcontrol->pen);
		//tcontrol->brush=CreateSolidBrush(tcontrol->textcolor[0]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
		SetDCBrushColor(twin->hdcWin,tcontrol->textcolor[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));


    tp[0].x=tcontrol->x3+2;
    tp[0].y=tcontrol->y1+1+bh/3;
    tp[1].x=tcontrol->x3+(tcontrol->x2-tcontrol->x3)/2;
    tp[1].y=tcontrol->y1+1+bh*2/3;
    tp[2].x=tcontrol->x2-2; 
    tp[2].y=tcontrol->y1+1+bh/3;

		hrgn=CreatePolygonRgn(tp,3,WINDING);
//		SelectObject(twin->hdcWin,hrgn); - caused CTImage to stop working
		//FillRgn(twin->hdcWin,hrgn,tcontrol->brush);
		FillRgn(twin->hdcWin,hrgn,(HBRUSH)GetStockObject(DC_BRUSH));
		DeleteObject(hrgn);

#endif

		
		//figure out text to put in dropdown text
      if (!(tcontrol->flags&CDoNotShowText))	{  //for now show text right of checkbox
				//probably just use tcontrol->text which is set on buttonup
				//get selected text - this is set in the dropdown box
				//in theory there could be subitems (submenus) from a dropdown item
				//in which case the selected item might be from a subitemlist (submenu)
				if (tcontrol->value==0) { //no item has been selected
					//fprintf(stderr,"No dropdown item selected\n");
					strcpy(tstr,tcontrol->text); //just copy original text if any
					//if a person escapes - there should be the original selection
					//strcpy(tstr,""); //make empty - any original text is ignored
				} else { //tcontrol->value!=0 - get item
						//Show selected item in dropdown text box
						//could be subitemlists
					//fprintf(stderr,"dropdown value=%d\n",tcontrol->value);
					//tlist=tcontrol2->ilist;
					//titem=GetFTItemFromFTControlByNum(tlist,tcontrol->value-1);
					titem=GetFTItemFromFTControlByNum(tcontrol,tcontrol->value);
					//titem=GetFTItemFromFTItemListByNum(tcontrol->ilist,tcontrol->ilist->cur);
					//titem=GetSelectedFTItem(tcontrol->ilist);
					//in theory get text from items in subitemlists
					if (titem!=0) {
						strcpy(tstr,titem->name);
						//fprintf(stderr,"item name=%d\n",titem->name);
						//and put it in the control text- I debated just drawing the text in the control here, but it's better to check the value and make sure the text is ok, because there are currently places in the FT code where the value is changed but the text is not updated
						strcpy(tcontrol->text,titem->name);
						//and text_16
					} //titem!=0

				} ////tcontrol->value==0

				//DefaultColor
//Now Draw actual item text in dropdown textbox
#if Linux
        XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);
#endif
#if WIN32
			SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
			SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);		
#endif
//        XDrawImageString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+tcontrol->font->max_bounds.width,tcontrol->y1+tcontrol->font->max_bounds.ascent,tcontrol->text,strlen(tcontrol->text));

//Check that text fits into box
				if ((int)strlen(tstr)>(bw/fw)) { //trim text
#if Linux
				 XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+fw,tcontrol->y1+fh,tstr,bw/fw);
#endif
#if WIN32
				MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tstr,bw/fw,(LPWSTR)tcontrol->text_16,FTMedStr);
				TextOut(twin->hdcWin,tcontrol->x1+fw,tcontrol->y1,(LPWSTR)tcontrol->text_16,bw/fw);
#endif
				} else {
#if Linux
	        XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1+fw,tcontrol->y1+fh,tstr,strlen(tstr));
#endif
#if WIN32
					MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tstr,strlen(tstr),(LPWSTR)tcontrol->text_16,FTMedStr);
					TextOut(twin->hdcWin,tcontrol->x1+fw,tcontrol->y1,(LPWSTR)tcontrol->text_16,strlen(tstr));
#endif
				}
        //fprintf(stderr,"%d = x2\n",tcontrol->x2);
        }  //!CDoNotShowText



        if (!(tcontrol->flags&C2D)) {
          //draw 3D lines 4 black
#if Linux
//          XSetForeground(PStatus.xdisplay,twin->xgc,white);
          XSetForeground(PStatus.xdisplay,twin->xgc,black);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-1,tcontrol->y1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x1,tcontrol->y2-1);


          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y2-1,tcontrol->x2-1,tcontrol->y2-1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x2-1,tcontrol->y1,tcontrol->x2-1,tcontrol->y2-1);
#endif
#if WIN32
					//tcontrol->pen=CreatePen(PS_SOLID,1,black); 
					//SelectObject(twin->hdcWin,tcontrol->pen);
					//SetDCPenColor(twin->hdcWin,tcontrol->color[1]);
					SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));

					tp[0].x=tcontrol->x1;	tp[0].y=tcontrol->y1;	tp[1].x=tcontrol->x2-1;	tp[1].y=tcontrol->y1;
					Polyline(twin->hdcWin,tp,2);
					tp[1].x=tcontrol->x1;	tp[1].y=tcontrol->y2-1;	
					Polyline(twin->hdcWin,tp,2);

					tp[0].y=tcontrol->y2-1;	tp[1].x=tcontrol->x2-1;	
					Polyline(twin->hdcWin,tp,2);
					tp[0].x=tcontrol->x2-1;	tp[0].y=tcontrol->y1;
					Polyline(twin->hdcWin,tp,2);
#endif

        }  //end if !C2D

		if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
			//This control has the focus so draw a border around the control
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,black);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
			tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
			tp[4].x=x1;	tp[4].y=y1;
			Polyline(twin->hdcWin,tp,5);
#endif

		} //if (!(PStatus.flags&PNoFocusHighlight) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl


    }  //end if type=CTDropdown
		

 //Draw FileFTControl Menu (not submenu) (basically identical to any button, except any underlines)
//in fact - buttons should have underline hotkey text too
//    if (tcontrol->type==CTFileMenu && !(tcontrol->flags&CSubMenu)) {

	//for a FileMenu with no image draw the button
    if (tcontrol->type==CTFileMenu) {

		if (tcontrol->image[0]==0) {  //this control has no image
			if ((tcontrol->flags&CItemListOpen) || (tcontrol->flags&CMouseOver))  { //filemenu is open or mouse over
	#if Linux
				XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[2]);
	#endif
	#if WIN32
				//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
				//SelectObject(twin->hdcWin,tcontrol->brush);
				SetDCBrushColor(twin->hdcWin,tcontrol->color[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
	#endif
			} else { //filemenu is not open and no image - use normal button color
	#if Linux
				XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]);
	#endif
	#if WIN32
				//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
				//SelectObject(twin->hdcWin,tcontrol->brush);
				SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
	#endif
			} //if (tcontrol->flags&CItemListOpen) { //filemenu is open or mouse is over
	#if Linux
			//fprintf(stderr,"xfillrect\n");
			XFillRectangle(PStatus.xdisplay,twin->xwindow, twin->xgc, tcontrol->x1, tcontrol->y1, bw, bh);
#endif
#if WIN32
			tcontrol->rect.left=tcontrol->x1;
			tcontrol->rect.top=tcontrol->y1;
			tcontrol->rect.right=tcontrol->x2;
			tcontrol->rect.bottom=tcontrol->y2;
			//GetClientRect(twin->hwindow,&twin->rect);
			//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
			FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));
	#endif
			
		} //if (tcontrol->image[0]==0) {  //this control has no image



        if (!(tcontrol->flags&CNoChangeOnMouseOver)) { // && !(twin->flags&WItemListOpen)) { 
			if (tcontrol->image[0]==0) {  //this control has no image
				//draw selected button if filemenu has focus (not for mouse over because that changes color) and button 1 not down
				//when tabbed to- 
				if (!(tcontrol->flags&CItemListOpen) && twin->FocusFTControl==tcontrol) { // && !(tcontrol->flags&CButtonDown(0))) {  //mouse is over FTControl
					//Draw a border around the FileMenu button
	#if Linux
					XSetForeground(PStatus.xdisplay,twin->xgc,black);
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
	#endif
	#if WIN32
					SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
					tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
					tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
					tp[4].x=x1;	tp[4].y=y1;
					Polyline(twin->hdcWin,tp,5);
	#endif

				} //if (!(tcontrol->flags&CItemListOpen) && (tcontrol->flags&CMouseOver || twin->FocusFTControl==tcontrol) && !(tcontrol->flags&CButtonDown(0)) {  //mouse is over FTControl

			} else { 			//if (tcontrol->image[0]==0)
				 //this control has an image

				  //draw scaled bitmap onto button
				  //fprintf(stderr,"Draw Image\n");
#if Linux
				  XPutImage(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->ximage[0],0,0,tcontrol->x1,tcontrol->y1,tcontrol->x2-tcontrol->x1,tcontrol->y2-tcontrol->y1);
#endif
#if WIN32
				  //todo: add
#endif
			} //		if (tcontrol->image[0]==0) {  //this control has no image

		} //if (!(tcontrol->flags&CNoChangeOnMouseOver)) { 


    // fprintf(stderr,"after rect\n");


		//todo: change to !(tcontrol->flags&CHideText)
    if (!(tcontrol->flags&CDoNotShowText)) {

        //figure out text color
		if (!(tcontrol->flags&CNoChangeOnMouseOver)) { // && !(twin->flags&WItemListOpen)) { 
			if (tcontrol->flags&CItemListOpen) {  //FileMenu is open - draw inverse colored text
	//              fprintf(stderr,"draw mouse move color[2]=%x\n",tcontrol->color[2]);
	//              if (tcontrol->flags&CButtonDown) //and button is down
	//              XSetForeground (PStatus.xdisplay, twin->xgc, ~tcontrol->color[1]);
	//            else //button is not down
	#if Linux
				//XSetForeground (PStatus.xdisplay, twin->xgc, ~tcontrol->textcolor[0]);
				XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[1]);  //selected text color
	#endif
	#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				SetBkMode(twin->hdcWin,TRANSPARENT); //do not allow text background color
				SetTextColor(twin->hdcWin,tcontrol->textcolor[1]);  //reverse text colors when in focus
				SetBkColor(twin->hdcWin,tcontrol->color[0]);		//control text has control color as background
	#endif

				} else { //	if (tcontrol->flags&CItemListOpen) {  //FileMenu is open - draw inverse colored text
					//filemenu is not open- use regular colors for text
				//fprintf(stderr,"fg color[0] %d",tcontrol->color[0]);
	#if Linux
				XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);
	#endif
	#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

				SetBkMode(twin->hdcWin,TRANSPARENT); //do not allow text background color
				SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
				SetBkColor(twin->hdcWin,tcontrol->color[0]);		//control text has control color as background
	#endif
			}  //if (tcontrol->flags&CItemListOpen) {  //FileMenu is open - draw inverse colored text
		}  //if NoChange
		else  { 
	#if Linux
			XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);
	#endif
	#if WIN32
			//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]);
			//SelectObject(twin->hdcWin,tcontrol->pen);
			SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
			SetBkColor(twin->hdcWin,tcontrol->color[0]);		//control text has control color as background
	#endif
		} //NoColorChangeMouse


          //fprintf(stderr,"drawing text\n");
//          XSetForeground (PStatus.xdisplay, twin->xgc, 0x0);

//          cx=0;//PStatus.PadX;
        
//					fprintf(stderr,"filemenu text= %s\n",tcontrol->text);  
         ex3=0;
         ul=0;
         strcpy(tstr," ");  //pad string - could just add to cx
         strcat(tstr,tcontrol->text);
//         while(ex3<strlen(tcontrol->text))
//check for going over control width
  //       while(ex3<strlen(tstr)) {// && cx<bw)	{
				cx=tcontrol->x1;//+tcontrol->font->max_bounds.width;
				cy=tcontrol->y1;
          

       while(ex3<(int)strlen(tstr) && cx<tcontrol->x2) {// && cx<bw)	{
//           if (tcontrol->text[ex3]==0x26)  //& underline
           if (tstr[ex3]==0x26) { //& underline
             ul=1;
           } else {
//             XDrawImageString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,tcontrol->y1+tcontrol->font->max_bounds.ascent,tcontrol->text[ex3],1);
#if Linux
				XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy+fh,&tstr[ex3],1);
#endif
#if WIN32
				MultiByteToWideChar(CP_UTF8,0,(LPCSTR)&tstr[ex3],1,(LPWSTR)tcontrol->text_16,FTMedStr);
				TextOut(twin->hdcWin,cx,cy,(LPWSTR)tcontrol->text_16,1);
#endif

				if (ul==1) {
		           ul=0; 
#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow, twin->xgc,cx,tcontrol->y1+fh+1,cx+fw,cy+fh+1);
#endif
#if WIN32
					//tp[0].x=cx;	tp[0].y=cy+fh+1;	tp[1].x=cx+fw;	tp[1].y=cy+fh+1; //+fh because Win text origin is top -left, Xwin bottom-left
					tp[0].x=cx;	tp[0].y=cy+fh-PStatus.PadY+2;	tp[1].x=cx+fw;	tp[1].y=cy+fh-PStatus.PadY+2; //+fh because Win text origin is top -left, Xwin bottom-left
					Polyline(twin->hdcWin,tp,2);
#endif


		         } //ul==1
             cx+=fw;
           }	//tstr[ex3]==0x26
           ex3++;
         }  //end while ex3


       }  //!CDoNotShowText



   }  //end if button is a filemenu 

#if Linux
	  XFlush(PStatus.xdisplay);
#endif
#if WIN32
	  GdiFlush();
#endif

//call any onDraw functions for this control
  if (tcontrol->OnDraw!=0) {
		(*tcontrol->OnDraw) (twin,tcontrol);
		twin=GetFTWindow(winname); //check to see if window was deleted
		tcontrol=GetFTControl(conname); //check to see if control was deleted
		if (tcontrol==0 || twin==0) return; //user deleted control or window
	}


#if 0
	//In Windows if the itemlist is open redraw itemlist windows anytime a filemenu control is redrawn
	
	if (tcontrol->type==CTFileMenu) {
		if (tcontrol==PStatus.ExclusiveFTControl) {
			DrawFTItemList(tcontrol->ilist);

/*
			if (tcontrol->ilist!=0) {
				if (tcontrol->ilist->window!=0) {
					UpdateWindow(tcontrol->ilist->window->hwindow); 
				}  //hwindow!=0
			} //ilist!=0
*/
		}
	} //if (tcontrol->type==CTFileMenu) {
	
#endif //WIN32

//    XFlush(PStatus.xdisplay);
//    XSync(PStatus.xdisplay,1);



//}  //end if filemenu || dropdown shared FTItem part



    break;  //CTFileMenu, CTDropdown
    case CTData:  //Data FTControl

//perhaps we should determine x1 and x2 of data controls here because a resize is not resizing data controls

//			fprintf(stderr,"Draw Data control %s\n",tcontrol->name);
//Data controls hold digital data, and are displayed as tracks
//this may represent any line-drawn data such as audio, oscilloscope (can be set for 1 bit a track - possibly I should change this - or somehow make it person customized), etc.

//fprintf(stderr,"here 2\n");
//draw track data

//if track is not on screen with have CNotVisible

//if there is a file attached to this FTControl, draw the data
     if (strlen(tcontrol->filename)>0 && tcontrol->data!=0 && !(tcontrol->flags2&CDoNotDrawData)) {


				tTimeLine=tcontrol->TimeLineFTControl;
				if (tTimeLine==0) {
					fprintf(stderr,"Error: Data control %s has no connected TimeLine control.\n",tcontrol->name);
					//create a timeline control for all the data?
				}
	
				rx2=tTimeLine->TimeScale*(long double)tcontrol->SamplesPerSecond;

				//bytes/sample
				bps=(tcontrol->BitsPerSample>>3)*tcontrol->Channels;
				if (tcontrol->BitsPerSample<8) {
					bps=tcontrol->Channels; //we presume a 1-bit data minimum
				}
//				fprintf(stderr,"Loading increment 1=%lli\n",rx);


				//rx may = 0 when only one sample covers the entire control
//				fprintf(stderr,"Loading increment 2=%lli\n",rx);
				

//data needs to be read in from the data file in order to draw
//this can be done here in DrawFTControl or in other places
//I think here is the best place since we should not depend on other places to read in data
//so we need to see what the range of data that has been loaded is and if this is the same as the current data range in view

//reading in data from the file has a number of issues:
//do we want to read in only samples that are showing on screen?
//if no, the data read in may be very large - for example looking at 1 second of 1us 1-byte samples is 1e6 bytes
//if the scale is 10 ms, and the display width is 100 pixels, the only data being drawn is 1 byte every 100us
//we only read in the samples that are going to be drawn
//in a similar way, only the audio samples, or video frames showing (that are drawn) are loaded when viewing data

//rx=quantized StartTimeVisible - starttime of timeline
//could also use tTimeLine->StartTimeVisible? 
//				rx=((long double)((unsigned int)(tcontrol->StartTimeVisible/tTimeLine->TimeScale)))*tTimeLine->TimeScale;
				rx=((long double)((unsigned int)(tTimeLine->StartTimeVisible/tTimeLine->TimeScale)))*tTimeLine->TimeScale;



				if (!(tcontrol->flags2&CDoNotReadInData)) {
					//See if data needs to be read in

//What is drawn is based only on tcontrol->StartTimeVisible, tcontrol->TimeScale, and x2-x1 (clipped coordinates of the control)

					//determine duration of data showing in seconds - duration changes (oDuration is full data duration)
					//possibly the control is bigger than the data at TimeScale
					//should user calculate duration too?
					//determine end byte of data visible on screen in this control
					//see if this range is the current range loaded in
						//if data file not open yet, or control start or end time is different from loaded data start or end time, or timescale changed since last draw, or data on this track is being recorded
					if (!(tcontrol->flags2&CDataFileOpen) || !(tcontrol->flags2&CDataDrawnInitially) || tcontrol->StartTimeBuffer!=tcontrol->StartTimeVisible || tcontrol->EndTimeBuffer!=tcontrol->EndTimeVisible || tcontrol->TimeScaleBuffer!=tTimeLine->TimeScale) {
						//we need to load this range of data
//						fprintf(stderr,"Reload data\n");





						//if drawing for a first time, may need to adjust x2
						//get filesize and determine duration of data
						if (!(tcontrol->flags2&CDataDrawnInitially)) {
							//need file pointer from open, not fopen
//							if (tcontrol->flags2&CDataFileOpen) {
//								fclose(tcontrol->datafptr);
//								tcontrol->flags2&=~CDataFileOpen;
//							} //if (tcontrol->flags2&CDataFileOpen) {
#if Linux
								x2=open(tcontrol->filename,O_RDONLY);
								fstat(x2,&statbuf); 
								close(x2);
#endif
#if WIN32
								x2=_open(tcontrol->filename,O_RDONLY);
								_fstat(x2,&statbuf); 
								_close(x2);
#endif
				
							 //st_size=size of file in bytes
							//for raw data use full size, but for wav, other files need a different approach
							tcontrol->EndTime=tcontrol->StartTime+(long double)(statbuf.st_size)/(long double)(tcontrol->Channels*tcontrol->SamplesPerSecond*tcontrol->BytesPerSample);
//								fprintf(stderr,"File size=%d\n",(int)statbuf.st_size);
//								fprintf(stderr,"timeline endtimevisible=%Lf\n",tTimeLine->EndTimeVisible);
//								fprintf(stderr,"control endtime=%Lf\n",tcontrol->EndTime);
							if (tcontrol->EndTime>tTimeLine->EndTimeVisible) {
								tcontrol->EndTimeVisible=tTimeLine->EndTimeVisible;
								x2=tTimeLine->x2;
							} else {
								tcontrol->EndTimeVisible=tcontrol->EndTime;
//								x2=x1+(int)((tcontrol->EndTimeVisible-tcontrol->StartTimeVisible)/(tTimeLine->TimeScale*tTimeLine->TimeSpace));
								x2=x1+(int)((tcontrol->EndTimeVisible-tcontrol->StartTimeVisible)/(tTimeLine->TimeScale));
							} //if (tcontrol->EndTime>tTimeLine->EndTimeVisible) {
							
						}//	if (!(tcontrol->flags2&CDataDrawnInitially)) {


						//fprintf(stderr,"Loading range of data\n");
						if (!(tcontrol->flags2&CDataFileOpen)) {
	//						fprintf(stderr,"Opening data file\n");
							if (tcontrol->flags&CReadOnly) {
								tcontrol->datafptr=fopen(tcontrol->filename,"rb");
							} else {
								tcontrol->datafptr=fopen(tcontrol->filename,"r+b");
							} //(tcontrol->flags&CReadOnly) {
							if (tcontrol->datafptr==0) {
								fprintf(stderr,"Error: Could not open data file '%s'.\n",tcontrol->filename);
								return;
							} 
							tcontrol->flags2|=CDataFileOpen;
						} //CDataFileOpen



					 //mask to align StartByte
						//for example 2 channel 16-bit, mask will be 2^3 100...1111100 00 01 10 11 | 100
						//1 channel 16-bit mask=~1 (10)- all even numbers
						//1 channel 8 bit, mask=~0 (all 1s)
//						mask=~((long long)((pow(2.0,(double)(tcontrol->Channels*tcontrol->BytesPerSample-1)))-1));
						mask=~(unsigned long long)(tcontrol->Channels*tcontrol->BytesPerSample-1);
						//fprintf(stderr,"mask=%Lx\n",mask);
						//all channels use the same data so no need to read in data from file if tcontrol->Channel>1
						//but possibly channel 1 has been deleted
				
						//StartTimeVisible is the start of the visible time from external timeline control
						//StartTimeBuffer,EndTimeBuffer - basically track the StartTime and EndTime of this data control at last drawing
						//TimeScaleBuffer - stores timeline/data control timescale at last drawing
						//StartTimeData is the start of the data at the beginning of the control 
						//StartTimeVisibleData is the start of the data at the visible beginning of the control (relative to datafile start - basically StartByte in timeunits) -Perhaps StartTimeVisibleData
						//StartByte is the first byte to read in the datafile

						//we could also get StartTimeBuffer from (lx1-tcontrol->lx1)*tcontrol->TimeScale
						//possibly I should calculate StartTimeBuffer by (lx1-tcontrol->lx1)*tcontrol->TimeScale
						//round start time to nearest TimeScale unit

						
//						fprintf(stderr,"TimeLine Starts at %Lf track starts at %Lf s, data start is %Lf s\n",tTimeLine->StartTimeVisible,tcontrol->StartTime,tcontrol->StartTimeData);
//						fprintf(stderr,"Data Bytes/Sample=%d\n",bps);


//						tcontrol->StartTimeBuffer=rx+tcontrol->StartTimeData;

						if (tcontrol->StartTime<=tTimeLine->StartTimeVisible) {
							//control is to left of timeline start - so advance start of buffer and add in StartTimeData - where the data starts on the track - which is usually 0 - but changes if the track is cut or trimmed
							//tcontrol->StartTimeData=tTimeLine->StartTimeVisible-tcontrol->StartTime+tcontrol->StartTimeData;
							tcontrol->StartTimeVisibleData=tTimeLine->StartTimeVisible-tcontrol->StartTime+tcontrol->StartTimeData;
						} else {
								//or the track is to the right of the timeline start and since is onscreen must just be StartTimeData - we have already determined that this data is visible
							tcontrol->StartTimeVisibleData=tcontrol->StartTimeData;
						}
						//fprintf(stderr,"buffer st:%Lg\n",tcontrol->StartTimeBuffer);
						//tcontrol->StartTimeBuffer=(long double)(x1-tcontrol->lx1)*tcontrol->TimeScale;
						tcontrol->TimeScaleBuffer=tTimeLine->TimeScale;
						//determine is x2 is too long and resize
						//adjust tcontrol->DurationVisible and tcontrol->EndTimeVisible just in case some how inaccurate

						DurationVisible=((long double)(x2-x1))*tTimeLine->TimeScale;
						tcontrol->EndTimeVisible=tcontrol->StartTimeVisible+DurationVisible;
						//tcontrol->DurationBuffer=tcontrol->DurationVisible; //pixels*seconds/pixel
						//tcontrol->DurationBuffer=((long double)(x2-x1))*tcontrol->TimeScale; //pixels*seconds/pixel
//						tcontrol->EndTimeBuffer=tcontrol->StartTimeBuffer+DurationVisible;
//						fprintf(stderr,"Data DurationVisible=%Lg timescale=%Lg\n",tcontrol->DurationVisible,tcontrol->TimeScale);

						//save StartTimeVisible and EndTimeVisible at this drawing, to determine if we need to reload data at next drawing
						tcontrol->StartTimeBuffer=tcontrol->StartTimeVisible;
						tcontrol->EndTimeBuffer=tcontrol->EndTimeVisible;





//						if (tcontrol->Channel==0) {
//							fprintf(stderr,"Draw st %Lg et %Lg\n",tcontrol->StartTimeVisible,tcontrol->EndTimeVisible);
						//	fprintf(stderr,"Draw x1=%d x2=%d\n",tcontrol->x1,tcontrol->x2);
//						}


						//determine start byte of data visible on screen in this control

//						tcontrol->StartByte=(long long)(tcontrol->StartTimeBuffer*(long double)(tcontrol->nSamplesPerSec*tcontrol->nBytesPerSample*tcontrol->Channels));  //s * sample/s * bytes/sample * channels = bytes-channels
//StartTimeBuffer is basically StartByte but in time units
						tcontrol->StartByte=(long long)(tcontrol->StartTimeVisibleData*(long double)(tcontrol->SamplesPerSecond*bps));  //s * sample/s * bytes/sample * channels = bytes-channels
//StartByte needs to be SampleSize byte aligned
//shift bps-1 right and then left - there must be a better way using an & mask
//presumes channels will always be power of 2^ - 1,2,4,8,16, etc
//mask =0xffffffff&bps
	
						tcontrol->StartByte&=mask;  //align to channel and sample
						//tcontrol->StartByte>>=(bps-1);
						//tcontrol->StartByte<<=(bps-1);

//&bps  

						//StartByte needs to be relative to StartTimeData, not StartTimeBuffer - since StartTimeBuffer is in absolute time relative to the timeline -= so if data track starts at 10s, StartTimeBuffer is 10s - even though the data may be 0 there
//						tcontrol->StartByte=(long long)(tcontrol->StartTimeData*(long double)(tcontrol->SamplesPerSecond*bps));  //s * sample/s * bytes/sample * channels = bytes-channels
						//fprintf(stderr,"Startbyte:%lli\n",tcontrol->StartByte);

						//perhaps should store fposition										
						//fgetpos(tcontrol->datafptr,&fposition);
							//fprintf(stderr,"get position\n");
						//store the file pointer position (fptr) and return it later
						//because there may be data being recorded (written) to it while drawing
						//probably the best approach is reading data into RAM then drawing it to a track
						//and not reading to ram, writing to disk, and then reading from disk to draw to track
						fposition=ftell(tcontrol->datafptr);
//						fprintf(stderr,"position is %lli\n",fposition);
						if (fposition!=(int)tcontrol->StartByte) {
							if (tcontrol->StartByte-fposition<0) {
								tcontrol->StartByte=fposition;
							} //(tcontrol->StartByte-fposition<0) {
//							fprintf(stderr,"seeking %lli to StartByte %lli\n",tcontrol->StartByte-fposition,tcontrol->StartByte);
#if Linux
							fseeko(tcontrol->datafptr,(off_t)tcontrol->StartByte-fposition,SEEK_CUR);							
#endif
#if WIN32
							_fseeki64(tcontrol->datafptr,tcontrol->StartByte-fposition,SEEK_CUR);							
#endif
						}
						//fsetpos(tcontrol->datafptr,tcontrol->StartByte,
						//read in the quantity of data showing
						j=0;						
						//for loop uses units in seconds
//						fprintf(stderr,"TimeScale=%Lg rx2=%Lg\n",tcontrol->TimeScale,rx2);

						//if data byte interval >0 (rx2=0 when a single sample 
						//at small time scales no byte will be read over a number of pixels
						//lindx=rx2;//
						//nextsample needs to start at the fractional portion of the time line to advance the data correctly
						//for example nextsample=0.01 when the timeline starts at 10ms (as opposed to 0.00 when starting at 0ms) at 10ms timelines (scale*timespace)
						//nextsample=0; 
						nextsample=tcontrol->StartTimeBuffer/(tTimeLine->TimeScale*tTimeLine->TimeSpace*10.0);
			//again need to use StartTimeData (relative to datafile 0) - not StartTimeBuffer (relative to timeline 0)
//						nextsample=tcontrol->StartTimeData/(tTimeLine->TimeScale*tTimeLine->TimeSpace*10.0);
						//we just need the fractional portion	
						nextsample-=(long double)((int)nextsample);
						//fprintf(stderr,"nextsample=%Lg\n",nextsample);
						//fprintf(stderr,"for: %s %d\n",tcontrol->name,x2-x1);

						//This loop fills data[] with the correct data samples relative to the timeline time scale
						for(i=0;i<x2-x1;i++) {
//each pixel represents tcontrol->TimeScale units of time
//rx2 is the number of samples each pixel represents
//						for(fi=tcontrol->StartTimeVisible;fi<tcontrol->StartTimeVisible+tcontrol->DurationVisible;fi+=tcontrol->TimeScale) {
						
	//						fprintf(stderr,"%d=%lli ",i,((unsigned long long)rx2)tcontrol->BitsPerSample);
							//need to read another sample?

							//if (tcontrol->Channel==0) { fprintf(stderr,"(%Lg)",nextsample);}
							//probably there is a more simple way of doing this
							if (nextsample>=1.0 || j==0) { //need to read another sample or first sample
							//if (j==0 || j!=lastsample) { //first sample or need to read another sample
								//read moves the file pointer forward by a single sample (BitsPerSample)
								//fread(tcontrol->data[j],tcontrol->BitsPerSample,1,tcontrol->datafptr);
//need to cast tcontrol->data[j]?
								fread(&tcontrol->data[j],bps,1,tcontrol->datafptr);
	//							fprintf(stderr,"read:%d\t",tcontrol->data[j]);
								
								if (nextsample>=1.0) {  //if j==0 do not subtract 1 or else nextsample becomes negative
									nextsample-=1.0; //subtract a single sample
								} 
								//if (tcontrol->Channel==0) { fprintf(stderr," %d-%d ",j,tcontrol->data[j]);}

								if (nextsample>1.0) { //need to seek forward more than a single sample
									fseek(tcontrol->datafptr,(unsigned long long)((int)nextsample)*bps,SEEK_CUR); //move to the next drawn sample

									//if (tcontrol->Channel==0) { fprintf(stderr," seek=%d %d=%d ",((int)nextsample)*bps,j,tcontrol->data[j]);}
									nextsample-=(int)nextsample; //keep any remainder was: nextsample=0.0;
								} //nextsample>1.0
								
								//lastsample=j;
							} else { //just copy last data
								memcpy(&tcontrol->data[j],&tcontrol->data[j-bps],bps); //just use last read sample
								//if (tcontrol->Channel==0) { fprintf(stderr," %d=%d ",j,tcontrol->data[j]);}
							} //nextbyte>0

				

							//fprintf(stderr,"s:%lli ",(rx2&((unsigned long long)tcontrol->BitsPerSample)));
							j+=bps;
							nextsample+=rx2; //nextsample accumulates until at least 1 or more, rx2 could be 0.01
//							fprintf(stderr,"s:%Lg ",nextsample);
							//if (tcontrol->Channel==0) { fprintf(stderr,"%Lg ",nextsample);}

						} //for i
						tcontrol->BufferSize=j;
//						fprintf(stderr,"Done with reading in data\n");
						//probably buffersize should be only the actual size loaded in memory not the size in the file

						//put back file pointer
//						fposition2=ftell(tcontrol->datafptr);
//						fseek(tcontrol->datafptr,fposition-fposition2,SEEK_CUR);
#if Linux
						fseeko(tcontrol->datafptr,(off_t)fposition,SEEK_SET);  
#endif
#if WIN32
						_fseeki64(tcontrol->datafptr,fposition,SEEK_SET);  
#endif
						//indicate that we have drawn the data at least once
	
						//perhaps EndBuffer in time units is the better
					}  //StartBuffer!=

				tcontrol->flags2|=CDataDrawnInitially;

				} //CDoNotReadInData

//END LOADING IN DATA FROM DATA FILE

//at this point DrawFTControl expects the necessary range of data to be loaded into tcontrol->data

//start and end time of data is determined by the width of the control (x2-x1) and timescale of control (ns/pixel) - perhaps timescale should be a double and seconds/pixel 0.000000001 being 1ns/pixel
//tcontrol->data is read in, in other places
       	//fprintf(stderr,"tcontrol->filename=%s,tcontrol->data=%p\n",tcontrol->filename,tcontrol->data);


	//depends if signed or unsigned
//				if (tcontrol->flags&CDataSigned)	{
				if (tcontrol->Signed)	{
					datasigned=1;					
//						fprintf(stderr,"data is signed\n");   //put ypos at missle
					ypos=tcontrol->y1+(tcontrol->y2-tcontrol->y1)/2;
	       } else {
						datasigned=0; //put ypos at bottom of control
						ypos=tcontrol->y1+(tcontrol->y2-tcontrol->y1)*9/10;
					}
	
	  //fprintf(stderr,"FTControl filename  %s with ypos %d\n",tcontrol->filename,ypos);
	  //scale of y for data
//  sy=(float)((y2-y1)*8/10)/pow(2.0,1-1*(!(tcontrol->flags&CDataSigned)));
//use unclipped points - drawing below will clip
//  sy=(float)((tcontrol->y2-tcontrol->y1)*8/10)/pow(2.0,1-1*(!(tcontrol->flags&CDataSigned)));
//  sy=(float)((tcontrol->y2-tcontrol->y1)*8/10)/pow(2.0,tcontrol->BitsPerSample-1*((tcontrol->flags&CDataSigned)>0));
//divide track height by 2^8=256 but 128 if signed, 
  sy=(float)((tcontrol->y2-tcontrol->y1)*8/10)/pow(2.0,(double)(tcontrol->BitsPerSample-1*(tcontrol->Signed>0)));

//	fprintf(stderr,"scaleY=%f ypos=%d\n",sy,ypos);
	//for bps=1 bpc=1

	//fprintf(stderr,"2^%d=%llx\n",tcontrol->Channel,b);



	//first calculate increment Samples/pixel
//	rx2=(long double)(tcontrol2->time*(long long)tcontrol->nSamplesPerSec*(long long)tcontrol->BitsPerSample*(long long)tcontrol->Channels)/((long double)PStatus.timespace*8000000000.0);
//Type 	flags 	Possible Values
//char 	8 	-127 to 127
//unsigned char 	8 	0 to 255
//short 	16 	-32,767 to 32,767
//unsigned short 	16 	0 to 65,535
//int 	32 	-2,147,483,647 to 2,147,483,647
//unsigned int 	32 	0 to 4,294,967,295
//long 	32 	-2,147,483,647 to 2,147,483,647
//unsigned long 	32 	0 to 4,294,967,295
//long long 	64 	-9,223,372,036,854,775,807 to 9,223,372,036,854,775,807
//unsigned long long 	64 	0 to 18,446,744,073,709,551,615 

//float 	32 	1e-38 to 1e+38
//double 	64 	2e-308 to 1e+308
//long double 	64 	2e-308 to 1e+308 

//so float holds much larger numbers than integers by providing less accuracy
//so convert all numbers to double because high timescale makes equation go over limit
//rx2 is how many bytes to advance for each pixel
//this is timescale (number of ns in 1 pixel) * samples per second * bits per sample * channels
//all divided by 1e9 to get bytes/pixel
//ns-sample-bits-channels/pixel-8gs-sample = bytes/pixel
//	rx2=((long double)tcontrol->timescale*(long double)tcontrol->nSamplesPerSec*(long double)tcontrol->BitsPerSample*(long double)tcontrol->Channels)/((long double)PStatus.timespace*8000000000.0);
//	rx2=((long double)tcontrol->timescale*(long double)tcontrol->nSamplesPerSec*(long double)tcontrol->BitsPerSample*(long double)tcontrol->Channels)/8.0;

//fprintf(stderr,"ts=%lli\n",PStatus.timescale);

	//now initial offset samples/pixel - can go by time
	//dc=starting sample
//	dc=((long double)((StartTimeVisible-tcontrol->StartTimeVisible+tcontrol->StartTime)*(long long)tcontrol->nSamplesPerSec*(long long)tcontrol->BitsPerSample*(long long)tcontrol->Channels))/8000000000.0;
//before StartTimeVisible was clipped, now this is done in human func
//StartTimeVisible is where in the data the track FTControl starts (the first sample at the start of the track)
//StartTime is original StartTimeVisible, where the data 
//StartTimeVisible is initially 0ns, then a FTControl is moved or split
//perhaps StartTimeVisible should be VisStartTime, or CurStartTime
//the StartTimeVisible is only related to the timeline FTControl (a nonfreethought FTControl)
//the only thing this part needs is an index to the start of the data
//this can be StartTimeVisible-StartTime, but perhaps should be done in outside functoin and called
//sindex or something
//StartTimeVisible-StartTime will be independent of timeline, FTControl is at 10ns, but is resize 10ns on left
//StartTime=10ns but StartTimeVisible will be 20ns, 20-10 = start 10ns in 

//dc is the initial byte of data to draw
//ia[0] holds the start time of the data - why not use StartTimeVisible?
//	dc=((long double)(tcontrol->ia[0]*(long long)tcontrol->nSamplesPerSec*(long long)tcontrol->BitsPerSample*(long long)tcontrol->Channels))/8000000000.0;
//	dc=0; //in the current system, the first byte of data loaded in memory is the start byte

//fprintf(stderr,"st=%lli ost=%lli nsamp=%d nbit=%d nchan=%d\n",tcontrol->StartTimeVisible,tcontrol->StartTime,tcontrol->nSamplesPerSec,tcontrol->BitsPerSample,tcontrol->Channels);
//fprintf(stderr,"dc=%LF\n",dc);
//fprintf(stderr,"rx2=%LF\n",rx2);
		//bps==1 only
//lindex is last data index - used to determine where to draw the line from
//	lindex=((long long)dc&(~0x1));

  //  fprintf(stderr,"aaa dc=%LF rx2=%LF\n",dc,rx2);

	//at ns scale tinc=.7  1ns/7pixels
	//fprintf(stderr,"timescale=%llins each pixel=rx2=%LF samples\n",tcontrol2->time,rx2);
 //       fprintf(stderr,"st=%lli et=%lli tinc=%lli\n",StartTimeVisible,EndTimeVisible,tinc);
//        fprintf(stderr,"dc=%LF\n",dc);
//        fprintf(stderr,"rx2=%LF\n",rx2);
        //fprintf(stderr,"channel=%d\n",tcontrol->Channel);
	//fprintf(stderr,"mask=%lli\n",mask);


		b=(long long)pow(2.0,(double)(tcontrol->Channel-1));

//if data FTControl is simply moving, should not have to look at data again
//simply redraw the image
//only if clipped (then draw only part of the image)
//or split (then have to recalc for two new FTControls)
//or resize (have to calc again)
//or scale (have to calc again)

//Human functions have to determine the x1 to x2 and y1 to y2 of any data FTControl
//DrawFTControls only draws the FTControl presuming the x1,y1 x2,y2 are correct
//User also determines StartTimeVisible and StartTime for control
//possibly DrawFTControls could adjust size of control based on size of data, and TimeScale
//and presume that the control will be clipped as part of a frame - but then need to clip before loading data
//START DRAW DATA FTControl
//	fprintf(stderr,"x1=%i\tx2=%i\n",tcontrol->x1,tcontrol->x2);
//	for(a=startx;a<endx;a++)

//remember that the data is already scaled, and so we only need to draw data[0], data[1], etc (for 8-bit data), data[0], data[2] for 16 bit, etc. 

	tsample[1]=0; //added to avoid warning - verify	
	tsample[0]=tsample[1];
//	fprintf(stderr,"from %d to %d\n",x1,x2);

		//rx2 is used to increment the time to track selected time
	//rx needs to be set here in case data was not read in, 
	//rx=((long double)((unsigned int)(tcontrol->StartTimeVisible/tTimeLine->TimeScale)))*tTimeLine->TimeScale;
	rx2=tTimeLine->TimeScale;//*(long double)tcontrol->nSamplesPerSec;
	StartSelected=tcontrol->StartTimeSelected;
	EndSelected=tcontrol->EndTimeSelected;
	//fprintf(stderr,"ss=%Lg es=%Lg sts=%Lg rx=%Lg\n",StartSelected,EndSelected,tcontrol->StartTimeSelected,rx);

	lindex=0;
	//mask=~(long long)(pow(2.0,(long double)tcontrol->Channels-1));
	j=0;
	for(i=x1;i<x2;i++) {
   // fprintf(stderr,"aa dc=%LF rx2=%LF\n",dc,rx2);
//	fprintf(stderr,"i=%i\t",i);
  //fprintf(stderr,"%7.2f d=%d\t",rx2,fdata[(int)(dc)]&0x1);

	//if this part of time line shows data
	//if there is any data in this part of timeline   //filesize=num of bytes

//		
		//if (tcontrol->BitsPerSample==1 && tcontrol->BitsPerChannel==1)
		if (tcontrol->BitsPerSample==1 && tcontrol->BitsPerChannel==1)
			{
			//draw background white line
//		  XSetForeground (PStatus.xdisplay, twin->xgc, 0xffffff);  //background color
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[3]);  //selected background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[3]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[3]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[3];
#endif
			} else {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[1]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[1]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[1];
#endif
			}

			//XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,b,tcontrol->y1,b,tcontrol->y2-2);
#if Linux
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1,i,y2);
#endif
#if WIN32
			tp[0].x=i;	tp[0].y=y1;	tp[1].x=i;	tp[1].y=y2;
			Polyline(twin->hdcWin,tp,2);
#endif


		        //need green outline on tracks
//			XSetForeground(PStatus.xdisplay, twin->xgc, green);
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[2]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->color[2];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[0]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->color[0];
#endif
			}

			if ((y1+1>tcontrol->y1) && (y1+1<tcontrol->y2)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+1);
#endif
#if WIN32
			SetPixel(twin->hdcWin,i,y1+1,curcolor);
#endif
			}
			if ((y2-1<tcontrol->y2) && (y2-1>tcontrol->y1)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y2-1);  
#endif
#if WIN32
				SetPixel(twin->hdcWin,i,y2-1,curcolor);
#endif
			}


			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[2];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  //text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[0];
#endif
			}

			//draw vertical line
		  //fprintf(stderr,"%f\t",ypos-((tcontrol->data[(long long)dc]&b)!=0)*sy);
			//fprintf(stderr,"%x\t",tcontrol->data[(long long)(dc-rx2)]);

//			j+=tcontrol->BitsPerSample;
			j+=bps;

			//dindex=((long long)dc&(~0x1));
			//dindex=((long long)dc&(~0x1));
			//dindex=((long long)dc&(~0x1));
			//fprintf(stderr,"st=%lli sps=%lli\n",(a-tcontrol->StartTimeVisible+tcontrol->StartTime),(long long)(tcontrol->nSamplesPerSec*tcontrol->BitsPerSample));

		  //fprintf(stderr,"dindex=%lli\n",dindex);
			//fprintf(stderr,"b=%lli data=%lli\n",b,tcontrol->filesize);

			//if (dindex<tcontrol->filesize) {
//			if (dc<tcontrol->BufferSize) {
//			if (j<tcontrol->BufferSize) {
//				tsample[1]=tcontrol->data[j];
				//draw vertical line of data (may be just top point of a square wave)
				//clip
				cy=(int)(ypos-((tcontrol->data[lindex]&b)!=0)*sy);
				cy2=(int)(ypos-((tcontrol->data[j]&b)!=0)*sy);
				if ((cy>y1 || cy2>y1) && (cy<y2 || cy2<y2)) { 
					//some portion of the data line is showing 
						if (cy<y1) { //clip vertical lines
							cy=y1+1;
						}
						if (cy2<y1) { //clip vertical lines
							cy2=y1+1;
						}
						if (cy2>y2) { //clip vertical lines
							cy2=y2-1;
						}
						if (cy>y2) { //clip vertical lines
							cy=y2-1;
						}
#if Linux
						XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,cy,i,cy2);		
#endif
#if WIN32
						tp[0].x=i;	tp[0].y=cy;	tp[1].x=i;	tp[1].y=cy2;
						Polyline(twin->hdcWin,tp,2);
#endif
					} //cy>tcontrol->y1

//			} //j<tcontrol->BufferSize

//			lindex=dindex;
//			lindex=dc;
			lindex=j;
			//draw horiz line
			} //tcontrol->BitsPerSample==1


		//  fprintf(stderr,"a dc=%LF rx2=%LF\n",dc,rx2);
		if (tcontrol->BitsPerSample==8) {// && tcontrol->BitsPerChannel==8)
			j+=bps; //bps is bytes/sample*channels
			if (j<(int)tcontrol->BufferSize) {
				if (datasigned) {
					tsample[1]=(long long)((char)tcontrol->data[j]*sy);
				} else {
					tsample[1]=(long long)(tcontrol->data[j]*sy);
				} //datasigned
			}

			//only need vertical line
//		  XSetForeground (PStatus.xdisplay, twin->xgc, 0xffffff);  //background color
//		  XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //background color
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[3]);  //selected background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[3]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[3]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[3];
#endif

			} else {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[1]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[1]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

				curcolor=tcontrol->textcolor[1];
#endif
			}

#if Linux
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1,i,y2-2);
#endif
#if WIN32
			tp[0].x=i;	tp[0].y=y1;	tp[1].x=i;	tp[1].y=y2-2;
			Polyline(twin->hdcWin,tp,2);
#endif

			//XSetForeground (PStatus.xdisplay, twin->xgc, green);  //draw border of track
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[2]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

				curcolor=tcontrol->color[2];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[0]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->color[0];
#endif
			}
			if ((y1+1>tcontrol->y1) && (y1+1<tcontrol->y2)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+1);
#endif
#if WIN32
				SetPixel(twin->hdcWin,i,y1+1,curcolor);
#endif
			}
			if ((y2-1<tcontrol->y2) && (y2-1>tcontrol->y1)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y2-1);  
#endif
#if WIN32
				SetPixel(twin->hdcWin,i,y2-1,curcolor);
#endif
			}

			//if signed draw gray line in middle
//			if (tcontrol->flags&CDataSigned) {
			if (tcontrol->Signed) {
//				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color - draw actual sample
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,wingray);  //selected text color - draw actual sample
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+((y2-y1)/2));
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,wingray); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,wingray);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

				SetPixel(twin->hdcWin,i,y1+((y2-y1)/2),wingray);
#endif
			}


			//XSetForeground (PStatus.xdisplay, twin->xgc,0x0);  //text color - draw actual sample
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[2];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  //text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[0];
#endif
			}

//			if (i==x1) { //first sample
//				tsample[0]=tsample[1];
//			}

			cy=	ypos-(int)tsample[0];
			cy2=ypos-(int)tsample[1];

//			fprintf(stderr," %d-%lli",tcontrol->data[j],tsample[1]);

			if ((cy>y1 || cy2>y1) && (cy<y2 || cy2<y2)) { 
				//some portion of the data line is showing 
					if (cy<y1) { //clip vertical lines
						cy=y1+1;
					}
					if (cy2<y1) { //clip vertical lines
						cy2=y1+1;
					}
					if (cy2>y2) { //clip vertical lines
						cy2=y2-1;
					}
					if (cy>y2) { //clip vertical lines
						cy=y2-1;
					}
//					fprintf(stderr,"Line: %d,%d to %d,%d ypos=%d y1=%d y2=%d\n",i,cy,i,cy2,ypos,y1,y2);
#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,cy,i,cy2);		
#endif
#if WIN32
					tp[0].x=i;	tp[0].y=cy;	tp[1].x=i;	tp[1].y=cy2;
					Polyline(twin->hdcWin,tp,2);
#endif
				} //cy>tcontrol->y1

//			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,a,ypos-tsample[0]*sy,a,ypos-tsample[1]*sy);
			tsample[0]=tsample[1];
//			lindex=j;
			}  // //tcontrol->BitsPerSample==8



		if (tcontrol->BitsPerSample==16)
			{
	//    fprintf(stderr,"b size=%lli\n",tcontrol->filesize);
			//fprintf(stderr,"dindex=%lli\n",dindex);
			//this must be aligned on 2 bytes for 16 bit
			///and aligned on number of channels*Bps

		//  fprintf(stderr,"b dc=%LF rx2=%LF\n",dc,rx2);				
				j+=bps; //advance to next sample bps=bytespersample*channels
	//		j+=bps*tcontrol->Channel; //advance to next sample
				//probably only one channel can be shown on 1 track anyway - probably remove COnlyShowOneChannel
//				if (tcontrol->flags2&COnlyShowOneChannel) {
					//dc+=tcontrol->Channel;
//					j+=(tcontrol->Channel-1);
//				}

	//    fprintf(stderr,"dindex=%lli tcontrol->Channel=%d\n",dindex,tcontrol->Channel);
	//    fprintf(stderr,"c dc=%LF rx2=%LF\n",dc,rx2);
		  if (j<(int)tcontrol->BufferSize)	{
				if (datasigned) {
			    //tsample[1]=(tcontrol->data[j+1]<<0x8)|(unsigned char)tcontrol->data[j];
					tsample[1]=*(short *)(tcontrol->data[j]);
				} else {
			    //tsample[1]=((unsigned char)(tcontrol->data[j+1]<<0x8))|(unsigned char)tcontrol->data[j];
					tsample[1]=*(unsigned short *)(tcontrol->data[j]);
				} //datasigned
			}// else {//added below to avoid a warning so verify

	 
//		  if (dc<tcontrol->BufferSize)	{
//		    tsample[1]=(tcontrol->data[dc+1]<<0x8)|(unsigned char)tcontrol->data[dc];
//			}// else {//added below to avoid a warning so verify
			//	tsample[1]=(tcontrol->data[tcontrol->filesize-1]<<0x8)|(unsigned char)tcontrol->data[tcontrol->filesize-2];  //end of file data?
			//}
			//fprintf(stderr,"dindex=%lli\n",(long long)dindex);
			//fprintf(stderr,"dci=%lli\n",(long long)dc);
			 //fprintf(stderr,"sample= %llx\t",tsample[1]);
			//fprintf(stderr,"sample= %llx\n",tsample[1]);
			//fprintf(stderr,"sample before scaling= %llx\n",tsample[1]);
	//    fprintf(stderr,"c\n");

			tsample[1]=ceil(((float)tsample[1]*sy));
	//	fprintf(stderr,"%lli\t",tsample[1]);
		//fprintf(stderr,"%llx\t",tsample[1]);
	//        fprintf(stderr,"sampy=%llx\n",ypos-tsample[1]);

			//if (a==startx)
		  //if (a==tcontrol->x1)
			//{
			//tsample[0]=tsample[1];
			//} 

	//    fprintf(stderr,"d\n");
			//only need vertical line
		  //XSetForeground (PStatus.xdisplay, twin->xgc, 0xffffff);  //background color
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[3]);  //selected background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[3]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[3]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[3];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[1]);  //background color
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[1]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[1]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[1];
#endif
			}
#if Linux
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1,i,y2-2);
#endif
#if WIN32
			tp[0].x=i;	tp[0].y=y1;	tp[1].x=i;	tp[1].y=y2-2;
			Polyline(twin->hdcWin,tp,2);
#endif

//			XSetForeground (PStatus.xdisplay, twin->xgc, green);  //draw border of track
			if (rx>StartSelected && rx<EndSelected) { //			if (tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[2]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->color[2];
#endif
			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[0]);  //draw border of track
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->color[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->color[0];
#endif
			}
//			XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,a,y1);
//			XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,a,y2-2); //1
			if ((y1+1>tcontrol->y1) && (y1+1<tcontrol->y2)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+1);
#endif
#if WIN32
				SetPixel(twin->hdcWin,i,y1+1,curcolor);
#endif
			}
			if ((y2-1<tcontrol->y2) && (y2-1>tcontrol->y1)) {
#if Linux
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y2-1);  
#endif
#if WIN32
				SetPixel(twin->hdcWin,i,y2-1,curcolor);
#endif
			}

			//if signed draw gray line in middle
//			if (tcontrol->flags&CDataSigned) {
			if (tcontrol->Signed) {
//				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color 
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,wingray);  //selected text color
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+((y2-y1)/2));
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,wingray); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,wingray);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				SetPixel(twin->hdcWin,i,y1+((y2-y1)/2),wingray);
#endif
			}


			//if (tcontrol==twin->FocusFTControl) {
			if (rx>StartSelected && rx<EndSelected) { //tcontrol==twin->FocusFTControl) {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[2]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[2]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[2];
#endif
			} else {
#if Linux
				XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]);  //text color - draw actual sample
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				curcolor=tcontrol->textcolor[0];
#endif
			}

//			if (i==x1) { //first sample
//				fprintf(stderr,"first sample %lli\n",tsample[1]);
//				tsample[0]=tsample[1];
//			}

//				cy=	ypos-((tcontrol->data[lindex]&b)!=0)*sy;
//				cy2=ypos-((tcontrol->data[j]&b)!=0)*sy;
//				cy=	ypos-(int)tcontrol->data[lindex]*sy;
//				cy2=ypos-(int)tcontrol->data[j]*sy;
				cy=	ypos-(int)tsample[1];
				cy2=ypos-(int)tsample[0];

				if ((cy>y1 || cy2>y1) && (cy<y2 || cy2<y2)) { 
				//some portion of the data line is showing 
					if (cy<y1) { //clip vertical lines
						cy=y1+1;
					}
					if (cy2<y1) { //clip vertical lines
						cy2=y1+1;
					}
					if (cy2>y2) { //clip vertical lines
						cy2=y2-1;
					}
					if (cy>y2) { //clip vertical lines
						cy=y2-1;
					}
#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,cy,i,cy2);		
#endif
#if WIN32
					tp[0].x=i;	tp[0].y=cy;	tp[1].x=i;	tp[1].y=cy2;
					Polyline(twin->hdcWin,tp,2);
#endif
				} //cy>tcontrol->y1

//			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,a,ypos-tsample[0],a,ypos-tsample[1]); //*sy?
			tsample[0]=tsample[1];
			lindex=j;
			}  // //tcontrol->BitsPerSample==16




	//    fprintf(stderr,"dc=%LF rx2=%LF\n",dc,rx2);
//		  dc+=rx2;  //for real need 64 bit floating point (long double)


	//    fprintf(stderr,"dc=%LF rx2=%LF\n",dc,rx2);
			rx+=rx2; //increase time - for comparison with selected part of data

		}  //end for i (each column of track)
        //free(tsample);


  }   //end if strlen filename>0  data/wave/video file attached to FTControl
//end is track data FTControl
//=====================================

				//Draw any label controls associated with the data control
				if (tcontrol->DataLabelFTControl!=0) {
					//redraw any label control for data control	- labels are usually CTTextBox
					//put this in draw CTData?
					DrawFTControl(tcontrol->DataLabelFTControl);
				} //(tcontrol->DataLabelFTControl!=0) {



//#if 0
#if Linux
	  XFlush(PStatus.xdisplay);
#endif
#if WIN32
	GdiFlush();
#endif

      if (tcontrol->OnDraw!=0) {
				(*tcontrol->OnDraw) (twin,tcontrol);
				twin=GetFTWindow(winname); //check to see if window was deleted
				tcontrol=GetFTControl(conname); //check to see if control was deleted
				if (tcontrol==0 || twin==0) return; //user deleted control or window
			}
//#endif

//#endif
//			fprintf(stderr,"Done drawing Data control\n");

    break;  //CTData
		case CTFrame:

				//To draw a frame - we need to erase all controls on the frame and draw them again
				//it is a duplication if we are drawing a window because the individual controls will be drawn again, but when drawing an individual frame for an update, as is usually the case, the controls on the frame are not redrawn

			//if this frame has a TimeLine control connected with it, draw that
			if (tcontrol->TimeLineFTControl!=0) {
				DrawFTControl(tcontrol->TimeLineFTControl);
			} //	if (tcontrol->TimeLineFTControl!=0) 



				//draw rectangle frame to erase all controls showing on frame
				//get background again for controls on frame
#if Linux
        XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->color[0]);
        XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw,bh);
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x1+bw;
		tcontrol->rect.bottom=tcontrol->y1+bh;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));
#endif
				
/*
				//first go through and erase all controls in the frame
				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					if (tcontrol2->FrameFTControl==tcontrol) {
						EraseFTControl(tcontrol2);
					} //	
					tcontrol2=tcontrol2->next;
				} //tcontrol2!=0
*/
				//then go thru and draw all controls in the frame
				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					if (tcontrol2->FrameFTControl==tcontrol) {  //a control is on this frame 
						//GetFTControlBackground(tcontrol2); //causes crash on getimage

//perhaps we should determine x1 and x2 of data controls here because a resize is not resizing data controls

						DrawFTControl(tcontrol2);
#if 0 
						if (tcontrol2->type==CTData && tcontrol2->DataLabelFTControl!=0) {
							//redraw any label control for data control	- labels are usually CTTextBox
							//put this in draw CTData?
							DrawFTControl(tcontrol2->DataLabelFTControl);
						} //CTData
#endif
					} //	
					tcontrol2=tcontrol2->next;
				} //tcontrol2!=0


				//Frame text may be above at top left or in other locations
				//for now only draw any scroll bars

	      //update scrollbars
		    if (tcontrol->flags&CVScroll) {
		      //Draw the scroll FTControls
	//        fprintf(stderr,"1\n");
				/*
		      sprintf(tstr,"%s%s",PStatus.PrefixVScrollUp,tcontrol->name);
		      tcontrol2=GetFTControl(tstr);
		      DrawFTControl(tcontrol2);
		      sprintf(tstr,"%s%s",PStatus.PrefixVScrollDown,tcontrol->name);
		      tcontrol2=GetFTControl(tstr);
		      DrawFTControl(tcontrol2);
		      sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
		      tcontrol2=GetFTControl(tstr);
		      DrawFTControl(tcontrol2);
		      sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
		      tcontrol2=GetFTControl(tstr);
	          CalcVScrollSlide(twin,tcontrol);  //determine size, and position
		      DrawFTControl(tcontrol2);
			  */

				DrawFTControl(tcontrol->FTC_VScrollUp);
				DrawFTControl(tcontrol->FTC_VScrollDown);
				DrawFTControl(tcontrol->FTC_VScrollBack);
				//todo: it's better to calculate the slide button size when ever it changes, not adjusting it every time the control is drawn
				//CalcVScrollSlide(twin,tcontrol);  //determine size, and position
				DrawFTControl(tcontrol->FTC_VScrollSlide);

	//        fprintf(stderr,"4\n");
		    }  //CVScroll
		    if (tcontrol->flags&CHScroll) {
				/*
				sprintf(tstr,"%s%s",PStatus.PrefixHScrollLeft,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				DrawFTControl(tcontrol2);
				sprintf(tstr,"%s%s",PStatus.PrefixHScrollRight,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				DrawFTControl(tcontrol2);
				sprintf(tstr,"%s%s",PStatus.PrefixHScrollBack,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				DrawFTControl(tcontrol2);
				sprintf(tstr,"%s%s",PStatus.PrefixHScrollSlide,tcontrol->name);
				tcontrol2=GetFTControl(tstr);
				CalcHScrollSlide(twin,tcontrol);  //determine size, and position
				DrawFTControl(tcontrol2);
				*/
				DrawFTControl(tcontrol->FTC_HScrollLeft);
				DrawFTControl(tcontrol->FTC_HScrollRight);
				DrawFTControl(tcontrol->FTC_HScrollBack);
				//CalcHScrollSlide(twin,tcontrol);  //determine size, and position
				DrawFTControl(tcontrol->FTC_HScrollSlide);
		    }

#if Linux
		  XFlush(PStatus.xdisplay);
#endif
#if WIN32
			GdiFlush();
#endif


			//call any onDraw functions for this control
				if (tcontrol->OnDraw!=0) {
					(*tcontrol->OnDraw) (twin,tcontrol);
					twin=GetFTWindow(winname); //check to see if window was deleted
					tcontrol=GetFTControl(conname); //check to see if control was deleted
					if (tcontrol==0 || twin==0) return; //user deleted control or window
				}


		break; //CTFrame
		case CTFolderTabGroup:

//note CTFolderTab controls are drawn here - and are not individually drawable - like scrollbar, and other subcontrols which are attached to some parent control

				//To draw a FolderTabGroup - we need to erase all controls on the FoldeTabControl and draw them again

			//if this FolderTabGroup has a TimeLine control connected with it, draw that
			if (tcontrol->TimeLineFTControl!=0) {
				DrawFTControl(tcontrol->TimeLineFTControl);
			} //	if (tcontrol->TimeLineFTControl!=0) 

			//correct any y3<=0
				//if (tcontrol->y3<=0) {
				//	tcontrol->y3=tcontrol->y1+FT_DEFAULT_FOLDERTAB_HEIGHT;  //if y3 is not defined just set 32 pixels below
				//}


				//draw rectangle frame to erase all controls showing on FolderTabGroup
				//get background again for controls on FolderTabGroup
#if Linux
        XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlBorderColor);
//        XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw,bh);
		//draw rectangle border
		XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
		XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
		XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
		XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
		//XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,tcontrol->y3);
		
		//draw top shaded background of FolderTabGroup
        XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlShadedBackgroundColor); 
        XDrawRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,bw-1,tcontrol->y3-tcontrol->y1);

		//draw background of FolderTabGroup
        XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->ControlBackgroundColor); 
        //XDrawRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y3+1,bw-1,bh-tcontrol->y3-1);
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y3+1,bw-1,tcontrol->y2-tcontrol->y3-1); //bottom of FolderTabGroup control
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		//draw border
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBorderColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		//tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
		tp[0].x=x1;	tp[0].y=tcontrol->y3;	tp[1].x=x2-1;	tp[1].y=tcontrol->y3;
		tp[2].x=x2-1;	tp[2].y=y2-1;	tp[3].x=x1;	tp[3].y=y2-1;
		tp[4].x=x1;	tp[4].y=y1;
		Polyline(twin->hdcWin,tp,5);
		//tp[1].y=tcontrol->y3; //and a line under the top FolderTab buttons
		//Polyline(twin->hdcWin,tp,1);

		//draw shaded top part, currently using border color
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlShadedBackgroundColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1+1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x1+bw-1;
		//tcontrol->rect.bottom=tcontrol->y1+bh-tcontrol->y3;
		tcontrol->rect.bottom=tcontrol->y3;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));


		//draw background
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[1]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->ControlBackgroundColor);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1+1;
		tcontrol->rect.top=tcontrol->y3+1;
		tcontrol->rect.right=tcontrol->x1+bw-2;
		//tcontrol->rect.bottom=tcontrol->y1+bh-tcontrol->y3;
		tcontrol->rect.bottom=y2-1; //bottom of FolderTabGroup control
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));


#endif

				//But visibly connect FolderTab with FolderTabGroup under selected tab
				tcontrol2=tcontrol->CurrentFolderTab;
				if (tcontrol2!=0) {
#if Linux
					XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->color[0]); //color[0] is foreground color
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol2->x1,tcontrol->y3,tcontrol2->x2,tcontrol->y3);		
#endif
#if WIN32
					//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->color[0]); 
					//SelectObject(twin->hdcWin,tcontrol->pen);
					SetDCPenColor(twin->hdcWin,tcontrol->color[0]);
					SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
					tp[0].x=tcontrol2->x1;	tp[0].y=tcontrol->y3;	tp[1].x=tcontrol2->x2;	tp[1].y=tcontrol->y3;
					Polyline(twin->hdcWin,tp,2);
#endif
				}
			


/*
				//first go through and erase all controls in the FolderTabGroup
				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					if (tcontrol2->FrameFTControl==tcontrol) {
						EraseFTControl(tcontrol2);
					} //	
					tcontrol2=tcontrol2->next;
				} //tcontrol2!=0
*/
			//Go through all controls
			//Draw all FolderTab controls that belong to this FolderTabGroup
			//and Draw all controls that belong to the current FolderTab

				tcontrol2=twin->icontrol;
				while(tcontrol2!=0) {
					//draw all FolderTabs that belong to this FolderTabGroup
					if (tcontrol2->type==CTFolderTab && tcontrol2->FolderTabGroup==tcontrol) {
						//DrawButtonFTControls(tcontrol2); //Draw the actual FolderTab button/tab
						//CTFolderTab controls are not drawn in DrawFTControls so we need to set their clip x1,y1,x2,y2 here
						tcontrol2->cx1=tcontrol2->x1;
						tcontrol2->cy1=tcontrol2->y1;
						tcontrol2->cx2=tcontrol2->x2;
						tcontrol2->cy2=tcontrol2->y2;
						
						DrawButtonFTControls(tcontrol2); //Draw the actual FolderTab button/tab
					} else {
						//draw all controls that are on the currently showing FolderTab
						//we could also just draw all controls that belong to a foldertab that do not have CNotShowing selected- but that would draw other FolderTabGroup controls so this might be faster
						if (tcontrol2->FolderTab!=0 && tcontrol2->FolderTab==tcontrol->CurrentFolderTab && tcontrol2!=tcontrol) {
							//GetFTControlBackground(tcontrol2); //causes crash on getimage
							DrawFTControl(tcontrol2);  //could be a (sub) FolderTabGroup and so is recursive
							if (tcontrol2->type==CTData && tcontrol2->DataLabelFTControl!=0) {
								//redraw any label control for data control	
								DrawFTControl(tcontrol2->DataLabelFTControl);
							} //CTData
						} //if (tcontrol2->FolderTab==tcontrol->FolderTab) {
					} //if (tcontrol2->FolderTab!=0 && tcontrol2->FolderTab==tcontrol->FolderTab && tcontrol2!=tcontrol) {
					//} //if (tcontrol2->type!=CTFolderTabGroup && tcontrol2->FolderTab==tcontrol->FolderTab)  {	
					tcontrol2=tcontrol2->next;
				} //tcontrol2!=0


				//Like a CTFrame CTFolderTabGroup text may be above at top left or in other locations
				//for now only draw any scroll bars

	      //update scrollbars
		    if (tcontrol->flags&CVScroll) {
		      //Draw the scroll FTControls
				DrawFTControl(tcontrol->FTC_VScrollUp);
				DrawFTControl(tcontrol->FTC_VScrollDown);
				DrawFTControl(tcontrol->FTC_VScrollBack);
				//todo: it's better to calculate the slide button size when ever it changes, not adjusting it every time the control is drawn
				//CalcVScrollSlide(twin,tcontrol);  //determine size, and position
				DrawFTControl(tcontrol->FTC_VScrollSlide);
		    }  //CVScroll
		    if (tcontrol->flags&CHScroll) {
				DrawFTControl(tcontrol->FTC_HScrollLeft);
				DrawFTControl(tcontrol->FTC_HScrollRight);
				DrawFTControl(tcontrol->FTC_HScrollBack);
				//CalcHScrollSlide(twin,tcontrol);  //determine size, and position
				DrawFTControl(tcontrol->FTC_HScrollSlide);
		    }	
#if Linux
		  XFlush(PStatus.xdisplay);
#endif
#if WIN32
			GdiFlush();
#endif



			//call any onDraw functions for this control
				if (tcontrol->OnDraw!=0) {
					(*tcontrol->OnDraw) (twin,tcontrol);
					twin=GetFTWindow(winname); //check to see if window was deleted
					tcontrol=GetFTControl(conname); //check to see if control was deleted
					if (tcontrol==0 || twin==0) return; //user deleted control or window
				}


		break; //CTFolderTabGroup
		case CTTimeLine:

#if Linux
			XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->color[0]);
	    XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,bw,bh);
 		  XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);  
#endif
#if WIN32
		//tcontrol->brush=CreateSolidBrush(tcontrol->color[0]);
		//SelectObject(twin->hdcWin,tcontrol->brush);
		SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
		SelectObject(twin->hdcWin,GetStockObject(DC_BRUSH));
		tcontrol->rect.left=tcontrol->x1;
		tcontrol->rect.top=tcontrol->y1;
		tcontrol->rect.right=tcontrol->x1+bw;
		tcontrol->rect.bottom=tcontrol->y1+bh;
		//FillRect(twin->hdcWin,&tcontrol->rect,tcontrol->brush);
		FillRect(twin->hdcWin,&tcontrol->rect,(HBRUSH)GetStockObject(DC_BRUSH));
		SetTextColor(twin->hdcWin,tcontrol->textcolor[0]);
		SetBkColor(twin->hdcWin,tcontrol->textcolor[1]);
#endif


		TimeScale=tcontrol->TimeScale;
//long double fpi,NextTimeLine;

//			rx=tcontrol->StartTimeVisible; //initial potential time line
//			fpi=tcontrol->TimeSpace*tcontrol->TimeScale;
//			rx2=rx+fpi;
			//use lx1,lx2?


//we need to find the closest unit to the TimeScale*TimeSpace
//for example TimeScale=1 (1 pixel=1 second), TimeSpace=10
//so a StartTime of 0.999999 and then 1.00001 should draw a timeline
//if the TimeScale=.1 (1 pixel=.1 second), TimeSpace=10
//a StartTime of 0.4901 and then .5001 should cause a timeline
//if TimeSpace=1, this means that there is a timeline on each pixel

				//perhaps should use tcontrol->TimeSpace for space between time lines, or i[0] instead of ScrollPixelsX
//			a=(unsigned long long)(10000000000ULL*VStatus.TimeSpace*tcontrol->TimeScale);
//			a=(unsigned long long)(10000000000ULL*tcontrol->TimeSpace*tcontrol->TimeScale);
			//time in nanoseconds
			//a=(unsigned long long)(10000000000ULL*PStatus.ScrollPixelsX*tcontrol->TimeScale);

			//a=(unsigned long long)(10000000000ULL*tcontrol->TimeSpace*tcontrol->TimeScale);
			//round StartTime to TimeSpace (in s)
			//rx=((long double)((unsigned long long)(tcontrol->StartTimeVisible/tcontrol->TimeSpace)))*tcontrol->TimeSpace;
			//rx=tcontrol->StartTimeVisible;///tcontrol->TimeSpace;
			//round rx to nearest time unit - otherwise we get 16.009us at 10ns/pixel resolution
		rx=((long double)((unsigned int)(tcontrol->StartTimeVisible/tcontrol->TimeScale)))*tcontrol->TimeScale;
//			fprintf(stderr,"Starttime=%Lg rounded=%Lg\n",tcontrol->StartTimeVisible,rx);

			//get TimeLine pixel interval (in s)
		rx2=tcontrol->TimeSpace*TimeScale;
			//rx2=tcontrol->TimeScale*tcontrol->TimeSpace;
			//get first digit relative to 10x TimeScale*TimeSpace, then add=1 for each TimeSpace unit


//truncate rx to .001*TimeScale for printing (and perhaps it should be truncated itself - it shouldn't matter (we don't loose a significant representation of data by truncating here) because it is 3 orders less than the time unit - it just looks bad displaying: 2.003ms @ 100ns because the first 100 pixels are 148-247, which moves 0.001483 to 0.001503 seconds by adding 2 at the timescale resolution. We could probe further by *10 *100, etc but it seems not necessary. we only need to quantize the start time since the interval should be in units
			//rx=(long double)((unsigned int)(rx*1000.0/tcontrol->TimeScale))/(1000.0/tcontrol->TimeScale);
			//rx2=(long double)((unsigned int)(rx2*1000.0/tcontrol->TimeScale))/(1000.0/tcontrol->TimeScale);
			
//possibly I can determine if there is a timeline connected to the frame, and only add or subtract a page that is quantized to the timeline unit, but that is not as flexible as option 1).
//need to change back to 1) because a page right or left moves the timeline by a non-timeline divisible quantity, and so we need to find on the timeline where the 10s are. This can be done by multiplying the time by 10*timescale*timespace (because of round off apparently - remainder is more accurate with integers) and finding the pixel that is closest to returning a remainder of 0, and then the first timeline is placed on that pixel.
//was: after more thought about this decision I decided to go with 2), because initially there is an empty space to the right of the data - at some high scales - like 10 min, there will always be empty space on the right, so it is ok to stay with the timeline based only on timescale*timespace, and let there be extra space on the right. In terms of scrolling, scrolling right should stop when maxx<=tframe.x2. So a timeline will always be at pixel 0, timespace, timespace*2, etc
//was: There was a tough decision in:
//make the timeline conform to the timescale data, or make the timescale data not align perfectly in the frame at the end - for example if the timescale=10 seconds, but the data ends on some multiple of 1-9 seconds, either 1) the timeline shifts so the start time is offset by 1-9 1/timespace (1/10) timescale units or 2) there is a space of empty frame - the data control ending earlier than the 10th second. I am choosing option 1, since it looks better and maybe more simple.
//x1 will not always be exact multiple of TimeScale*TimeSpace

			//quantize StartTime to TimeScale*TimeSpace - possiblyl should not be needed
		//	rx-=fmodl(rx,rx2); //remove any remainder from division by TimeScale*TimeSpace

//			tcontrol->StartTimeVisible=rx;
#if 0 
			//find offset pixels to first line
			j=(int)(fmodl(rx,rx2)); //10 minus this remainder is the number of pixels until a time line
			//j=(int)(rx/rx2); //this rounded number is the initial time line (for example 3 of 10)
			fprintf(stderr,"Start Time=%Lg divide by %Lg remain=%d\n",rx,rx2,j);
			if (j>0) {
				//advance first pixel to nearest time line
				//time line (rx) doesn't change
				//rx+=(10-j)*tcontrol->TimeScale;
				//rx+=j*tcontrol->TimeScale;
				//fprintf(stderr,"first line=%Lg first pixel=%d (%d)\n",rx,(10-j),x1);
				fprintf(stderr,"first line=%Lg first pixel=%d (%d)\n",rx,j,x1);
				//x1+=(10-j);
				x1+=j;//(10-j);				
			} //j>0
#endif

			//this part is needed because a data control may not end on a TimeScale*TimeSpace timeline and this throws off the beginning time on the timeline (which will not be at pixel 0, and TimeSpace pixel intervals after that, but instead at pixel 1 to TimeSpace-1. 
			//now find offset line to nearest 10 line
			//within the next 10 timelines, find a 1000s, or 100s, or 10s in that order
			//this gives the pixel that the 100s (or 10s) line is on
			//if 100s 100-j=pixel of starting timeline
			h=0;
			i=0;
			j=0;
			k=0;
			match=0; //each byte = a 10s match 0x100=found 1000s, 0x10=found 100s
			a=(unsigned long long)(rx/TimeScale);	
			//with long double we are getting repeats from rounding like: 524 525 526 526 527
			//so using long integer instead
			//for(rxi=rx;rxi<rx+10*rx2;rxi+=tcontrol->TimeScale) { //goes through 10 timelines one pixel at a time
			for(cx=x1;cx<x1+10*tcontrol->TimeSpace;cx++) {//goes through 10 timelines one pixel at a time
				//a=(unsigned long long)(rxi/tcontrol->TimeScale);					
//				fprintf(stderr,"%lli ",a);
				if (!(match&0x100) && (a%1000)==0) { //1000s line
					h=k;
					match|=0x100;
				}
				if (!(match&0x10) && (a%100)==0) { //100s line
					i=k;
					match|=0x10;
				}
				if (!(match&0x1) && (a%10)==0) { //10s line - gives first 10 line - used to find first time line
					j=k;
					match|=1;
				}
				k++;
				a++; //we can just add 1 because units are not in s, but in pixels/timelines which corresponds exactly to timeunits/timelines
			} //for cx
//			if (j==0) { fprintf(stderr,"no 100s or 10s line found in 10 lines\n");}
//			fprintf(stderr,"\n1000s=%d 100s=%d 10s=%d\n",h,i,j);
			//there are two key points:
			//1) find the first pixel of a timeline (i, below determines this)
			//2) number the first timeline correctly (0-9) so that k=10 will be a 10,100,or 1000s line (the below code determines this)
			//k is the first timeline number of 10
			//if h=95, (presuming timespace=10), this means that pixel 95 has a 1000s line,
			//so the first showing timeline will be (100-95)/10(timespace)=.5 rounds to 0 - the first line will be 0, sp the 10s line will be 10 lines from the first
			//if h=42, pixel 42 has a 1000s line, first pixel is (100-42)/10=5.8 rounds to 5 
			//if i=92, pixel 82 has a 100s line, (100-82)/10=1.8 rounds to 1, 
			//
			//k=rintl(rx/rx2); //this rounded number is the initial time line of the 10s (for example 3 of 10)
//this part finds where the first 10s,100s, and 1000s lines are if any (there is always at least 1 10s line)
			if (h>0) { //found a 1000s line
				//k=10-(int)rintl((float)h/((float)tcontrol->TimeSpace)); //move forward so that
				//result should not be rounded but truncated instead
				k=10-(int)((float)h/((float)tcontrol->TimeSpace)); //move forward so that
//				fprintf(stderr,"1000s sets line to k=%d\n",k);
			} else { 
				if (i>0) { //found a 100s line
					//k=10-(int)rintl((float)i/((float)tcontrol->TimeSpace));
					k=10-(int)((float)i/((float)tcontrol->TimeSpace));
//					fprintf(stderr,"100s (%d) sets line to k=%d (timespace=%d) result=%f rounded=%Lg\n",i,k,tcontrol->TimeSpace,(float)i/((float)tcontrol->TimeSpace),rintl((float)i/((float)tcontrol->TimeSpace)));
				} else { 
					//go with 10s line (which there must be at least 1 of unless the 0 pixel is the 10s,100s,or 1000s)
//					k=10-(int)rint((float)j/((float)tcontrol->TimeSpace));
					k=10-(int)((float)j/((float)tcontrol->TimeSpace));
//					fprintf(stderr,"10s sets line to k=%d\n",k);
				} //i>0
			} //h



			h=x1; //store original x1			

//this part goes to the first timespace unit (10s) timeline and adjusts the starting time of that timeline
			if (j>0) {
				//advance first time to nearest time line
//				fprintf(stderr,"inexact timeline: first timeline moved forward %d pixels from %Lg to time %Lg.\n",j,rx,rx+j*TimeScale);
				x1+=j; //move j pixels forward
				//rx+=j*tcontrol->TimeScale; //move j timescale units forward, 1 for each pixel
				//we have to use this method of adding j to the integer to get the correct time because with double the rounding causes problems.
				a=(unsigned long long)(rx/TimeScale);	
				a+=j;
				rx=((long double)a)*TimeScale;
//				fprintf(stderr,"integer addition: timeline moved to time %Lg (a=%lli).\n",rx,a);

				//k++; //since initial timeline is behind starttime of timeline
				//fprintf(stderr,"%Lg\n",rx);
			} //j
//				rx+=(10-i)*tcontrol->TimeScale; //decrease starting visible time to
//				x1+=(10-i); //add 10-remainder pixels to the first timeline
//				k++; //go to next timeline 


			//if you can find a 100s line this is better
//			k=rintl(rx/rx2); //this rounded number is the initial time line (for example 3 of 10)
//			fprintf(stderr,"k=%d\n",j);
			//all we need to find is the 10s 
			//lx=(long int)((rx*1000000000.0)/(rx2*1000000000.0))/1000000000; //this rounded number is the initial time line (for example 3 of 10)
//fmodl does not seem to work, for example returns 1e-5 for fmodl(0.00062,0.00001), remainderl also returns inaccurate remainders, but different from fmodl, for example: remainder(0.0001,1e-05)=1.65436e-24 - sometimes the same as fmodl.
//so shift to unsigned long long (to 100 picosecond resolution) and get remainder
#if 0
//			a=(unsigned long long)((unsigned long long)(rx*10000000000.0)%(unsigned long long)(rx2*10000000000.0))/(unsigned long long)(rx2*1000000000.0);
			i=(int)((unsigned long long)((unsigned long long)(rx*10000000000.0)%(unsigned long long)(rx2*10000000000.0))/(unsigned long long)(rx2*1000000000.0));


			//
			fprintf(stderr,"stvisible rx=%Lg tscale*tspace rx2=%Lg (first line of 10: k=%d) (offset pixels to first timeline: i=%d)\n",rx,rx2,k,i);
//			fprintf(stderr,"stvisible rx=%Lg tscale*tspace rx2=%Lg (first line of 10: j=%d) (offset pixels to first timeline: i=%d) remainder(rx,rx2)=%Lg remainder=%Lg\n",rx,rx2,j,i,fmodl(rx,rx2),remainderl(rx,rx2));
//#if 0 
			if (i>0) {
				//advance first time to nearest time line
				rx+=(10-i)*tcontrol->TimeScale; //decrease starting visible time to
				x1+=(10-i); //add 10-remainder pixels to the first timeline
				k++; //go to next timeline 
				//k++; //move to next timeline tenth
				//fprintf(stderr,"first line=%Lg first pixel=%d (%d)\n",rx,(10-j),x1);
				//fprintf(stderr,"first line time=%Lg (first line of 10: j=%d) (offset pixels to first timeline: i=%d) x1=%d\n",rx,j,i,x1);
			} //j>0
#endif
//		StartSelected=tcontrol->StartTimeSelected;
//		EndSelected=tcontrol->EndTimeSelected;

//fprintf(stderr,"Start Time=%Lg interval=%Lg\n",rx,rx2);
	//		rx2=tcontrol->TimeScale*tcontrol->TimeSpace; //rx2 is now used as the interval
			//we have to use integers to advance time because adding floating point introduces errors. This goes to show how using a time base of nanoseconds is a good idea, but the work around was bot too bad, and the future is open for a relatively easy transition to picoseconds and beyond.
			//a=(unsigned long long)(rx/TimeScale);	
			cx=x1; //last character printed or start of timeline control
			for(i=x1;i<x2;i+=tcontrol->TimeSpace) {
				//round StartTime to TimeScale (not TimeScale and TimeSpace, TimeSpace is, for example, 10x larger)
//				b=(unsigned long long)(10000000000ULL*rx);  //time line units in nanoseconds

//			if (rx>StartSelected && rx<EndSelected) { //tcontrol==twin->FocusFTControl) {
//				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[2]);  //selected text color - draw actual sample
//			} else {
#if Linux
				XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcolor[0]); 
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[0]); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,tcontrol->textcolor[0]);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

#endif

//			}

				//if (rx%(10*rx2)==0) {
				//if (fmodl(rx,10*rx2)==0.0) {
				if (k%10==0.0) {
			    //add time numbers and units
				  //ns2ht(tstr,tx,0);
				  //fprintf(stderr,"time=%lli\n",tcontrol->time);
					sy=(float)(y1+(y2-y1)*4/10); //10s

#if Linux
					XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,(int)sy,i,y2-1);
#endif
#if WIN32
					tp[0].x=i;	tp[0].y=(int)sy;	tp[1].x=i;	tp[1].y=y2-1;
					Polyline(twin->hdcWin,tp,2);
#endif

					//do not text if it would extend past end of control
					//rx>0
					FT_FormatTime(tstr,rx,0,tcontrol->TimeScale,tcontrol->TimeSpace); //create label for time (tx is in seconds - but labels can be "1 ns","1 us", etc)
//					FT_FormatTime(tstr,a*TimeScale,0); //create label for time (tx is in seconds - but labels can be "1 ns","1 us", etc)
					//starttime>0, not first pixel of timeline
				  if (rx>0.0 && i>x1 && ((i+(((int)strlen(tstr)/2)*fw))<x2) && (i-((int)strlen(tstr)/2+1)*fw)>cx) {
//			      XSetForeground (PStatus.xdisplay, twin->xgc, tcontrol->textcolor[0]);

#if Linux
				    XDrawString(PStatus.xdisplay,twin->xwindow,twin->xgc,i-(strlen(tstr)/2)*fw,y1+fh,tstr,strlen(tstr));
#endif
#if WIN32
						MultiByteToWideChar(CP_UTF8,0,(LPCSTR)tstr,strlen(tstr),(LPWSTR)tcontrol->text_16,FTMedStr);
						TextOut(twin->hdcWin,i-(strlen(tstr)/2)*fw,y1+fh,(LPWSTR)tcontrol->text_16,1);
#endif
						cx=i+(strlen(tstr)/2)*fw; //last character printed
	  			}
	 			 } else { //fmod 10*
					//if (rx%(5*rx2)==0) {
//					if (fmodl(rx,5*rx2)==0.0) {
					if (k%5==0) {
					//draw 5x medium line
					  sy=(float)(y1+(y2-y1)*6/10);  //5s
#if Linux
						XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,(int)sy,i,y2-1);
#endif
#if WIN32
					tp[0].x=i;	tp[0].y=(int)sy;	tp[1].x=i;	tp[1].y=y2-1;
					Polyline(twin->hdcWin,tp,2);
#endif
				}	else {
					//if (rx%rx2==0) {
					//if (fmodl(rx,rx2)==0.0) {
						//draw regular 1x short line
					  sy=(float)(y1+(y2-y1)*9/10);  //1s
#if Linux
						XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,i,(int)sy,i,y2-1);
#endif
#if WIN32
					tp[0].x=i;	tp[0].y=(int)sy;	tp[1].x=i;	tp[1].y=y2-1;
					Polyline(twin->hdcWin,tp,2);
#endif
						//}
				} //fmod 5.0
		  } //				if (b%(10*NSTime)==0) {


//#endif

			rx+=rx2;  //add TimeScale (s/pixel)
			//a++;
			k++; 
	//fprintf(stderr,"%lli\n",rx);
		}  //end for i



//		fprintf(stderr,"rx=%Lg cursor=%Lg	",rx,tcontrol->StartTimeCursor);
		x1= h;//h is original x1
		if (tcontrol->StartTimeVisible<tcontrol->StartTimeCursor && tcontrol->StartTimeCursor<tcontrol->EndTimeVisible) {
			//XSetForeground (PStatus.xdisplay,twin->xgc,tcontrol->textcol			
			x1=x1+(int)((tcontrol->StartTimeCursor-tcontrol->StartTimeVisible)/tcontrol->TimeScale);
			//fprintf(stderr,"Drawing line for rx=%Lg cursor=%Lg	",rx,tcontrol->StartTimeCursor); //selected color 
#if Linux
			XSetForeground(PStatus.xdisplay,twin->xgc,tcontrol->textcolor[3]); 
			XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1+1,x1,y2-1);
#endif
#if WIN32
			//tcontrol->pen=CreatePen(PS_SOLID,1,tcontrol->textcolor[3]); 
			//SelectObject(twin->hdcWin,tcontrol->pen);
			SetDCPenColor(twin->hdcWin,tcontrol->textcolor[3]);
			SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
			tp[0].x=x1;	tp[0].y=y1+1;	tp[1].x=x1;	tp[1].y=y2-1;
			Polyline(twin->hdcWin,tp,2);
#endif
		} //rx==tcontrol->StartTimeCursor

#if Linux
	  XFlush(PStatus.xdisplay);
#endif
#if WIN32
	GdiFlush();
#endif


	//call any onDraw functions for this control
		if (tcontrol->OnDraw!=0) {
			(*tcontrol->OnDraw) (twin,tcontrol);
			twin=GetFTWindow(winname); //check to see if window was deleted
			tcontrol=GetFTControl(conname); //check to see if control was deleted
			if (tcontrol==0 || twin==0) return; //user deleted control or window
		}

//if the timeline's starttimevisible has changed, any data controls connected to this timeline control need to have their starttimevisible changed, and they need to be redrawn
		if (tcontrol->StartTimeVisible!=tcontrol->StartTimeBuffer) {
			tcontrol2=twin->icontrol;
			while(tcontrol2!=0) {
				if (tcontrol2->type==CTData && tcontrol2->TimeLineFTControl==tcontrol) {
					AdjustDataFTControlDimensions(tcontrol2);
					DrawFTControl(tcontrol2);
				}
				tcontrol2=tcontrol2->next;
			} //while
			tcontrol->StartTimeBuffer=tcontrol->StartTimeVisible;
		} //(tcontrol->StartTimeVisible!=tcontrol->StartTimeBuffer) {

		break; //CTTimeLine
  }  //end switch on FTControl type

	//there must be a better way to do z-buffering and drawing, but for now I simply redraw any controls that have a lower z and cover any part of this control
//if drawing all controls - order by z, set WControlsZOrdered bit (reset if any control added or deleted to/from this window)
//if drawing only a single control - use the belo method of redrawing any controls higher
//one idea is ordering controls according to z and drawing in that order, but since we can draw only a single control
	tcontrolz=twin->icontrol;
	while(tcontrolz!=0) {
		if (tcontrolz!=tcontrol && tcontrolz->z<tcontrol->z) {
//			fprintf(stderr,"Control %s has lower z than %s\n",tcontrolz->name,tcontrol->name);
//			fprintf(stderr,"z x1=%d x2=%d, x1=%d x2=%d\n",tcontrolz->x1,tcontrolz->x2,x1,x2);
				//control on a higher plane, see if any part is in x-y rectangle of this control
			if (tcontrolz->x1<x2 && tcontrolz->x2>x1) {//part of X is in
//				fprintf(stderr,"X of %s is within X of %s\n",tcontrolz->name,tcontrol->name);
				if (tcontrolz->y1<y2 && tcontrolz->y2>y1) {//part of Y is in
//					fprintf(stderr,"And %s is clipped by %s\n",tcontrolz->name,tcontrol->name);
					DrawFTControl(tcontrolz); //redraw overlapped control on higher plane
				} 	//tcontrolz->y1<y2
			} //tcontrolz->x1<x2
		} //tcontrolz!=tcontrol
		tcontrolz=tcontrolz->next;
	} //while tcontrolz!=0


	} //clipped

  } //end if this FTControl is in this window


  }  //end if FTControl is visible

 
   //fprintf(stderr,"b4 next\n");

  if (allFTControls==0) {
		tcontrol=0;
	} else {
    tcontrol=(FTControl *)tcontrol->next;
	}


}//end while tcontrol!=0



#if Linux
XFlush(PStatus.xdisplay);
#endif
#if WIN32
GdiFlush();
#endif



#if 0//WIN32
//end wait for mutex
// Release ownership of the mutex object
if (!ReleaseMutex(PStatus.DrawFTControlsMutex))
{
	// Handle error.
}
#endif //WIN32



if (PStatus.flags&PInfo)
  fprintf(stderr,"after DrawFTControls\n");

}//end DrawFTControls



//Formated time
//itime is the time in seconds
//This is used only for TimeLine controls - perhaps FT_FormatTimeLine
void FT_FormatTime(char *text,long double itime,int format,long double TimeScale,int TimeSpace)
{
int hrs,mns,sec,msec,usec,nsec;
//FTControl *tcontrol;
char units[10];
long double timeunit;

//probably each CTTimeLine should have a TimeSpace value

//itime is time in floating point, for example 100.0= 100 seconds, 0.001 = 1ms
//fprintf(stderr,"ns2ht\n");


hrs=(int)(itime/3600.0);
itime-=hrs*3600;
mns=(int)(itime/60.0);
itime-=mns*60;
sec=(int)(itime);
itime-=sec;
msec=(int)(itime*1000.0);
itime-=(msec*0.001);
usec=(int)(itime*1000000.0);
itime-=(usec*0.000001);
nsec=(int)(itime*1000000000.0);


//fprintf(stderr,"in ns2tm usec=%lli\n",usec);
//  fprintf(stderr,"%i:%02i:%02i.%03i",hrs,mns,sec,msec);
if (format==1)
{
//if (hrs==0 && mns==0 && sec==0)
//  sprintf(text,"0.%03lli %03lli %03lli",msec,usec,nsec);
//else
  sprintf(text,"%i:%02i:%02i.%03i",hrs,mns,sec,msec);
}
else
{

//tcontrol=GetFTControl("btnTimeLine");
//fprintf(stderr,"Timescale= %Lg\n",tcontrol->TimeScale);

//timeunit is = to the space between two lines on the timeline
//timeunit=tcontrol->TimeScale*tcontrol->TimeSpace;
timeunit=((long double)TimeScale)*TimeSpace;
//fprintf(stderr,"timeunit=%Lg\n",timeunit);
//what time is displayed depends on the scale of resolution
//all larger resolutions are displayed too, although perhaps this can be a setting - to ignore the higher scale values at small scale
//at scale=1 day, we do not display hours, minutes, etc
//at scale=1 second we do not display milliseconds
//at scale=1 ms we do not display us
//at scale=1 us we do not display ns

//TimeScale is in seconds
strcpy(units,"");
strcpy(text,"");
//scale is >= 1 day or there is at least 1 day on the timeline
if (timeunit>=86400.0 || hrs>86400)	{
	if (hrs!=0) { //dont bother to print if no hours
		if ((hrs/24)<24) {
			sprintf(text,"%i day ",hrs/24);
		} else {
			sprintf(text,"%i days ",hrs/24);
		}
		hrs-=hrs/24;
	}
//  sprintf(text,"%i days %i:%02i:%02i",hrs/24,hrs-hrs/24,mns,sec);
}

//scale is >= 1 second or there is at least 1 second on the timeline
if (timeunit>=1.0 || sec>0 || mns>0 || hrs>0)	{
	if (!(sec==0 && mns==0 && hrs==0)) { //dont bother to print if no seconds minutes or hours
		if (hrs>0) {
			sprintf(text,"%i:%02i:%02i",hrs,mns,sec);
		} else {
			if (mns>0) {
				sprintf(text,"%i:%02i",mns,sec);
			} else {
				if (sec>0) {
					sprintf(text,"%i",sec);
					strcpy(units,"s");
				} //sec>0
			} //mns>0
		} //hrs>0
	} //!(sec==0 && min==0
}  //TimeScale>=1.0 || sec>0

//scale is 1-999 milliseconds
//fprintf(stderr,"tcontrol->TimeScale=%Lg timescale>=0.0001 is %d\n",tcontrol->TimeScale,(tcontrol->TimeScale>=(long double)0.0001));
//for some reason tcontrol->TimeScale>=0.0001 does not work when tcontrol->TimeScale==0.0001
//fprintf(stderr,"%Lg>1.0 is %d\n",tcontrol->TimeScale*10000.0,((tcontrol->TimeScale*10000.0)>=1.0));
//if (tcontrol->TimeScale>=(long double)0.0001)	{

if ((timeunit*1000.0)>=1.0)	{
	if (strlen(text)>0) {
		sprintf(text,"%s.%03i%s",text,msec,units);
	} else {
		sprintf(text,"%ims",msec);
	}
} else {

	//scale is 1-999 microseconds
//	if (tcontrol->TimeScale>=0.0000001)	{
	if ((timeunit*1000000.0)>=1.0)	{
		if (strlen(text)>0) {
			sprintf(text,"%s.%03i%03i%s",text,msec,usec,units);
		} else {
			if (msec>0) {
				sprintf(text,"%i.%03ims",msec,usec);
				//sprintf(text,"%i%03ius",msec,usec);
			} else {
				sprintf(text,"%ius",usec);
			} //msec>0
		}
	} else {
	
		//scale is 1-999 nanoseconds
//		if (tcontrol->TimeScale>=0.0000000001)	{
		if ((timeunit*10000000000.0)>=1.0)	{
			if (strlen(text)>0) { //there is at least 00:00:00
				sprintf(text,"%s.%03i%03i%03i%s",text,msec,usec,nsec,units);
			} else {
				if (msec>0) {
					sprintf(text,"%i.%03i%03ims",msec,usec,nsec);
				} else { //msec>0
					if (usec>0) {
						sprintf(text,"%i.%03ius",usec,nsec);
					} else {
						sprintf(text,"%ins",nsec);
					} //usec>0
				} //msec>0
			} //strlen(text)>0
		} //>=0.000000001
	} //microsecond >0.000001

} //0.001


//later make PStatus bit that makes txtMarkIn/Out hh:mm:ss.mls
#if 0 
if (hrs>23)
  {
  sprintf(text,"%i days ",hrs/24);
//  sprintf(text,"%i days %i:%02i:%02i",hrs/24,hrs-hrs/24,mns,sec);
  }
else
  {

	if (hrs>0 || mns>0) //||sec>0)
		{
		sprintf(text,"%i:%02i:%02i ",hrs,mns,sec);
		}
	else
		{

		if (sec>0)
		  {
		  //if (tcontrol->TimeScale*VStatus.TimeSpace<1.0)
//			if (tcontrol->TimeScale*PStatus.ScrollPixelsX<1.0)
			if (tcontrol->TimeScale*tcontrol->TimeSpace<1.0)
		    sprintf(text,"%i.%03is",sec,msec);
		  else
		    sprintf(text,"%is",sec);
		  }
		else
		  {
		  if (msec>0)
		     {
//		    if (tcontrol->TimeScale*VStatus.TimeSpace<0.001)
//		    if (tcontrol->TimeScale*PStatus.ScrollPixelsX<0.001)
		    if (tcontrol->TimeScale*tcontrol->TimeSpace<0.001)
		      sprintf(text,"%i.%03ims",msec,usec);
		    else
		      sprintf(text,"%ims",msec);
		    }
		  else
		    {
		    if (usec>0)
		      {
//		      if (tcontrol->TimeScale*VStatus.TimeSpace<0.000001)
//		      if (tcontrol->TimeScale*PStatus.ScrollPixelsX<0.000001)
		      if (tcontrol->TimeScale*tcontrol->TimeSpace<0.000001)
		        sprintf(text,"%i.%03ius",usec,nsec);
					else
		        sprintf(text,"%ius",usec);
		      }
		    else
		      {
		      sprintf(text,"%ins",nsec);
		      }  //usec
		    }  //msec
		  }  //sec
		} //hr:mn:sec
	} //days
#endif
}  //format 0

} //FT_FormatSeconds


//unsigned int FTMessageBox(char *message,unsigned int bits,char *title)
unsigned int FTMessageBox(char *message,unsigned int flags,char *title,long delay)
{
int i,numbut,bw,bh;
#if Linux
int fw,fh;
#endif
FTWindow *twin,*twin2;
unsigned int retval;
char winname[FTMedStr]; 
int x,y;
long tempus;
#if WIN32
DWORD dwWaitResult;
#endif

//fprintf(stderr,"bits=%d\n",bits);

//for Windows, probably just use MessageBox() function
//we need to stop the FT_GetInput loop
#if WIN32
//lock the GetInput thread while creating the window controls
//not really needed, except when called from a thread- and AddFTControl functions shouldn't be called from a thread
dwWaitResult = WaitForSingleObject(
	PStatus.GetInputMutex,    // handle to mutex
	INFINITE);  // no time-out interval

if (dwWaitResult != WAIT_OBJECT_0) {
	fprintf(stderr, "Could not lock GetInputMutex");
	//return(0);
}
#endif
//returns the button that was clicked
strcpy(PStatus.mbmesg,message);
PStatus.mbflags=flags;
//determine number of buttons
numbut=0;
for(i=0;i<32;i++) {numbut+=((flags&(1<<i))>0);}

//fprintf(stderr,"numbut1=%d\n",numbut);
//determine dimensions of FTMessageBox
bw=64;
bh=32;

twin=(FTWindow *)malloc(sizeof(FTWindow));
//buttons are 64x32
memset(twin,0,sizeof(FTWindow));
//get free window name
i=0;
sprintf(winname,"mbw%d",i);

while(GetFTWindow(winname)!=0) {
//fprintf(stderr,"here\n");
i++;
sprintf(winname,"mbw%d",i);
}


strcpy(twin->name,winname);
strcpy(twin->title,title);
strcpy(twin->ititle,title);
twin->flags|=(WExclusive|WMsgBox); //the FTMessageBox traditionally has exclusive FTControl over a program
if (delay>0) {
	//twin->CloseTime=delay;  //delay is in ms
	memcpy(&twin->CloseTime,&PStatus.utime,sizeof(struct timeval));

	tempus=twin->CloseTime.tv_usec+delay*1000;
	if (tempus>999999) {
		twin->CloseTime.tv_sec+=tempus/1000000;  //number of seconds
		twin->CloseTime.tv_usec=tempus-tempus/1000000; //remaining us
	} else {
		twin->CloseTime.tv_usec+=delay*1000;
	}
	twin->flags|=FTW_CloseOnTimer;
}
//if ((bw*numbut+2*PStatus.PadX)>tfont->max_bounds.width*(strlen(message)+2)) 
//  twin->w=bw*numbut+2*PStatus.Padx;

#if Linux
	twin->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (twin->font==0) {
		fprintf(stderr,"Could not find font for FTWindow %s.\n",twin->name);
	}

	twin->fontwidth=twin->font->max_bounds.width;
	twin->fontheight=twin->font->max_bounds.ascent+twin->font->max_bounds.descent;
#endif
#if WIN32
	if (twin->hdcWin==0) {
		twin->hdcWin=GetDC(twin->hwindow);
	} else {  //free the DC and get a new one - or else we can't draw on the window
		ReleaseDC(twin->hwindow,twin->hdcWin);
		twin->hdcWin=GetDC(twin->hwindow);
	}
	if (twin->hdcMem==0) {
		twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
	}	else {  //free the DC and get a new one - or else we can't draw on the window
		ReleaseDC(twin->hwindow,twin->hdcMem);
		twin->hdcMem=CreateCompatibleDC(twin->hdcWin);  //for bitmap/jpg images
	}

	twin->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
	if (twin->font==0) {
		fprintf(stderr,"Could not find font for FTWindow %s.\n",twin->name);
	}
	//the problem is that twin->hdcWin has not been definited yet since the window has not been created yet
	SelectObject(twin->hdcWin,twin->font); 
	//memset(tcontrol->textmet,0,sizeof(TEXTMETRIC));
	GetTextMetrics(twin->hdcWin,&twin->textmet);
	twin->fontwidth=twin->textmet.tmAveCharWidth;
	twin->fontheight=twin->textmet.tmHeight;
#endif

if ((bw*numbut)>twin->fontwidth*(strlen(message)+2)) {
  twin->w=bw*numbut;
} else {
  twin->w=twin->fontwidth*(strlen(message)+2);
}
twin->h=(twin->fontheight)*2+bh+2*PStatus.PadY;

//twin->h=(tfont->max_bounds.ascent+tfont->max_bounds.descent)+bh;

//if (bits&MB_CENTER) {
//  twin2=GetFTWindowN(PStatus.focus);
  twin2=PStatus.FocusFTWindow; //get window with focus
  if (twin2!=0) {
    x=twin2->x+twin2->w/2-twin->w/2;
    y=twin2->y+twin2->h/2-twin->h/2;
  }
  else {
    x=0;
    y=0;
  }
//}

twin->x=x;  //location of window
twin->y=y;
//twin->Keypress=(FTControlfuncwk *)win0key;
//twin->ButtonDownUpOnFTControl[0]=(FTControlfunccxy *)main_ButtonDownUpOnFTControl;
twin->AddFTControls=(FTControlfuncw *)FTMB_AddFTControls;
CreateFTWindow(twin);
twin2=GetFTWindow(twin->name);
free(twin);
#if WIN32
//release ownership of the GetInput Mutex
if (!ReleaseMutex(PStatus.GetInputMutex)) {
	// Handle error.
	fprintf(stderr, "Could not release GetInputMutex");
}

#endif
//OpenFTWindow(twin2);
//if (twin2->flags&WOpen) {
DrawFTWindow(twin2);
//}


//move window to position
//twin2=GetFTWindow(winname);
//XMoveWindow(PStatus.xdisplay,twin2->xwindow,x,y);


//need to intercept all input thru here, to determine what button was pressed
 //wait until a FTMessageBox button was pressed (PStatus.mb bit is set)
//while(PStatus.mb==0 && (PStatus.flags&PExit)==0) {
PStatus.mb=0; 
while(PStatus.mb==0) {
//there can be no while(); in any part of ft FT_GetInput needs to be checked
GetTime();  //update PStatus time variables utim (us) and ctim (h:m:s)
FT_GetInput();
//if FTMessageBox is closed with x that represents MB_CANCEL
//otherwise this would become the main loop of ft
}

retval=PStatus.mb;

PStatus.mb=0;  //clear mb bit
//DestroyFTWindowByName(winname);
DestroyFTWindow(twin2);
//CloseFTWindow(twin2);

return retval;
} //end FTMessageBox


void FTMB_AddFTControls(FTWindow *twin)
{
FTControl *tcontrol;
int bw,bh,i,j,k,numbut,fw,fh;
char tstr[FTMedStr];

bw=64;
bh=32;

numbut=0;
for(i=0;i<32;i++) {numbut+=((PStatus.mbflags&(1<<i))>0);}

//fprintf(stderr,"numbut=%d\n",numbut);

tcontrol=(FTControl *)malloc(sizeof(FTControl));

//message FTControl
memset(tcontrol,0,sizeof(FTControl));
//get a free FTControl name
i=0;
sprintf(tstr,"mbc%d",i);
while(GetFTControl(tstr)!=0) {
i++;
sprintf(tstr,"mbc%d",i);
}
strcpy(tcontrol->name,tstr);
#if Linux
	tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol->font==0) {
		fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
	}

	tcontrol->fontwidth=tcontrol->font->max_bounds.width;
	tcontrol->fontheight=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
		tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		if (tcontrol->font==0) {
			fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
		}
		SelectObject(twin->hdcWin,tcontrol->font); 
		GetTextMetrics(twin->hdcWin,&tcontrol->textmet);
		tcontrol->fontwidth=tcontrol->textmet.tmAveCharWidth;
		tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;

tcontrol->type=CTLabel;
tcontrol->x1=0;
tcontrol->y1=PStatus.PadY;
tcontrol->x2=fw*(strlen(PStatus.mbmesg)+2);
tcontrol->y2=fh*2+PStatus.PadY; 
if (PStatus.depth==16) {tcontrol->color[0]=wingray16;}  //background no press
else {tcontrol->color[0]=wingray;}  //background no press
tcontrol->color[1]=black;//black; //dkgray;  //text
tcontrol->color[2]=gray;//blue;  //background (selected with mouse over or tab)
tcontrol->color[3]=dkgray;  //selected with mouse button 0 down
//tcontrol->win=twin->num;
//tcontrol->hotkey=PAlt|0x66;  //alt-f
//tcontrol->hotkey2=PAlt|0x46;  //alt-F
strcpy(tcontrol->text,PStatus.mbmesg);
AddFTControl(twin,tcontrol);


//for each button
k=0;
for(j=0;j<numbut;j++) {

while((PStatus.mbflags&(1<<k))==0) {k++;}


memset(tcontrol,0,sizeof(FTControl));
//get a free FTControl name
i=1;
sprintf(tstr,"mbc%d",i);
while(GetFTControl(tstr)!=0) {
i++;
sprintf(tstr,"mbc%d",i);
}
strcpy(tcontrol->name,tstr);
#if Linux
	tcontrol->font=XLoadQueryFont(PStatus.xdisplay,"*ISO8859*");
	if (tcontrol->font==0) {
		fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
	}

	tcontrol->fontwidth=tcontrol->font->max_bounds.width;
	tcontrol->fontheight=tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent;
#endif
#if WIN32
		tcontrol->font=CreateFont(FT_DefaultFontHeight,FT_DefaultFontWidth,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,0);
		if (tcontrol->font==0) {
			fprintf(stderr,"Could not find font for FTControl %s.\n",tcontrol->name);
		}
		SelectObject(twin->hdcWin,tcontrol->font); 
		GetTextMetrics(twin->hdcWin,&tcontrol->textmet);
		tcontrol->fontwidth=tcontrol->textmet.tmAveCharWidth;
		tcontrol->fontheight=tcontrol->textmet.tmHeight;
#endif
fw=tcontrol->fontwidth;
fh=tcontrol->fontheight;

tcontrol->type=CTButton;
//tcontrol->x1=PStatus.PadX+bw*j;
tcontrol->x1=bw*j;
tcontrol->y1=fh*2+PStatus.PadY;
//tcontrol->y1=(tcontrol->font->max_bounds.ascent+tcontrol->font->max_bounds.descent);
tcontrol->x2=tcontrol->x1+bw;
tcontrol->y2=tcontrol->y1+bh;
tcontrol->color[0]=ltgray;  //background no press
tcontrol->color[1]=black;//dkgray;  //text
tcontrol->color[2]=gray;//blue;  //background (mouse over)
tcontrol->color[3]=dkgray;  //bkground press
//tcontrol->win=twin->num;
//tcontrol->hotkey=PAlt|0x66;  //alt-f
//tcontrol->hotkey2=PAlt|0x46;  //alt-F
switch(1<<k) {
  case FTMB_OK:
    strcpy(tcontrol->text,"OK");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbok_ButtonDownUp;
    break;
  case FTMB_CANCEL:
    strcpy(tcontrol->text,"Cancel");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbcancel_ButtonDownUp;
    break;
  case FTMB_YES:
    strcpy(tcontrol->text,"Yes");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbyes_ButtonDownUp;
    break;
  case FTMB_NO:
    strcpy(tcontrol->text,"No");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbno_ButtonDownUp;
    break;
  case FTMB_YESALL:
    strcpy(tcontrol->text,"Yes All");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbyesall_ButtonDownUp;
    break;
  case FTMB_NOALL:
    strcpy(tcontrol->text,"No All");
    tcontrol->ButtonDownUp[0]=(FTControlfuncwcxy *)mbnoall_ButtonDownUp;
    break;
} //endswitch
k++;

AddFTControl(twin,tcontrol);

}  //end for j

free(tcontrol);

}  //end FTMB_AddFTControls


void mbok_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_OK;
} //mbok_ButtonDownUp

void mbcancel_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_CANCEL;
} //mbcancel_ButtonDownUp

void mbyes_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_YES;
} //mbyes_ButtonDownUp

void mbno_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_NO;
} //mbno_ButtonDownUp

void mbyesall_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_YESALL;
} //mbyesall_ButtonDownUp

void mbnoall_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
PStatus.mb=FTMB_NOALL;
} //mbnoall_ButtonDownUp

//Scrollbar functions
//VERTICAL SCROLLBAR FUNCTIONS
void VScrollUp_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
{
FTControl *tc;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"VScrollUp_ButtonDown\n");
}

//Get parent control attached to this scroll button or control itself if ItemList control
tc=tcontrol->parent;

if (tc!=0) {

	if (tc->type==CTFileOpen || tc->type==CTItemList) {
		if (tc->FirstItemShowing>1) {
			tc->FirstItemShowing--;
			UpdateItemList(tc);  
			DrawFTControl(tc);  //redraw parent FTControl	
		} //i[0]>0
	} //CTFileOpen||CTItemList

	if (tc->type==CTTextArea) {
		//same as up arrow key 
		FT_TextAreaUp(tc,0);  //0=do not move cursor, only move i[1] top of page
		DrawFTControl(tc);
	}
	if (tc->type==CTFrame) { //scrollbar connected to frame
	//perhaps should pass twin?
		FT_FrameScrollUp(twin,tc,0); //moves all controls on frame up 0=only 1 line
	} //CTFrame


	//if (twin!=0) twin->focus=tcontrol->tab;
	//fprintf(stderr,"Set focus6\n");
	FT_SetFocus(twin,tc);
} //if (tc!=0) 
}  //VScrollUp_ButtonDown

void VScrollDown_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
{
FTControl *tc;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"VScrollDown_ButtonDown\n");
}

//Get parent control attached to this scroll button or control itself if ItemList control
tc=tcontrol->parent;


if (tc!=0) {

	//fprintf(stderr,"i[0]=%d num=%d-numvis=%d \n",tcontrol->CursorLocationInChars,tcontrol->num,tcontrol->numvisible);
	if (tc->type==CTFileOpen || tc->type==CTItemList) {
		if (tc->FirstItemShowing<(tc->ilist->num-tc->numvisible+1)) {
			tc->FirstItemShowing++;
			UpdateItemList(tc);
			DrawFTControl(tc);  //redraw FTControl
			} 
	} //CTFileOpen

	if (tc->type==CTTextArea) {
		//same as arrow down key
		FT_TextAreaDown(tc,0); //0=do not move cursor
		DrawFTControl(tc);
	} //CTTextArea

	if (tc->type==CTFrame) { //scrollbar connected to frame
	//perhaps should pass twin?
		FT_FrameScrollDown(twin,tc,0); //moves all controls on frame up 0=only scroll 1 line
	} //CTFrame

	//twin=GetFTWindowN(tcontrol->win);
	//if (twin!=0) twin->focus=tcontrol->tab;
	//fprintf(stderr,"Set focus7\n");
	FT_SetFocus(twin,tc);
//watch = XCreateFontCursor (PStatus.xdisplay, XC_watch);
//XDefineCursor(PStatus.xdisplay,twin->xwindow,watch);
} //if (tc!=0)
//fprintf(stderr,"scroll fileopen\n");
} //VScrollDown_ButtonDown

//control sent is scroll back button
void VScrollBack_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x, int y, int button) 
{
FTControl *tslide,*tc;
char tstr[FTMedStr];

//Get parent control attached to this scroll button or control itself if ItemList control
tc=tcontrol->parent;

if (tc!=0) {
	if (tc->type==CTTextArea) {  //scrollbar connected to textarea
		if (y<tcontrol->y1) {
			FT_TextAreaPgUp(tc,0); //0=do not move cursor
		} 
		if (y>tcontrol->y2) {
			FT_TextAreaPgDown(tc,0);  //0=do not move cursor
		}//y>tcontrol->y2
	} //tparent->type==CTTextArea
	if (tc->type==CTFrame) { //scrollbar connected to frame
			//get slidebutton
			sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tc->name);
			//tcontrol3=GetFTControl("sm_foOpenFile2");
			tslide=GetFTControl(tstr);
			if (y<tslide->y1) {
				FT_FrameScrollUp(twin,tc,1);
			} 
			if (y>tslide->y2) {
				FT_FrameScrollDown(twin,tc,1);
			}//y<tcontrol->y1
	} //CTFrame
	if (tc->type==CTFileOpen || tc->type==CTItemList) {
		FT_SetFocus(twin,tc);

		//get slidebutton
		//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tparent->name);
		//tcontrol3=GetFTControl("sm_foOpenFile2");
		//tslide=GetFTControl(tstr);
		tslide=tc->FTC_VScrollSlide;

		//tslide is slider
		if (y>tslide->y2) {
			ScrollFTItemList(twin,tc,FT_SCROLL_FTITEMLIST_PAGE_DOWN);
		}
		if (y<tslide->y1) {
			ScrollFTItemList(twin,tc,FT_SCROLL_FTITEMLIST_PAGE_UP);  
		}
	} //CTFileOpen

	DrawFTControl(tc); //this also redraws any attached scrollbars

} //if (tc!=0) {
} //VScrollBack_ButtonDown

//perhaps should be VScrollSlide_Button0DownMouseMove - since mousemove over scrollslide without buttondown is not important
//tcontrol is the slide button
void VScrollSlide_Button0DownMouseMove(FTWindow *twin,FTControl *tcontrol,int x,int y)
{
FTControl *tcontrol3,*tback,*tlabel,*tslide,*tc;
//FItemList *tlist;
int tnum,bh,miny,maxy,numentries,totalentries,fh;//bw,bh;
float ratio;
//char tstr[FTMedStr];


//Get parent control attached to this scroll button or control itself if ItemList control
if (tcontrol->type==CTItemList) {
	tc=tcontrol;
} else {
	tc=tcontrol->parent;
}
if (tc!=0) {

//This function drags scroll bar slide buttons
//it does not determine the scroll bar slide size, that is done in InitScroll
//moving scroll bar slide buttons by not dragging (by keys or scroll buttons) is done in UpdateScrollSlide()

//tcontrol is control with CVSlide bit set - the slide button itself
//tcontrol2 is parent control of control with scrollbar (FileOpen or Frame)
//tback is scroll back


//sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol2->name);
//tback=GetFTControl(tstr);
tback=tc->FTC_VScrollBack;
//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol2->name);
tslide=tcontrol;//GetFTControl(tstr);


//fprintf(stderr,"Ly=%d y=%d\n",PStatus.LastMouseY,y);

//this is already true
//if (tcontrol->flags&CButtonDown(0)) {// && y<tcontrol3->y2 && y>tcontrol3->y1) { //only move if within y of middle button

//difference in mouse move y position
  tnum=(y-PStatus.LastMouseY);

//  bw=(tcontrol->x2-tcontrol->x1);
  bh=(tcontrol->y2-tcontrol->y1);
  

//fprintf(stderr,"foOpenFile2_MouseMove tnum=%d\n",tnum);

//possible timesaver (needed because scroll bar is slow)=
//no erase, just draw two FTControls

//  EraseFTControl(twin,tcontrol);

if (tc->type==CTFileOpen || tc->type==CTItemList) {
//move scroll slider control
  if (tnum>0) {  //going down
    if (tslide->y2+tnum>tback->y2) {
      tslide->y2=tback->y2;
	  tslide->y1 = tback->y2 -bh;
    }
    else {
      tslide->y1+=tnum;
      tslide->y2+=tnum;
      //fprintf(stderr,"added tcontrol->y1=%d\n",tcontrol->y1);
    }  //tcontrol->y2+tnum>
  } //tnum>0
  else {  //going up
    if (tslide->y1+tnum<tback->y1) {
      tslide->y1=tback->y1;
      tslide->y2=tback->y1+bh;
    }
    else {
      tslide->y1+=tnum;
      tslide->y2+=tnum;
    }
  }//num>0

	//adjust i[0] (the first FTItem showing at the top of the fileopen FTControl) 
	if (tc->ilist!=0) {
		//get ratio of scroll button to back bar
		//ratio=1.0-((float)(tcontrol->y2-tcontrol->y1)/(float)(tback->y2-tback->y1));
		//ratio=1.0-((float)(tslide->y2-tslide->y1)/(float)(tback->y2-tback->y1));
		ratio=(float)(tback->y2-tslide->y1)/(float)(tback->y2-tback->y1);
	//  fprintf(stderr,"ratio=%f\n",ratio);
		if (ratio<=1.0 && ratio!=0) {

		//  ratio=(1.0-((float)(tback->y2-tcontrol->y1)/(float)(tback->y2-tback->y1)))/ratio;
			//ratio=(1.0-((float)(tback->y2-tslide->y1)/(float)(tback->y2-tback->y1)))/ratio;
			//get ratio of 
			//ratio2=
		  //tc->CursorLocationInChars=(int)(ratio*(tc->ilist->num-tc->numvisible));
		  //tc->CurrentItem=(int)(ratio*(tc->ilist->num-tc->numvisible));
			totalentries = tc->ilist->num;
			tc->FirstItemShowing=(int)((1.0-ratio)*totalentries)+1;
			//there is padding of 2 at the top currently
			fh = tc->ilist->fontheight;
			if (fh <= 0) {
				fh = tc->fontheight;
			}
			//todo: add tcontrol->PadY, and/or tlist->PadY because FileOpen does not add PadY
			if (tc->type == CTFileOpen) {
				numentries = (tc->y2 - tc->y1 - 2) / fh;
			}
			else {
				numentries = (tc->y2 - tc->y1) / (fh + PStatus.PadY);
			} 

			if (tc->FirstItemShowing + numentries>=totalentries) { //check for went too far
				tc->FirstItemShowing = totalentries- numentries+1;
			} //if (tc->FirstItemShowing + numentries>=tc->ilist->num)
		  //fprintf(stderr,"ratio=%f tcontrol->CursorLocationInChars=%d\n",ratio,tcontrol->CursorLocationInChars);
		  DrawFTControl(tc);  //this will redraw scroll bar FTControls
		} //ratio<1.0
	} //tcontrol2->ilist!=0

} //CTFileOpen

//	fprintf(stderr,"tcontrol=%s\n",tcontrol->name);

if (tc->type==CTFrame) {
//	fprintf(stderr,"tnum=%d\n",tnum);
	//move all frame controls by same amount as mouse times the ratio of controls to tback
//possibly adapt one of the other Vscroll functions.

	if (tslide==0 || tback==0) {
		return;
	} //tslide==0
//move scroll slider control
  if (tnum>0) {  //going down
    if (tslide->y2+tnum>tback->y2) {
			tnum=tback->y2-tslide->y2;//mouse move larger than available space
      tslide->y2=tback->y2;
      tslide->y1=tback->y2-bh;
    }
    else {
      tslide->y1+=tnum;
      tslide->y2+=tnum;
      //fprintf(stderr,"added tcontrol->y1=%d\n",tcontrol->y1);
    }
  } else {  //going up
    if (tslide->y1+tnum<tback->y1) { 
			tnum=tslide->y1-tback->y1;//mouse move larger than available space
      tslide->y1=tback->y1;
      tslide->y2=tback->y1+bh;
    }
    else {
      tslide->y1+=tnum;
      tslide->y2+=tnum;
    } //tslide->y1+
  } //tnum>0

	//find size of controls
	miny=tc->y1;
	maxy=tc->y2;
	tcontrol3=twin->icontrol;
	while(tcontrol3!=0) {
		if (tcontrol3->FrameFTControl==tc && !(tcontrol3->flags&CNotVisible)) {
//EraseFTControl?
			tlabel=tcontrol3->DataLabelFTControl;
			if (tlabel!=0) {
				if (!(tlabel->flags2&CDoNotScrollY)) {
					if (tcontrol3->y1<miny) {
						miny=tcontrol3->y1;
					}
					if (tcontrol3->y2>maxy) {
						maxy=tcontrol3->y2;
					}
				} //tlabel-?flags2
			} //CDoNotScrollY
		} //if tcontrol3->FrameFTControl			
	tcontrol3=tcontrol3->next;
	} //while(tcontrol3!=0)
	
	//find ration of controls to tback
	ratio=(float)(maxy-miny)/(float)(tback->y2-tback->y1);

	//scroll any controls by the same amount
	tcontrol3=twin->icontrol;
	while(tcontrol3!=0) {
		if (tcontrol3->FrameFTControl==tc && !(tcontrol3->flags&CNotVisible)) {
//EraseFTControl?

			if (!(tcontrol3->flags2&CDoNotScrollY)) {
				tcontrol3->y1-=(int)((float)tnum*ratio);
				tcontrol3->y2-=(int)((float)tnum*ratio);
	
				//also scroll any connected label control
				if (tcontrol3->DataLabelFTControl!=0) {
					tlabel=tcontrol3->DataLabelFTControl;
					if (tlabel!=0) {
							if (!(tlabel->flags2&CDoNotScrollY)) {
							tlabel->y1-=(int)((float)tnum*ratio);
							tlabel->y2-=(int)((float)tnum*ratio);
							} //tlabel->flags
						} //tlabel!=0
				} //tcontrol3

			} //tcontrol->flags2&CDoNotScrollY

		} //if tcontrol3->FrameFTControl			
	tcontrol3=tcontrol3->next;
	} //while(tcontrol3!=0)

	if (tnum!=0) {
		DrawFTControl(tc); //redraw frame controls
	}
} //CTFrame	


//UpdateScrollSlide(tcontrol2);

//}  //end if button down within middle button

//twin=GetFTWindow("openproject");
//have to put back focus on sliding button, scroll button puts back on fileopen FTControl
//twin->focus=tcontrol->tab;
//fprintf(stderr,"Set focus1\n");
FT_SetFocus(twin,tc);

//#endif
} //if (tc!=0) {

} //end VScrollSlide_Button0DownMouseMove


void CalcVScrollSlide(FTWindow *twin,FTControl *tcontrol)
{
//#define MIN_VSCROLL_SLIDE_HEIGHT 10

float pos,ratio;
FTControl *tcontrol2,*tslide,*tback;
//char tstr[FTMedStr];
int miny,maxy;//,ipos;
int slideheight,minslideheight;

//fprintf(stderr,"UpdateVScrollSlide\n");

//should be made into 2 functions
//CalcScrollSlideSizeAndPosition() called when fileopen FTItemList is refreshed
//UpdateVScrollSlide() called whenever anything but a drag moves the scroll slide

//UpdateVScrollSlide draws the scroll slide button from i[0]
//

//tcontrol=control vertical slide scrollbar button is connected to fileopen,frame,etc
//tslide=slide
//tback=back
//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
//tslide=GetFTControl(tstr);
tslide=tcontrol->FTC_VScrollSlide;
//tcontrol=GetFTControl("foOpenFile2");
//tcontrol2=GetFTControl("sb_foOpenFile2");
//sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
//tback=GetFTControl(tstr);
tback=tcontrol->FTC_VScrollBack;
//tcontrol3=GetFTControl("sm_foOpenFile2");


if (tcontrol->type==CTFileOpen || tcontrol->type==CTItemList) {
	if (tcontrol->ilist!=0) {
		pos=(float)(tcontrol->FirstItemShowing-1)/(float)tcontrol->ilist->num;
	//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
	//position may change depending on dragging scroll bar sliding button
		tslide->y1=tback->y1+(int)(((float)(tback->y2-tback->y1))*pos);

	//now calculate y2 (size) of scroll bar button
		ratio=(float)tcontrol->numvisible/(float)tcontrol->ilist->num;
	  if (ratio>1.0) {
//      fprintf(stderr,"here0\n");
		//slider is full size, same as back
	    tslide->y1=tback->y1;
	    tslide->y2=tback->y2;
	  }  else {
		  //slider is not full size
//      fprintf(stderr,"here1\n");
//      pos=(float)tcontrol->CursorLocationInChars/(float)tcontrol->ilist->num;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
//      tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;
	    tslide->y2=tslide->y1+((float)(tback->y2-tback->y1))*ratio;//-1;//+1; //+1
//    fprintf(stderr,"y1=%d y2=%d\n",tcontrol2->y1,tcontrol2->y2);
//    fprintf(stderr,"numvis=%d num=%d\n",tcontrol->numvisible,tcontrol->ilist->num);
/*
//keep slider from getting too small:
		if (tslide->y2 - tslide->y1 < MIN_VSCROLL_SLIDE_HEIGHT) {
			tslide->y2 = tslide->y1 + MIN_VSCROLL_SLIDE_HEIGHT;
			if (tslide->y2 > tback->y2) {
				tslide->y2 = tback->y2;
				tslide->y1 = tslide->y2 - MIN_VSCROLL_SLIDE_HEIGHT;
				if (tslide->y1 < tback->y1) {
					tslide->y1 = tback->y1;
				}
			}
		}
*/

	  }  //if ratio else

	  DrawFTControl(tback);
	  DrawFTControl(tslide);
	}  //if ilist!=0
} //if (tcontrol->type==CTFileOpen || tcontrol->type==CTItemList) {

if (tcontrol->type==CTFrame) {
	//calculate position and size (y1,y2) of slide button
	//go through controls connected to frame and get miny and maxy
	//perhaps there is a faster method
	tcontrol2=twin->icontrol;
	miny=tcontrol->y1;
	maxy=tcontrol->y2;
	while(tcontrol2!=0) {
		
		if (tcontrol2->FrameFTControl==tcontrol) {
			if (tcontrol2->y1<miny) {
				miny=tcontrol2->y1;
			} //y1<miny
			if (tcontrol2->y2>maxy) {
				maxy=tcontrol2->y2;
			} //y1<miny
		}	//FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0

//	minimum unit=PStatus.ScrollPixelsY
	ratio=(float)(tcontrol->y2-tcontrol->y1)/(float)(maxy-miny);
	if (ratio>=1.0) { //control is bigger or equal to miny to maxy
		tslide->y1=tback->y1;
		tslide->y2=tback->y2;
	} else {
		pos=tback->y1+(int)((float)(tcontrol->y1-miny)*ratio);//PStatus.ScrollPixelsY;
		slideheight=(int)(ratio*(float)(tback->y2-tback->y1));
		minslideheight=(tback->y2-tback->y1)/20;
		if (slideheight<minslideheight) {
			slideheight=minslideheight;
		}
		tslide->y1=(int)pos;
		tslide->y2=(int)pos+slideheight;
		if (tslide->y2>tback->y2) { //adjust in case of minslideheight
			tslide->y2=tback->y2;
		}
	}

 	DrawFTControl(tback);
	DrawFTControl(tslide);
} //CTFrame

	
//fprintf(stderr,"End UpdateScrollSlide\n");
}  //CalcVScrollSlide


//update scroll bar button
//only for fileopen
void UpdateVScrollSlide(FTControl *tcontrol)
{
	float pos;//,ratio;
	FTControl *tslide,*tback;//*tcontrol2,*tcontrol3;
	//char tstr[FTMedStr];
	int bh;
	FTItemList *tlist;

	//fprintf(stderr,"UpdateVScrollSlide\n");

	//should be made into 2 functions
	//CalcScrollSlideSize() called when fileopen FTItemList is refreshed
	//UpdateVScrollSlide() called whenever anything but a drag moves the scroll slide

	//UpdateVScrollSlide draws the scroll slide button from i[0]
	//

	//tcontrol=fileopen
	//tcontrol2=slide
	//tcontrol3=back
	//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
	//tcontrol2=GetFTControl(tstr);
	tslide=tcontrol->FTC_VScrollSlide;
	bh=tslide->y2-tslide->y1;
	//sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
	//tcontrol3=GetFTControl(tstr);
	tback=tcontrol->FTC_VScrollBack;
	tlist=tcontrol->ilist;

	if (tlist!=0) {
	  //pos=(float)tcontrol->CursorLocationInChars/(float)tlist->num;
		pos=(float)(tcontrol->FirstItemShowing-1)/(float)tlist->num;
	//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
	//position may change depending on dragging scroll bar sliding button
		tslide->y1=tback->y1+((float)(tback->y2-tback->y1))*pos;
		tslide->y2=tslide->y1+bh;
		DrawFTControl(tback);  //back
		DrawFTControl(tslide);  //slide
	  }  //if ilist!=0

	//fprintf(stderr,"End UpdateVScrollSlide\n");

}  //UpdateVScrollSlide


//FT_TextAreaPgUp
//MoveCursor: 0=no 1=yes (cursor is i[0], top of page is i[1])
void FT_TextAreaPgUp(FTControl *tcontrol,int MoveCursor) 
{
int i,j,maxheight,maxwidth,ix,cx,ExitLoop;//,redraw;
char *tmpstr;

	//redraw=0;
	maxheight=(int)((tcontrol->y2-tcontrol->y1)/tcontrol->fontheight); 
	maxwidth=(int)((tcontrol->x2-tcontrol->x1)/tcontrol->fontwidth); 


//todo: move to the shift key press code
	//this might be the first key after the shift is pressed - porbably this should be done on the shift key
	if (MoveCursor && (PStatus.flags&PShift)) {
//this may be the first move with the shift key down
		 if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
		   tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		   tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		 }
	} //if (MoveCursor && (PStatus.flags&PShift)) {



	if (!MoveCursor) {
		//go through a page of text from the first character showing until a page of columns has been passed
		//the complexity arises because we can't simply just add up characters that = 10, because text could be wrapped
		i=tcontrol->FirstCharacterShowing;  //because pgup can be scrolling without moving the cursor
		j=0; //count of rows = maxheight 
		//we presume the first character showing is in column 0
		ix=0;
		tmpstr=tcontrol->text;
		ExitLoop=0;
		while(i>0 && !ExitLoop) {
			if (ix==-1) { //cursor is at left of control
				j++; //count new line
				if (j==maxheight-1) { //reached full page in columns
					ExitLoop=1; //exit loop
					ix=0;
				} else {
					ix=maxwidth; //presume maxwidth and it will be adjusted after
					if (tmpstr[i]==10) { //if this character is a new line, don't count another line
						//and skip past it
						i--; //move current character back 1
						ix--;//move cursor x position left 1
					} else {
						//otherwise
						i++; //process this character normally
					} //if (tcontrol->text[i]==10)
				} //if (j==maxheight-1) { //reached full page in columns
			} else { //if (ix==0) { //reached left of control
				//didn't reach left of control check for a carriage return
				if (tmpstr[i]==10) {
					ix=maxwidth+1; //reset position of cursor, +1 to compensate for next ix--
					j++;			
					if (j==maxheight-1) {  //-1 because a full page minus one line is moved
						ExitLoop=1;
						ix=1; //found chr(10) so ix must = 0
					} //j==maxheight-1

					//if (ix==0) { //reset cursor position to maxwidth
					//	ix=maxwidth+1; //+1 to compensate for next ix--
					//}
				} //text[i]==10
				i--;
				ix--;
			} //if (ix==0) { //reached left of control
		}  //while i<=tlen

			//now move to the first character in the row which is the first character showing
		while(ix>0 && i>0 && tmpstr[i]!=10) {
			i--;
			ix--;
		}

		if (i>0 && tmpstr[i]==10) { //found carriage return
			i++; //move forward 1 character
		} //if (i>0 && tp[i]==10) { //found carriage return
		tcontrol->FirstCharacterShowing=i;  //set the first character showing
	} //if (!MoveCursor) {


	if (MoveCursor) {
		//Move the Cursor up a page, scrolling the text down if necessary
		i=tcontrol->CursorLocationInChars;
		ix=tcontrol->CursorX;
		tmpstr=tcontrol->text;
		j=0; //numrows moved
		
		ExitLoop=0;
		i-=ix; //move back to start of row
		while(i>0 && !ExitLoop) {
			//get num of chars in the previous row
			cx=GetNumCharsInRowFromEnd(tmpstr,i-1,maxwidth);
			i-=cx; //move to the start of the previous row 
			if (i==0) { //start of text
				//ExitLoop=1;  //i==0 will cause loop to exit anyway
				tcontrol->CursorX=0; //moves to 0 because is not a full page
			} else { //if (i==0) { //start of text
				j++; //increment the column count
				if (j==maxheight-1) {  //reached last row to page back
					ExitLoop=1;
					//move forward to the cursor position if possible
					if (cx-1<ix) { //not enough chars
						i+=(cx-1);
						tcontrol->CursorX=cx-1;
					} else {
						//CursorX stays the same
						i+=ix; //move to CursorX
					}
				} //if (j==maxheight-1) {  //reached last row to page back
				//else move up another row
			} //if (i==0) { //start of text

		} //while(!ExitLoop) {
		tcontrol->CursorLocationInChars=i;

		//scroll text down so cursor is visible in the textarea control
		//scroll until beginning of text or j is reached
		i=tcontrol->FirstCharacterShowing;
		if (i>0) { //need to scroll
			while(j>0) {
				//get the number of chars in each row and move forward by that many chars
				cx=GetNumCharsInRowFromEnd(tmpstr,tcontrol->FirstCharacterShowing-1,maxwidth);
				i-=cx;
				j--;
				if (i==0) {
					tcontrol->CursorY-=j;
					j=0; //exit while
				} //if (i==0) { 
			} //while
			tcontrol->FirstCharacterShowing=i;
		} //if (i>0) { //need to scroll
#if 0 
		if (j<maxheight-1) { //didn't move a full page
			//only scroll until the first row is reached
			if ((tcontrol->CursorY-j)<0) {  //moved more rows than CursorY
				//there is text above				
				//only scroll until the first row of text is on the top row of the textarea control
				for(i=0;i<j-tcontrol->CursorY;i++) { 
					//get the number of chars in each row and move forward by that many chars
					cx=GetNumCharsInRowFromEnd(tmpstr,tcontrol->FirstCharacterShowing-1,maxwidth);
					tcontrol->FirstCharacterShowing-=cx;
				} //for i
				tcontrol->CursorY=j-tcontrol->CursorY;
			} else { //if ((tcontrol->CursorY-j)<0) {  //moved more rows than CursorY
				//there is no text above the control and no need to scroll
				tcontrol->CursorY-=j; //subtract num of rows moved up from CursorY
			} //if ((tcontrol->CursorY-j)<0) {  //moved more rows than CursorY
			
		} else { //if (j<maxheight-1) { //didn't move a full page

			//moved a full page- scroll a full page up
			for(i=0;i<j;i++) {
				//get the number of chars in each previous row and move backward by that many chars
				cx=GetNumCharsInRowFromEnd(tmpstr,tcontrol->FirstCharacterShowing-1,maxwidth);
				tcontrol->FirstCharacterShowing-=cx;
			} //for i

			//CursorY stays the same

		} //if (j<maxheight-1) { //didn't move a full page
#endif

#if 0 
	if (MoveCursor) { 
		//just move the cursor to the same column and row, and adjust row if needed
		//count cursor location from FirstCharacterShowing
		i=tcontrol->FirstCharacterShowing;
		j=0; //first column
		ix=0; //current cursor X location
		//while not end of text, still haven't reached CursorY, and current cursor column is less than CursorX
		while(tp[i]!=0 && j<tcontrol->CursorY && ix<tcontrol->CursorX) {
			if (i==10) {  //newline
				j++;
				ix=0;
				i++; 
			} else {
				i++; //move to next character
				ix++;
				if (ix==maxwidth) {  //word wrap to next column
					j++;
					ix=0;
				}
			} //i==10
			
		} //while
		tcontrol->CursorX=ix; //update CursorX  (could be moved back)
		tcontrol->CursorLocationInChars=i;
#endif

		//selecting text on a PGUP
		if (PStatus.flags&PShift) {
			if (tcontrol->CursorLocationInChars<=tcontrol->StartSelectedTextInChars) {
				tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars; 
			} else {
				tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars; 
			}
		} //if (PStatus.flags&PShift) {

		//redraw=1;

	} //	if (MoveCursor) { 
	



	//if (redraw) {
		DrawFTControl(tcontrol);
	//} //if (redraw) {

} //FT_TextAreaPgUp



//FT_TextAreaPgDn
//MoveCursor: 0=no 1=yes (cursor is i[0], top of page is i[1])
void FT_TextAreaPgDown(FTControl *tcontrol,int MoveCursor) 
{
int i,j,maxwidth,maxheight,ix,cx,ExitLoop;
char *tmpstr;

	//redraw=0;
	maxheight=(int)((tcontrol->y2-tcontrol->y1)/tcontrol->fontheight); 
	maxwidth=((tcontrol->x2-tcontrol->x1)/tcontrol->fontwidth)-1; 

//todo: move to shift keypress code
	//this might be first key after shift
	if (PStatus.flags&PShift) {

			 if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
			   tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
			   tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
			   //fprintf(stderr,"2==3\n");
			 }

	} //PShift


	//if (MoveCursor) {
	//	oix=GetCursorXFromLocationInChars(tcontrol); //get Cursor X position before below code changes it
	//} 

	if (!MoveCursor) { //not moving cursor, only moving textarea text down a line, for example with middle mouse wheel
		//move the cursor location ahead a page
		//tlen=strlen(tcontrol->text);
		//i=tcontrol->CursorLocationInChars;
		i=tcontrol->FirstCharacterShowing;  //because pgdn can be scrolling without moving the cursor
		j=0;
		//the first character showing is in column 0 
		ix=0;
		tmpstr=tcontrol->text;
		while(j<maxheight-1 && tmpstr[i]!=0) {
			if (tmpstr[i]==10 || ix==maxwidth) {		//carraige return or maximum X reached
				j++;  //number of columns moved
				ix=-1; //set column=0
			} 
			ix++;
			i++;
		}  //while i<=tlen
		//move to column position 0
		while(ix>0) {
			ix--;
			i--;
		} //while(ix>0) {
		tcontrol->FirstCharacterShowing=i;  //set the first character showing
	} //if (!MoveCursor)

	if (MoveCursor) { 
		//Move the Cursor down a page, scrolling the text up if necessary

		i=tcontrol->CursorLocationInChars;
		ix=tcontrol->CursorX;
		tmpstr=tcontrol->text;
		j=0; //numrows moved
		
		ExitLoop=0;
		i-=ix; //move back to start of row
		while(!ExitLoop) {
			//get num of chars in the current row
			cx=GetNumCharsInRowFromStart(tmpstr,i,maxwidth);
			i+=cx; //move to the end of the row (or next row if not the end of the text)
			if (tmpstr[i]==0) { //end of text
				tcontrol->CursorX=cx-1;
				ExitLoop=1;
			} else { //if (tmpstr[i]==0) {
				j++; //increment the column count
				if (j==maxheight-1) {  //reached last row to page forward
					ExitLoop=1;
					//move forward to the cursor position if possible
					cx=GetNumCharsInRowFromStart(tmpstr,i,maxwidth);
					if (cx-1<ix) { //not enough chars
						i+=cx;
						tcontrol->CursorX=cx-1;
					} else {
						//CursorX stays the same
						i+=ix; //move to CursorX
					}
				} //else move down another row
			} //if (tmpstr[i]==0) {

		} //while
		tcontrol->CursorLocationInChars=i;

		//scroll text up so cursor is visible in the textarea control
		if (j<maxheight-1) { //didn't move a full page
			//only scroll until the end of text is reached (this puts the cursor on the bottom row if text is below the control)
			if ((tcontrol->CursorY+j)>maxheight-1) {
				//there is text below				
				for(i=0;i<(j+tcontrol->CursorY)-(maxheight-1);i++) { //only scroll until the last row of text is on the bottom row of the textarea control
					//get the number of chars in each row and move forward by that many chars
					cx=GetNumCharsInRowFromStart(tmpstr,tcontrol->FirstCharacterShowing,maxwidth);
					tcontrol->FirstCharacterShowing+=cx;
				} //for i
				tcontrol->CursorY=maxheight-1;
			} else { //if ((tcontrol->CursorY+j)>maxheight-1) {
				//there is no text below the control and no need to scroll
				tcontrol->CursorY+=j; //add num of rows moved down to CursorY
			} //if ((tcontrol->CursorY+j)>maxheight-1) {
			
		} else { //if (j<maxheight-1) { //didn't move a full page

			//moved a full page- scroll a full page down
			for(i=0;i<j;i++) {
				//get the number of chars in each row and move forward by that many chars
				cx=GetNumCharsInRowFromStart(tmpstr,tcontrol->FirstCharacterShowing,maxwidth);
				tcontrol->FirstCharacterShowing+=cx;
			} //for i

			//CursorY stays the same

		} //if (j<maxheight-1) { //didn't move a full page

		//selecting on a PGDN
		if (PStatus.flags&PShift) {
			if (tcontrol->CursorLocationInChars<=tcontrol->StartSelectedTextInChars) {
				tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars; 
			} else {
				tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars; 
			}
		} //if (PStatus.flags&PShift) {
	} //if (MoveCursor) {

	//if (redraw) {
		DrawFTControl(tcontrol);
	//} //if (redraw) {

} //FT_TextAreaPgDown


void FT_TextAreaUp(FTControl *tcontrol,int MoveCursor) 
{
int cx,i,ix,maxwidth,ExitLoop;
char *tmpstr;

	if	(MoveCursor) {
		//set select area at current cursor if not already defined
		 if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
		   tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
		   tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
		   //fprintf(stderr,"2==3\n");
		 } 
	} //if	(MoveCursor) {


	if (!MoveCursor) { //if not moving cursor then just move textarea up by a line (mouse scroll button)
		i=tcontrol->FirstCharacterShowing;
		if (i>0) { //we can scroll up a line
			tmpstr=tcontrol->text;
			maxwidth=(int)(((tcontrol->x2-tcontrol->x1)/tcontrol->fontwidth))-1;
			ix=maxwidth;
			ExitLoop=0;
			while(!ExitLoop) { //go to first character - until chr(10) or word wrap
				if (i==0) { 
					ExitLoop=1; //reached start of text
				} else {
					if (tmpstr[i]==10 && i!=tcontrol->FirstCharacterShowing) { //not first chr(10)
						ExitLoop=1; //reached first column
						i++; //move to next char
					} else { //didn't reach chr(10)
						i--;  //move back a character
						ix--; //keep track for word wrap
						if (ix==0) {
							ExitLoop=1;
						} //ix==0
					}
				} //i==0
			} //while
			tcontrol->FirstCharacterShowing=i;
		} //i>0
	} //	if (!MoveCursor) { //if not moving cursor then just move textarea up by a line (mouse scroll button)


	if	(MoveCursor) {
		//only moving cursor (not using mouse scroll wheel)
		
		tmpstr=tcontrol->text;
		i=tcontrol->CursorLocationInChars;
		ix=tcontrol->CursorX;
		maxwidth=(int)(((tcontrol->x2-tcontrol->x1)/tcontrol->fontwidth))-1;

		//move cursor back to the start of the line
		i-=ix;
		if (i>0) { //this is not the first line
			if (tcontrol->CursorY==0) { //scroll text up
				cx=GetNumCharsInRowFromEnd(tmpstr,tcontrol->FirstCharacterShowing-1,maxwidth);  //-1 to move to previous line
				tcontrol->FirstCharacterShowing-=cx;
			} else { //if (tcontrol->CursorY==0) { //scroll text up
				tcontrol->CursorY--;
			} //if (tcontrol->CursorY==0) { //scroll text up
			cx=GetNumCharsInRowFromEnd(tmpstr,i-1,maxwidth);  //-1 to move back 1 more character
			if (cx-1<ix) { //less characters in this line than CursorX
				tcontrol->CursorX=cx-1;  //put cursor at the end of the row
				tcontrol->CursorLocationInChars=i-1; //one character back to the end of the previous line
			} else {
				//CursorX stays the same
				tcontrol->CursorLocationInChars=i-cx+ix;
			}
		} else { //if (i>0) { //this is not the first line
			//this is the first line
			tcontrol->CursorX=0;
			tcontrol->CursorLocationInChars=0;			
		} //if (i>0) { //this is not the first line

/*
			if (tcontrol->CursorY>0 || tcontrol->FirstCharacterShowing>0) {
			//determine new CursorX and CursorLocationInChars

			//move back CursorX characters then -1 to previous line
			//then move back to the start of the previous line counting characters (until a second chr(10) or maxwidth)
			//then move forward the lesser of CursorX or counted characters
			tmpstr=tcontrol->text;
			i=tcontrol->CursorLocationInChars;

			ix=tcontrol->CursorX;
			cx=0; //counted characters


			//move back CursorX characters to first char of the line
			i-=tcontrol->CursorX;
			
			//then move back to the start of the previous line counting characters (until a second chr(10) or maxwidth)
			cx=GetNumCharsInRowFromEnd(tmpstr,i-1,maxwidth);  //-1 to move back 1 more character
			if (tcontrol->CursorY>0) {
				tcontrol->CursorY--;
			} else { 
				//CursorY==0 - scrolled text
				tcontrol->FirstCharacterShowing-=cx;  //move the number of characters in the line above
			}
			if (cx-1<tcontrol->CursorX) {   //-1 because cx is character count, so we want CursorX=0 for cx=1 character
				tcontrol->CursorX=cx-1;
			} //otherwise CursorX stays the same
				
			//new cursor location equals start of current line-number of characters in previous line plus position of new CursorX	
			tcontrol->CursorLocationInChars=i-cx+tcontrol->CursorX; 

		} //if (tcontrol->CursorY>0 || tcontrol->FirstCharacterShowing>0) {
*/
	//selecting text with the shift key
		if (PStatus.flags&PShift) {
			if (tcontrol->CursorLocationInChars<=tcontrol->StartSelectedTextInChars) {
				tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;						
			} else {
				tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;						
			} //[2]>[3]
		} //if (PStatus.flags&PShift) {


	} //MoveCursor


} //void FT_TextAreaUp(FTControl *tcontrol) 


//FT_TextAreaDown - down arrow key pressed in a TextArea control
void FT_TextAreaDown(FTControl *tcontrol,int MoveCursor) 
{
//int tlen,cx,cy,cx2,cy2,i,fh,maxheight,maxwidth;
	int tlen,cx,i,fh,maxheight,maxwidth,ix;
	char *tmpstr;


//fprintf(stderr,"FT_TextAreaDown\n");
	fh=tcontrol->fontheight;

	maxheight=(int)((tcontrol->y2-tcontrol->y1)/fh); 
	maxwidth=(int)((tcontrol->x2-tcontrol->x1)/tcontrol->fontwidth)-1;

	 //move text in textarea down by 1
	if (MoveCursor) {

		//CTTextArea - down arrow may move down a row
		if (tcontrol->StartSelectedTextInChars==tcontrol->EndSelectedTextInChars) {
			tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;
			tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;
			//fprintf(stderr,"2==3\n");
		}


		//move down until CursorX or new line
		i=tcontrol->CursorLocationInChars;
		ix=tcontrol->CursorX;
		tmpstr=tcontrol->text;
		//get characters in this row
		cx=GetNumCharsInRowFromStart(tmpstr,i-ix,maxwidth);
		//move to the next row
		i+=(cx-ix); //move: num characters in row - CursorX
		if (tmpstr[i]!=0) { //not end of text
			if (tcontrol->CursorY==maxheight-1) {
				//scroll text up a row instead of increasing CursorY
				cx=GetNumCharsInRowFromStart(tmpstr,tcontrol->FirstCharacterShowing,maxwidth);
				tcontrol->FirstCharacterShowing+=cx; //move FirstCharacterShowing to first character of next row
			} else {
				tcontrol->CursorY++; //move CursorY down one row
			}
			//get characters in this next row
			cx=GetNumCharsInRowFromStart(tmpstr,i,maxwidth);
			if (cx<ix) {
				tcontrol->CursorX=cx-1;  //not enough characters so move back to end of row
				tcontrol->CursorLocationInChars=i+tcontrol->CursorX;
			} else {
				//CursorX doesn't change
				tcontrol->CursorLocationInChars=i+ix; //add previous CursorX to CursorLocationInChars
			}
		} else {//if (tmpstr[i]!+0) { //not end of text
			tcontrol->CursorX=cx-1;
			tcontrol->CursorLocationInChars=i; //go to the end of the row (but not down a row- since this is the end of the text)
		} //if (tmpstr[i]!+0) { //not end of text
/*
		ExitLoop=0;
		while(!ExitLoop && tmpstr[i]!=0) { //while no exit and not end of string
			if (ix==tcontrol->CursorX && i!=tcontrol->CursorLocationInChars) { //reached same CursorX, exit
				ExitLoop=1;
			} else {
				if (tmpstr[i]==10 || ix==maxwidth) { //reached cr or maxwidth
					ix=-1; //ix++ happens after this
					if (tcontrol->CursorY==maxheight-1) {  //last row
						//need to scroll FirstCharacterShowing a line
						cx=GetNumCharsInRowFromStart(tmpstr,tcontrol->FirstCharacterShowing,maxwidth);
						tcontrol->FirstCharacterShowing+=cx;
					} else { //if (tcontrol->CursorY==maxheight-1) {
						tcontrol->CursorY++;
					} //if (tcontrol->CursorY==maxheight-1) {
				}  //if (tmpstr[i]==10) {
				i++; //move to the next characterl
				ix++; //move Cursor X position forward 1 too
			} //if (ix==tcontrol->CursorX && i!=tcontrol->CursorLocationInChars) { //reached same CursorX, exit
		} //while
		tcontrol->CursorLocationInChars=i;
		tcontrol->CursorX=ix;
*/

		
		
		//move bottom of selection if selecting text with shift key
		if (PStatus.flags&PShift) {
			//if (tcontrol->CursorLocationInChars>tcontrol->i[5]) tcontrol->i[5]=tcontrol->CursorLocationInChars;
			//else tcontrol->i[4]=tcontrol->CursorLocationInChars;
			if (tcontrol->CursorLocationInChars>tcontrol->EndSelectedTextInChars) {
				tcontrol->EndSelectedTextInChars=tcontrol->CursorLocationInChars;						
			} else {
				tcontrol->StartSelectedTextInChars=tcontrol->CursorLocationInChars;						
			} //[0]>[3]
		} //PShift

		
	 } else {  //if (MoveCursor)
		 //if not moving the cursor then this function only scrolls text down 1 row
		i=tcontrol->FirstCharacterShowing;
		tlen=strlen(tcontrol->text);
		tmpstr=tcontrol->text;
		if (i<tlen) {
			cx=0;
			while(tmpstr[i]!=10 && cx<maxwidth && i<tlen) {  //while there is no carriage return or the maximum number of characters has been reached
				i++;
				cx++;
			}
			tcontrol->FirstCharacterShowing=i;  //move the FirstCharacterShowing down a line
		 } //if (i<tlen) {

	 } //if (MoveCursor)


} //void FT_TextAreaDown(FTControl *tcontrol) 


//button down on scrollbar down button connected to frame control - move controls on frame up
void FT_FrameScrollDown(FTWindow *twin,FTControl *tcontrol,int Page)
{
FTControl *tcontrol2,*tlabel;
long long maxy; //need long long for negative tcontrol->y comparison
int numpix;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_FrameScrollDown\n");
}
//scroll all controls on this frame up if still controls hidden below frame
//go through all controls on this window, for those with this FrameFTControl, find maxy
//if maxy>tcontrol->y2 - scroll all controls
maxy=tcontrol->y2;
tcontrol2=twin->icontrol;
while(tcontrol2!=0) {
	if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollY)) {
		if (tcontrol2->y2>maxy) {
			maxy=tcontrol2->y2;
		}
	} //FrameFTControl==tcontrol
tcontrol2=tcontrol2->next;
} //tcontrol2!=0

if (maxy>tcontrol->y2) {  //at least one control is lower than frame control - scroll all frame controls up
//fprintf(stderr,"maxy=%lli y2=%d\n",maxy,tcontrol->y2);
	if (Page) { //scroll page
		numpix=tcontrol->y2-tcontrol->y1;
	} else { //only scroll line
//		numpix=5;
		numpix=PStatus.ScrollPixelsY;
	}
	if (maxy-numpix<tcontrol->y2) {
		numpix=maxy-tcontrol->y2;
	}
	tcontrol2=twin->icontrol;
	while(tcontrol2!=0) {
		if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollY)) {
//EraseFTControl?
			tcontrol2->y1-=numpix;
			tcontrol2->y2-=numpix;
			
			//also scroll any connected label control
			if (tcontrol2->DataLabelFTControl!=0) {
				tlabel=tcontrol2->DataLabelFTControl;
				if (!(tlabel->flags2&CDoNotScrollY)) {
					tlabel->y1-=numpix;
					tlabel->y2-=numpix;
					DrawFTControl(tlabel);
				}
			}


		} //FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0
DrawFTControl(tcontrol); //redraw frame (also redraws DataLabel controls)
} //maxy>tcontrol->y2

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_FrameScrollDown\n");
}

}//FT_FrameScrollDown(tcontrol3)


//button down on scrollbar up button connected to frame control - move controls on frame down
void FT_FrameScrollUp(FTWindow *twin,FTControl *tcontrol,int Page)
{
FTControl *tcontrol2,*tlabel;
long long miny;
int numpix;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_FrameScrollUp\n");
}

//scroll all controls on this frame up if still controls hidden below frame
//go through all controls on this window, for those with this FrameFTControl, find maxy
//if maxy>tcontrol->y2 - scroll all controls
miny=tcontrol->y1;
tcontrol2=twin->icontrol;
while(tcontrol2!=0) {
	if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollY)) {
		if (tcontrol2->y1<miny) {
			miny=(long long)tcontrol2->y1;
		}
	} //FrameFTControl==tcontrol
tcontrol2=tcontrol2->next;
} //tcontrol2!=0

if (miny<tcontrol->y1) {  //at least one control is higher than frame control - scroll all frame controls down
	if (Page) { //scroll page
		numpix=tcontrol->y2-tcontrol->y1;
	} else {	//scroll line
	//	numpix=5;
		numpix=PStatus.ScrollPixelsY;
	}
	if (miny+numpix>tcontrol->y1) {
		numpix=tcontrol->y1-miny;
	}
	tcontrol2=twin->icontrol;
	while(tcontrol2!=0) {
		if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollY)) {
//EraseFTControl?
			tcontrol2->y1+=numpix;
			tcontrol2->y2+=numpix;

			//also scroll any connected label control
			if (tcontrol2->DataLabelFTControl!=0) {
				tlabel=tcontrol2->DataLabelFTControl;
				if (!(tlabel->flags2&CDoNotScrollY)) {
					tlabel->y1+=numpix;
					tlabel->y2+=numpix;
					DrawFTControl(tlabel);
				}
			}


		} //FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0
DrawFTControl(tcontrol); //redraw frame
} //maxy>tcontrol->y2

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_FrameScrollUp\n");
}

}//FT_FrameScrollUp(tcontrol3)


//END VERTICAL SCROLLBAR FUNCTIONS

//HORIZONTAL SCROLL BAR FUNCTIONS
void HScrollLeft_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
{
	//int fw;
	FTItemList *tlist;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"HScrollLeft_ButtonDown\n");
}

tcontrol=tcontrol->parent;//GetFTControl("foOpenFile2");

if (tcontrol->type==CTItemList) { //scrollbar connected to an itemlist control
	tlist=tcontrol->ilist;
	if (tlist!=0) {
		//fw=tlist->fontwidth;
		if (tcontrol->FirstItemCharacterShowing>0) { //note FirstItemShowing starts with 1, but FirstItemCharacterShowing starts with 0
			tcontrol->FirstItemCharacterShowing--;
			CalcHScrollSlide(twin,tcontrol);
			DrawFTControl(tcontrol);
		}
	} //if (tlist!=0) {
} //CTItemList

if (tcontrol->type==CTFrame) { //scrollbar connected to frame
//perhaps should pass twin?
	FT_FrameScrollLeft(twin,tcontrol,0); //moves all controls on frame up 0=only scroll 1 line
} //CTFrame

FT_SetFocus(twin,tcontrol);

}  //HScrollLeft_ButtonDown

void HScrollRight_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
{
	int fw;
	FTItemList *tlist;

if (PStatus.flags&PInfo) {
	fprintf(stderr,"HScrollRight_ButtonDown\n");
}

tcontrol=tcontrol->parent;//GetFTControl("foOpenFile2");

if (tcontrol->type==CTItemList) { //scrollbar connected to an itemlist control
	tlist=tcontrol->ilist;
	if (tlist!=0) {
		fw=tlist->fontwidth;
		if ((tcontrol->FirstItemCharacterShowing*fw+tcontrol->x2-tcontrol->x1)<=tcontrol->ilist->width) {
			tcontrol->FirstItemCharacterShowing++;
			CalcHScrollSlide(twin,tcontrol);
			DrawFTControl(tcontrol);
		}
	} //if (tlist!=0) {
} //CTItemList


if (tcontrol->type==CTFrame) { //scrollbar connected to frame
//perhaps should pass twin?
	FT_FrameScrollRight(twin,tcontrol,0); //moves all controls on frame up 0=only scroll 1 line
} //CTFrame

FT_SetFocus(twin,tcontrol);

//if (PStatus.flags&PInfo) {
//	fprintf(stderr,"End ScrollRight_ButtonDown\n");
//}

} //HScrollRight_ButtonDown



void HScrollBack_ButtonDown(FTWindow *twin,FTControl *tcontrol,int x,int y,int button)
{
FTControl *tslide,*tparent;
FTItemList *tlist;
//char tstr[FTMedStr];

if (PStatus.flags&PInfo) {
	fprintf(stderr,"HScrollBack_ButtonDown\n");
}


	tparent=tcontrol->parent;
	if (tparent!=0) {
		tslide=tparent->FTC_HScrollSlide;

		if (tparent->type==CTItemList) { //scrollbar connected to ItemList control
			if (x<tslide->x1) {  //scroll page left
				tlist=tparent->ilist;
				if (tlist!=0) {
					if (tparent->FirstItemCharacterShowing-(tparent->x2-tparent->x1)*tlist->fontwidth>0) {
						//can scroll a page left
						tparent->FirstItemCharacterShowing-=(tparent->x2-tparent->x1)/tlist->fontwidth;
						CalcHScrollSlide(twin,tparent);						
						DrawFTControl(tparent); //redraw entire control
					} else {  //not enough space to scroll a page just scroll to end
						//todo: add calculation to see if we don't need to redraw because is already at end
						if (tparent->FirstItemCharacterShowing>0) {
							tparent->FirstItemCharacterShowing=0;
							CalcHScrollSlide(twin,tparent);						
							DrawFTControl(tparent); //redraw entire control
						}
					} //
				} //tparent->ilist!=0
			}  //x<tslide->x1
			if (x>tslide->x2) {  //scroll page right
				tlist=tparent->ilist;
				if (tlist!=0) {
					if (tparent->FirstItemCharacterShowing*tlist->fontwidth+2*(tparent->x2-tparent->x1)<tlist->width) {
						//can scroll a page
						tparent->FirstItemCharacterShowing+=(tparent->x2-tparent->x1)/tlist->fontwidth;
						CalcHScrollSlide(twin,tparent);						
						DrawFTControl(tparent); //redraw entire control
					} else {  //not enough space to scroll a page just scroll to end
						//todo: add calculation to see if we don't need to redraw because is already at end
						tparent->FirstItemCharacterShowing=(tlist->width-(tparent->x2-tparent->x1))/tlist->fontwidth;
						CalcHScrollSlide(twin,tparent);						
						DrawFTControl(tparent); //redraw entire control
					} //
				} //tparent->ilist!=0
			}

		}//CTItemList
		if (tparent->type==CTFrame) { //scrollbar connected to frame
			//get slidebutton
			//sprintf(tstr,"%s%s",PStatus.PrefixHScrollSlide,tparent->name);
			//tslide=GetFTControl(tstr);			
			if (x<tslide->x1) {
				FT_FrameScrollLeft(twin,tparent,1); //1=scroll page
			} 
			if (x>tslide->x2) {
				FT_FrameScrollRight(twin,tparent,1);
			}
		} //CTFrame
	} //tparent!=0

//fprintf(stderr,"scroll fileopen\n");
} //HScrollBack_ButtonDown

//END HORIZONTAL SCROLL BAR FUNCTIONS

//update scroll bar button
//void update_sb_foOpenFile2(void)
void UpdateItemList(FTControl *tcontrol)
{
float pos,ratio;
FTControl *tslide,*tback;
//char tstr[FTMedStr];

//tcontrol=GetFTControl("foOpenFile2");
//sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
tslide=tcontrol->FTC_VScrollSlide;
//sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
tback=tcontrol->FTC_VScrollBack;

//tcontrol2=GetFTControl("sb_foOpenFile2");
//tcontrol3=GetFTControl("sm_foOpenFile2");


if (tcontrol->ilist!=0) {
	pos=(float)tcontrol->FirstItemShowing/(float)tcontrol->ilist->num;
	//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
	//position may change depending on dragging scroll bar sliding button
	tslide->y1=tback->y1+((float)(tback->y2-tback->y1))*pos;

//now calculate y2 (size) of scroll bar button
    ratio=(float)tcontrol->numvisible/(float)tcontrol->ilist->num;
    if (ratio>1.0) {
      tslide->y1=tback->y1;
      tslide->y2=tback->y2;
    } else {
//position may change depending on dragging scroll bar sliding button
		tslide->y2=tslide->y1+((float)(tback->y2-tback->y1))*ratio+1;
//    fprintf(stderr,"y1=%d y2=%d\n",tcontrol2->y1,tcontrol2->y2);
    }  //if ratio else

	DrawFTControl(tback);
	DrawFTControl(tslide);
  }  //if ilist!=0
}  //UpdateItemList



void HScrollSlide_Button0DownMouseMove(FTWindow *twin,FTControl *tcontrol,int x,int y,int custom)
{
FTControl *tc,*tc2,*tback,*tslide,*tTimeLine,*tlabel;
int tnum,bw;
long double ratio;
long long minx,maxx,lx;
char tstr[FTMedStr];

//fprintf(stderr,"foOpenFile2_MouseMove\n");
//fprintf(stderr,"%s\n",tcontrol->name);

//#if 0 


//This function drags scroll bar slide buttons
//it does not determine the scroll bar slide size, that is done in InitScroll
//moving scroll bar slide buttons by not dragging (by keys or scroll buttons) is done in UpdateScrollSlide()

//tcontrol is scroll slide
//tc is FileOpen, Frame

//Get parent control attached to this scroll button or control itself if ItemList control
if (tcontrol->type==CTItemList) {
	tc=tcontrol;
} else {
	tc=tcontrol->parent;
}

//tc=tcontrol->parent;
sprintf(tstr,"%s%s",PStatus.PrefixHScrollBack,tc->name);
tback=tc->FTC_HScrollBack;//GetFTControl(tstr);
tslide=tc->FTC_HScrollSlide;

//fprintf(stderr,"Ly=%d y=%d\n",PStatus.LastMouseY,y);

//  tnum=(int)((float)(y-PStatus.LastMouseY)*(float)tc2->time/(float)PStatus.timespace);
if (custom>0) {
	tnum=custom; 
//		tslide=tcontrol->FTC_HScrollSlide;
//		tback=tcontrol->FTC_HScrollBack;
//		tc=tcontrol; //frame is passed
} else {
	//difference in mouse move y position
	tnum=(x-PStatus.LastMouseX);
}
bw=(tslide->x2-tslide->x1);
  

//possible timesaver (needed because scroll bar is slow)=
//no erase, just draw two FTControls

//  EraseFTControl(twin,tcontrol);

if (tc->type==CTFileOpen || tc->type==CTItemList) {
//move scroll slider control
  if (tnum>0) {  //going right
    if (tslide->x2+tnum>tback->x2) {
      tslide->x2=tback->x2;
      tslide->x1=tback->x2-bw;
    } else {
      tslide->x1+=tnum;
      tslide->x2+=tnum;
      //fprintf(stderr,"added tcontrol->y1=%d\n",tcontrol->y1);
    }  //tcontrol->y2+tnum>
  } //tnum>0
  else {  //going left
    if (tslide->x1+tnum<tback->x1) {
      tslide->x1=tback->x1;
      tslide->x2=tback->x1+bw;
    }
    else {
      tslide->x1+=tnum;
      tslide->x2+=tnum;
    }
  }//num>0

	//adjust i[0] (the first FTItemCharacterShowing at the top of the fileopen FTControl) 
	if (tc->ilist!=0) {
		//get ratio of scroll button to back bar
		//ratio=1.0-((float)(tcontrol->y2-tcontrol->y1)/(float)(tback->y2-tback->y1));
		//ratio=1.0-((float)(tslide->y2-tslide->y1)/(float)(tback->y2-tback->y1));
		ratio=(float)(tback->x2-tslide->x1)/(float)(tback->x2-tback->x1);
	//  fprintf(stderr,"ratio=%f\n",ratio);
		if (ratio<1.0 && ratio!=0) {
			//tcontrol->FirstItemShowing=(int)((1.0-ratio)*(tcontrol->ilist->num))+1;
			tc->FirstItemCharacterShowing=(int)((1.0-ratio)*(tc->ilist->width/tc->ilist->fontwidth));
		  //fprintf(stderr,"ratio=%f tcontrol->CursorLocationInChars=%d\n",ratio,tcontrol->CursorLocationInChars);
			DrawFTControl(tc);  //this will redraw scroll bar FTControls
		} //ratio<1.0
	} //tc->ilist!=0

} //if (tc->type==CTFileOpen || tc->type==CTItemList) {


if (tc->type==CTFrame) {
//	fprintf(stderr,"tnum=%d\n",tnum);
	//move all frame controls by same amount as mouse times the ratio of controls to tback
//possibly adapt one of the other Vscroll functions.

	tTimeLine=tc->TimeLineFTControl;

	if (tback==0) {
		return;
	} //tcontrol==0
//move scroll slider control
  if (tnum>0) {  //going right
    if (tslide->x2+tnum>tback->x2) {
			tnum=tback->x2-tslide->x2;//mouse move larger than available space
      tslide->x2=tback->x2;
      tslide->x1=tback->x2-bw;
    }
    else {
      tslide->x1+=tnum;
      tslide->x2+=tnum;
      //fprintf(stderr,"added tcontrol->y1=%d\n",tcontrol->y1);
    }
  } else {  //going left
    if (tslide->x1+tnum<tback->x1) { 
			tnum=tslide->x1-tback->x1;//mouse move larger than available space
      tslide->x1=tback->x1;
      tslide->x2=tback->x1+bw;
    }
    else {
      tslide->x1+=tnum;
      tslide->x2+=tnum;
    } //tslide->x1+
  } //tnum>0

	//find size of controls
	minx=(long long)tc->x1;
	maxx=(long long)tc->x2;
	tc2=twin->icontrol;
	while(tc2!=0) {
		if (tc2->FrameFTControl==tc && !(tc2->flags&CNotVisible)) {
//EraseFTControl?
			if (!(tc2->flags2&CDoNotScrollX)) {

				if (tc2->type!=CTTimeLine) { //ignore TimeLine controls
					if (tc2->type==CTData) { //data controls have a virtual size and this needs to be calculated from StartTime and EndTime
							if (tTimeLine==0) {
								fprintf(stderr,"Error: Data control is on a frame that has no timeline control.\n");
							}	
							lx=tTimeLine->x1+(long long)((tc2->StartTime-tTimeLine->StartTimeVisible)/tTimeLine->TimeScale);
							if (lx<minx) {
								minx=lx;
							}
							lx=tTimeLine->x1+(long long)((tc2->EndTime-tTimeLine->StartTimeVisible)/tTimeLine->TimeScale);
							if (lx>maxx) {
								maxx=lx;
							}
					} else { //not CTData
						if ((long long)tc2->x1<minx) {
							minx=tc2->x1;
						} //x1<minx
						if ((long long)tc2->x2>maxx) {
							maxx=tc2->x2;
						} //x2>maxx
					} //CTData
				} //!CTTimeLine
			} //CDoNotScrollX

		} //if tc2->FrameFTControl			
	tc2=tc2->next;
	} //while(tc2!=0)
	
	//find ration of controls to tback
	ratio=(long double)(maxx-minx)/(long double)(tback->x2-tback->x1);

	//if timeline move that first before data
	if (tTimeLine!=0) {
		//pixels*seconds/pixel = seconds
		tTimeLine->StartTimeVisible+=((long double)tnum*ratio)*tTimeLine->TimeScale;		
		tTimeLine->EndTimeVisible+=((long double)tnum*ratio)*tTimeLine->TimeScale;
		if (tTimeLine->StartTimeVisible<0.0) {
			tTimeLine->StartTimeVisible=0.0;
			tTimeLine->EndTimeVisible=(long double)(tTimeLine->x2-tTimeLine->x1)*tTimeLine->TimeScale;
			//tnum=0;
		}//tTimeLine->StartTimevisible<
#if 0 
		if (tTimeLine->StartTimeVisible>((long double)maxx*tTimeLine->TimeScale)) {
			tTimeLine->EndTimeVisible=(long double)maxx*tTimeLine->TimeScale;
			tTimeLine->StartTimeVisible=tTimeLine->EndTimeVisible-(long double)(tTimeLine->x2-tTimeLine->x1)*tTimeLine->TimeScale;
			tnum=0;
		}//tTimeLine->StartTimevisible>
#endif
	} //tTimeLine!=0

	//scroll any controls by the same amount
	tc2=twin->icontrol;
	while(tc2!=0) {
		if (tc2->FrameFTControl==tc && !(tc2->flags&CNotVisible)) {
//EraseFTControl?

			if (!(tc2->flags2&CDoNotScrollX)) {

				if (tc2->type!=CTTimeLine) {
					if (tc2->type==CTData) {
						AdjustDataFTControlDimensions(tc2); //resize control if needed (data control either has same dimensions as timeline, is smaller or is set to not visible)

						//also scroll any connected label control
						if (tc2->DataLabelFTControl!=0) {
							tlabel=tc2->DataLabelFTControl;
							if (tlabel!=0) {
									if (!(tlabel->flags2&CDoNotScrollX)) {
									//tlabel->x1-=(int)((long double)tnum*ratio);
									//tlabel->x2-=(int)((long double)tnum*ratio);
									} //tlabel->flags
								} //tlabel!=0
						} //tc2
					} else { //not CTData
						tc2->x1-=(long long)((long double)tnum*ratio);
						tc2->x2-=(long long)((long double)tnum*ratio);
					} //tc2->type!=CTData
				} //CTTimeLine

			} //tcontrol->flags2&CDoNotScrollX

		} //if tc2->FrameFTControl			
	tc2=tc2->next;
	} //while(tcontrol3!=0)

	if (tnum!=0) {
		DrawFTControl(tc); //redraw frame controls
	}
} //CTFrame	


} //end HScrollSlide_Button0DownMouseMove


void CalcHScrollSlide(FTWindow *twin,FTControl *tcontrol)
{
long long minx,maxx,lx,controlswidth;
FTControl *tTimeLine,*tslide,*tback,*tcontrol2,*tleft,*tright;
//char tstr[FTMedStr];
long double framecontrolratio;
float ratio;//,ratiobar;
int pos,slidewidth,framewidth,backwidth,fw;//,slideratio;//,newframewidth;
FTItemList *tlist;

//tcontrol=control horizontal slide scrollbar button is connected to fileopen,frame,etc
//tslide=slide
//tback=back
//sprintf(tstr,"%s%s",PStatus.PrefixHScrollSlide,tcontrol->name);
//tslide=GetFTControl(tstr);
tslide=tcontrol->FTC_HScrollSlide;
//sprintf(tstr,"%s%s",PStatus.PrefixHScrollBack,tcontrol->name);
//tback=GetFTControl(tstr);
tback=tcontrol->FTC_HScrollBack;
tleft=tcontrol->FTC_HScrollLeft;
tright=tcontrol->FTC_HScrollRight;

//frame has timeline
tTimeLine=0;
if (tcontrol->TimeLineFTControl!=0) {
	tTimeLine=tcontrol->TimeLineFTControl;
} //TimeLineFTControl!=0


if (tcontrol->type==CTItemList) {
	//determine the width of the horizontal slide button

	//how much of the item text is showing
	//itemlist->width is the width of the itemlist in pixels with no regard to the control it is in
	if (tcontrol->ilist!=0) {
		tlist=tcontrol->ilist;
		fw=tlist->fontwidth;
		//ratio=(float)(tcontrol->x2-tcontrol->x1-(tleft->x2-tleft->x1)-(tright->x2-tright->x1))/(float)tcontrol->ilist->width;
		//ratio=(float)(tback->x2-tback->x1)/(float)tcontrol->ilist->width;
		ratio=(float)(tcontrol->x2-tcontrol->x1-PStatus.PadX)/(float)tlist->width;
		//ratiobar=(float)(tback->x2-tback->x1)/(float)(tcontrol->x2-tcontrol->x1);
		//tslide->x1=tleft->x2+(int)((float)(tcontrol->FirstItemCharacterShowing*tcontrol->ilist->fontwidth)/(float)ratio);
		//tslide->x1=tleft->x2+(int)((float)(tcontrol->FirstItemCharacterShowing*tcontrol->ilist->fontwidth)/(float)ratio);
		//tslide->x1=tleft->x2+(int)(((float)(tcontrol->FirstItemCharacterShowing*tcontrol->ilist->fontwidth)/ratio)*ratiobar);
		
		tslide->x1=tleft->x2+(int)(((float)(tcontrol->FirstItemCharacterShowing*fw)/(float)tlist->width)*(tback->x2-tback->x1));
		//if (tcontrol->FirstItemCharacterShowing*fw+(tcontrol->x2-tcontrol->x1)<tlist->width) {	
		if (ratio<1.0) {	
			//is fraction of bar
			tslide->x2=tslide->x1+(int)(ratio*(float)(tback->x2-tback->x1));		
		} else {
			tslide->x2=tright->x1-1;  //is at end
		}
		//tslide->x2=tslide->x1+(int)(ratio*(tcontrol->x2-tcontrol->x1));
		//tslide->x2=tslide->x1+(int)(ratiobar*(tback->x2-tback->x1));
		
		DrawFTControl(tback);
		DrawFTControl(tslide);
	} //if (tcontro->ilist!=0

} //if (tcontrol->type==CTItemList) {

if (tcontrol->type==CTFrame) {
	//calculate position and size (x1,x2) of slide button
	//go through controls connected to frame and get minx and maxx
	//we could use times making mixx and maxx long double
	//perhaps there is a faster method
	tcontrol2=twin->icontrol;
//	minx=tcontrol->x1;
//	maxx=tcontrol->x2;
	framewidth=tcontrol->x2-tcontrol->x1;
	backwidth=tback->x2-tback->x1;
//	minx=tback->x1;
//	maxx=tback->x1;
	minx=tcontrol->x1;
	maxx=tcontrol->x2;

	while(tcontrol2!=0) {
		
		if (tcontrol2->FrameFTControl==tcontrol) {
			if (tcontrol2->type==CTData) { //data controls have a virtual size and this needs to be calculated from StartTime and EndTime
					if (tTimeLine==0) {
						fprintf(stderr,"Error: Data control is on a frame that has no timeline control.\n");
					}	
					lx=tTimeLine->x1+(long long)((tcontrol2->StartTime-tTimeLine->StartTimeVisible)/tTimeLine->TimeScale);
					if (lx<minx) {
						minx=lx;
					}
					lx=tTimeLine->x1+(long long)((tcontrol2->EndTime-tTimeLine->StartTimeVisible)/tTimeLine->TimeScale);
					if (lx>maxx) {
						maxx=lx;
					}
			} else { //CTData
				if (tcontrol2->x1<minx) {
					minx=tcontrol2->x1;
				} //x1<minx
				if (tcontrol2->x2>maxx) {
					maxx=tcontrol2->x2;
				} //x2>maxx
			} //CTData
		}	//FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0


//	if (minx==maxx) { //no controls changed minx and maxx
//		maxx=tback->x2;
//	}

	
//	minimum unit=PStatus.ScrollPixelsY

//	ratio=(long double)(tcontrol->x2-tcontrol->x1)/(long double)(maxx-minx);
//	ratio=(long double)(tback->x2-tback->x1)/(long double)(maxx-minx);
//also subtract a single frame, which is the width of the frame
	//ratio=(long double)(tback->x2-tback->x1)/(long double)(maxx-minx-(tcontrol->x2-tcontrol->x1)/2);
//	ratio=(long double)(tTimeLine->x2-tTimeLine->x1)/(long double)(maxx-minx);

	controlswidth=maxx-minx;
//	ratio=(long double)(tTimeLine->x2-tTimeLine->x1)/(long double)(maxx-minx);
	framecontrolratio=(long double)framewidth/(long double)controlswidth;
//	fprintf(stderr,"minx=%lli maxx=%lli ratio=%Lg\n",minx,maxx,ratio);
//	fprintf(stderr,"back x1=%d x2=%d w=%d wtimeline=%d\n",tback->x1,tback->x2,tback->x2-tback->x1,tTimeLine->x2-tTimeLine->x1);
	if (framecontrolratio>=1.0) { //frame is bigger or equal to controls
		tslide->x1=tback->x1;
		tslide->x2=tback->x2;
	} else {
		//recalculate ratio minus a full screen
//		ratio=(long double)(tTimeLine->x2-tTimeLine->x1)/(long double)(maxx-minx-(long double)(tcontrol->x2-tcontrol->x1));


		//minslidewidth=(tback->x2-tback->x1)/20;
		//pos=tback->x1+(int)((float)(tTimeLine->StartTimeVisible-minx)*ratio);//PStatus.ScrollPixelsY;
		slidewidth=(int)(((long double)backwidth)*framecontrolratio);
		//we should not use the timeline since scrolling a frame is for other controls besides data controls
//		pos=(int)((tTimeLine->StartTimeVisible/tTimeLine->TimeScale)*(tback->x2-tback->x1)/(maxx-minx));
		pos=(int)((long double)(tcontrol->x1-minx)*(long double)backwidth/(long double)controlswidth);


		if (slidewidth<FT_MIN_SLIDE_SIZE) {
			//if slide width is < minimum size, scale up the size (easier than choosing some fixed size) until > minimum size
			//this function just adjusts the size and position of the slide button based on the frame position relative to all contols on the frame
//			fprintf(stderr,"using minsize\n");

			//increase new frame width
//			newframewidth=framewidth*2;//((long double)FT_MIN_SLIDE_SIZE/(long double)slidewidth);
			//ratio=(long double)newframewidth/(long double)controlswidth;
			//pos=(int)(((long double)(tcontrol->x1-(newframewidth-framewidth)/2-minx)*(long double)backwidth)/(long double)controlswidth);

//			pos=(int)(((int)((unsigned long long)((long double)tcontrol->x1*)-minx))*(tback->x2-tback->x1)/((maxx-minx)*slidewidth));
			//this is like resizing the frame dimensions - think of the frame as the slide button to the scrolling control width
//			pos*=(((long double)FT_MIN_SLIDE_SIZE/(long double)slidewidth)/(long double)(tcontrol->x2-tcontrol->x1));
			//scale position
			//slideratio=(float)FT_MIN_SLIDE_SIZE/(float)slidewidth;
			//adjust position by tiny fraction of change in slidewidth
//			pos=(int)((float)pos-slideratio*((float)pos/(float)backwidth));
			pos=(int)((float)pos-(float)FT_MIN_SLIDE_SIZE*((float)pos/(float)backwidth));
			//pos+=(int)((float)FT_MIN_SLIDE_SIZE/2.0);
			slidewidth=FT_MIN_SLIDE_SIZE;
		}

		//to find start position of slider, I use ratio of starttime of timeline/full size of data * full size of scrollbar back button
//		pos=(int)((tTimeLine->StartTimeVisible/tTimeLine->TimeScale)*(tback->x2-tback->x1)/(maxx-minx));



#if 0 
		//adjust pos and size if slide button too small
		if (slidewidth<FT_MIN_SLIDE_SIZE) {
			ratio=(long double)(tback->x2-tback->x1-FT_MIN_SLIDE_SIZE)/(long double)(tback->x2-tback->x1-slidewidth);
			fprintf(stderr,"pos was %d (ratio=%Lg %d/%d)\n",pos,ratio,FT_MIN_SLIDE_SIZE,slidewidth);
			pos=(int)((long double)pos/ratio); //so if slide is 2x original size, position is 1/2 original position
			fprintf(stderr,"pos is now %d\n",pos);
			slidewidth=FT_MIN_SLIDE_SIZE;
		}
#endif

		tslide->x1=tback->x1+pos+1;
		tslide->x2=tslide->x1+slidewidth;
		//tslide->x2=(int)pos+(int)(ratio*(float)(tTimeLine->x2-tTimeLine->x1));
		//tslide->x2=(int)pos+(int)(ratio*(float)(tcontrol->x2-tcontrol->x1));
//		fprintf(stderr,"slide x1=%d x2=%d w=%d slidewidth=%d\n",tslide->x1,tslide->x2,tslide->x2-tslide->x1,slidewidth);
	}

 	DrawFTControl(tback);
	DrawFTControl(tslide);
} //CTFrame
//fprintf(stderr,"End CalcHScrollSlide\n");
}  //CalcHScrollSlide


//update scroll bar button
void UpdateHScrollSlide(FTControl *tcontrol)
{
#if 0 
float pos;//,ratio;
FTControl *tcontrol2,*tcontrol3;
char tstr[FTMedStr];
int bh;

//fprintf(stderr,"UpdateVScrollSlide\n");

//should be made into 2 functions
//CalcScrollSlideSize() called when fileopen FTItemList is refreshed
//UpdateVScrollSlide() called whenever anything but a drag moves the scroll slide

//UpdateHScrollSlide draws the scroll slide button from i[0]
//




//tcontrol=fileopen
//tcontrol2=slide
//tcontrol3=back
sprintf(tstr,"%s%s",PStatus.PrefixVScrollSlide,tcontrol->name);
tcontrol2=GetFTControl(tstr);
bh=tcontrol2->y2-tcontrol2->y1;
//tcontrol=GetFTControl("foOpenFile2");
//tcontrol2=GetFTControl("sb_foOpenFile2");
sprintf(tstr,"%s%s",PStatus.PrefixVScrollBack,tcontrol->name);
tcontrol3=GetFTControl(tstr);
//tcontrol3=GetFTControl("sm_foOpenFile2");


if (tcontrol->ilist!=0) {
  pos=(float)tcontrol->CursorLocationInChars/(float)tcontrol->ilist->num;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
  tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;
  tcontrol2->y2=tcontrol2->y1+bh;
  DrawFTControl(tcontrol3);  //back
  DrawFTControl(tcontrol2);  //slide
  }  //if ilist!=0

//fprintf(stderr,"End UpdateVScrollSlide\n");
#endif

}  //UpdateHScrollSlide


//button down on horizontal scroll bar left button, scroll controls on frame to right
void FT_FrameScrollLeft(FTWindow *twin,FTControl *tcontrol,int Page)
{
FTControl *tcontrol2,*tTimeLine,*tlabel;
long long minx,lx1; 
int numpix;
long double st,stt;

if (PStatus.flags&PInfo) {
  fprintf(stderr,"Start FT_FrameScrollLeft\n");
}


//scroll all controls on this frame right if still controls hidden to left of frame
//go through all controls on this window, for those with this FrameFTControl, find minx
//if minx<tcontrol->x1 - (the leftmost control is left of frame x1) scroll all controls
minx=tcontrol->x1;
tcontrol2=twin->icontrol;
while(tcontrol2!=0) {
	if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollX)) {
		//could be a data control which uses lx1,lx2 - but ly1,ly2 are probably not used and are mirrored in y1,y2
		if (tcontrol2->type==CTData) {
			//only scroll data controls if there is an associated timeline control
			if (tcontrol2->TimeLineFTControl!=0) {
				//determine x1 position of data based on (data->starttime-timeline->StartTimeVisible) (s) / timescale s/pixel)
				tTimeLine=tcontrol2->TimeLineFTControl;
				st=tcontrol2->StartTime;
				stt=tTimeLine->StartTimeVisible;
				lx1=tTimeLine->x1+(long long)((st-stt)/tTimeLine->TimeScale); 
				if (lx1<minx) {
					minx=lx1;
				} //lx1<minx
			} //TimeLineFTControl!=0
		} else {
			if (tcontrol2->x1<minx) {
				minx=(long long)tcontrol2->x1;
			}
		} //CTData
	} //FrameFTControl==tcontrol
tcontrol2=tcontrol2->next;
} //tcontrol2!=0

			//fprintf(stderr,"minx=%lli\n",minx);

if (minx<(long long)tcontrol->x1) {  //at least one control is left of the frame control - scroll all frame controls right

//fprintf(stderr,"minx=%lli x1=%d\n",minx,tcontrol->x1);
	if (Page) { //scroll page
		numpix=tcontrol->x2-tcontrol->x1;
	} else { //only scroll line
		numpix=PStatus.ScrollPixelsX;//tcontrol2->TimeSpace;
	}
	if (minx+numpix>(long long)tcontrol->x1) {
		numpix=tcontrol->x1-minx;
			//fprintf(stderr,"adjusted minx=%lli\n",minx);
	}

//if frame has an associated TimeLine control, scroll that by changing the TimeLine control StartTimeVisible
	if (tcontrol->TimeLineFTControl!=0) {
		tTimeLine=tcontrol->TimeLineFTControl;
		tTimeLine->StartTimeVisible-=tTimeLine->TimeScale*(long double)numpix; //move time, in s
		//quantize starttime to TimeScale*TimeSpace by removing remainder
		tTimeLine->StartTimeVisible-=fmodl(tTimeLine->StartTimeVisible,tTimeLine->TimeScale*tTimeLine->TimeSpace);
		if (tTimeLine->StartTimeVisible<0.0) {
			tTimeLine->StartTimeVisible=0.0;
		}
	} //tcontrol->TimeLineFTControl!=0



	tcontrol2=twin->icontrol;
	while(tcontrol2!=0) {
		if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollX)) {
//EraseFTControl?



			//Data controls do not scroll in the x dimension (but do in the y)- only the data scrolls in the x, otherwise data would need to have 64-bit x1,x2 values, when for example there is a few seconds of data being looked at with a TimeScale of 1ns.
			//for a data control, any timeline has already been adjusted, so just adjust the data control StartTimeVisible and dimensions if necessary
			if (tcontrol2->type==CTData) {
				//Adjust data control dimensions relative to the associated timeline control
					AdjustDataFTControlDimensions(tcontrol2); //resize control if needed (data control either has same dimensions as timeline, is smaller or is set to not visible)

					//also scroll any connected label control
					if (tcontrol2->DataLabelFTControl!=0) {
						tlabel=tcontrol2->DataLabelFTControl;
						if (!(tlabel->flags2&CDoNotScrollX)) {
							tlabel->x1+=numpix;
							tlabel->x2+=numpix;
						}
					}

			} else {//CTData
				//other controls actually physically move (change x1,x2, etc)
				tcontrol2->x1+=numpix;
				tcontrol2->x2+=numpix;

			} //tcontrol2->type==CTData

//			fprintf(stderr,"scroll left %s st=%Lg et=%Lg\n",tcontrol2->name, tcontrol2->StartTimeVisible,tcontrol2->EndTimeVisible);
//			fprintf(stderr,"Scroll %s left x1=%d x2=%d\n",tcontrol2->name,tcontrol2->x1,tcontrol2->x2);
		} //FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0
DrawFTControl(tcontrol); //redraw frame
} //maxy>tcontrol->y2




if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_FrameScrollLeft\n");
}

}//FT_FrameScrollLeft


//button down on scrollbar right button connected to frame control - move controls on frame left
void FT_FrameScrollRight(FTWindow *twin,FTControl *tcontrol,int Page)
{
FTControl *tcontrol2,*tTimeLine,*tlabel;
long long maxx,lx2; 
int numpix;
long double et,stt,inc;


//tcontrol is the frame control
//if (PStatus.flags&PInfo) {
//  fprintf(stderr,"Start FT_FrameScrollRight\n");
//}

//scroll all controls on this frame up if still controls hidden below frame
//go through all controls on this window, for those with this FrameFTControl, find maxy
//if maxy>tcontrol->y2 - scroll all controls
maxx=tcontrol->x2;
tcontrol2=twin->icontrol;
while(tcontrol2!=0) {
	if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollX)) {
		if (tcontrol2->type==CTData) {  
			//determine x2 position of data based on endtime (s) * timescale s/pixel)
			if (tcontrol2->TimeLineFTControl!=0) {
				//determine x1 position of data based on (data->starttime-timeline->StartTimeVisible) (s) / timescale s/pixel)
				tTimeLine=tcontrol2->TimeLineFTControl;
				et=tcontrol2->EndTime;
				stt=tTimeLine->StartTimeVisible;
				lx2=tTimeLine->x1+(long long)((et-stt)/tTimeLine->TimeScale); 
				if (lx2>maxx) {
					maxx=lx2;
				} //lx2>maxx
			} //TimeLineFTControl!=0
		} else { //regular control like timeline, button, etc
			if (tcontrol2->x2>maxx) {
				maxx=(long long)tcontrol2->x2;
			}
		} //CTData
	} //FrameFTControl==tcontrol
tcontrol2=tcontrol2->next;
} //tcontrol2!=0


if (maxx>(long long)tcontrol->x2) {  //at least one control is farther to the right than frame control - scroll all frame controls left
//Note that ScrollPixelsX needs to be the same as tcontrol->TimeSpace
//use tcontrol->TimeSpace for data and timeline controls?
	if (Page) { //scroll page
		numpix=tcontrol->x2-tcontrol->x1; //possibly should be in units of 10? so 10s time lines do not have one's digits
	} else { //only scroll line
		numpix=PStatus.ScrollPixelsX;//tcontrol2->TimeSpace; //perhaps make tcontrol->TimeScale*tcontrol->nSamplesPerSecond;
	} //Page

//	if (maxx-numpix<(long long)tcontrol->x2) {
//		numpix=maxx-tcontrol->x2;
//	}
//	if ((long long)tcontrol->x1-numpix<0.0) {
//		numpix=tcontrol->x1-numpix;
//	}

			//fprintf(stderr,"maxx %lli > %lli tcontrol->x2 numpix=%d\n",maxx,(long long)tcontrol->x2,numpix);
//if frame has an associated TimeLine control, scroll that by changing the TimeLine control StartTimeVisible
	if (tcontrol->TimeLineFTControl!=0) {
		tTimeLine=tcontrol->TimeLineFTControl;
		inc=tTimeLine->TimeScale*(long double)numpix;

		//quantize tTimeLine->StartTimeVisible to TimeScale*TimeSpace
//		tTimeLine->StartTimeVisible-=fmodl(tTimeLine->StartTimeVisible,tTimeLine->TimeScale);
//		tTimeLine->StartTimeVisible=(long double)((long long)tTimeLine->StartTimeVisible/tTimeLine->TimeScale*tTimeLine->TimeSpace)*tTimeLine->TimeScale*tTimeLine->TimeSpace;


//		fprintf(stderr,"StartTimeVisible=%Lg\n",tTimeLine->StartTimeVisible);

		tTimeLine->StartTimeVisible+=inc; //move time, in s
		//quantize starttime to TimeScale*TimeSpace by removing remainder
//		fprintf(stderr,"StartTimeVisible=%Lg inc=%Lg divide by = %Lg\n",tTimeLine->StartTimeVisible,inc,tTimeLine->TimeScale*tTimeLine->TimeSpace);
//		fprintf(stderr,"remainder=%Lg fmodl=%Lg\n",remainderl(tTimeLine->StartTimeVisible,tTimeLine->TimeScale*tTimeLine->TimeSpace),fmodl(tTimeLine->StartTimeVisible,tTimeLine->TimeScale*tTimeLine->TimeSpace));
//fmodl and remainderl both return a remainder for 3e-05,1e-05
//		tTimeLine->StartTimeVisible-=remainderl(tTimeLine->StartTimeVisible,tTimeLine->TimeScale*tTimeLine->TimeSpace);
		//quantize to TimeScale

		//AdjustDataFTControls will set timeline EndTime
//		fprintf(stderr,"timeline st=%Lg numpix=%d +%Lg\n",tTimeLine->StartTimeVisible,numpix,tTimeLine->TimeScale*(long double)numpix);
//			fprintf(stderr,"after StartTimeVisible=%Lg inc=%Lg\n",tTimeLine->StartTimeVisible,inc);
	} //tcontrol->TimeLineFTControl!=0


	tcontrol2=twin->icontrol;
	while(tcontrol2!=0) {
		if (tcontrol2->FrameFTControl==tcontrol && !(tcontrol2->flags2&CDoNotScrollX)) {
//EraseFTControl?
//			fprintf(stderr,"Scrolling control %s\n",tcontrol2->name);

			if (tcontrol2->type==CTData) {
				//Adjust data control dimensions relative to the associated timeline control
					AdjustDataFTControlDimensions(tcontrol2); //resize control if needed (data control either has same dimensions as timeline, is smaller or is set to not visible)

					//also scroll any connected label control
					if (tcontrol2->DataLabelFTControl!=0) {
						tlabel=tcontrol2->DataLabelFTControl;
						if (!(tlabel->flags2&CDoNotScrollX)) {
							tlabel->x1-=numpix;
							tlabel->x2-=numpix;
						}
					}

			} else {
				tcontrol2->x1-=numpix;
				tcontrol2->x2-=numpix;

			}//CTData


//		fprintf(stderr,"Scroll %s right x1=%d x2=%d\n",tcontrol2->name,tcontrol2->x1,tcontrol2->x2);
		} //FrameFTControl==tcontrol
	tcontrol2=tcontrol2->next;
	} //tcontrol2!=0
DrawFTControl(tcontrol); //redraw frame
} //maxy>tcontrol->y2

if (PStatus.flags&PInfo) {
  fprintf(stderr,"End FT_FrameScrollRight\n");
}

}//FT_FrameScrollRight

//END HORIZONTAL SCROLLBAR FUNCTIONS

//AdjustDataFTControlDimensions
//Calculate the dimensions (coordinates) of a data control
//requires data control to have an associated TimeLine control (as a reference for StartTimeVisible)
//uses only data control's StartTime and EndTime
//data control is adjusted in 1 of 3 ways:
//1) is set to not visible, 2) is set to the same dimensions as the associated timeline control, or 3) is set to dimensions smaller than the associated timeline control
//this occurs when data is 1) open 2) moved 3) resized 4) cut 5) TimeScale is changed +-
//data controls are now drawn using StartTimeVisible only, and are not the large size controls they were
void AdjustDataFTControlDimensions(FTControl *tcontrol) {
FTControl *tTimeLine;
long double st,et,stt,ett;//,durt;
int bw;

//get associated timeline
if (tcontrol->TimeLineFTControl!=0) {


	tTimeLine=tcontrol->TimeLineFTControl;
	bw=tTimeLine->x2-tTimeLine->x1;
//	if (tTimeLine->EndTimeVisible<=tTimeLine->StartTimeVisible) { //TimeLine endtime was probably not set correctly
		//just set EndTimeVisible in case it is incorrect
	tTimeLine->EndTimeVisible=tTimeLine->StartTimeVisible+bw*tTimeLine->TimeScale;
//	}


	//st=starttime of data control, et=end time of data control
	//stt=visible starttime of timeline, ett=visible endtime of timeline
	//then see if they are on screen
	//DrawFTControl uses StartTimeVisible, and presumes control is already clipped
	//in fact a frame is only necessary for the scrollbar controls to be automated
	//scrollbar buttons only change the timeline's (and data's) StartTimeVisible time
	//we only need to see if the data control size fits the visible portion of the timeline, is smaller, or is offscreen


	st=tcontrol->StartTime;
	et=tcontrol->EndTime;
	stt=tTimeLine->StartTimeVisible;
	ett=tTimeLine->EndTimeVisible;
//	durt=ett-stt;
	//alternative method:
	//ett=stt+(long double)(tTimeLine->x2-tTimeLine->x1)*tTimeLine->TimeScale; 

	if (et<stt) { //data control is off screen left
		tcontrol->flags|=CNotVisible;
		return;
	}
	if (st>ett) { //data control is off screen right
		tcontrol->flags|=CNotVisible;
		return;
	}

	//some part of the data control is on screen
	tcontrol->flags&=~CNotVisible;
//	fprintf(stderr,"Data control is visible\n");


	//set data control's visible start time
	if (st<stt) {
		tcontrol->StartTimeVisible=stt;
	} else {
		tcontrol->StartTimeVisible=st;
	}
	//adjust st to StartTime visible
	st=tcontrol->StartTimeVisible;

	//position of data x1 is proportional to timeline width
	//tcontrol->x1=tTimeLine->x1+(int)((st-stt)(long double)bw);
	tcontrol->x1=tTimeLine->x1+(int)((st-stt)/tTimeLine->TimeScale);  
	//et has already been calculated to end when the data ends
	if (et>ett) { //trim control visible end time
		tcontrol->x2=tTimeLine->x2;
		tcontrol->EndTimeVisible=ett;
	} else {
		//data ends before timeline ends
		tcontrol->x2=tcontrol->x1+(int)((et-st)/tTimeLine->TimeScale);
	} //et>ett

if (tcontrol->x2-tcontrol->x1<2) {
	tcontrol->x2=tcontrol->x1+2;
} //x1==x2

//if (tcontrol->Channel==0) {
//	fprintf(stderr,"x1=%d x2=%d st=%Lg et=%Lg stt=%Lg ett=%Lg\n",tcontrol->x1,tcontrol->x2,st,et,stt,ett);
//}

} else { //tcontrol->TimeLineFTControl!=0
	fprintf(stderr,"Data Control needs to have an associated TimeLine control to be drawn correctly.\n");
}


}  //AdjustDataFTControlDimensions


//TimeLine is being selected by MouseMove, determine selected start and end time, move timeline forward or back if necessary, set start and end selection on all connected data controls, and redraw
void FT_AdvanceTimeLine(FTWindow *twin,FTControl *tTimeLine,int x,int y) {
FTControl *tcontrol2;
long double lx;
int direction;

	if (tTimeLine!=0) { 
	//	lx=tcontrol->StartTimeVisible+((long double)(x-tcontrol->x1))*tTimeLine->TimeScale; //time at mouse
		lx=tTimeLine->StartTimeVisible+((long double)(x-tTimeLine->x1))*tTimeLine->TimeScale; //time at mouse

		direction=x-PStatus.LastMouseX;


		if (direction<0) {
			//fprintf(stderr,"Moving left\n");
			//moving left
			//passes start of selection
			if (lx<tTimeLine->StartTimeSelected) {
				if (lx<0.0) {
					lx=0.0;
				}
				tTimeLine->StartTimeSelected=lx;
				tTimeLine->StartTimeCursor=lx;
			} else { //lx<tTimeLine->StartTimeSelected
				//is left of end but right of start
				if (lx<tTimeLine->EndTimeSelected) {
					tTimeLine->EndTimeSelected=lx;
				} //lx<tTimeLine->StartTimeSelected
			} //lx<tTimeLine->StartTimeSelected
		} //direction<0
		if (direction>0) { //direction>0
			//fprintf(stderr,"Moving right\n");
			//moving right
			if (lx>tTimeLine->StartTimeSelected) {
				if (lx>tTimeLine->EndTimeSelected) {
					tTimeLine->EndTimeSelected=lx;
				} else {
					tTimeLine->StartTimeSelected=lx;
					tTimeLine->StartTimeCursor=lx;
				} //lx>tTimeLine->EndTimeSelected
			} //lx>tTimeLine->StartTimeSelected
		} //direction>0
		//redraw timeline
		DrawFTControl(tTimeLine);

		//adjust other data controls connected to timeline control
		tcontrol2=twin->icontrol;
		while(tcontrol2!=0) {
			if (tcontrol2->type==CTData && tcontrol2->TimeLineFTControl==tTimeLine) {
				tcontrol2->StartTimeSelected=tTimeLine->StartTimeSelected;
				tcontrol2->EndTimeSelected=tTimeLine->EndTimeSelected;
				DrawFTControl(tcontrol2);
			} //tcontrol2->type==CTData

			//update any StartTimeSelected controls connected to data 
			if (tcontrol2->StartTimeSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->StartTimeSelectedFTControl->text,tTimeLine->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->StartTimeSelectedFTControl); //redraw SelectedTime control
			} //StartTimeSelectedFTControl
			//update any EndTimeSelected controls connected to data 
			if (tcontrol2->EndTimeSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->EndTimeSelectedFTControl->text,tTimeLine->EndTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->EndTimeSelectedFTControl); //redraw SelectedTime control
			} //EndTimeSelectedFTControl
			//update any StartTimeSelected controls connected to data 
			if (tcontrol2->DurationSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->DurationSelectedFTControl->text,tTimeLine->EndTimeSelected-tTimeLine->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->DurationSelectedFTControl); //redraw SelectedTime control
			} //DurationSelectedFTControl

			tcontrol2=tcontrol2->next;
		} //tcontrol2!=0

	} //tTimeLine!=0

} //FT_AdvanceTimeLine


//mouse is selecting data
void FT_SelectData(FTWindow *twin,FTControl *tcontrol,int x,int y) {
FTControl *tcontrol2,*tTimeLine;
long double lx;
int direction;

	tTimeLine=tcontrol->TimeLineFTControl;

	if (tTimeLine!=0) { //data has timeline connected to it
		lx=tcontrol->StartTimeVisible+((long double)(x-tcontrol->x1))*tTimeLine->TimeScale; //time at mouse
		direction=x-PStatus.LastMouseX;

		if (direction<0) {
			//moving left
			//passes start of selection
			if (lx<tcontrol->StartTimeSelected) {
				if (lx<0.0) {
					lx=0.0;
				}
				tcontrol->StartTimeSelected=lx;
				tcontrol->StartTimeCursor=lx;
			} else { //lx<tcontrol->StartTimeSelected
				//is left of end but right of start
				if (lx<tcontrol->EndTimeSelected) {
					tcontrol->EndTimeSelected=lx;
					tTimeLine->EndTimeSelected=lx;
				} //lx<tcontrol->StartTimeSelected
			} //lx<tcontrol->StartTimeSelected
		} //direction<0 

		if (direction>0) { //direction>0
			//moving right
			if (lx>tcontrol->StartTimeSelected) {
				if (lx>tcontrol->EndTimeSelected) {
					tcontrol->EndTimeSelected=lx;
					tTimeLine->EndTimeSelected=lx;
				} else {
					tcontrol->StartTimeSelected=lx;
					tcontrol->StartTimeCursor=lx;
					tTimeLine->StartTimeSelected=lx;
					tTimeLine->StartTimeCursor=lx;
				} //lx>tcontrol->EndTimeSelected
			} //lx>tcontrol->StartTimeSelected
		} //direction>0
	
		//redraw control
	//	DrawFTControl(tcontrol);

		//adjust other data controls connected to timeline control
		tcontrol2=twin->icontrol;
		while(tcontrol2!=0) {
/*
			if (tcontrol2->type==CTData && tcontrol2->TimeLineFTControl==tTimeLine) {
				tcontrol2->StartTimeSelected=tTimeLine->StartTimeSelected;
				tcontrol2->EndTimeSelected=tTimeLine->EndTimeSelected;
				DrawFTControl(tcontrol2);
			} //tcontrol2->type==CTData
*/
			//update any StartTimeSelected controls connected to data 
			if (tcontrol2->StartTimeSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->StartTimeSelectedFTControl->text,tcontrol->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->StartTimeSelectedFTControl); //redraw SelectedTime control
			} //StartTimeSelectedFTControl
			//update any EndTimeSelected controls connected to data 
			if (tcontrol2->EndTimeSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->EndTimeSelectedFTControl->text,tcontrol->EndTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->EndTimeSelectedFTControl); //redraw SelectedTime control
			} //EndTimeSelectedFTControl
			//update any StartTimeSelected controls connected to data 
			if (tcontrol2->DurationSelectedFTControl) {
				//fprintf(stderr,"update start time label %s\n",tcontrol2->StartTimeSelectedFTControl->name);
				FT_FormatTime(tcontrol2->DurationSelectedFTControl->text,tcontrol->EndTimeSelected-tcontrol->StartTimeSelected,0,tTimeLine->TimeScale,tTimeLine->TimeSpace);
				//strcpy(,tstr);
				DrawFTControl(tcontrol2->DurationSelectedFTControl); //redraw SelectedTime control
			} //DurationSelectedFTControl

			tcontrol2=tcontrol2->next;
		} //tcontrol2!=0

	} //tTimeLine!=0

	DrawFTControl(tcontrol); //draw data control
} //FT_SelectData

//SetFolderTabDimensions
//set the x1,y1,x2,y2 for the FolderTab control button
void SetFolderTabDimensions(FTControl *tcontrol)
{
FTControl *tcontrol2,*tc;
FTWindow *twin;
int maxx2;

//Go through each FolderTab control on this FolderTabGroup
//and add up the widths to get the next
//if it doesn't fit add scroll bars if they don't already exist

//perhaps a better way is to use a linked list of FolderTab controls for each FolderTabGroup
if (tcontrol!=0) {
	tcontrol2=tcontrol->FolderTabGroup;
	if (tcontrol2!=0) { //there is a linked FolderTabGroup
		//todo: if this FolderTab has a FolderTabNumber then find the folder tab before it if any to determine it's x1
		//Go through all controls on this window and adjust it's (and the other's if necessary) x1 and x2
		twin=tcontrol->window;
		if (twin!=0) {
			//go through control and 
			maxx2=tcontrol2->x1;
			tc=twin->icontrol;
			while(tc!=0) {
				if (tc->type==CTFolderTab && tc->FolderTabGroup==tcontrol2 && tc!=tcontrol) {
					//get the maximum x2
					if (tc->x2>maxx2) {
						maxx2=tc->x2;
					} 
				}
				tc=(FTControl *)tc->next;
			}//while tc!=0
			//now add this folder tab after the farthest to the right
			tcontrol->x1=maxx2+1;
			tcontrol->y1=tcontrol2->y1; //presume foldertab starts at top of FolderTabGroup
			tcontrol->x2=tcontrol->x1+strlen(tcontrol->text)*(tcontrol->fontwidth)+2*PStatus.PadX;  //+padding on button
			if (tcontrol2->y3>0) {
				tcontrol->y2=tcontrol2->y3;
			} else {
				tcontrol->y2=tcontrol->y1+FT_DEFAULT_FOLDERTAB_HEIGHT;
			}
			tcontrol->cx1=tcontrol->x1;
			tcontrol->cy1=tcontrol->y1;
			tcontrol->cx2=tcontrol->x2;
			tcontrol->cy2=tcontrol->y2;
			//need to add horizontal scroll bar if tcontrol->x2>tcontrol->FolderTabGroup->x2
			if (tcontrol->x2>tcontrol->FolderTabGroup->x2) {
				tcontrol->FolderTabGroup->flags|=CHScroll;
			}

			//also set clip dimensions for now because DrawFTControls only draws Group control which then calls DrawButtonFTControls for Folder controls
		} //twin!=0
	} else {
			fprintf(stderr,"Error: Added a CTFolderTab control (%s) without any linked ->FolderTabGroup control\n",tcontrol->name);
	}//tcontrol2!=0
} //tcontrol!=0
} //void SetFolderTabDimensions(FTControl *tcontrol)


void printhelp(void)
{
fprintf(stderr,"\nParameters:\n");
fprintf(stderr,"-h,--help Show this menu\n");
fprintf(stderr,"-i,--info Show info (solve error mode)\n"); 
fprintf(stderr,"\n");
}//end printhelp


void mem_init_destination(j_compress_ptr cinfo) 
{ 
  struct jpeg_destination_mgr *dmgr = 
      (struct jpeg_destination_mgr *)(cinfo->dest);
  dmgr->next_output_byte = jpgdest;
  dmgr->free_in_buffer = jpgdestlen;
}

boolean mem_empty_output_buffer(j_compress_ptr cinfo)
{ 
    printf("jpeg mem overflow!\n");
    exit(1);
}

void mem_term_destination(j_compress_ptr cinfo) 
{ 
  struct jpeg_destination_mgr *dmgr = 
      (struct jpeg_destination_mgr *)(cinfo->dest);
  jpglen = jpgdestlen - dmgr->free_in_buffer;
  dmgr->free_in_buffer = 0;
}


void ConvertBMPtoJPGFile(unsigned char *bmp,char *name,int quality)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
FILE *jpgfile;
int offset1;
unsigned char *offset2;


jpgfile=fopen(name,"wb");
if (!jpgfile) {
	return;
}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=25;//100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


//jpeg_stdio_dest(&cinfo,jpgfile);
cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);


fwrite(jpgdest,1,jpglen,jpgfile);

fclose(jpgfile);
free(jpgdest);
} //void ConvertBMPtoJPGFile(u8 *bmp,char *name,int quality)


//Currently have to use global variables because of destination manager
void ConvertBMPtoJPG(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;




//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;

		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);


*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);
//*len=jpglen;

free(jpgdest);

//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPtoJPG(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)


//Currently have to use global variables because of destination manager
void ConvertBMPDatatoJPG(unsigned char *bdata,int quality,int w,int h,int BytesPerPixel,unsigned char *jpg,int *jlen)
{
//BITMAPINFOHEADER *bmi;
//unsigned char *bdata;
//int w,h,d;
	int d;
int x,y,line_width;//,quality;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;




//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

//bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
//bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
//w=bmi->biWidth ;
//h=bmi->biHeight;
d=BytesPerPixel;//bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=100;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,quality,1);
cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,TRUE);

row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);
	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);

//*len=jpglen;

*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);

free(jpgdest);
//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPDatatoJPG(unsigned char *bdata,int quality,int w,int h,int BytesPerPixel,unsigned char *jpg,int *jlen)


//Currently have to use global variables because of destination manager
//MakeMJPG - make an mjpg frame
void ConvertBMPtoMJPGFrame(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)
{
BITMAPINFOHEADER *bmi;
unsigned char *bdata;
int w,h,d;
int x,y,line_width;
JSAMPROW row_ptr[1];
struct jpeg_destination_mgr mgr;
struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *line;
unsigned char marker[60];
int i;
int offset1;
unsigned char *offset2;
//unsigned char *row;
//FILE *jpgfile;
//int *jpglen;
//u8 **jpgdata;

//jpgfile=fopen("C:\\temparea\\ted\\Projects\\VC\\Draw_Points\\test.jpg","wb");
//if (!jpgfile) {
//	return;
//}

bmi=(BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
bdata=(unsigned char *)(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
w=bmi->biWidth ;
h=bmi->biHeight;
d=bmi->biBitCount>>3;
//d=3;

//jpgdest=(u8 *)malloc(w*h*d);

line=(unsigned char *)malloc(w*d);
if (!line) {
	  return;
}
//quality=25;

memset(&cinfo, 0, sizeof(cinfo));
memset(&jerr, 0, sizeof(jerr));
memset(&mgr, 0, sizeof(mgr));

jpgdest=(unsigned char *)malloc(w*h*d);

cinfo.err=jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);


mgr.init_destination = mem_init_destination;
mgr.empty_output_buffer = mem_empty_output_buffer;
mgr.term_destination = mem_term_destination;
cinfo.dest = &mgr;


cinfo.image_width=w;
cinfo.image_height=h;
cinfo.input_components=3;
cinfo.in_color_space=JCS_RGB;

jpeg_set_defaults(&cinfo);
jpeg_set_colorspace (&cinfo,JCS_YCbCr);
jpeg_set_quality(&cinfo,quality,TRUE);

	cinfo.dc_huff_tbl_ptrs[0]->sent_table = TRUE;
	cinfo.dc_huff_tbl_ptrs[1]->sent_table = TRUE;
	cinfo.ac_huff_tbl_ptrs[0]->sent_table = TRUE;
	cinfo.ac_huff_tbl_ptrs[1]->sent_table = TRUE;

	cinfo.comp_info[0].component_id = 0;
	cinfo.comp_info[0].v_samp_factor = 1;
	cinfo.comp_info[1].component_id = 1;
	cinfo.comp_info[2].component_id = 2;

	cinfo.write_JFIF_header = FALSE;

//cinfo.dct_method=JDCT_FASTEST;
//jpeg_stdio_dest(&cinfo,jpg);

jpeg_start_compress(&cinfo,FALSE);



i=0;
	marker[i++] = 'A';
	marker[i++] = 'V';
	marker[i++] = 'I';
	marker[i++] = '1';
	marker[i++] = 0;
	while (i<60)
		marker[i++] = 32;

	jpeg_write_marker (&cinfo, JPEG_APP0, marker, 60);

	i=0;
	while (i<60)
		marker[i++] = 0;

	jpeg_write_marker (&cinfo, JPEG_COM, marker, 60);



row_ptr[0]=line;
line_width=w*3;
for(y=0;y<h;y++) {
//	 row= &bdata[w*3*y];
	//memcpy(line,bdata+y*w*d,w*d);
	for(x=0;x<w;x++) {
		//memcpy(line+x*3,bdata+x*d+y*w*d,3);
		offset1=x*3;
		offset2=bdata+x*d+y*w*d;
		line[offset1]=*(offset2+2);
		line[offset1+1]=*(offset2+1);
		line[offset1+2]=*(offset2);

	} //x
	jpeg_write_scanlines(&cinfo,row_ptr,1);
	// jpeg_write_scanlines(&cinfo,&row,1);
	//bdata += line_width;
} //y
jpeg_finish_compress(&cinfo);


jpeg_destroy_compress(&cinfo);
free(line);

//*len=jpglen;

*jlen=jpglen;
memcpy(jpg,jpgdest,jpglen);

free(jpgdest);

//fwrite(*jpgdata,1,*jpglen,jpgfile);
//fwrite(jpgdest,1,jpglen,jpgfile);

//fclose(jpgfile);
//free(jpgdest);
} //void ConvertBMPtoMJPGFrame(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen)



