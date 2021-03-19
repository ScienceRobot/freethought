//win_openfile.h

int openfile_CreateFTWindow(void);
void win1key(FTWindow *twin,unsigned char key);
int openfile_AddFTControls(FTWindow *twin);
void btnSendUSB_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
int InitUSB(void);
void CloseUSB(void);
void btnGetUSB_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnGetFile_Click(FTWindow *twin,FTControl *tFTControl,int x, int y);
void txtGetFile_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key);
int  LoadDataFile(char *tstr,FTControl *tFTControl);
void txtOpen_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key);
