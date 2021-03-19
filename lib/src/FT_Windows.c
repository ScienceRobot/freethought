//FT_Windows.c
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
// FT_Windows.c: FreeThought Library Windows functions.
//

#include "freethought.h"
#include "FT_Windows.h"
#include "FT_Controls.h"
#include "FT_UserInput.h"

extern ProgramStatus PStatus;
#if Linux
extern Atom XA_WM_DELETE_WINDOW;
#endif

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
		twin->color=FT_WINDOW_GRAY;
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
  WinAttr.override_redirect=1; //True;
//WinAttr.event_mask =(ButtonPressMask|ButtonReleaseMask|KeyPressMask|ButtonMotionMask|ExposureMask|PointerMotionMask);
	WinAttr.event_mask = xeventmask; //WinMask;

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
void ResizeFTWindow(FTWindow *twin) 
{

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
	  XSetForeground(PStatus.xdisplay,twin->xgc,FT_WINDOW_GRAY16);
	} else {
	  XSetForeground(PStatus.xdisplay,twin->xgc,FT_WINDOW_GRAY);
	}

//for some reason this is not getting drawn immediately
XFillRectangle(PStatus.xdisplay,twin->xwindow,twin->xgc,0,0,twin->w,twin->h);
#endif
#if WIN32

//PAINTSTRUCT lpPaint;
//HDC hdcWin;
//hdcWin=BeginPaint(twin->hwindow,&lpPaint);
//winbrush=CreateSolidBrush(FT_WINDOW_GRAY); 
//SelectObject(hdcWin,winbrush);
//SelectObject(twin->hdcWin,winbrush);
SetBkMode(twin->hdcWin,OPAQUE);
twin->brush=CreateSolidBrush(FT_WINDOW_GRAY); 
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
//SetBkColor(twin->hdcWin,FT_WINDOW_GRAY);

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
if (PStatus.depth==16) {tcontrol->color[0]=FT_WINDOW_GRAY16;}  //background no press
else {tcontrol->color[0]=FT_WINDOW_GRAY;}  //background no press
tcontrol->color[1]=FT_BLACK;//black; //dkgray;  //text
tcontrol->color[2]=FT_GRAY;//blue;  //background (selected with mouse over or tab)
tcontrol->color[3]=FT_DARK_GRAY;  //selected with mouse button 0 down
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
tcontrol->color[0]=FT_WINDOW_GRAY;  //background no press
tcontrol->color[1]=FT_BLACK;//dkgray;  //text
tcontrol->color[2]=FT_GRAY;//blue;  //background (mouse over)
tcontrol->color[3]=FT_DARK_GRAY;  //bkground press
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

