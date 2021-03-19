//FT_Controls.c
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
// FT_Controls.c: FreeThought Library Controls functions.
//

#include "freethought.h"
#include "FT_Controls.h"
#include "FT_UserInput.h"
#include "FT_Utils.h"
#include <fcntl.h>  //O_RDONLY
#if Linux
#include <sys/stat.h>   //S_ISDIR
#endif
extern ProgramStatus PStatus;


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
		if ((tcontrol->color[0]==tcontrol->color[1] || tcontrol->textcolor[0]==tcontrol->textcolor[1]) && 
			!(tcontrol->type==CTButton && ((tcontrol->flags&CVScrollSlide) || (tcontrol->flags&CVScrollBack) || (tcontrol->flags&CVScrollUp) || (tcontrol->flags&CVScrollDown) || (tcontrol->flags&CHScrollSlide) || (tcontrol->flags&CHScrollBack) || (tcontrol->flags&CHScrollLeft) || (tcontrol->flags&CHScrollRight)))) {
			tcontrol->flags|=CSetDefaultColors;
		}
		
		if (tcontrol->flags&CSetDefaultColors) {

			if (tcontrol->type==CTButton) {
				tcontrol->ControlBackgroundColor=FT_LIGHTER_GRAY;  //background no press
				tcontrol->ControlButtonDownColor=FT_GRAY;  //button down color
				tcontrol->ControlSelectedBackgroundColor=FT_GRAY;  //background (selected or mouse over)
				tcontrol->ControlBorderColor=FT_GRAY;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK; 
					tcontrol->textcolor[1]=FT_WHITE; 
					tcontrol->textcolor[2]=FT_BLUE; 
				} 
			}  //CTButton


			if (tcontrol->type==CTLabel) {
				tcontrol->ControlBackgroundColor=FT_WINDOW_GRAY;
				tcontrol->ControlButtonDownColor=FT_GRAY;
				tcontrol->ControlSelectedBackgroundColor=FT_GRAY;
				tcontrol->ControlBorderColor=FT_GRAY;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK;//~tcontrol->textcolor[0];
					tcontrol->textcolor[1]=FT_WINDOW_GRAY;//~tcontrol->textcolor[0];
					tcontrol->textcolor[2]=FT_BLUE;//~tcontrol->textcolor[0];
				} //if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
			}

			if (tcontrol->type==CTTextBox || tcontrol->type==CTTextArea) {
				tcontrol->ControlBackgroundColor=FT_WHITE;//ControlBackgroundColor ~tcontrol->textcolor[0];
				tcontrol->ControlBorderColor=FT_BLACK;	//color[1] is ControlBorderColor
				tcontrol->ControlSelectedBackgroundColor=FT_BLUE;
				tcontrol->ControlBorderColor=FT_GRAY;

				//tcontrol->TextColor=FT_BLACK;//~tcontrol->textcolor[0];
				tcontrol->TextBackgroundColor=FT_WHITE;//~tcontrol->textcolor[0];
				//tcontrol->textcolor[2]=FT_BLUE; //selected text color PStatus->DefaultSelectedTextColor
				//tcontrol->textcolor[3]=~tcontrol->textcolor[2];
				tcontrol->SelectedTextColor=FT_WHITE; 
				tcontrol->SelectedTextBackgroundColor=FT_BLUE;
			}

			if (tcontrol->type==CTFileMenu) {
				tcontrol->ControlBackgroundColor=FT_LIGHTER_GRAY;  //background no press
				tcontrol->ControlButtonDownColor=FT_BLACK;
				tcontrol->ControlSelectedBackgroundColor=FT_LIGHT_BLUE;//dkgray;  //background (mouse over)
				tcontrol->ControlMouseOverBackgroundColor=FT_LIGHT_GRAY;  //bkground press
				tcontrol->ControlBorderColor=FT_GRAY;
				
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK;//~tcontrol->textcolor[0]; control and item text color
					tcontrol->textcolor[1]=FT_WHITE;//only item text background color- control text has control color as background
					tcontrol->textcolor[2]=FT_BLUE;//~tcontrol->textcolor[0];
				}
			} //CTFileMenu

			if (tcontrol->type==CTDropdown || tcontrol->type==CTItemList) {
				tcontrol->ControlBackgroundColor=FT_LIGHTER_GRAY;  //background no press-Dropdown button
				tcontrol->ControlButtonDownColor=FT_GRAY;  //control - button down
				tcontrol->ControlSelectedBackgroundColor=FT_BLUE;  //background (mouse over) - color behind triangle
				tcontrol->ControlMouseOverBackgroundColor=FT_LIGHT_GRAY;;  //bkground press
				tcontrol->ControlBorderColor=FT_GRAY;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK;//~tcontrol->textcolor[0];
					tcontrol->textcolor[1]=FT_WHITE;//text in comb background color
					tcontrol->textcolor[2]=FT_BLUE;//~tcontrol->textcolor[0];
				} 
			} //CTDropdown

			if (tcontrol->type==CTOption) {
				tcontrol->ControlBackgroundColor=FT_WINDOW_GRAY;
				tcontrol->ControlButtonDownColor=FT_GRAY;
				tcontrol->ControlSelectedBackgroundColor=FT_LIGHT_GRAY;
				tcontrol->ControlBorderColor=FT_GRAY;
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK; 
					tcontrol->textcolor[1]=FT_WINDOW_GRAY; 
					tcontrol->textcolor[2]=FT_BLUE; 
				} 
			}  //CTOption

			if (tcontrol->type==CTCheckBox) {
				tcontrol->ControlBackgroundColor=FT_LIGHTER_GRAY;  //background no press
				tcontrol->ControlButtonDownColor=FT_GRAY;  //button down color
				tcontrol->ControlSelectedBackgroundColor=FT_LIGHT_GRAY;  //background (selected or mouse over)
				tcontrol->ControlMouseOverBackgroundColor=FT_LIGHT_GRAY;  //?
				tcontrol->ControlBorderColor=FT_GRAY;
				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK; 
					tcontrol->textcolor[1]=FT_WINDOW_GRAY; 
					tcontrol->textcolor[2]=FT_BLUE; 
				} 
			}  //CTCheckBox

			if (tcontrol->type==CTFolderTabGroup) {
				tcontrol->ControlBackgroundColor=FT_WINDOW_GRAY;  //background color
				tcontrol->ControlButtonDownColor=FT_LIGHTER_GRAY;//button down background color
				tcontrol->ControlSelectedBackgroundColor=FT_LIGHT_GRAY;//?
				tcontrol->ControlMouseOverBackgroundColor=FT_GRAY;  //?
				tcontrol->ControlBorderColor=FT_GRAY;
				tcontrol->ControlShadedBackgroundColor=FT_LIGHTER_GRAY;

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK;//~tcontrol->textcolor[0]; control and item text color
					tcontrol->textcolor[1]=FT_WHITE;//only item text background color- control text has control color as background
					tcontrol->textcolor[2]=FT_BLUE;//~tcontrol->textcolor[0];
				} 
			} //if (tcontrol->type==CTFolderTabGroup || tcontrol->type==CTFolderTab) {
			if (tcontrol->type==CTFolderTab) {  //foldertab button
				tcontrol->ControlBackgroundColor=FT_LIGHTER_GRAY;  //background no press
				tcontrol->ControlButtonDownColor=FT_GRAY;  //button down color
				tcontrol->ControlSelectedBackgroundColor=FT_LIGHT_GRAY;  //background (selected or mouse over)

				if (tcontrol->textcolor[0]==0 && tcontrol->textcolor[1]==0 && tcontrol->textcolor[2]==0) {
					tcontrol->textcolor[0]=FT_BLACK; 
					tcontrol->textcolor[1]=FT_WHITE; 
					tcontrol->textcolor[2]=FT_BLUE; 
				} 
				tcontrol->ControlBorderColor=FT_GRAY;
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
			tcontrol->SelectedTextBackgroundColor=FT_BLUE;
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
		tcontrol2->color[0]=FT_GRAY;
		tcontrol2->color[1]=FT_LIGHTER_GRAY;
		tcontrol2->color[2]=FT_DARK_GRAY;
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
		tcontrol2->color[0]=FT_GRAY;
		tcontrol2->color[1]=FT_LIGHTER_GRAY;
		tcontrol2->color[2]=FT_DARK_GRAY;
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
		tcontrol2->color[0]=FT_GRAY;
		tcontrol2->color[1]=FT_LIGHTER_GRAY;
		tcontrol2->color[2]=FT_DARK_GRAY;
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
	tcontrol2->color[0]=FT_LIGHTER_GRAY;
	tcontrol2->color[1]=FT_BLACK;
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
	tcontrol2->color[0]=FT_LIGHTER_GRAY;
	tcontrol2->color[1]=FT_BLACK;
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
	tcontrol2->color[0]=FT_GRAY;
	tcontrol2->color[1]=FT_DARK_GRAY;
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
	tcontrol2->flags=CNoChangeOnMouseOver|CDoNotShowText|CNoColorChangeButton(0)|CVScrollSlide|CNoTab;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX1|CGrowX2;
	}
	//tcontrol2->color[0]=FT_LIGHTER_GRAY;
	//tcontrol2->color[1]=FT_LIGHTER_GRAY; //buttondown
	//tcontrol2->color[2]=FT_LIGHTER_GRAY; //mouseover
	tcontrol2->ControlBackgroundColor = FT_LIGHTER_GRAY;
	tcontrol2->ControlButtonDownColor = FT_LIGHTER_GRAY;  //button down color
	tcontrol2->ControlSelectedBackgroundColor = FT_LIGHTER_GRAY;  //background (selected or mouse over)
	tcontrol2->ControlMouseOverBackgroundColor = FT_LIGHTER_GRAY;
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
	tcontrol2->color[0]=FT_LIGHTER_GRAY;
	tcontrol2->color[1]=FT_BLACK;
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

	tcontrol2->color[0]=FT_LIGHTER_GRAY;
	tcontrol2->color[1]=FT_BLACK;
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

	tcontrol2->color[0]=FT_GRAY;
	tcontrol2->color[1]=FT_DARK_GRAY;
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
	tcontrol2->flags= CNoChangeOnMouseOver|CDoNotShowText|CNoColorChangeButton(0)|CHScrollSlide;
	if (tcontrol->flags&CGrowY2) {
		tcontrol2->flags|=CGrowY1|CGrowY2;
	}
	if (tcontrol->flags&CGrowX2) {
		tcontrol2->flags|=CGrowX2;
	}
	//tcontrol2->color[0]=FT_LIGHTER_GRAY;
	//tcontrol2->color[1]=FT_LIGHTER_GRAY; //buttondown
	//tcontrol2->color[2]=FT_LIGHTER_GRAY; //mouseover
	tcontrol2->ControlBackgroundColor = FT_LIGHTER_GRAY;
	tcontrol2->ControlButtonDownColor = FT_LIGHTER_GRAY;  //button down color
	tcontrol2->ControlSelectedBackgroundColor = FT_LIGHTER_GRAY;  //background (selected or mouse over)
	tcontrol2->ControlMouseOverBackgroundColor = FT_LIGHTER_GRAY;
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
void OpenNextFileControl(FTControl *tcontrol) 
{
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
void OpenPrevFileControl(FTControl *tcontrol) 
{
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
	XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);
	if (tlist->flags&FTIL_IS_IN_A_CONTROL) {  //only fill control
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-tcontrol->x1,tcontrol->y2-tcontrol->y1);
	} else {  //fill entire window
		XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,0,twin->w,twin->h);
	}
	XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLUE);  
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,tcontrol->x2-tcontrol->x1,fh+PStatus.PadY);			
			} else {	
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			}
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);  
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
			SetDCBrushColor(twin->hdcWin,FT_BLUE);
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
			//twin->pen=CreatePen(PS_SOLID,1,FT_BLACK);
			//SelectObject(twin->hdcWin,twin->pen);
			SelectObject(twin->hdcWin,GetStockObject(WHITE_PEN)); //so line will be white too
			SetTextColor(twin->hdcWin,FT_WHITE);
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
//			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);  
//			XSetBackground(PStatus.xdisplay,twin->xgc,FT_WHITE);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);  
			//XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,cx,cy-fh,tlist->width,fh+PStatus.PadY);  //width may be twin->w or tcontrol width
			if (tlist->flags&FTIL_IS_IN_A_CONTROL) { //itemlist is in a control not a window
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,tlist->control->x2-tlist->control->x1,fh+PStatus.PadY);			
			} else {	
				XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			}

			//XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,cy-fh,twin->w,fh+PStatus.PadY);
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);  
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
			//SetDCBrushColor(twin->hdcWin,FT_WHITE);
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
			//twin->pen=CreatePen(PS_SOLID,1,FT_BLACK);  //polyline is done with pen
			//SelectObject(twin->hdcWin,twin->pen);
			SelectObject(twin->hdcWin,GetStockObject(BLACK_BRUSH));  
			SelectObject(twin->hdcWin,GetStockObject(BLACK_PEN));
			SetTextColor(twin->hdcWin,FT_BLACK);
			//SetBkColor(twin->hdcWin,FT_WHITE);	
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
//			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);  
//			XSetBackground(PStatus.xdisplay,twin->xgc,FT_WHITE);


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
//void ScrollFTItemList(FTWindow *twin,FTControl *tcontrol, unsigned int flags)
void ScrollFTItemList(FTWindow *twin, FTControl *tcontrol, unsigned int flags, int ScrollInc)
{
	FTItemList *tlist;
	FTItem *litem;
	char winname[FTMedStr];
	char conname[FTMedStr];
	//int ScrollInc;


	strcpy(winname,twin->name);
	strcpy(conname,tcontrol->name);


	//it's important to make clear that scrolling down is viewed here as scrolling the viewable itemlist entries down (although the actual items appear to scroll up- so it can be confusing)
	//so, for example pressing the down arrow key is scrolling down, the up arrow key is scrolling up

	//todo: FirstItemShowing should =1 when the first item is showing
	//tcontrol=CTFileopen, or CTItemList

	if (flags&FT_SCROLL_FTITEMLIST_DOWN) {  //for example the down arrow key, or mouse wheel
		if (tcontrol->ilist!=0) {
/*
			if (flags&FT_SCROLL_FTITEMLIST_MOUSEWHEEL) {
				ScrollInc = 3; //for mousewheel scroll 3 items at a time
			}
			else {
				ScrollInc = 1; //otherwise only scroll 1 item at a time
			}
*/
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
			} else { //if (!(flags&FT_SCROLL_FTITEMLIST_SELECTION)) { //only scrolling itemlist- not selected item(s)
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
				if (tcontrol->flags&CVScroll) {  //need to update slider
					UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button					
				}
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
/*
			if (flags&FT_SCROLL_FTITEMLIST_MOUSEWHEEL) {
				ScrollInc = 3; //for mousewhell scroll 3 items at a time
			}
			else {
				ScrollInc = 1; //otherwise only scroll 1 item at a time
			}
*/
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
					if (tcontrol->flags&CVScroll) {  //need to update slider
						UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
					}
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
			UpdateVScrollSlide(tcontrol);  //need to update location of scroll slide button
			// CalcVScrollSlide(twin,tcontrol);  //need to update location of scroll slide button
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
			//removed: this is done in CloseFTItemlist, where text2 is checked for an actual change, and then updated 
			//possibly just exclude dropdown itemlist controls here?
			//if (tlist->control->OnChange!=0) {
			//	(*tlist->control->OnChange) (tlist->window,tlist->control);
				//fprintf(stderr,"after onchange\n");
		//		tlist->control=GetFTControl(conname);
		//		tlist->window=GetFTWindow(winname);
		//		if (tlist->control==0 || tlist->window==0) return; //user deleted control or window
			//} //titem->control->OnChange
		} //tlist->window!=0 && tlist->control!=0


	} //titem!=0
} //tlist!=0
//call OnChange functions?

if (PStatus.flags&PInfo)
  fprintf(stderr,"Done FTItemButtonUp\n");

} //FTItemButtonUp


//returns the number of characters in a line starting from the first character
//tmpstr is the text, i is the CursorLocationInChars, maxwidth is the maximum num of characters on the line
int GetNumCharsInRowFromStart(char *tmpstr,int i,int maxwidth) 
{
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
int GetNumCharsInRowFromEnd(char *tmpstr,int i,int maxwidth) 
{
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
int FTDrawText(FTWindow *twin, char *tstr, int numc, int fw, int fh, int cx, int cy) 
{


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

} //int FTDrawText(FTWindow *twin, char c, int fw, int fh, int cx, int cy, int ul) {




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
			//SetDCBrushColor(twin->hdcWin,tcontrol->color[1]);
			SetDCBrushColor(twin->hdcWin, tcontrol->ControlButtonDownColor);
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
			//SetDCBrushColor(twin->hdcWin,tcontrol->color[0]);
			SetDCBrushColor(twin->hdcWin, tcontrol->ControlBackgroundColor);
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
				//SetDCBrushColor(twin->hdcWin,tcontrol->color[1]);
				SetDCBrushColor(twin->hdcWin, tcontrol->ControlButtonDownColor);
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
				XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,FT_WHITE);
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SelectObject(twin->hdcWin,GetStockObject(WHITE_PEN));
				tp[0].x=x1;	tp[0].y=y1;	tp[1].x=x2-1;	tp[1].y=y1;
				Polyline(twin->hdcWin,tp,2);
				tp[1].x=x1;	tp[1].y=y2-1;
				Polyline(twin->hdcWin,tp,2);
				//DeleteObject(tcontrol->pen);
				//tcontrol->pen=CreatePen(PS_SOLID,1,FT_BLACK);
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
				XSetForeground(PStatus.xdisplay,twin->xgc,FT_GRAY);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
				//SetDCBrushColor(twin->hdcWin,FT_GRAY);
				SetDCBrushColor(twin->hdcWin, tcontrol->ControlBackgroundColor);
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
				XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y1,x1,y2-1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x2-1,y1,x2-1,y1);
				XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,x1,y2-1,x2-1,y2-1);
#endif
#if WIN32
				//SetDCBrushColor(twin->hdcWin,FT_BLACK);
				SetDCBrushColor(twin->hdcWin, tcontrol->ControlBorderColor);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
          XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-1,tcontrol->y1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x1,tcontrol->y2-1);
          XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y2-1,tcontrol->x2-1,tcontrol->y2-1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x2-1,tcontrol->y1,tcontrol->x2-1,tcontrol->y2-1);
#endif
#if WIN32
		  //todo: implement for windows
					//tcontrol->pen=CreatePen(PS_SOLID,1,FT_WHITE); 
					//SelectObject(twin->hdcWin,tcontrol->pen);
					//tp[0].x=tcontrol->x1;	tp[0].y=tcontrol->y1-1;	tp[1].x=tcontrol->x2-1;	tp[1].y=tcontrol->y1-1;
					//Polyline(twin->hdcWin,tp,2);
					//tp[1].x=tcontrol->x1;	tp[1].y=tcontrol->y2;	
					//Polyline(twin->hdcWin,tp,2);
					//tcontrol->pen=CreatePen(PS_SOLID,1,FT_BLACK); 
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
//          XSetForeground(PStatus.xdisplay,twin->xgc,FT_WHITE);
          XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x2-1,tcontrol->y1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y1,tcontrol->x1,tcontrol->y2-1);


          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x1,tcontrol->y2-1,tcontrol->x2-1,tcontrol->y2-1);
          XDrawLine(PStatus.xdisplay,twin->xwindow,twin->xgc,tcontrol->x2-1,tcontrol->y1,tcontrol->x2-1,tcontrol->y2-1);
#endif
#if WIN32
					//tcontrol->pen=CreatePen(PS_SOLID,1,FT_BLACK); 
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
			XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
					XSetForeground(PStatus.xdisplay,twin->xgc,FT_BLACK);
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
				XSetForeground (PStatus.xdisplay,twin->xgc,FT_WINDOW_GRAY);  //selected text color - draw actual sample
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+((y2-y1)/2));
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,FT_WINDOW_GRAY); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,FT_WINDOW_GRAY);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));

				SetPixel(twin->hdcWin,i,y1+((y2-y1)/2),FT_WINDOW_GRAY);
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
				XSetForeground (PStatus.xdisplay,twin->xgc,FT_WINDOW_GRAY);  //selected text color
				XDrawPoint(PStatus.xdisplay,twin->xwindow,twin->xgc,i,y1+((y2-y1)/2));
#endif
#if WIN32
				//tcontrol->pen=CreatePen(PS_SOLID,1,FT_WINDOW_GRAY); 
				//SelectObject(twin->hdcWin,tcontrol->pen);
				SetDCPenColor(twin->hdcWin,FT_WINDOW_GRAY);
				SelectObject(twin->hdcWin,GetStockObject(DC_PEN));
				SetPixel(twin->hdcWin,i,y1+((y2-y1)/2),FT_WINDOW_GRAY);
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

