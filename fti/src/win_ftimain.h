//win_ftimain.h
#define TrackX 96


#if 0 
//I would have to make addwin,delwin,addFTControl,delFTControl functions for these

struct _fwindow; //forward declaration - should not be needed, but yet is.
typedef struct _fwindow fwindow;
struct _fFTControl; //forward declaration - should not be needed, but yet is.
typedef struct _fFTControl fFTControl;


//holds variables of window not in effect while designing
struct _fwindow
{
unsigned int flags;
char name[FTMedStr];
hwindow *next;
fFTControl *iFTControl;
};

//holds variables of FTControl not in effect while designing
struct _fFTControl
{
unsigned int flags;
char name[FTMedStr];
fFTControl *next;
};

#endif

struct _FTIStatus; //forward declaration - should not be needed, but yet is.
typedef struct _FTIStatus FTIStatus;


#define FDrawingFTControl 0x0001 //Drawing new FTControl on new window
//global variables for fti
struct _FTIStatus
{
unsigned int flags;
//fwindow *iwindow;
char Prefix[FTMedStr];  //prefix for duplicate windows and FTControl
char PrefixRClick[FTMedStr];  //prefix for Right Click FTControl
int StartX,StartY,LastX,LastY;  //start and last points of drawing FTControl box
int CType; //type of FTControl being drawn
#if Linux
XImage *erase[4];  //for drawing FTControl 4 lines
#endif
};




int main_CreateFTWindow(void);
/*
int winfmsFile_CreateFTWindow(void);
int winfmsEdit_CreateFTWindow(void);
int winfmsNew_CreateFTWindow(void);
int winfmsOpen_CreateFTWindow(void);
int winfmsSaveAs_CreateFTWindow(void);
int winfmsSave_CreateFTWindow(void);
int winfmsRecent_CreateFTWindow(void);
int winfms2Open_CreateFTWindow(void);
*/

int main_AddFTControls(FTWindow *twin);
/*
int winfmsFile_AddFTControls(FTWindow *twin);
int winfmsEdit_AddFTControls(FTWindow *twin);
int winfmsNew_AddFTControls(FTWindow *twin);
int winfmsOpen_AddFTControls(FTWindow *twin);
int winfmsSaveAs_AddFTControls(FTWindow *twin);
int winfmsSave_AddFTControls(FTWindow *twin);
int winfmsRecent_AddFTControls(FTWindow *twin);
int winfms2Open_AddFTControls(FTWindow *twin);
*/

//void main_ButtonDownUpOnFTControl(FTControl *tFTControl,int x,int y);
void main_Keypress(FTWindow *twin,unsigned char key);
void btnRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
long long pixel2time(int x);
int time2pixel(long long t);
void btnGet_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnOpen_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnSave_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnOut_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void ScrollBarClick(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnScrollRight_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnScrollLeft_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnScrollLeft2_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnOpenProject_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnPlay_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnStop_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnClick_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnExit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnTest_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btn_fmFile_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void FTItemNewWindow_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void main_OnOpen(FTWindow *twin);
void NewWin_ButtonDownUp(FTWindow *twin,int x,int y);
void NewWin_GotFocus(FTWindow *twin);
void NewWin_LostFocus(FTWindow *twin);
void NewWin_ButtonDown(FTWindow *twin,int x,int y);
void NewWin_ButtonUp(FTWindow *twin,int x,int y);
void NewWin_MouseMove(FTWindow *twin,int x,int y);
void EraseFTControlBox(FTWindow *twin,int x,int y); 
void NewWin_Keypress(FTWindow *twin,unsigned char key);
void ResetFTControlButton(void);
void FTControl_GotFocus(FTWindow *twin,FTControl *tFTControl);
void FTControl_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key);
void main_GotFocus(void);
void FTControl_OnMove(FTWindow *twin,FTControl *tFTControl,int x,int y);
void FTControl_OnResize(FTWindow *twin,FTControl *tFTControl,int x,int y);
void NewWin_OnMove(FTWindow *twin);
void NewWin_OnResize(FTWindow *twin);
void NewWin_Button2Down(FTWindow *twin,int x,int y);
void NewWin_Button2Up(FTWindow *twin,int x,int y);

