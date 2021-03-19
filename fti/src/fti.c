//Freethought Interface

#if WIN32
#include "stdafx.h"
#endif

#include "freethought.h"
#include "FT_Windows.h"
#include "FT_Controls.h"
#include "FT_UserInput.h"
#include "win_ftimain.h"

//Global variables
extern ProgramStatus PStatus;
//FTControl *iFTControl;  //in theory unlimited number of FTControls
//FTWindow *iwindow;  //still used?

unsigned long xforeground,xbackground;
#if Linux
Colormap cmap;
XColor xcolor[256];
#endif
//int depth;
//Visual *visual;
//unsigned int *pmp;

/*
int *x_return, *y_return;
u32 *width_return, *height_return;
unsigned int *border_width_return;
unsigned int *depth_return;
*/

int samplerate=48000;


#if Linux
//run the Free thought interface program
int main(int argc, char *argv[])  //argument count and values
#endif
#if WIN32
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
#endif
{
int recog;
//FTWindow *twin;

#if WIN32
#define MAX_ARGS 10
int argc;
char *argv[MAX_ARGS ];
char *token;
HWND hWin;


//convert lpCmdLine to argv
argc = 0;
token = strtok((char *)lpCmdLine,(char *)_T(" "));

while (token)
{
argv[argc++] = token;
token = strtok((char *)NULL,(char *)_T(" "));
}
#endif

memset(&PStatus,0,sizeof(ProgramStatus));

#if WIN32
PStatus.hInstance=hInstance;
#endif

recog=0;
if (argc>1) {
//look for debug flag
#if Linux
	if (strcasecmp(argv[1],"-h")==0 || strcasecmp(argv[1],"--help")==0) {
#endif
#if WIN32
	if (_stricmp(argv[1],"-h")==0 || _stricmp(argv[1],"--help")==0) {
#endif
    printhelp();
    return 0;
  }
#if Linux
	if (strcasecmp(argv[1],"-i")==0 || strcasecmp(argv[1],"--info")==0) {
#endif
#if WIN32
	if (_stricmp(argv[1],"-i")==0 || _stricmp(argv[1],"--info")==0) {
#endif
    PStatus.flags|=PInfo;
    fprintf(stderr,"\nShow Info Mode on\n");
    recog=1;
  } 
  if (recog==0) {
    fprintf(stderr,"\nI do not recognize argument %s\n",argv[1]);
    printhelp();
    return 0;
  }
}

getcwd(PStatus.ProjectPath,FTMedStr);
strcat(PStatus.ProjectPath,"/");
fprintf(stderr,"ProjectPath=%s\n",PStatus.ProjectPath);




//maybe should just call OpenWindow("main")
PStatus.FirstWindow=(FTControlfunc *)main_CreateFTWindow;

//fprintf(stderr,"after firstwin\n");
//can customize
/*
PStatus.timespace=7;
PStatus.PadY=4;
PStatus.PadX=2;
PStatus.ResizePix=6;
PStatus.dclickdelay=100;  //200 is too slow
PStatus.timescale=1;
*/

InitFreeThought();
//init XWindows (including font and Freethought)
//InitX();


//GetTime();  //update PStatus time variables utim (us) and ctim (h:m:s)
//PStatus.lastexpose=localtime(&ttime);  //is mallocing?
FT_GetInput();

//call universal (global) function to make firstwindow
(*PStatus.FirstWindow) ();
//alternatively:
//main_CreateFTWindow();
//or
//InitFTI();

#if WIN32
hWin=PStatus.iwindow->hwindow;
#endif

//PStatus.exitprogram=0;
while((PStatus.flags&PExit)==0)
{
//GetTime();  //update PStatus time variables utim (us) and ctim (h:m:s)
FT_GetInput();
//fprintf(stderr,"0 ");

}//end while  ex==0


//fprintf(stderr,"before CloseX\n ");
//CloseX();
CloseFreeThought();


return(1);
}  //end main()



