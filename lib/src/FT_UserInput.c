//FT_UserInput.c
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
// FT_UserInput.c: FreeThought Library User Input functions.
//

#include "freethought.h"
#include "FT_Windows.h"
#include "FT_Controls.h"
#include "FT_Utils.h"
#include "FT_UserInput.h"
#include <math.h> //for fmodl

extern ProgramStatus PStatus;
#if Linux
//time_t ttime;
extern Atom XA_CLIPBOARD,XA_UTF8,XA_UNICODE,XA_WM_PROTOCOLS,XA_WM_DELETE_WINDOW,XA_WM_TAKE_FOCUS,XA_MOTIF_WM_HINTS,XA_WM_EXTENDED_HINTS;
extern Atom XA_TASKBAR,XA_NET_WM_WINDOW_TYPE,XA_NET_WM_WINDOW_TYPE_SPLASH,XA_NET_WM_WINDOW_TYPE_DIALOG,XA_NET_WM_WINDOW_TYPE_TOOLBAR,XA_NET_WM_WINDOW_TYPE_UTILITY,XA_NET_WM_WINDOW_TYPE_NORMAL,XA_NET_WM_WINDOW_TYPE_MENU,XA_NET_WM_STATE_SKIP_TASKBAR,XA_NET_WM_STATE;
#endif

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
int SelectFolderTab(FTControl *tcontrol) 
{
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


//Check Window hot keys
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



//Check control hot keys
//key does not need to have focus to match a hotkey at this level
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

#if 0 
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
#endif 

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

//store button location (for vertical scroll bar buttons)
PStatus.ButtonX[button] = x;
PStatus.ButtonY[button] = y;

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

		//store difference between button down location and control X1 and Y1 (for vertical scroll bar buttons)
		PStatus.DeltaButtonX[button] = x-tcontrol->x1;
		PStatus.DeltaButtonY[button] = y-tcontrol->y1;


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
					ScrollFTItemList(twin, tcontrol, FT_SCROLL_FTITEMLIST_UP | FT_SCROLL_FTITEMLIST_MOUSEWHEEL,3);  //scroll up, do not scroll selection too
				} else { 
					if (Delta<0) {//Delta <0 - scroll down
						ScrollFTItemList(twin, tcontrol, FT_SCROLL_FTITEMLIST_DOWN | FT_SCROLL_FTITEMLIST_MOUSEWHEEL,3);  //scroll down, do not scroll selection too
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
	ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_DOWN|FT_SCROLL_FTITEMLIST_SELECTION,1);  //scroll down, scroll selection too
break;
case XK_Up:
	ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_UP|FT_SCROLL_FTITEMLIST_SELECTION,1);  //scroll up, scroll selection too
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
  ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_PAGE_DOWN|FT_SCROLL_FTITEMLIST_SELECTION,3);  //scroll down, scroll selection too
break;
case XK_Page_Up:
  ScrollFTItemList(twin,tcontrol,FT_SCROLL_FTITEMLIST_PAGE_UP|FT_SCROLL_FTITEMLIST_SELECTION,3);  //scroll up, scroll selection too
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


//************************
//SCROLLBAR FUNCTIONS
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
			//UpdateItemList(tc);  
			UpdateVScrollSlide(tc); //update the slider position (and size if necessary)
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
			//UpdateItemList(tc);
			UpdateVScrollSlide(tc); //update the slider position (and size if necessary)
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
			ScrollFTItemList(twin,tc,FT_SCROLL_FTITEMLIST_PAGE_DOWN,0);
		}
		if (y<tslide->y1) {
			ScrollFTItemList(twin,tc,FT_SCROLL_FTITEMLIST_PAGE_UP,0);  
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
int tnum,bh,miny,maxy,numentries,totalentries,fh,ScrollInc,NoRedraw;//bw,bh;
unsigned int flags;
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

//difference in mouse move y position - the problem with this method is that some times this code will not get called when the mouse has moved and 
//so the scrollbar button will appear to be stuck and unmovable- because the change in mouse position is not being detected- for a slow mouse move, the change happens too quickly.
  //tnum=(y-PStatus.LastMouseY);
	tnum = y-tback->y1-PStatus.DeltaButtonY[0]; //current mouse y- top Y of scrollbar background - position of initial mouse button down with control top y



//  bw=(tcontrol->x2-tcontrol->x1);
  bh=(tslide->y2-tslide->y1);
  
   //add mouse offset

//fprintf(stderr,"foOpenFile2_MouseMove tnum=%d\n",tnum);

//possible timesaver (needed because scroll bar is slow)=
//no erase, just draw two FTControls

//  EraseFTControl(twin,tcontrol);

	if (tc->type==CTFileOpen || tc->type==CTItemList) {
	//move scroll slider control

		NoRedraw = 0;
		//if (tnum > 0) {
		if (tnum < 0) {
			tnum = 0;
			if (tc->FirstItemShowing==1) {  //already at first item
				NoRedraw = 1;
			}
		}
		//determine number of items to move and send to ScrollItemList()
		//calculate ratio of pixels moved to size of scroll bar
		//ratio = (float)tnum/(float)(tback->y2 - tback->y1-bh);
		//ScrollInc = (int)((float)(tnum *(tc->ilist->num + 1)))/ (float)(tback->y2 - tback->y1);
		ratio = fabs((float)tnum) / (float)(tback->y2 - tback->y1); //tnum is how much the mouse has moved in TOTAL since the mouse 0 button was first pressed, 
		//ratio = (float)tnum / (float)(tback->y2 - tback->y1 - bh); //tnum is how much the mouse has moved in TOTAL since the mouse 0 button was first pressed, 
		//ratio is the fraction of that distance compared to the total size of the scroll bar back
		//ratio = fabs((float)tnum) / (float)(tback->y2 - PStatus.ButtonY[0]); //bottom of control minus the Y position on the slide that the mouse button was first pressed down on
		//ScrollInc = (int)(ratio*(tc->ilist->num + 1)); //ScrollInc is the number of items to scroll from item 0
		ScrollInc = (int)(ratio*(tc->ilist->num + 1));
		if (ScrollInc < 1) {
			ScrollInc = 1;
			if (tc->FirstItemShowing == 1) {  //already at first item
				NoRedraw = 1;
			}
		}
		if (ScrollInc > (tc->ilist->num - tc->numvisible + 2)) {
			ScrollInc = tc->ilist->num - tc->numvisible + 1;
			if (tc->FirstItemShowing == tc->ilist->num - tc->numvisible + 1) { //already at last item
				NoRedraw = 1;
			}
		}
		if (!NoRedraw) {
 			//if (ScrollInc >= 1 && ScrollInc < (tc->ilist->num-tc->numvisible+2) ) {
			tc->FirstItemShowing = ScrollInc;
			//just update scrollbar here
			if (tc->flags&CVScroll) {
				UpdateVScrollSlide(tc);  //need to update location of scroll slide button
			}
			DrawFTControl(tc); //DrawSelectedFTItem for FileOpen doesn't currently work well
		} //if (!NoRedraw) {
													 //} //if (ScrollInc >= 0 && ScrollInc < (tc->ilist->num-tc->numvisible) ) {
		//} //if (tnum>0)
	} //if (tc->type==CTFileOpen || tc->type==CTItemList) {

	//	fprintf(stderr,"tcontrol=%s\n",tcontrol->name);

	//todo: most likely need to copy and adapt aboveCFileOpen and CItelList code to here for CTFrame
  if (tc->type == CTFrame) {
	  //	fprintf(stderr,"tnum=%d\n",tnum);
		  //move all frame controls by same amount as mouse times the ratio of controls to tback
	  //possibly adapt one of the other Vscroll functions.

	  if (tslide == 0 || tback == 0) {
		  return;
	  } //tslide==0
  //move scroll slider control
	  if (tnum > 0) {  //going down
		  if (tslide->y2 + tnum > tback->y2) {
			  tnum = tback->y2 - tslide->y2;//mouse move larger than available space
			  tslide->y2 = tback->y2;
			  tslide->y1 = tback->y2 - bh;
		  }
		  else {
			  tslide->y1 += tnum;
			  tslide->y2 += tnum;
			  //fprintf(stderr,"added tcontrol->y1=%d\n",tcontrol->y1);
		  }
	  }
	  else {  //going up
		  if (tslide->y1 + tnum < tback->y1) {
			  tnum = tslide->y1 - tback->y1;//mouse move larger than available space
			  tslide->y1 = tback->y1;
			  tslide->y2 = tback->y1 + bh;
		  }
		  else {
			  tslide->y1 += tnum;
			  tslide->y2 += tnum;
		  } //tslide->y1+
	  } //tnum>0

		//find size of controls
	  miny = tc->y1;
	  maxy = tc->y2;
	  tcontrol3 = twin->icontrol;
	  while (tcontrol3 != 0) {
		  if (tcontrol3->FrameFTControl == tc && !(tcontrol3->flags&CNotVisible)) {
			  //EraseFTControl?
			  tlabel = tcontrol3->DataLabelFTControl;
			  if (tlabel != 0) {
				  if (!(tlabel->flags2&CDoNotScrollY)) {
					  if (tcontrol3->y1 < miny) {
						  miny = tcontrol3->y1;
					  }
					  if (tcontrol3->y2 > maxy) {
						  maxy = tcontrol3->y2;
					  }
				  } //tlabel-?flags2
			  } //CDoNotScrollY
		  } //if tcontrol3->FrameFTControl			
		  tcontrol3 = tcontrol3->next;
	  } //while(tcontrol3!=0)

	  //find ratio of controls to tback
	  ratio = (float)(maxy - miny) / (float)(tback->y2 - tback->y1);

	  //scroll any controls by the same amount
	  tcontrol3 = twin->icontrol;
	  while (tcontrol3 != 0) {
		  if (tcontrol3->FrameFTControl == tc && !(tcontrol3->flags&CNotVisible)) {
			  //EraseFTControl?

			  if (!(tcontrol3->flags2&CDoNotScrollY)) {
				  tcontrol3->y1 -= (int)((float)tnum*ratio);
				  tcontrol3->y2 -= (int)((float)tnum*ratio);

				  //also scroll any connected label control
				  if (tcontrol3->DataLabelFTControl != 0) {
					  tlabel = tcontrol3->DataLabelFTControl;
					  if (tlabel != 0) {
						  if (!(tlabel->flags2&CDoNotScrollY)) {
							  tlabel->y1 -= (int)((float)tnum*ratio);
							  tlabel->y2 -= (int)((float)tnum*ratio);
						  } //tlabel->flags
					  } //tlabel!=0
				  } //tcontrol3

			  } //tcontrol->flags2&CDoNotScrollY

		  } //if tcontrol3->FrameFTControl			
		  tcontrol3 = tcontrol3->next;
	  } //while(tcontrol3!=0)

	  DrawFTControl(tc); //redraw frame controls
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


//Calculate position and size of vertical scroll slider button.
//This func will change the size of the slider. To just update the position of the slider of a FileOpen control use UpdateVScrollSlide() below.
//Position based on:
//FileOpen or ItemList: ((FirstItemShowing-1)/ilist->num) {number of items}
void CalcVScrollSlide(FTWindow *twin,FTControl *tcontrol)
{
#define MIN_VSCROLL_SLIDE_HEIGHT 10

float pos,ratio;
FTControl *tcontrol2,*tslide,*tback;
//char tstr[FTMedStr];
int miny,maxy;//,ipos;
int slideheight,minslideheight,slidediff;

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
	  ratio = (float)tcontrol->numvisible / (float)tcontrol->ilist->num;
	  if (ratio>1.0) {
//      fprintf(stderr,"here0\n");
		//slider is full size, same as back
	    tslide->y1=tback->y1;
	    tslide->y2=tback->y2;
	  }  else {
		  //slider is not full size
		  pos = (float)(tcontrol->FirstItemShowing - 1) / (float)tcontrol->ilist->num;
		  //    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
		  //position may change depending on dragging scroll bar sliding button (tslide->y3 is any added extra height to the slider needed because there are so many files the actual ratio of files to control vertical scroll bar height would make the slider too small)
		  tslide->y1 = tback->y1 + ceil((((float)(tback->y2 - tback->y1))*pos));

		  //now calculate y2 (size) of scroll bar button

//      pos=(float)tcontrol->CursorLocationInChars/(float)tcontrol->ilist->num;
//    fprintf(stderr,"ratio=%f pos=%f\n",ratio,pos);
//position may change depending on dragging scroll bar sliding button
//      tcontrol2->y1=tcontrol3->y1+((float)(tcontrol3->y2-tcontrol3->y1))*pos;
	    tslide->y2=tslide->y1+ceil(((float)(tback->y2-tback->y1))*ratio);//-1;//+1; //+1
//    fprintf(stderr,"y1=%d y2=%d\n",tcontrol2->y1,tcontrol2->y2);
//    fprintf(stderr,"numvis=%d num=%d\n",tcontrol->numvisible,tcontrol->ilist->num);

//keep slider from getting too small:
		if (tslide->y2 - tslide->y1 < MIN_VSCROLL_SLIDE_HEIGHT) {
			slidediff = MIN_VSCROLL_SLIDE_HEIGHT - (tslide->y2 - tslide->y1);
			tslide->y3 = slidediff; //store extra height for other functions to use
//			tslide->y2 = tslide->y1 + ((float)(tback->y2 - tback->y1-slidediff))*ratio;//update ratio because slider is larger			
			tslide->y2 = tslide->y1 + MIN_VSCROLL_SLIDE_HEIGHT;
			if (tslide->y2 > tback->y2) {
				tslide->y2 = tback->y2;
				tslide->y1 = tslide->y2 - MIN_VSCROLL_SLIDE_HEIGHT;
				if (tslide->y1 < tback->y1) {
					tslide->y1 = tback->y1;
				}
			}
			
		}


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
		pos=tback->y1+ceil((float)(tcontrol->y1-miny)*ratio);//PStatus.ScrollPixelsY;
		slideheight=ceil(ratio*(float)(tback->y2-tback->y1));
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
//#define MIN_VSCROLL_SLIDE_HEIGHT 10

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
		//tslide->y1=tback->y1+((float)(tback->y2-tback->y1))*pos;
		tslide->y1 = tback->y1 + ceil(((float)(tback->y2 - tback->y1 - tslide->y3))*pos);  //tslide->y3 is any extra height that was added because slider would be too smal lin height
		//tslide->y1 = tback->y1 + (((float)(tback->y2 - tback->y1))*pos) - tslide->y3;  //tslide->y3 is any extra height that was added because slider would be too smal lin height
		//tslide->y1 = tback->y1 + ((float)(tback->y2 - tback->y1-tslide->y3))*pos;  //tslide->y3 is extra height that needed to be added because slider would be too small in height
		tslide->y2=tslide->y1+bh;

		//do not adjust slide height in this function - Use CalcVScrollSlide for that
/*
		//keep slider from getting too small:
		if ((tslide->y2 - tslide->y1) < MIN_VSCROLL_SLIDE_HEIGHT) {
			tslide-y3=
			tslide->y2 = tslide->y1 + MIN_VSCROLL_SLIDE_HEIGHT;
		}
		*/
/*
		//stop slider from going too low
		if (tslide->y2 > tback->y2) {
			tslide->y2 = tback->y2;
			tslide->y1 = tslide->y2 - MIN_VSCROLL_SLIDE_HEIGHT;
			if (tslide->y1 < tback->y1) {
				tslide->y1 = tback->y1;
			}
		}
*/

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
/* Use UpdateVScroll
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
*/


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
void AdjustDataFTControlDimensions(FTControl *tcontrol) 
{
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
void FT_AdvanceTimeLine(FTWindow *twin,FTControl *tTimeLine,int x,int y) 
{
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
void FT_SelectData(FTWindow *twin,FTControl *tcontrol,int x,int y) 
{
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

