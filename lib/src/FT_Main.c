//FT_Main.c
//Freethought library main
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
// FT_Main.c: Main functions associated with FreeThought.
//

#include "freethought.h"
#include "FT_Main.h"
#include "FT_Windows.h"

#if WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Windows.h"
#endif  //WIN32



//I think drop PStatus.PInfo and just go with a macro
#define PINFO 0  //print info (debug)

//Global variables
ProgramStatus PStatus;

#if Linux
//time_t ttime;
Atom XA_CLIPBOARD,XA_UTF8,XA_UNICODE,XA_WM_PROTOCOLS,XA_WM_DELETE_WINDOW,XA_WM_TAKE_FOCUS,XA_MOTIF_WM_HINTS,XA_WM_EXTENDED_HINTS;
Atom XA_TASKBAR,XA_NET_WM_WINDOW_TYPE,XA_NET_WM_WINDOW_TYPE_SPLASH,XA_NET_WM_WINDOW_TYPE_DIALOG,XA_NET_WM_WINDOW_TYPE_TOOLBAR,XA_NET_WM_WINDOW_TYPE_UTILITY,XA_NET_WM_WINDOW_TYPE_NORMAL,XA_NET_WM_WINDOW_TYPE_MENU,XA_NET_WM_STATE_SKIP_TASKBAR,XA_NET_WM_STATE;
#endif

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
	

	//get root window width and weight
	PStatus.rw = GetDeviceCaps(GetDC(NULL), HORZRES); 
	PStatus.rh = GetDeviceCaps(GetDC(NULL), VERTRES); 


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

void printhelp(void)
{
fprintf(stderr,"\nParameters:\n");
fprintf(stderr,"-h,--help Show this menu\n");
fprintf(stderr,"-i,--info Show info (solve error mode)\n"); 
fprintf(stderr,"\n");
}//end printhelp

